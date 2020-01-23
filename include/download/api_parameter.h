#ifndef API_PARAMETER_H
#define API_PARAMETER_H

#include "utils/nlohmann/json.hpp"
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class api_parameter_base {

public:
  api_parameter_base(const nlohmann::json &json);
  ~api_parameter_base(){};
  std::string to_string() const;

  enum from_type { REQUEST, RESPONSE };
  enum value_type { STRING, INT, INT64 };

private:
  std::string _api_name;
  from_type _from;
  value_type _value_type;
  std::optional<std::string> _position;
  std::optional<std::string> _name;
  std::optional<bool> _required;
  std::optional<bool> _relevant;
  std::optional<std::string> _default_value;
  std::vector<std::string> _values;
};

#endif
