#include "download/api_parameter.h"

unordered_map<api_parameter_base::value_type, string>
    api_parameter_base::_value_type_strings{
        {api_parameter_base::value_type::IMAGE_LINK, "image_link"},
        {api_parameter_base::value_type::INT, "int"},
        {api_parameter_base::value_type::STRING, "string"},
        {api_parameter_base::value_type::DATE, "date"}};

unordered_map<string, string> api_parameter_base::_default_descriptions{
    {"_api_id", "Unique ID of an element for this API"},
    {"_page", "Page number to use or start at for queries"},
    {"_page_count", "Number of elements per page"},
    {"query", "Search query"},
    {"language_iso", "Language code in ISO form"}};

api_parameter_base::api_parameter_base(const nlohmann::json &json) {

  this->_api_name = json.at("api_name").get<string>();
  if (json.contains("name")) {
    this->_name = json.at("name").get<string>();
  } else {
    this->_name = this->_api_name;
  }

  this->_relevant = json.at("relevant").get<bool>();

  this->_description = std::nullopt;
  if (json.contains("description")) {
    this->_description = json.at("description");
  } else if (_default_descriptions.find(_name) != _default_descriptions.end()) {
    _description = _default_descriptions.at(_name);
  }

  bool value_type_found = false;
  for (const auto &el : _value_type_strings) {
    if (el.second == json.at("type").get<string>()) {
      _value_type = el.first;
      value_type_found = true;
    }
  }
  if (!value_type_found)
    throw std::runtime_error("Unrecognized type for parameter.");

  if (_value_type == value_type::DATE)
    _date_format = json.at("date_format");
}

string api_parameter_base::to_string() const {
  std::stringstream out;
  out << "------- api_name: " << this->_api_name << " -------" << std::endl;
  out << "name: " << this->_name << std::endl;
  out << "desc: " << this->_description.value_or("none") << std::endl;
  out << "value_type: " << this->get_value_type_string() << std::endl;
  out << "date_format: " << _date_format.value_or("none") << std::endl;
  out << "relevant: " << this->_relevant;
  return out.str();
}

nlohmann::json api_parameter_base::serialize() const {
  nlohmann::json j;
  j["api_name"] = this->_api_name;
  j["name"] = this->_name;
  j["value_type"] = this->get_value_type_string();
  j["date_format"] = nullptr;
  if (_date_format)
    j["date_format"] = _date_format.value();
  j["relevant"] = this->_relevant;
  j["description"] = nullptr;
  if (_description)
    j["description"] = _description.value();
  return j;
}

string api_parameter_base::get_value_type_string() const {
  if (_value_type_strings.find(_value_type) != _value_type_strings.end())
    return _value_type_strings.at(_value_type);
  else
    throw std::runtime_error("Unsupported parameter type to string.");
}

api_parameter_base::value_type api_parameter_base::get_value_type() const {
  return this->_value_type;
}

string
api_parameter_base::json_value_to_string(const nlohmann::json &val) const {
  string result;
  try {
    if (this->_value_type == value_type::INT) {
      result = std::to_string(val.get<int>());
    } else if (this->_value_type == value_type::STRING) {
      result = val.get<string>();
    } else if (this->_value_type == value_type::IMAGE_LINK) {
      result = val.get<string>();
    } else if (_value_type == value_type::DATE) {
      struct tm time_parsed;
      strptime(val.get<string>().c_str(), _date_format.value().c_str(),
               &time_parsed);
      char buffer[70];
      strftime(buffer, 70, API_PARAMETER_DATE_FORMAT, &time_parsed);
      result = buffer;
    }
  } catch (const nlohmann::json::exception &e) {
    logger::error("Error parsing a JSON value to a string");
    logger::debug(this->_name + ":" + val.dump());
    throw e;
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
  this->_position = json.at("position").get<string>();
  this->_required = json.at("required").get<bool>();

  this->_is_public = true;
  if (json.contains("is_public")) {
    this->_is_public = json.at("is_public").get<bool>();
  }

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
    this->set_default_value(json.at("default_value").get<string>());
  } else {
    this->_default_value = std::nullopt;
  }
}

string api_parameter_request::to_string() const {
  string base = api_parameter_base::to_string();
  std::stringstream out;
  out << base << std::endl;
  out << "position: " << this->_position << std::endl;
  out << "required: " << this->_required << std::endl;
  out << "public: " << this->_is_public << std::endl;
  out << "default_value: " << this->_default_value.value_or("no default value")
      << std::endl;

  out << "values: ";
  for (const string &el : this->_values)
    out << el << ", ";
  return out.str();
}

nlohmann::json api_parameter_request::serialize() const {
  nlohmann::json j = api_parameter_base::serialize();
  j["position"] = this->_position;
  j["required"] = this->_required;
  j["is_public"] = this->_is_public;
  j["default_value"] = nullptr;
  if (this->_default_value.has_value())
    j["default_value"] = this->_default_value.value();
  j["values"] = this->_values;
  return j;
}

bool api_parameter_request::is_value_valid(const string &val) const {

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

bool api_parameter_request::is_value_correctly_typed(const string &val) const {

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
  } else if (_value_type == value_type::DATE) {
    struct tm t;
    if (strptime(val.c_str(), _date_format.value().c_str(), &t))
      return true;
    else
      return false;
  }

  return true;
}

void api_parameter_request::set_default_value(const string &val) {
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
      string param_name = el.at("value").get<string>();
      this->_string_prepends.push_back(
          std::make_pair(param_name, el.at("is_parameter_name").get<bool>()));
    }
  }

  if (json.contains("string_appends")) {
    for (const auto &el : json.at("string_appends")) {
      string param_name = el.at("value").get<string>();
      this->_string_appends.push_back(
          std::make_pair(param_name, el.at("is_parameter_name").get<bool>()));
    }
  }
}

string api_parameter_response::to_string() const {
  string base = api_parameter_base::to_string();
  std::stringstream out;
  out << base << std::endl;
  out << "name: " << this->_name << std::endl;
  out << "string prepend: [" << std::endl;
  for (std::pair<string, bool> el : this->_string_prepends) {
    out << "value: \"" << el.first << "\", is_parameter: " << el.second
        << std::endl;
  }
  out << "]" << std::endl;
  out << "string append: [" << std::endl;
  for (std::pair<string, bool> el : this->_string_appends) {
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
