#ifndef API_PARAMETER_H
#define API_PARAMETER_H

#include "utils/exceptions.h"
#include "utils/logger.h"
#include "utils/nlohmann/json.hpp"
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <time.h>
#include <unordered_map>
#include <vector>

#define API_PARAMETER_DATETIME_FORMAT "%Y-%m-%d %T"
#define API_PARAMETER_DATE_FORMAT "%Y-%m-%d"

using std::optional;
using std::string;
using std::unordered_map;
using std::vector;

/**
 * Common class for request and response parameters
 *
 * get_type_string() gives the parameter type in string (int, string...)
 *
 * get_name() gives the name of the parameter to use when referencing it
 *
 */
class api_parameter_base {

  friend class api_loader;
  friend class download_item;
  friend class response_item;

  friend void test_api_parameter_base_construct();
  friend void test_api_paramter_base_values();

public:
  /** Value describing the type of the parameter */
  enum value_type { STRING, INT, IMAGE_LINK, DATETIME, DATE };

  static unordered_map<value_type, string> _value_type_strings;

  /**
   * String of current state
   * @return string state of the parameter
   */
  virtual string to_string() const;

  /**
   * JSON serialization of parameter
   */
  virtual nlohmann::json serialize() const;

  /**
   * Gets the string associated to the parameter type
   */
  virtual string get_value_type_string() const;

  /** Gets the parameter type */
  virtual api_parameter_base::value_type get_value_type() const;

  /**
   * Converts the given JSON value to a string based on its type
   * @param val JSON value to convert
   * @return string string associated to the value
   */
  virtual string json_value_to_string(const nlohmann::json &val) const;

  virtual string get_name() const;

  /**
   * Returns a prepared SQL string where the prepared value is cast to the
   * parameter type
   */
  virtual string get_sql_cast_prepared_string(const string &param) const;

protected:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_base(const nlohmann::json &json);
  virtual ~api_parameter_base(){};

  /** Name of the parameter on the API side */
  string _api_name;
  /** Display name of the tag (DB, selector...) */
  string _name;
  /** Type of the parameter values */
  value_type _value_type;
  /** Whether the parameter is relevant to keep in the database */
  bool _relevant;
  /** Description of the parameter, optional */
  optional<string> _description;
  /** C++ format to parse the date */
  optional<string> _date_format;

  /**
   * Is filled with default descriptions to be used
   */
  static unordered_map<string, string> _default_descriptions;
};

/**
 * Class extending the base parameter to specify a request parameter
 * @see api_parameter_base
 *
 * get_required() tells if the parameter is required by the API
 *
 * get_default_value() gives an optional with a string of the default value if
 * it is set
 *
 * get_values() gives a list of the only value allowed by this parameter (if
 * empty, all value of the valid type are accepted)
 *
 */
class api_parameter_request : public api_parameter_base {
  friend class api_loader;
  friend class download_item;
  friend class response_item;

  friend void test_api_parameter_request_construct();

public:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_request(const nlohmann::json &json);

  /**
   * String of current state
   * @return string state of the parameter
   */
  virtual string to_string() const;

  /**
   * JSON serialization of parameter
   */
  virtual nlohmann::json serialize() const;

  /**
   * Checks if a given value as a string can be typed to the parameter type and
   * is present in the enum of values (if it exists)
   * @param val value to check
   * @return whether the value is valid for the parameter
   */
  bool is_value_valid(const string &val) const;

  /**
   * Checks if a given value as a string can be typed to the parameter type
   * @param val value to check
   * @return whether the value is correctly typed
   */
  bool is_value_correctly_typed(const string &val) const;

  /**
   * Adds the given value as a default value
   * @param val value to set as default
   * @throw api_parameter_incompatible_value if value is incompatible to the
   * object
   */
  void set_default_value(const string &val);

  bool get_required() const;
  optional<string> get_default_value() const;
  vector<string> get_values() const;

private:
  /** Position of the parameter in the HTTP request (body, header...) */
  string _position;
  /** Whether the parameter is required in a request */
  bool _required;
  /** Default value to use in requests */
  std::optional<string> _default_value;
  /** Enum of values valid to use */
  std::vector<string> _values;
  /** Whether the parameter should be exposed to user, true by default */
  bool _is_public;
};

/**
 * Class extending the base parameter to specify a response parameter
 */
class api_parameter_response : public api_parameter_base {
  friend class api_loader;
  friend class download_item;
  friend class response_item;

  friend void test_api_parameter_response_construct();

private:
  /** Values to prepend to responses, string is the value and bool is if it
   * is a parameter's name or a direct string */
  std::vector<std::pair<string, bool>> _string_prepends;

  /** Values to append to responses, string is the value and bool is if it
   * is a parameter's name or a direct string */
  std::vector<std::pair<string, bool>> _string_appends;

public:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_response(const nlohmann::json &json);

  /**
   * String of current state
   * @return string state of the parameter
   */
  virtual string to_string() const;

  /**
   * JSON serialization of parameter
   */
  virtual nlohmann::json serialize() const;
};

#endif
