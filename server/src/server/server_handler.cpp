#include "server/server_handler.h"
#include "server/websocket_server.h"

void server_handler::fill_available_functions(
    unordered_map<string, handler_function_data> &functions_data,
    unordered_map<string, handler_function_no_data> &functions_no_data) {

  functions_no_data.emplace("get_apis_infos", &get_apis_infos);
  functions_no_data.emplace("get_logger_infos", &get_logger_infos);
  functions_no_data.emplace("get_storage_infos", &get_storage_infos);
  functions_no_data.emplace("clear_logfile", &clear_logfile);
  functions_no_data.emplace("refresh_apis", &refresh_apis);
  functions_no_data.emplace("get_db_stats", &get_db_stats);

  functions_data.emplace("update_logger", &update_logger);
  functions_data.emplace("storage_migration", &storage_migration);
  functions_data.emplace("api_builder_query", &api_builder_query);
  functions_data.emplace("get_corpuses", &get_corpuses);
  functions_data.emplace("add_build_to_corpus", &add_build_to_corpus);
  functions_data.emplace("set_corpus_title", &set_corpus_title);
  functions_data.emplace("delete_corpus", &delete_corpus);
  functions_data.emplace("export_corpus", &export_corpus);
}

pair<string, json> server_handler::dispatch_request(ConnectionData &con,
                                                    const json &j) {
  unordered_map<string, handler_function_data> functions_data;
  unordered_map<string, handler_function_no_data> functions_no_data;

  fill_available_functions(functions_data, functions_no_data);

  string request = j.at("request").get<string>();

  logger::debug("Dispatching client request to " + request);
  if (functions_data.find(request) != functions_data.end() &&
      j.contains("data")) {
    return (*functions_data.at(request))(con, j.at("data"));
  } else if (functions_no_data.find(request) != functions_no_data.end()) {
    return (*functions_no_data.at(request))(con);
  } else {
    logger::error("Request not supported: " + request);
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

pair<string, json> server_handler::get_storage_infos(ConnectionData &con) {
  const string path = con._mr.get_storage_path();
  json j;
  j["storage_path"] = path;
  j["port"] = WebsocketServer::get_file_server_port();
  return make_pair("get_storage_infos", j);
}

pair<string, json> server_handler::clear_logfile(ConnectionData &con) {
  con._mr.clear_logfile();
  return make_pair("clear_logfile", json::object());
}

pair<string, json> server_handler::refresh_apis(ConnectionData &con) {
  con._mr.refresh_apis();
  auto res = get_apis_infos(con);
  res.first = "refresh_apis";
  WebsocketServer::broadcast_json(res);
  return make_pair("_refresh_apis", json::object());
}

pair<string, json> server_handler::get_db_stats(ConnectionData &con) {
  auto stats = con._mr.get_app_stats();
  json j;
  j["stats"] = json::object();
  j["stats"]["corpus_count"] = stats.corpus_count;
  j["stats"]["file_count"] = stats.file_count;
  j["stats"]["image_count"] = stats.image_count;
  j["stats"]["text_count"] = stats.text_count;
  j["stats"]["total_size"] = stats.total_size;
  return make_pair("get_db_stats", j);
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

  // Restart file server to load new file path
  WebsocketServer::restart_file_server();

  auto to_return = get_storage_infos(con);
  WebsocketServer::broadcast_json(to_return);
  return make_pair("storage_migration", json::object());
}

pair<string, json> server_handler::api_builder_query(ConnectionData &con,
                                                     const json &j) {
  if (!j.contains("builder") || !j.at("builder").is_array())
    throw wss_invalid_request();

  if (!j.contains("is_web") || !j.at("is_web").is_boolean())
    throw wss_invalid_request();

  bool is_web = j.at("is_web").get<bool>();

  con._mr.api_builder_clear_all(is_web);

  for (const auto &request : j.at("builder")) {
    if (!request.contains("name") || !request.at("name").is_string())
      throw wss_invalid_request();

    unsigned long idx = con._mr.api_builder_add_request(
        is_web, request.at("name").get<string>());

    if (!request.contains("values") || !request.at("values").is_object())
      throw wss_invalid_request();

    for (auto &[key, value] : request.at("values").items()) {
      con._mr.api_builder_add_request_parameter(is_web, idx, key,
                                                value.at("val").get<string>(),
                                                value.at("op").get<string>());
    }
  }

  unsigned int nbr = 0;
  if (j.contains("number")) {
    nbr = j.at("number").get<unsigned int>();
  }

  auto files = con._mr.api_builder_build(is_web, nbr);

  json result = json::object();
  result["files"] = json::array();
  result["is_web"] = is_web;

  for (const auto &file : files) {
    result.at("files").push_back(file->serialize());
  }

  return make_pair("api_builder_query", result);
}

pair<string, json> server_handler::get_corpuses(ConnectionData &con,
                                                const json &j) {
  map<string, string> filters;
  string order;
  if (j.contains("title")) {
    filters.emplace("title", j.at("title").get<string>());
  }

  if (j.contains("order")) {
    order = j.at("order").get<string>();
  }

  auto corpuses = con._mr.get_corpuses(filters, order);
  json res = json::object();
  res["corpuses"] = json::array();
  for (const auto &corpus : corpuses) {
    res.at("corpuses").push_back(corpus->serialize());
  }
  return make_pair("get_corpuses", res);
}

pair<string, json> server_handler::add_build_to_corpus(ConnectionData &con,
                                                       const json &j) {
  if (!j.contains("create") && !j.contains("type"))
    throw wss_invalid_request();

  bool create = j.at("create").get<bool>();
  string type = j.at("type").get<string>();
  bool is_web = type == "web";

  ApiRequestBuilder &builder = con._mr.api_builder_get_based_on_bool(is_web);

  int id;

  if (create) {
    if (!j.contains("title"))
      throw wss_invalid_request();
    id = con._mr.create_corpus(j.at("title").get<string>(),
                               builder.get_latest_build(), std::nullopt);
  } else {
    if (!j.contains("id"))
      throw wss_invalid_request();
    id = j.at("id").get<int>();
    con._mr.add_to_corpus(id, builder.get_latest_build());
  }

  json res = {{"id", id}};

  return make_pair("add_build_to_corpus", res);
}

pair<string, json> server_handler::set_corpus_title(ConnectionData &con,
                                                    const json &j) {
  if (!j.contains("id") || !j.at("id").is_number_unsigned())
    throw wss_invalid_request();

  if (!j.contains("title") || !j.at("title").is_string())
    throw wss_invalid_request();

  int id = j.at("id").get<int>();
  string name = j.at("title").get<string>();

  con._mr.set_corpus_title(id, name);

  json res = json::object();
  res["id"] = id;
  res["title"] = name;

  return make_pair("set_corpus_title", res);
}

pair<string, json> server_handler::delete_corpus(ConnectionData &con,
                                                 const json &j) {
  if (!j.contains("id") || !j.at("id").is_number_unsigned())
    throw wss_invalid_request();

  int id = j.at("id").get<int>();

  con._mr.delete_corpus(id);

  json res = json::object();
  res["id"] = id;

  return make_pair("delete_corpus", res);
}

pair<string, json> server_handler::export_corpus(ConnectionData &con,
                                                 const json &j) {
  if (!j.contains("id") || !j.at("id").is_number_unsigned())
    throw wss_invalid_request();

  int id = j.at("id").get<int>();

  json res = json::object();
  res["id"] = id;
  auto export_response = con._mr.export_corpus(id, "zip");
  res["path"] = export_response.first;
  res["size"] = export_response.second;

  return make_pair("export_corpus", res);
}
