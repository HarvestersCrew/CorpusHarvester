#include "download/api_parameter.h"

api_parameter_base::api_parameter_base(const nlohmann::json &json)
    : _api_name(json.at("api_name").get<std::string>()) {
  if (json.at("from").get<std::string>() == "request")
    this->_from = from_type::REQUEST;
  else if (json.at("from").get<std::string>() == "response")
    this->_from = from_type::RESPONSE;
  else
    throw std::runtime_error("From field in parameter unknown");

  if (json.contains("position"))
    this->_position.emplace(json.at("position").get<std::string>());
  if (json.contains("name"))
    this->_name.emplace(json.at("name").get<std::string>());
  if (json.contains("required"))
    this->_required.emplace(json.at("required").get<bool>());
  if (json.contains("relevant"))
    this->_relevant.emplace(json.at("relevant").get<bool>());
}

bool api_parameter_base::is_valid() const {
  if (this->_api_name == "")
    return false;
  if (this->_from == from_type::REQUEST) {
    if (!this->_position.has_value() || (this->_position.value() != "header" &&
                                         this->_position.value() != "body"))
      return false;
    if (!this->_required.has_value() || !this->_relevant.has_value())
      return false;
  } else {
    if (!this->_name.has_value() || this->_name.value() == "")
      return false;
  }
  return true;
}

api_parameter_base *api_parameter_factory::get(const nlohmann::json &j) {
  if (j.at("type").get<std::string>() == "int")
    return new api_parameter<int>(j);
  else if (j.at("type").get<std::string>() == "int64")
    return new api_parameter<int64_t>(j);
  else if (j.at("type").get<std::string>() == "string")
    return new api_parameter<std::string>(j);
  else
    throw std::runtime_error("Can't match parameter type.");
}

template <class T> std::string api_parameter<T>::to_string() const {
  std::stringstream out;
  out << "------- api_name: " << this->_api_name << " -------" << std::endl;
  out << "from: "
      << (this->_from == from_type::REQUEST ? "request" : "response")
      << std::endl;

  if (this->_position.has_value())
    out << "position: " << this->_position.value() << std::endl;
  else
    out << "no position" << std::endl;

  if (this->_name.has_value())
    out << "name: " << this->_name.value() << std::endl;
  else
    out << "no name" << std::endl;

  if (this->_required.has_value())
    out << "required: " << this->_required.value() << std::endl;
  else
    out << "no required" << std::endl;

  if (this->_relevant.has_value())
    out << "relevant: " << this->_relevant.value() << std::endl;
  else
    out << "no relevant" << std::endl;

  if (this->_default_value.has_value())
    out << "default_value: " << this->_default_value.value() << std::endl;
  else
    out << "no default_value" << std::endl;

  out << "values: [";
  for (const T &el : this->_values)
    out << el << ",";
  out << "]";
  return out.str();
}

template <class T>
api_parameter<T>::api_parameter(const nlohmann::json &json)
    : api_parameter_base::api_parameter_base(json) {
  if (json.contains("default_value"))
    this->_default_value.emplace(json.at("default_value").get<T>());
  else
    this->_default_value = std::nullopt;
  if (json.contains("values")) {
    for (auto &el : json.at("values")) {
      this->_values.push_back(el.get<T>());
    }
  }
}
