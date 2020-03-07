#include "indexation/search_builder.h"

SearchBuilder::SearchBuilder(sql::Connection *db, bool verbose)
    : RequestBuilder(), _first_prepared_values(0), _prepared_values(0),
      _current_prepared_values(0), _current_clause_only_on_file(true),
      _verbose(verbose), _db(db) {}

void SearchBuilder::valid_current_clause() {
  std::string distinct = "";
  if (_current_clause_only_on_file) {
    _first_select += "(" + _current_clause + ") AND ";
    for (std::string pv : _current_prepared_values) {
      _first_prepared_values.push_back(pv);
    }
  } else {
    _left_request =
        "(SELECT DISTINCT " + distinct + "f.* FROM " + _left_request;
    _right_request +=
        " f, Tag t WHERE f.id = t.file_id AND (" + _current_clause + "))";
    for (std::string pv : _current_prepared_values) {
      _prepared_values.push_back(pv);
    }
  }
}

SearchBuilder *SearchBuilder::add_condition(std::string condition_name,
                                            std::string condition_value,
                                            std::string op) {
  _current_clause += "(f." + condition_name + " " + op + " ?)";
  _current_prepared_values.push_back(condition_value);
  _filters += condition_name + " " + op + " " + condition_value;
  return this;
}

SearchBuilder *SearchBuilder::add_tag_condition(std::string tag_name,
                                                std::string tag_value,
                                                std::string op) {
  _current_clause_only_on_file = false;
  if (op == "=") {
    _current_clause += "(t.name = ? AND t.value = ?)";
  } else {
    _current_clause += "(t.name = ? AND cast(t.value as int) " + op + " ?)";
  }
  _current_prepared_values.push_back(tag_name);
  _current_prepared_values.push_back(tag_value);
  _filters += tag_name + " " + op + " " + tag_value;
  return this;
}

SearchBuilder *SearchBuilder::logical_and() {
  valid_current_clause();
  _current_clause = "";
  _current_prepared_values.clear();
  _current_clause_only_on_file = true;
  _filters += " AND ";
  return this;
}

SearchBuilder *SearchBuilder::logical_or() {
  _current_clause += " OR ";
  _filters += " OR ";
  return this;
}

std::list<shared_ptr<File>> SearchBuilder::build() {
  valid_current_clause();
  std::string request;
  if (_left_request == "") {
    _first_select = "SELECT f.* FROM File f WHERE " + _first_select + "1";
  } else {
    if (_first_select == "") {
      _first_select = "File";
    } else {
      _first_select = "(SELECT f.* FROM File f WHERE " + _first_select + "1)";
    }
  }
  request = _left_request + _first_select + _right_request;
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  std::list<shared_ptr<File>> files;

  print_if_verbose(request, _verbose);
  prep_stmt = _db->prepareStatement(request);
  int i = 1;
  for (auto &preparedValue : _first_prepared_values) {
    prep_stmt->setString(i, preparedValue);
    i++;
  }
  for (auto &preparedValue : _prepared_values) {
    prep_stmt->setString(i, preparedValue);
    i++;
  }
  res = prep_stmt->executeQuery();

  while (res->next()) {
    shared_ptr<File> file = std::make_shared<File>();
    file->fill_from_statement(_db, res);
    files.push_back(file);
  }
  delete prep_stmt;
  delete res;
  return files;
}

void SearchBuilder::clear() {
  _left_request = "";
  _right_request = "";
  _current_clause = "";
  _first_select = "";
  _filters = "";
  _current_clause_only_on_file = true;
  _prepared_values.clear();
  _first_prepared_values.clear();
  _current_prepared_values.clear();
}
