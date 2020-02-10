#include "indexation/search_builder.h"

using std::cout;
using std::endl;

SearchBuilder::SearchBuilder(sql::Connection *db, bool verbose)
    : RequestBuilder(), _currentClauseOnlyOnFile(true), _verbose(verbose),
      _db(db) {}

void SearchBuilder::validCurrentClause() {
  string distinct = "";
  if (_currentClauseOnlyOnFile) {
    _firstSelect += "(" + _currentClause + ") AND ";
    for (string pv : _currentPreparedValues) {
      _firstPreparedValues.push_back(pv);
    }
  } else {
    _left_request =
        "(SELECT DISTINCT " + distinct + "f.* FROM " + _left_request;
    _right_request +=
        " f, Tag t WHERE f.id = t.file_id AND (" + _currentClause + "))";
    for (string pv : _currentPreparedValues) {
      _preparedValues.push_back(pv);
    }
  }
}

SearchBuilder *SearchBuilder::addCondition(string conditionName,
                                           string conditionValue, string op) {
  _currentClause += "(f." + conditionName + " " + op + " ?)";
  _currentPreparedValues.push_back(conditionValue);
  return this;
}

SearchBuilder *SearchBuilder::addTagCondition(string tagName, string tagValue,
                                              string op) {
  _currentClauseOnlyOnFile = false;
  if (op == "=") {
    _currentClause += "(t.name = ? AND t.value = ?)";
  } else {
    _currentClause += "(t.name = ? AND cast(t.value as int) " + op + " ?)";
  }
  _currentPreparedValues.push_back(tagName);
  _currentPreparedValues.push_back(tagValue);
  return this;
}

SearchBuilder *SearchBuilder::logicalAnd() {
  validCurrentClause();
  _currentClause = "";
  _currentPreparedValues.clear();
  _currentClauseOnlyOnFile = true;
  return this;
}

SearchBuilder *SearchBuilder::logicalOr() {
  _currentClause += " OR ";
  return this;
}

list<File *> SearchBuilder::build() {
  validCurrentClause();
  string request;
  if (_left_request == "") {
    _firstSelect = "SELECT f.* FROM File f WHERE " + _firstSelect + "1";
  } else {
    if (_firstSelect == "") {
      _firstSelect = "File";
    } else {
      _firstSelect = "(SELECT f.* FROM File f WHERE " + _firstSelect + "1)";
    }
  }
  request = _left_request + _firstSelect + _right_request;
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  list<File *> files;

  printIfVerbose(request, _verbose);
  prep_stmt = _db->prepareStatement(request);
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
    file->fillFromStatement(_db, res);
    files.push_back(file);
  }
  return files;
}

void SearchBuilder::clear() {
  _left_request = "";
  _right_request = "";
  _currentClause = "";
  _firstSelect = "";
  _currentClauseOnlyOnFile = true;
  _preparedValues.clear();
  _firstPreparedValues.clear();
  _currentPreparedValues.clear();
}
