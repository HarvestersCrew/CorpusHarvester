#ifndef RESPONSE_ITEM_H
#define RESPONSE_ITEM_H

#include "download/api_parameter.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

/** Class used to store a single entry of a response */
class response_item {
private:
  /** Map of response parameters as keys and received values as values */
  std::map<const api_parameter_response *, std::string> _parameters;

public:
  /** Constructor simply calling parse method
   * @see response_item::parse
   */
  response_item(const nlohmann::json &single_entry,
                const std::vector<api_parameter_response *> &params);

  std::string to_string() const;

  /**
   * Parses a JSON single entry and a list of supposed received params to the
   * response item
   * @param single_entry JSON of an entry
   * @param params Response params describing the API
   */
  void parse(const nlohmann::json &single_entry,
             const std::vector<api_parameter_response *> &params);

  const std::map<const api_parameter_response *, std::string> &
  get_parameters() const;

  /**
   * Gets a parameter and its value with its name
   * @param name name of the parameter
   * @return pair of the parameter and its value
   */
  std::pair<const api_parameter_response *, std::string>
  get_named_parameter(const std::string &name) const;

  /**
   * Returns the prepended string designed by the API
   * @param param parameter which the value depend
   * @param value string to prepend
   * @return std::string value prepended
   */
  std::string string_prepend(const api_parameter_response *param,
                             std::string value) const;
};

#endif
