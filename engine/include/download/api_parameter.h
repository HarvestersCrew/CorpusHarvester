#ifndef API_PARAMETER_H
#define API_PARAMETER_H

#include "utils/nlohmann/json.hpp"
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#define API_PARAMETER_INT "int"
#define API_PARAMETER_INT64 "int64"
#define API_PARAMETER_STRING "string"
#define API_PARAMETER_IMAGE_LINK "image_link"

using std::optional;
using std::string;
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

public:
  /** Value describing the type of the parameter */
  enum value_type { STRING, INT, INT64, IMAGE_LINK };

  /**
   * String of current state
   * @return std::string state of the parameter
   */
  virtual std::string to_string() const;

  /**
   * Gets the string associated to the parameter type
   */
  virtual std::string get_type_string() const;

  /**
   * Converts the given JSON value to a string based on its type
   * @param val JSON value to convert
   * @return std::string string associated to the value
   */
  virtual std::string json_value_to_string(const nlohmann::json &val) const;

  virtual string get_name() const;

protected:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_base(const nlohmann::json &json);
  virtual ~api_parameter_base(){};

  /** Name of the parameter on the API side */
  std::string _api_name;
  /** Display name of the tag (DB, selector...) */
  std::string _name;
  /** Type of the parameter values */
  value_type _value_type;
  /** Whether the parameter is relevant to keep in the database */
  bool _relevant;
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

public:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_request(const nlohmann::json &json);

  /**
   * String of current state
   * @return std::string state of the parameter
   */
  virtual std::string to_string() const;

  /**
   * Checks if a given value as a string can be typed to the parameter type
   * @param val value to check
   * @return whether it can be typed as the parameter type
   */
  bool is_value_valid(const std::string &val) const;

  /**
   * Adds the given value as a default value
   * @param val value to set as default
   */
  void set_default_value(const std::string &val);

  bool get_required() const;
  optional<string> get_default_value() const;
  vector<string> get_values() const;

private:
  /** Position of the parameter in the HTTP request (body, header...) */
  std::string _position;
  /** Whether the parameter is required in a request */
  bool _required;
  /** Default value to use in requests */
  std::optional<std::string> _default_value;
  /** Enum of values valid to use */
  std::vector<std::string> _values;
};

/**
 * Class extending the base parameter to specify a response parameter
 */
class api_parameter_response : public api_parameter_base {
  friend class api_loader;
  friend class download_item;
  friend class response_item;

private:
  /** Values to prepend to responses, string is the value and bool is if it
   * is a parameter's name or a direct string */
  std::vector<std::pair<std::string, bool>> _string_prepends;

  /** Values to append to responses, string is the value and bool is if it
   * is a parameter's name or a direct string */
  std::vector<std::pair<std::string, bool>> _string_appends;

public:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_response(const nlohmann::json &json);

  /**
   * String of current state
   * @return std::string state of the parameter
   */
  virtual std::string to_string() const;
};

#endif
