#include <iostream>
#include <regex>

#include "indexation/file.h"
#include "indexation/search_builder.h"
#include "utils/utils.h"

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#define TO_REPLACE "%s"

using std::cout;
using std::endl;

void SearchBuilder::validCurrentClause(bool build) {
  if (_currentClauseOnlyOnFile) {
    _firstSelect += _currentClause + " AND ";
    if (build) {
      _request =
          std::regex_replace(_request, std::regex("SELECT"), "SELECT DISTINCT",
                             std::regex_constants::format_first_only);
    }
  } else {
    if (_request != TO_REPLACE) {
      _request = "(" + _request + ")";
    }
    string distinct = ((build) ? "DISTINCT " : "");
    _request = "SELECT " + distinct + "f.* FROM " + _request +
               " f, Tag t WHERE f.id = t.file_id AND " + _currentClause;
  }
}

SearchBuilder::SearchBuilder(sql::Connection *db, bool verbose)
    : RequestBuilder(), _request(TO_REPLACE), _currentClauseOnlyOnFile(true),
      _verbose(verbose), _db(db) {}

SearchBuilder *SearchBuilder::addCondition(string conditionName,
                                           string conditionValue, string op) {
  if (_currentClauseOnlyOnFile) {
    _firstSelect += "(f." + conditionName + " " + op + " ?)";
    _firstPreparedValues.push_back(conditionValue);
  } else {
    _currentClause += "(f." + conditionName + " " + op + " ?)";
    _preparedValues.push_back(conditionValue);
  }
  return this;
}

SearchBuilder *SearchBuilder::addTagCondition(string tagName, string tagValue,
                                              string op) {

  _currentClauseOnlyOnFile = false;
  _currentClause += "(t.name = ? AND t.value " + op + " ?)";
  _preparedValues.push_back(tagName);
  _preparedValues.push_back(tagValue);
  return this;
}

SearchBuilder *SearchBuilder::logicalAnd() {
  validCurrentClause(false);
  _currentClause = "";
  _currentClauseOnlyOnFile = true;
  return this;
}

SearchBuilder *SearchBuilder::logicalOr() {
  _currentClause += " OR ";
  return this;
}

list<File *> SearchBuilder::build() {
  validCurrentClause(true);
  if (_firstSelect == "") {
    _firstSelect = "File";
  } else {
    _firstSelect += "1";
    _firstSelect = "SELECT * FROM File f WHERE " + _firstSelect;
    if (_request != TO_REPLACE) {
      _firstSelect = "(" + _firstSelect + ")";
    }
  }
  _request = std::regex_replace(_request, std::regex(TO_REPLACE), _firstSelect);
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  list<File *> files;

  print(_request, _verbose);
  prep_stmt = _db->prepareStatement(_request);
  int i = 1;
  for (auto &preparedValue : _firstPreparedValues) {
    prep_stmt->setString(i, preparedValue);
    i++;
  }
  for (auto &preparedValue : _preparedValues) {
    prep_stmt->setString(i, preparedValue);
    i++;
  }
  res = prep_stmt->executeQuery();

  while (res->next()) {
    File *file = new File();
    file->fillFromStatement(res);
    file->fetchTags(_db);
    files.push_back(file);
  }
  return files;
}

void SearchBuilder::clear() {
  _request = TO_REPLACE;
  _currentClause = "";
  _firstSelect = "";
  _currentClauseOnlyOnFile = true;
  _preparedValues.clear();
  _firstPreparedValues.clear();
}
