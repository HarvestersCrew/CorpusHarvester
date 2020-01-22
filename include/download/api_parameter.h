#ifndef API_PARAMETER_H
#define API_PARAMETER_H

#include "utils/nlohmann/json.hpp"
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class api_parameter_base {

public:
  virtual ~api_parameter_base(){};
  virtual std::string to_string() const = 0;
  enum from_type { REQUEST, RESPONSE };

protected:
  api_parameter_base(const nlohmann::json &json);
  std::string _api_name;
  from_type _from;
  std::optional<std::string> _position;
  std::optional<std::string> _name;
  std::optional<bool> _required;
  std::optional<bool> _relevant;
};

class api_parameter_factory {
private:
  api_parameter_factory() {}

public:
  static api_parameter_base *get(const nlohmann::json &j);
};

template <class T> class api_parameter : public api_parameter_base {

public:
  friend api_parameter_base *
  api_parameter_factory::get(const nlohmann::json &j);

  virtual ~api_parameter(){};
  virtual std::string to_string() const;

private:
  api_parameter(const nlohmann::json &json);
  api_parameter(const std::string &path);
  std::optional<T> _default_value;
  std::vector<T> _values;
};

#endif
