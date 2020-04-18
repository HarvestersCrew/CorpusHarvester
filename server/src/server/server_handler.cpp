#include "server/server_handler.h"

void server_handler::fill_available_functions(
    unordered_map<string, handler_function_data> &functions_data,
    unordered_map<string, handler_function_no_data> &functions_no_data) {

  functions_no_data.emplace("get_apis_infos", &get_apis_infos);
  functions_no_data.emplace("get_logger_infos", &get_logger_infos);
  functions_no_data.emplace("get_storage_path", &get_storage_path);
  functions_no_data.emplace("clear_logfile", &clear_logfile);

  functions_data.emplace("update_logger", &update_logger);
  functions_data.emplace("storage_migration", &storage_migration);
  functions_data.emplace("download_query", &download_query);
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

/**
 * ---------------------
 * METHODS WITHOUT DATA
 * ---------------------
 */

pair<string, json> server_handler::get_apis_infos(ConnectionData &con) {
  json res = json::array();
  for (const auto &api : con._mr.get_api_loaders()) {
    res.push_back(api->serialize());
  }
  return make_pair("get_apis", res);
}

pair<string, json> server_handler::get_logger_infos(ConnectionData &con) {
  const auto loggers = con._mr.get_logger_settings();
  json j;
  j["level"] = get<0>(loggers);
  j["output"] = get<1>(loggers);
  j["output_path"] = get<2>(loggers);
  return make_pair("get_logger_infos", j);
}

pair<string, json> server_handler::get_storage_path(ConnectionData &con) {
  const string path = con._mr.get_storage_path();
  json j;
  j["storage_path"] = path;
  return make_pair("get_storage_path", j);
}

pair<string, json> server_handler::clear_logfile(ConnectionData &con) {
  con._mr.clear_logfile();
  return make_pair("clear_logfile", json::object());
}

/**
 * ---------------------
 * METHODS WITH DATA
 * ---------------------
 */

pair<string, json> server_handler::update_logger(ConnectionData &con,
                                                 const json &j) {
  if (!j.contains("setting") || !j.contains("value")) {
    throw wss_invalid_request();
  }
  string setting = j.at("setting").get<string>();
  string value = j.at("value").get<string>();
  if (setting == "level") {
    con._mr.set_logger_level(value);
  } else if (setting == "output") {
    con._mr.set_logger_output(value);
  } else if (setting == "output_path") {
    con._mr.set_logger_output_path(value);
  }
  auto to_return = get_logger_infos(con);
  WebsocketServer::broadcast_json(to_return);
  return make_pair("update_logger", json::object());
}

pair<string, json> server_handler::storage_migration(ConnectionData &con,
                                                     const json &j) {
  if (!j.contains("new_path")) {
    throw wss_invalid_request();
  }
  con._mr.migrate_storage(j.at("new_path").get<string>());
  auto to_return = get_storage_path(con);
  WebsocketServer::broadcast_json(to_return);
  return make_pair("storage_migration", json::object());
}

pair<string, json> server_handler::download_query(ConnectionData &con,
                                                  const json &j) {
  if (!j.contains("builder") || !j.at("builder").is_array())
    throw wss_invalid_request();

  con._mr.api_builder_clear_all(true);

  for (const auto &request : j.at("builder")) {
    if (!request.contains("name") || !request.at("name").is_string())
      throw wss_invalid_request();

    unsigned long idx =
        con._mr.api_builder_add_request(true, request.at("name").get<string>());

    if (!request.contains("values") || !request.at("values").is_object())
      throw wss_invalid_request();

    for (auto &[key, value] : request.at("values").items()) {
      con._mr.api_builder_add_request_parameter(true, idx, key,
                                                value.get<string>(), "=");
    }
  }

  auto files = con._mr.api_builder_build(true, 0);

  json result = json::object();
  result["files"] = json::array();

  for (const auto &file : files) {
    result.at("files").push_back(file->serialize());
  }

  return make_pair("download_query", result);
}
