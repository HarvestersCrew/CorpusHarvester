#ifndef API_REQUEST_BUILDER_H
#define API_REQUEST_BUILDER_H

#include "download/api_factory.h"
#include "download/api_loader.h"
#include "indexation/file.h"
#include "utils/exceptions.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using std::list;
using std::make_pair;
using std::pair;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

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
   * 2nd map: parameter name, value to use
   */
  vector<pair<shared_ptr<api_loader>, unordered_map<string, string>>> _requests;

  /**
   * Search for these types if no requests are precised or if specified
   */
  unordered_set<api_loader::api_type> _types;

  /**
   * Protected constructor to avoid building this abstract class
   */
  ApiRequestBuilder();

  /**
   * Adds a new request
   * @param api_name Name of the API to use
   * @param params List of parameters to use for this request
   * @throw api_factory_name_not_found if the given API is not found
   * @throw api_no_setting_exception if a parameter isn't found
   */
  virtual void add_request(const string &api_name,
                           const unordered_map<string, string> &params);

  /**
   * Fetches the list of files from whatever source we want
   * @param number number of elements to retrieve, 0 means value not set and
   * outcome depends on the implementation
   */
  virtual list<shared_ptr<File>> build(unsigned int number) const = 0;

public:
  /**
   * Clears filled types
   */
  virtual void clear_types();
  /**
   * Add the given type
   * @param t Type of API to load from
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
   * Get requests
   */
  virtual const vector<
      pair<shared_ptr<api_loader>, unordered_map<string, string>>> &
  get_requests() const;
};

#endif
