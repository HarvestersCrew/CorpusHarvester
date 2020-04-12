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
using std::pair;
using std::string;
using std::unordered_map;
using websocketpp::connection_hdl;

struct ConnectionData;

namespace server_handler {

typedef json (*handler_function)(const connection_hdl &, const ConnectionData &,
                                 const json &);

pair<string, json> dispatch_request(const connection_hdl &hdl,
                                    const ConnectionData &con, const json &j);

} // namespace server_handler

#endif
