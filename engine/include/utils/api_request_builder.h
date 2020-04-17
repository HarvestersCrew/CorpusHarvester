#ifndef API_REQUEST_BUILDER_H
#define API_REQUEST_BUILDER_H

#include "download/api_factory.h"
#include "download/api_loader.h"
#include "indexation/file.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using nlohmann::json;
using std::endl;
using std::list;
using std::make_pair;
using std::pair;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::unordered_set;
using std::vector;

typedef pair<shared_ptr<api_loader>,
             unordered_map<string, pair<string, string>>>
    api_builder_request;

/**
 * Interface to make requests against APIs
 */
class ApiRequestBuilder {

  friend void test_types();
  friend void test_requests();

protected:
  /**
   * List of API-bound requests
   * 1st map: API name, map of parameters to use
   * 2nd map: parameter name, value and operator to use
   */
  vector<api_builder_request> _requests;

  /**
   * Search for these types if no requests are precised or if specified
   */
  unordered_set<api_loader::api_type> _types;

  /**
   * Protected constructor to avoid building this abstract class
   */
  ApiRequestBuilder();

public:
  /**
   * Serializes the request
   * @return JSON to be save wherever you want
   */
  virtual json serialize() const;

  /**
   * Deserializes to the request
   * @param j JSON to load from
   * @throw api_factory_name_not_found if the given API is not found
   * @throw api_no_setting_exception if the parameter isn't found
   * @throw api_builder_request_not_found if the request ID isn't found
   * @throw api_builder_incompatible_operator If an operator is incompatible
   * @throw api_request_builder_invalid_type the given type is invalid
   */
  virtual void deserialize(const json &j);

  /**
   * Fetches the list of files from whatever source we want
   * @param number number of elements to retrieve, 0 means value not set and
   * outcome depends on the implementation
   * DatabaseBuilder -> 0 = everything that matches
   * DownloadBuilder -> 0 = single pass over the sources
   */
  virtual list<shared_ptr<File>> build(unsigned int number) const = 0;

  /**
   * Adds a new request with only the API name
   * @param api_name Name of the API to use
   * @return index of the newly inserted request
   * @throw api_factory_name_not_found if the given API is not found
   */
  virtual long unsigned int add_request(const string &api_name);

  /**
   * Inserts or replace a parameter value and operator in the request defined by
   * the ID
   * @param request_id Request ID
   * @param param_name Name of the API parameter
   * @param param_value Value to set
   * @param op Operator to use to search
   * @throw api_no_setting_exception if the parameter isn't found
   * @throw api_builder_request_not_found if the request ID isn't found
   * @throw api_builder_incompatible_operator If an operator is incompatible
   */
  virtual void add_request_parameter(long unsigned int request_id,
                                     const string &param_name,
                                     const string &param_value,
                                     const string &op);

  virtual void clear_all();

  /**
   * Clears filled types
   */
  virtual void clear_types();
  /**
   * Removes a type from the list
   * @param t type to remove
   */
  virtual void remove_type(const api_loader::api_type t);
  /**
   * Add the given type
   * @param t Type of API to load from
   * @throw api_request_builder_invalid_type the given type is invalid
   */
  virtual void add_type(const api_loader::api_type t);
  /**
   * Get filled types
   */
  virtual const unordered_set<api_loader::api_type> &get_types() const;

  /**
   * Clears filled requests
   */
  virtual void clear_requests();

  /**
   * Retrieves the requests without the operators as only the = is supported
   */
  virtual vector<pair<shared_ptr<api_loader>, unordered_map<string, string>>>
  get_no_op_requests(const vector<api_builder_request> &requests) const;

  /**
   * Get requests
   */
  virtual const vector<api_builder_request> &get_requests() const;

  virtual string to_string() const;

  /**
   * Generates a list of requests prepared to be used by the builders based on
   * the interface parameters
   */
  virtual vector<api_builder_request> get_usable_requests() const;
};

#endif
