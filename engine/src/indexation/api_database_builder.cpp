#include "indexation/api_database_builder.h"

ApiDatabaseBuilder::ApiDatabaseBuilder()
    : ApiRequestBuilder(), _order(ordering_method::NONE), _page(0) {
  _order_queries = {{ordering_method::NONE, DB_BUILDER_ORDER_NONE},
                    {ordering_method::API_ASC, DB_BUILDER_ORDER_API_ASC},
                    {ordering_method::API_DESC, DB_BUILDER_ORDER_API_DESC},
                    {ordering_method::SIZE_ASC, DB_BUILDER_ORDER_SIZE_ASC},
                    {ordering_method::SIZE_DESC, DB_BUILDER_ORDER_SIZE_DESC}};
}

json ApiDatabaseBuilder::serialize() const {
  json j = ApiRequestBuilder::serialize();
  j["order"] = _order;
  j["page"] = _page;
  return j;
}
void ApiDatabaseBuilder::deserialize(const json &j) {
  ApiRequestBuilder::deserialize(j);
  _order = (ordering_method)j.at("order").get<int>();
  _page = j.at("page").get<unsigned int>();
}

list<shared_ptr<File>> &ApiDatabaseBuilder::build(unsigned int number) {

  _latest_build.clear();
  list<shared_ptr<File>> &res = _latest_build;
  auto requests = this->get_usable_requests();

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
  for (auto request_it = requests.begin(); request_it != requests.end();
       ++request_it) {

    const shared_ptr<api_loader> &api = request_it->first;
    const unordered_map<string, pair<string, string>> &params =
        request_it->second;

    // If it isn't the first separate request, add the UNION keyword to get new
    // results appended
    if (request_it != requests.begin()) {
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

      // Retrieving the API parameter request in order to apply proper casting
      // to the values, and we consider its existence was checked when adding
      // the request
      const shared_ptr<api_parameter_response> api_param =
          api->find_response_parameter(param_it->first).value();

      // This is the query to select the entry corresponding to the parameter
      // name and value
      query << " AND t.name = ? AND ";
      query << api_param->get_sql_cast_prepared_string("t.value") << " "
            << param_it->second.second << " "
            << api_param->get_sql_cast_prepared_string("?");
      prepared_values.push_back(param_it->first);
      prepared_values.push_back(param_it->second.first);
    }

    query << ")";
  }

  query << ") as result_table";
  query << _order_queries.at(_order);

  if (number != 0) {
    unsigned int start = _page * number;
    query << " LIMIT " << start << "," << number;
  }

  logger::debug(query.str());

  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *sql_res;
  auto con = PoolDB::borrow_from_pool();
  prep_stmt = con->prepareStatement(query.str());

  for (long unsigned int i = 1; i <= prepared_values.size(); ++i) {
    prep_stmt->setString(i, prepared_values.at(i - 1));
  }

  sql_res = prep_stmt->executeQuery();

  while (sql_res->next()) {
    shared_ptr<File> file = std::make_shared<File>();
    file->fill_from_statement(sql_res);
    res.push_back(file);
  }

  delete prep_stmt;
  delete sql_res;
  PoolDB::unborrow_from_pool(con);

  logger::info("Retrieved " + std::to_string(res.size()) + " from DB");

  return res;
}

void ApiDatabaseBuilder::add_request_parameter(long unsigned int request_id,
                                               const string &param_name,
                                               const string &param_value,
                                               const string &op) {
  if (this->_requests.size() <= request_id) {
    throw api_builder_request_not_found(request_id);
  }
  if (op != "=" && op != "<=" && op != ">=" && op != "<" && op != ">" &&
      op != "!=") {
    throw api_builder_incompatible_operator(op, "database");
  }
  shared_ptr<api_loader> &api = this->_requests[request_id].first;
  if (!api->find_response_parameter(param_name).has_value()) {
    throw api_no_setting_exception(param_name);
  }
  ApiRequestBuilder::add_request_parameter(request_id, param_name, param_value,
                                           op);
}

void ApiDatabaseBuilder::set_order(ordering_method order) {
  if (order >= ordering_method::NONE && order <= ordering_method::SIZE_DESC) {
    _order = order;
  } else {
    throw api_builder_feature_not_supported("Given DB order not supported");
  }
}
ApiDatabaseBuilder::ordering_method ApiDatabaseBuilder::get_order() const {
  return _order;
}

void ApiDatabaseBuilder::set_page(unsigned int page) { _page = page; }

unsigned int ApiDatabaseBuilder::get_page() const { return _page; }

void ApiDatabaseBuilder::clear_all() {
  _order = ordering_method::NONE;
  ApiRequestBuilder::clear_all();
}
