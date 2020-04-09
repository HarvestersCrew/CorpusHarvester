#include "indexation/api_database_builder.h"

ApiDatabaseBuilder::ApiDatabaseBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDatabaseBuilder::build(unsigned int number) const {

  list<shared_ptr<File>> res;
  vector<string> prepared_values;

  stringstream query;
  query << "SELECT * FROM (";

  string select_distinct =
      "SELECT DISTINCT f.* FROM File f INNER JOIN Tag t ON f.id = t.file_id";

  for (auto request_it = this->get_requests().begin();
       request_it != this->get_requests().end(); ++request_it) {

    const shared_ptr<api_loader> &api = request_it->first;
    const unordered_map<string, string> &params = request_it->second;

    string select_distinct_api = select_distinct + " WHERE f.source = ?";

    if (request_it != this->get_requests().begin()) {
      query << " UNION ";
    }

    query << "(";

    query << select_distinct_api;
    prepared_values.push_back(api->get_name());
    for (auto param_it = params.begin(); param_it != params.end(); ++param_it) {

      if (param_it != params.begin()) {
        query << " INTERSECT " << select_distinct_api;
        prepared_values.push_back(api->get_name());
      }

      query << " AND t.name = ? AND t.value = ?";
      prepared_values.push_back(param_it->first);
      prepared_values.push_back(param_it->second);
    }

    query << ")";
  }

  query << ") as result_table";
  query << " ORDER BY id";

  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *sql_res;
  prep_stmt = HarvesterDatabase::init()->prepareStatement(query.str());

  for (long unsigned int i = 1; i <= prepared_values.size(); ++i) {
    prep_stmt->setString(i, prepared_values.at(i - 1));
  }

  sql_res = prep_stmt->executeQuery();

  while (sql_res->next()) {
    shared_ptr<File> file = std::make_shared<File>();
    file->fill_from_statement(HarvesterDatabase::init(), sql_res);
    res.push_back(file);
  }

  logger::info("Retrieved " + to_string(res.size()) + " from DB");

  return res;
}
