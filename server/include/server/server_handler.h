#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "indexation/file.h"
#include "server/websocket_server.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <websocketpp/server.hpp>

using nlohmann::json;
using std::get;
using std::list;
using std::make_pair;
using std::map;
using std::optional;
using std::pair;
using std::shared_ptr;
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
 * Gets the current storage path
 */
pair<string, json> get_storage_path(ConnectionData &con);

/**
 * Clears logfile
 */
pair<string, json> clear_logfile(ConnectionData &con);

/**
 * Refresh the loaded APIs and broadcasts them to everyone
 */
pair<string, json> refresh_apis(ConnectionData &con);

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

/**
 * Storage migration
 * @param new_path new path to migrate to
 * @return get_storage_path
 * @throw StorageMigrationException if the new path can't be used
 */
pair<string, json> storage_migration(ConnectionData &con, const json &j);

/**
 * Queries a download with the DL builder
 * @param builder array of object requests following this pattern:
 * {name:"API_NAME", values:{PARAM1:"VAL1",PARAM2:"VAL2"...}}
 * @param number optional, specifies the number of files wanted
 */
pair<string, json> download_query(ConnectionData &con, const json &j);

/**
 * Retrieve corpuses
 * @param title not required, to filter by name
 * @param order not required, to order (none, date_asc, date_desc, name_asc,
 * name_desc)
 */
pair<string, json> get_corpuses(ConnectionData &con, const json &j);

/**
 * Add the result of a build to a corpus
 * @param create bool to create or not a corpus
 * @param id int if appending to a corpus
 * @param title name if creating a corpus
 * @param type "web" or "db"
 * @throw db_id_not_found if corpus wasn't found
 * @return "id" ID affected or created
 */
pair<string, json> add_build_to_corpus(ConnectionData &con, const json &j);

} // namespace server_handler

#endif
