#include "server/server_handler.h"

pair<string, json> server_handler::dispatch_request(const connection_hdl &hdl,
                                                    const ConnectionData &con,
                                                    const json &j) {
  unordered_map<string, handler_function> functions;

  string request = j.at("request").get<string>();

  if (functions.find(request) == functions.end()) {
    throw wss_invalid_request();
  }
}
