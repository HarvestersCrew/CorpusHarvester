#include "download/api_parameter.h"

api_parameter_base::api_parameter_base(const nlohmann::json &json) {

  this->_api_name = json.at("api_name").get<std::string>();
  if (json.contains("name")) {
    this->_name = json.at("name").get<std::string>();
  } else {
    this->_name = this->_api_name;
  }

  this->_relevant = json.at("relevant").get<bool>();

  if (json.at("type").get<std::string>() == API_PARAMETER_STRING) {
    this->_value_type = value_type::STRING;
  } else if (json.at("type").get<std::string>() == API_PARAMETER_INT) {
    this->_value_type = value_type::INT;
  } else if (json.at("type").get<std::string>() == API_PARAMETER_IMAGE_LINK) {
    this->_value_type = value_type::IMAGE_LINK;
  } else {
    throw std::runtime_error("Unrecognized type for parameter.");
  }
}

std::string api_parameter_base::to_string() const {
  std::stringstream out;
  out << "------- api_name: " << this->_api_name << " -------" << std::endl;
  out << "name: " << this->_name << std::endl;
  out << "value_type: " << this->get_value_type_string() << std::endl;
  out << "relevant: " << this->_relevant;
  return out.str();
}

nlohmann::json api_parameter_base::serialize() const {
  nlohmann::json j;
  j["api_name"] = this->_api_name;
  j["name"] = this->_name;
  j["value_type"] = this->get_value_type_string();
  j["relevant"] = this->_relevant;
  return j;
}

std::string api_parameter_base::get_value_type_string() const {
  switch (this->_value_type) {
  case api_parameter_base::value_type::INT:
    return API_PARAMETER_INT;
    break;
  case api_parameter_base::value_type::STRING:
    return API_PARAMETER_STRING;
    break;
  case api_parameter_base::value_type::IMAGE_LINK:
    return API_PARAMETER_IMAGE_LINK;
    break;
  default:
    throw std::runtime_error("Unsupported parameter type to string.");
    break;
  }
}

api_parameter_base::value_type api_parameter_base::get_value_type() const {
  return this->_value_type;
}

std::string
api_parameter_base::json_value_to_string(const nlohmann::json &val) const {
  std::string result;
  if (this->_value_type == value_type::INT) {
    result = std::to_string(val.get<int>());
  } else if (this->_value_type == value_type::STRING) {
    result = val.get<std::string>();
  } else if (this->_value_type == value_type::IMAGE_LINK) {
    result = val.get<std::string>();
  }
  return result;
}

string api_parameter_base::get_name() const { return this->_name; }

string
api_parameter_base::get_sql_cast_prepared_string(const string &param) const {
  string res;

  switch (this->get_value_type()) {
  case api_parameter_base::value_type::INT:
    res += "CAST(" + param + " as INT)";
    break;
  default:
    res += param;
    break;
  }

  return res;
}

api_parameter_request::api_parameter_request(const nlohmann::json &json)
    : api_parameter_base(json) {
  this->_position = json.at("position").get<std::string>();
  this->_required = json.at("required").get<bool>();

  if (json.contains("values")) {
    for (const auto &el : json.at("values")) {
      string val = el.get<string>();
      if (this->is_value_correctly_typed(val)) {
        this->_values.push_back(val);
      } else {
        throw api_parameter_incompatible_value(this->get_value_type_string(),
                                               "values");
      }
    }
  }

  if (json.contains("default_value")) {
    this->set_default_value(json.at("default_value").get<std::string>());
  } else {
    this->_default_value = std::nullopt;
  }
}

std::string api_parameter_request::to_string() const {
  std::string base = api_parameter_base::to_string();
  std::stringstream out;
  out << base << std::endl;
  out << "position: " << this->_position << std::endl;
  out << "required: " << this->_required << std::endl;
  out << "default_value: " << this->_default_value.value_or("no default value")
      << std::endl;

  out << "values: ";
  for (const std::string &el : this->_values)
    out << el << ", ";
  return out.str();
}

nlohmann::json api_parameter_request::serialize() const {
  nlohmann::json j = api_parameter_base::serialize();
  j["position"] = this->_position;
  j["required"] = this->_required;
  j["default_value"] = nullptr;
  if (this->_default_value.has_value())
    j["default_value"] = this->_default_value.value();
  j["values"] = this->_values;
  return j;
}

bool api_parameter_request::is_value_valid(const std::string &val) const {

  if (!this->is_value_correctly_typed(val)) {
    return false;
  }

  if (this->_values.size() != 0 &&
      std::find(this->_values.begin(), this->_values.end(), val) ==
          this->_values.end()) {
    return false;
  }

  return true;
}

bool api_parameter_request::is_value_correctly_typed(
    const std::string &val) const {

  if (this->_value_type == value_type::INT) {
    try {
      std::stoi(val);
    } catch (const std::invalid_argument &e) {
      return false;
    } catch (const std::out_of_range &e) {
      return false;
    }

  } else if (this->_value_type == value_type::STRING) {
  } else if (this->_value_type == value_type::IMAGE_LINK) {
  }

  return true;
}

void api_parameter_request::set_default_value(const std::string &val) {
  if (!this->is_value_valid(val))
    throw api_parameter_incompatible_value(this->get_value_type_string(),
                                           this->get_name());
  this->_default_value = val;
}

bool api_parameter_request::get_required() const { return this->_required; }

optional<string> api_parameter_request::get_default_value() const {
  return this->_default_value;
}

vector<string> api_parameter_request::get_values() const {
  return this->_values;
}

api_parameter_response::api_parameter_response(const nlohmann::json &json)
    : api_parameter_base(json) {

  if (json.contains("string_prepends")) {
    for (const auto &el : json.at("string_prepends")) {
      std::string param_name = el.at("value").get<std::string>();
      this->_string_prepends.push_back(
          std::make_pair(param_name, el.at("is_parameter_name").get<bool>()));
    }
  }

  if (json.contains("string_appends")) {
    for (const auto &el : json.at("string_appends")) {
      std::string param_name = el.at("value").get<std::string>();
      this->_string_appends.push_back(
          std::make_pair(param_name, el.at("is_parameter_name").get<bool>()));
    }
  }
}

std::string api_parameter_response::to_string() const {
  std::string base = api_parameter_base::to_string();
  std::stringstream out;
  out << base << std::endl;
  out << "name: " << this->_name << std::endl;
  out << "string prepend: [" << std::endl;
  for (std::pair<std::string, bool> el : this->_string_prepends) {
    out << "value: \"" << el.first << "\", is_parameter: " << el.second
        << std::endl;
  }
  out << "]" << std::endl;
  out << "string append: [" << std::endl;
  for (std::pair<std::string, bool> el : this->_string_appends) {
    out << "value: \"" << el.first << "\", is_parameter: " << el.second
        << std::endl;
  }
  out << "]";

  return out.str();
}

nlohmann::json api_parameter_response::serialize() const {
  nlohmann::json j = api_parameter_base::serialize();
  j["string_prepends"] = this->_string_prepends;
  j["string_appends"] = this->_string_appends;
  return j;
}
