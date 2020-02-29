#ifndef API_LOADER_H
#define API_LOADER_H

#include "download/api_parameter.h"
#include "download/download_manager.h"
#include "indexation/file.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json-schema.hpp"
#include "utils/nlohmann/json.hpp"
#include "utils/utils.h"
#include <fstream>
#include <iostream>
#include <list>
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
   * @param schema_path path to the JSON file (pass it as std::string, not a C
   * char string)
   */
  api_loader(const std::string &schema_path);

  ~api_loader();

  /**
   * Returns a string to print the object
   */
  std::string to_string() const;

  /**
   * Queries and parses
   * @param params JSON of parameters to insert in place of the default ones
   * @param dl download manager
   * @return std::list<File *> Parsed response suitable for the Harvester
   */
  std::list<File *> query_and_parse(const nlohmann::json &params,
                                    const download_manager &dl) const;
};

#endif
