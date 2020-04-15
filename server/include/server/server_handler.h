#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "server/websocket_server.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <websocketpp/server.hpp>

using nlohmann::json;
using std::get;
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

/**
 * ---------------------
 * METHODS WITHOUT DATA
 * ---------------------
 */

/**
 * Get informations about the APIs
 */
pair<string, json> get_apis_infos(ConnectionData &con);

/**
 * Get informations about the logger settings
 */
pair<string, json> get_logger_infos(ConnectionData &con);

/**
 * ---------------------
 * METHODS WITH DATA
 * ---------------------
 */

/**
 * Update a logger setting
 * @param setting name of the setting (level, output, output_path)
 * @param value value to set
 * @throw manager_request_invalid_parameter if either the level or output is
 * wrong
 * @throw logger_exception if the output_path is wrong
 */
pair<string, json> update_logger(ConnectionData &con, const json &j);

} // namespace server_handler

#endif
