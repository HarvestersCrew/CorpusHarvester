#ifndef API_REQUEST_BUILDER_H
#define API_REQUEST_BUILDER_H

#include "utils/exceptions.h"
#include <download/api_loader.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::nullopt;
using std::optional;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

class ApiRequestBuilder {

  friend void test_types();

protected:
  /**
   * List of API-bound requests
   * 1st map: API name, map of parameters to use
   * 2nd map: parameter name, value to use (if empty optional, specify to use a
   * default value)
   */
  vector<unordered_map<string, unordered_map<string, optional<string>>>>
      _requests;

  /**
   * List of parameters to apply to every API
   */
  unordered_map<string, string> _global_params;

  /**
   * Search for these types if no requests are precised or if specified
   */
  unordered_set<api_loader::api_type> _types;

  /**
   * Protected constructor to avoid building this abstract class
   */
  ApiRequestBuilder();

  /**
   * Clears filled types
   */
  virtual void clear_types();
  /**
   * Add the given type
   */
  virtual void add_type(const api_loader::api_type t);
  /**
   * Get filled types
   */
  virtual const unordered_set<api_loader::api_type> &get_types() const;
};

#endif
