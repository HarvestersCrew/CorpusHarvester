#include <iostream>
#include <regex>

#include "indexation/mysql/file.h"
#include "indexation/mysql/search_builder.h"
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
      _statement = std::regex_replace(_statement, std::regex("SELECT"),
                                      "SELECT DISTINCT",
                                      std::regex_constants::format_first_only);
    }
  } else {
    if (_statement != TO_REPLACE) {
      _statement = "(" + _statement + ")";
    }
    string distinct = ((build) ? "DISTINCT " : "");
    _statement = "SELECT " + distinct + "f.* FROM " + _statement +
                 " f, Tag t WHERE f.id = t.file_id AND " + _currentClause;
  }
}

SearchBuilder::SearchBuilder(bool verbose)
    : _statement(TO_REPLACE), _currentClauseOnlyOnFile(true),
      _verbose(verbose) {}

SearchBuilder *SearchBuilder::fileTagEquals(string tag_name, string tag_value,
                                            string op) {
  _currentClauseOnlyOnFile = false;
  _currentClause += "(t.name = ? AND t.value " + op + " ?)";
  _preparedValues.push_back(tag_name);
  _preparedValues.push_back(tag_value);
  return this;
}

SearchBuilder *SearchBuilder::fileColumnEquals(string column_name,
                                               string column_value, string op) {
  if (_currentClauseOnlyOnFile) {
    _firstSelect += "(f." + column_name + " " + op + " ?)";
    _firstPreparedValues.push_back(column_value);
  } else {
    _currentClause += "(f." + column_name + " " + op + " ?)";
    _preparedValues.push_back(column_value);
  }
  return this;
}

SearchBuilder *SearchBuilder::sqlAnd() {
  validCurrentClause(false);
  _currentClause = "";
  _currentClauseOnlyOnFile = true;
  return this;
}

SearchBuilder *SearchBuilder::sqlOr() {
  _currentClause += " OR ";
  return this;
}

list<File *> SearchBuilder::build(sql::Connection *db) {
  validCurrentClause(true);
  if (_firstSelect == "") {
    _firstSelect = "File";
  } else {
    _firstSelect += "1";
    _firstSelect = "SELECT * FROM File f WHERE " + _firstSelect;
    if (_statement != TO_REPLACE) {
      _firstSelect = "(" + _firstSelect + ")";
    }
  }
  _statement =
      std::regex_replace(_statement, std::regex(TO_REPLACE), _firstSelect);
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  list<File *> files;

  print(_statement, _verbose);
  prep_stmt = db->prepareStatement(_statement);
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
    file->fetchTags(db);
    files.push_back(file);
  }
  return files;
}

void SearchBuilder::clear() {
  _statement = TO_REPLACE;
  _currentClause = "";
  _firstSelect = "";
  _currentClauseOnlyOnFile = true;
  _preparedValues.clear();
  _firstPreparedValues.clear();
}
