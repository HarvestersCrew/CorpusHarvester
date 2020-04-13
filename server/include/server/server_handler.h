#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "server/websocket_server.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <string>
#include <unordered_map>
#include <utility>
#include <websocketpp/server.hpp>

using nlohmann::json;
using std::make_pair;
using std::pair;
using std::string;
using std::unordered_map;
using websocketpp::connection_hdl;

struct ConnectionData;

namespace server_handler {

typedef pair<string, json> (*handler_function_data)(ConnectionData &,
                                                    const json &);

typedef pair<string, json> (*handler_function_no_data)(ConnectionData &);

void fill_available_functions(
    unordered_map<string, handler_function_data> &functions_data,
    unordered_map<string, handler_function_no_data> &functions_no_data);

pair<string, json> dispatch_request(ConnectionData &con, const json &j);

pair<string, json> get_apis_infos(ConnectionData &con);

} // namespace server_handler

#endif