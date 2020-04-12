#include "server/server_handler.h"

pair<string, json> server_handler::dispatch_request(const ConnectionData &con,
                                                    const json &j) {
  unordered_map<string, handler_function_data> functions_data;
  unordered_map<string, handler_function_no_data> functions_no_data;

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

pair<string, json> server_handler::get_apis_infos(const ConnectionData &con) {
  return make_pair("get_apis", json::array());
}
