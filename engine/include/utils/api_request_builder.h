#ifndef API_REQUEST_BUILDER_H
#define API_REQUEST_BUILDER_H

#include "download/api_factory.h"
#include "indexation/file.h"
#include "utils/exceptions.h"
#include <download/api_loader.h>
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

class ApiRequestBuilder {

  friend void test_types();
  friend void test_requests();

protected:
  /**
   * List of API-bound requests
   * 1st map: API name, map of parameters to use
   * 2nd map: parameter name, value to use
   */
  vector<pair<string, unordered_map<string, string>>> _requests;

  /**
   * Search for these types if no requests are precised or if specified
   */
  unordered_set<api_loader::api_type> _types;

  /**
   * Protected constructor to avoid building this abstract class
   */
  ApiRequestBuilder();

  /**
   * Fetches the list of files from whatever source we want
   */
  virtual list<shared_ptr<File>> build() const;

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
   * Adds a new request
   * @param api_name Name of the API to use
   * @param params List of parameters to use for this request
   */
  virtual void add_request(const string &api_name,
                           const unordered_map<string, string> &params);

  /**
   * Get requests
   */
  virtual const vector<pair<string, unordered_map<string, string>>> &
  get_requests() const;
};

#endif
