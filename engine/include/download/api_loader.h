#ifndef API_LOADER_H
#define API_LOADER_H

#include "database/harvester_database.h"
#include "database/setting.h"
#include "download/api_parameter.h"
#include "download/download_item.h"
#include "download/download_manager.h"
#include "download/response_item.h"
#include "indexation/file.h"
#include "utils/exceptions.h"
#include "utils/logger.h"
#include "utils/nlohmann/json-schema.hpp"
#include "utils/nlohmann/json.hpp"
#include "utils/utils.h"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#define API_TYPE_TXT "text"
#define API_TYPE_IMG "image"

#define API_LOADER_SCHEMA_NAME "api_schema.json"

using nlohmann::json;
using std::list;
using std::make_shared;
using std::optional;
using std::pair;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::vector;

/**
 * API class, used to load API settings and use its tags
 *
 * api_type_string() returns a string to describe what format this API retrieves
 * (image, text...)
 *
 * query_and_parse(json params, download_manager dl) takes the given parameters
 * and the download manager to load into a list of File the response of the API
 *
 * get_request_parameters() retrieves a list of api_parameter_request that
 * describe what can be used to configure query_and_parse
 *
 */
class api_loader {

public:
  /** Enum used to define the type of the main API value */
  enum api_type { TEXT, IMAGE };

  string get_name() const;

private:
  /** Base URL of the API to call queries on */
  string _url;

  /** HTTP method to execute queries */
  string _method;

  /** Name of the API */
  string _name;

  /** API type */
  api_type _api_type;

  /** API main response value */
  string _response_main_item;

  /** Path to an array of objects in resulting JSON */
  vector<string> _path_to_results;

  /** List of parameters used in requests */
  vector<shared_ptr<api_parameter_request>> _requests;

  /** List of responses from a query */
  vector<shared_ptr<api_parameter_response>> _responses;

  /** Number of characters to truncate at the start of the response to parse it
   */
  optional<int> _truncate_before;

  /** Number of characters to truncate at the end of the response to parse it */
  optional<int> _truncate_after;

  /**
   * Takes a JSON describing the API, checks against the defined schema and
   * insert it in the object
   * @param j JSON of the API to load into the object
   */
  void init(const json &j);

  /**
   * Gets the full path of the API schema
   * @return string Absolute path of the file to use
   */
  string get_api_schema_full_path() const;

  /**
   * Manage the retrieval of the main value of the API (text, image...)
   * @param result_to_manage JSON object corresponding to the object
   * @param param definition of the given result
   * @param file_to_save_to file to save results to
   * @param dl download manager
   */
  void manage_main_value(const response_item &result_to_manage,
                         shared_ptr<File> file_to_save_to,
                         const download_manager &dl) const;

  /**
   * Manages the saving of a text by the API.
   * Follows a call to manage_main_value
   * @param api_result given value by the API
   * @param file_to_save_to file to save results to
   */
  void manage_text(const string &api_result,
                   shared_ptr<File> file_to_save_to) const;

  /**
   * Manages the retrieval of a media from the API.
   * Follows a call to manage_main_value
   * @param path_api given value by the API
   * @param param definition of the given value
   * @param file_to_save_to file to save results to
   * @param dl download manager
   */
  void manage_media(const string &path_api,
                    const shared_ptr<const api_parameter_response> param,
                    shared_ptr<File> file_to_save_to,
                    const download_manager &dl) const;

  void manage_parsed_responses(
      const vector<response_item> &parsed_responses,
      list<shared_ptr<File>> &files,
      vector<pair<shared_ptr<const api_parameter_request>, string>>
          &relevant_parameters,
      const download_manager &dl) const;

public:
  /**
   * Creates the object from a JSON describing the API
   * @param j JSON of the API
   */
  api_loader(const json &j);

  /**
   * Creates the object from a JSON describing the API at schema_path
   * @param schema_path path to the JSON file (pass it as string, not a C
   * char string)
   */
  api_loader(const string &schema_path);

  /**
   * Creates the object from a JSON describing the API at schema_path and loads
   * additionnal default values from default_values_path
   * @param schema_path path to the JSON file (pass it as string, not a C
   * char string)
   * @param default_values_path path to the JSON file
   */
  api_loader(const string &schema_path, const string &default_values_path);

  /**
   * Serializes the API informations to a JSON
   * @return JSON of the API
   */
  json serialize() const;

  /**
   * @param key key of the default value
   * @param val default value to set
   */
  void set_parameter_request_default_value(const string &key,
                                           const string &val);

  /**
   * Returns a string to print the object
   */
  string to_string() const;

  /** Gets the official string of the API type used in JSON */
  string get_api_type_string() const;

  /** Gets the enum value representing the API type */
  api_loader::api_type get_api_type() const;

  /**
   * Queries and parses
   * @param params map parameters to insert in place of the default ones
   * @param dl download manager
   * @return list<shared_ptr<File>> Parsed response suitable for the
   * Harvester
   */
  list<shared_ptr<File>>
  query_and_parse(const unordered_map<string, string> &params,
                  const download_manager &dl) const;

  /**
   * Gets a request parameter named as
   * @param name API name to search for
   * @return optional<api_parameter_request *> optional containing maybe
   * the parameter pointer
   */
  optional<shared_ptr<api_parameter_request>>
  find_request_parameter(const string &name) const;

  /**
   * Gets a response parameter named as
   * @param name API name to search for
   * @return optional<api_parameter_request *> optional containing maybe
   * the parameter pointer
   */
  optional<shared_ptr<api_parameter_response>>
  find_response_parameter(const string &name) const;

  /**
   * Gets vector of available request parameters
   */
  const vector<shared_ptr<api_parameter_request>> &
  get_request_parameters() const;

  /**
   * Gets vector of available response parameters
   */
  const vector<shared_ptr<api_parameter_response>> &
  get_response_parameters() const;
};

#endif
