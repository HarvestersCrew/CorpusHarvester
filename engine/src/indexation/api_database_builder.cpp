#include "indexation/api_database_builder.h"

ApiDatabaseBuilder::ApiDatabaseBuilder() : ApiRequestBuilder() {}

void ApiDatabaseBuilder::add_request(
    const string &api_name,
    const unordered_map<string, pair<string, string>> &params) {

  const shared_ptr<api_loader> api = ApiFactory::get_api(api_name);
  for (const auto &el : params) {
    if (!api->find_response_parameter(el.first).has_value()) {
      throw api_no_setting_exception(el.first);
    }
    const string &op = el.second.second;
    if (op != "=" && op != "<=" && op != ">=" && op != "<" && op != ">" &&
        op != "!=") {
      throw api_builder_incompatible_operator(op, "database");
    }
  }

  ApiRequestBuilder::add_request(api_name, params);
}

list<shared_ptr<File>> ApiDatabaseBuilder::build(unsigned int number) const {

  list<shared_ptr<File>> res;

  // Used to store the prepared SQL values to send to the SQL engine
  vector<string> prepared_values;

  // SQL query to use at the end
  stringstream query;
  query << "SELECT * FROM (";

  // Piece of request used in each tag selection
  string select_distinct_api =
      "SELECT DISTINCT f.* FROM File f INNER JOIN Tag t "
      "ON f.id = t.file_id WHERE f.source = ?";

  // For each request we use an iterator, used to easily know if it is the first
  // request to add commas and other caracters
  for (auto request_it = this->get_requests().begin();
       request_it != this->get_requests().end(); ++request_it) {

    const shared_ptr<api_loader> &api = request_it->first;
    const unordered_map<string, pair<string, string>> &params =
        request_it->second;

    // If it isn't the first separate request, add the UNION keyword to get new
    // results appended
    if (request_it != this->get_requests().begin()) {
      query << " UNION ";
    }

    query << "(";

    // If there is a request, even if there are no parameters we should fetch
    // every entry of this API
    query << select_distinct_api;
    prepared_values.push_back(api->get_name());

    // For each parameter in the request, we also use iterators to check if it
    // is the first
    for (auto param_it = params.begin(); param_it != params.end(); ++param_it) {

      // If this is the first parameter, we don't have to add the INTERSECT and
      // not the SELECT query because it was inserted before the for
      if (param_it != params.begin()) {
        query << " INTERSECT " << select_distinct_api;
        prepared_values.push_back(api->get_name());
      }

      // This is the query to select the entry corresponding to the parameter
      // name and value
      query << " AND t.name = ? AND t.value = ?";
      prepared_values.push_back(param_it->first);
      prepared_values.push_back(param_it->second.first);
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

  logger::info("Retrieved " + std::to_string(res.size()) + " from DB");

  return res;
}
