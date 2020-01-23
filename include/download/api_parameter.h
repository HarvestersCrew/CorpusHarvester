#ifndef API_PARAMETER_H
#define API_PARAMETER_H

#include "utils/nlohmann/json.hpp"
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class api_parameter_base {

  friend class api_loader;

public:
  virtual std::string to_string() const;
  enum value_type { STRING, INT, INT64 };

protected:
  api_parameter_base(const nlohmann::json &json);
  virtual ~api_parameter_base(){};
  std::string _api_name;
  value_type _value_type;
};

class api_parameter_request : public api_parameter_base {
public:
  api_parameter_request(const nlohmann::json &json);
  virtual std::string to_string() const;

private:
  std::string _position;
  bool _required;
  bool _relevant;
  std::optional<std::string> _name;
  std::optional<std::string> _default_value;
  std::vector<std::string> _values;
};

class api_parameter_response : public api_parameter_base {
public:
  api_parameter_response(const nlohmann::json &json);
  virtual std::string to_string() const;

private:
  std::string _name;
};

#endif
