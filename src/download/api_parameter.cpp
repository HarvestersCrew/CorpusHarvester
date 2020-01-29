#include "download/api_parameter.h"

api_parameter_base::api_parameter_base(const nlohmann::json &json) {

  this->_api_name = json.at("api_name").get<std::string>();

  if (json.at("type").get<std::string>() == "string") {
    this->_value_type = value_type::STRING;
  } else if (json.at("type").get<std::string>() == "int") {
    this->_value_type = value_type::INT;
  } else if (json.at("type").get<std::string>() == "int64") {
    this->_value_type = value_type::INT64;
  } else {
    throw std::runtime_error("Unrecognized type for parameter.");
  }
}

std::string api_parameter_base::to_string() const {
  std::stringstream out;
  out << "------- api_name: " << this->_api_name << " -------" << std::endl;
  out << "value_type: " << this->_value_type;
  return out.str();
}

api_parameter_request::api_parameter_request(const nlohmann::json &json)
    : api_parameter_base(json) {
  this->_position = json.at("position").get<std::string>();
  this->_required = json.at("required").get<bool>();
  this->_relevant = json.at("relevant").get<bool>();

  if (json.contains("name"))
    this->_name = json.at("name").get<std::string>();
  else if (this->_relevant)
    throw std::runtime_error("A relevant parameter needs a name");
  else
    this->_name = std::nullopt;

  if (json.contains("values")) {
    for (const auto &el : json.at("values")) {
      this->_values.push_back(el.get<std::string>());
    }
  }

  if (json.contains("default_value")) {
    if (!this->is_value_valid(json.at("default_value").get<std::string>()))
      throw std::runtime_error("Default value incompatible");
    this->_default_value = json.at("default_value").get<std::string>();

  } else
    this->_default_value = std::nullopt;
}

std::string api_parameter_request::to_string() const {
  std::string base = api_parameter_base::to_string();
  std::stringstream out;
  out << base << std::endl;
  out << "position: " << this->_position << std::endl;
  out << "required: " << this->_required << std::endl;
  out << "relevant: " << this->_relevant << std::endl;
  out << "name: " << this->_name.value_or("no value") << std::endl;
  out << "default_value: " << this->_default_value.value_or("no default value")
      << std::endl;

  out << "values: ";
  for (const std::string &el : this->_values)
    out << el << ", ";
  return out.str();
}

bool api_parameter_request::is_value_valid(const std::string &val) const {
  if (this->_values.size() != 0 &&
      std::find(this->_values.begin(), this->_values.end(), val) ==
          this->_values.end())
    return false;

  if (this->_value_type == value_type::INT64) {
    try {
      std::stoull(val);
    } catch (const std::invalid_argument &e) {
      return false;
    } catch (const std::out_of_range &e) {
      return false;
    }

  } else if (this->_value_type == value_type::INT) {
    try {
      std::stoi(val);
    } catch (const std::invalid_argument &e) {
      return false;
    } catch (const std::out_of_range &e) {
      return false;
    }

  } else if (this->_value_type == value_type::STRING) {
  }
  return true;
}

api_parameter_response::api_parameter_response(const nlohmann::json &json)
    : api_parameter_base(json) {
  this->_name = json.at("name").get<std::string>();
}

std::string api_parameter_response::to_string() const {
  std::string base = api_parameter_base::to_string();
  std::stringstream out;
  out << base << std::endl;
  out << "name: " << this->_name;
  return out.str();
}
