#ifndef API_LOADER_H
#define API_LOADER_H

#include "download/api_parameter.h"
#include "download/download_manager.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json-schema.hpp"
#include "utils/nlohmann/json.hpp"
#include "utils/utils.h"
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

/**
 * API class, used to load API settings and use its tags
 */
class api_loader {
private:
  /** Base URL of the API to call queries on */
  std::string _url;

  /** HTTP method to execute queries */
  std::string _method;

  /** Name of the API */
  std::string _name;

  /** Path to an array of objects in resulting JSON */
  std::vector<std::string> _path_to_results;

  /** List of parameters used in requests */
  std::vector<api_parameter_request *> _requests;

  /** List of responses from a query */
  std::vector<api_parameter_response *> _responses;

  /**
   * Takes a JSON describing the API, checks against the defined schema and
   * insert it in the object
   * @param j JSON of the API to load into the object
   */
  void init(const nlohmann::json &j);

public:
  /**
   * Creates the object from a JSON describing the API
   * @param j JSON of the API
   */
  api_loader(const nlohmann::json &j);

  /**
   * Creates the object from a JSON describing the API at path
   * @param path path to the JSON file (pass it as std::string, not a C char
   * string)
   */
  api_loader(const std::string &path);

  ~api_loader();

  /**
   * Returns a string to print the object
   */
  std::string to_string() const;

  /**
   * Executes a query with the given parameters with the given download_manager
   * object
   * @param params JSON of parameters to insert in place of the default ones
   * @param dl download manager
   * @return nlohmann::json response of the API
   */
  nlohmann::json query(const nlohmann::json &params,
                       const download_manager &dl) const;

  /**
   * Parse a response returned by the API from a file
   * @param path path to the JSON file
   * @param relevant relevant parameters to extract from query parameters
   * @return nlohmann::json parsed response
   */
  nlohmann::json parse(const std::string &path,
                       const nlohmann::json &relevant) const;

  /**
   * Parse a response by the API
   * @param result Response sent by the API
   * @return nlohmann::json Parsed response suitable for the Harvester
   */
  nlohmann::json parse(const nlohmann::json &result) const;

  /**
   * Queries and parses
   * @param params JSON of parameters to insert in place of the default ones
   * @param dl download manager
   * @return nlohmann::json Parsed response suitable for the Harvester
   */
  nlohmann::json query_and_parse(const nlohmann::json &params,
                                 const download_manager &dl) const;
};

#endif
