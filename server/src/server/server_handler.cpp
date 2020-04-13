#include "server/server_handler.h"

void server_handler::fill_available_functions(
    unordered_map<string, handler_function_data> &functions_data,
    unordered_map<string, handler_function_no_data> &functions_no_data) {

  functions_no_data.emplace("get_apis_infos", &get_apis_infos);
}

pair<string, json> server_handler::dispatch_request(ConnectionData &con,
                                                    const json &j) {
  unordered_map<string, handler_function_data> functions_data;
  unordered_map<string, handler_function_no_data> functions_no_data;

  fill_available_functions(functions_data, functions_no_data);

  string request = j.at("request").get<string>();

  if (functions_data.find(request) != functions_data.end() &&
      j.contains("data")) {
    return (*functions_data.at(request))(con, j.at("data"));
  } else if (functions_no_data.find(request) != functions_no_data.end()) {
    return (*functions_no_data.at(request))(con);
  } else {
    throw wss_invalid_request();
  }
}

pair<string, json> server_handler::get_apis_infos(ConnectionData &con) {
  json res = json::array();
  for (const auto &api : con._mr.get_api_loaders()) {
    res.push_back(api->serialize());
  }
  return make_pair("get_apis", res);
}
