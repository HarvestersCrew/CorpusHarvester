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

/**
 * Common class for request and response parameters
 */
class api_parameter_base {

  friend class api_loader;

public:
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

  /** Value describing the type of the parameter */
  enum value_type { STRING, INT, INT64, IMAGE_LINK };

protected:
  /**
   * Loads parameters data from a JSON
   * @param json parameters to load
   */
  api_parameter_base(const nlohmann::json &json);
  virtual ~api_parameter_base(){};

  /** Name of the parameter on the API side */
  std::string _api_name;
  /** Type of the parameter values */
  value_type _value_type;
};

/**
 * Class extending the base parameter to specify a request parameter
 */
class api_parameter_request : public api_parameter_base {
  friend class api_loader;

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

private:
  /** Position of the parameter in the HTTP request (body, header...) */
  std::string _position;
  /** Whether the parameter is required in a request */
  bool _required;
  /** Whether the parameter is relevant to keep in the database */
  bool _relevant;
  /** If parameter is relevant, name to use as tag in the database */
  std::optional<std::string> _name;
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

private:
  /** Name to use as tag in the database */
  std::string _name;
};

#endif
