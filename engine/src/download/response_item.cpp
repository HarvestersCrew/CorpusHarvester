#include "download/response_item.h"

response_item::response_item(
    const nlohmann::json &single_entry,
    const std::vector<shared_ptr<api_parameter_response>> &params) {
  this->parse(single_entry, params);
}

std::string response_item::to_string() const {
  std::stringstream out;
  for (auto el : this->_parameters) {
    out << el.first->_api_name << ": \"" << el.second << "\"" << std::endl;
  }
  return out.str();
}

void response_item::parse(
    const nlohmann::json &single_entry,
    const std::vector<shared_ptr<api_parameter_response>> &params) {
  for (const shared_ptr<const api_parameter_response> el : params) {
    std::string value =
        el->json_value_to_string(single_entry.at(el->_api_name));
    this->_parameters.insert_or_assign(el, value);
  }
  for (auto &el : this->_parameters) {
    if (el.first->_string_prepends.size() != 0 ||
        el.first->_string_appends.size() != 0) {
      el.second = this->string_prepend_append(el.first, el.second);
    }
  }
}

const std::map<shared_ptr<const api_parameter_response>, std::string> &
response_item::get_parameters() const {
  return this->_parameters;
}

std::pair<shared_ptr<const api_parameter_response>, std::string>
response_item::get_named_parameter(const std::string &name) const {
  for (const std::pair<shared_ptr<const api_parameter_response>, std::string>
           &el : this->_parameters) {
    if (el.first->_name == name) {
      return std::make_pair(el.first, el.second);
    }
  }
  throw api_missing_settings_exception(name);
}

std::string response_item::string_prepend_append(
    const shared_ptr<const api_parameter_response> param,
    std::string value) const {

  for (int i = param->_string_prepends.size() - 1; i >= 0; --i) {
    const std::pair<std::string, bool> &el = param->_string_prepends.at(i);
    std::string prep("");
    if (el.second) {
      prep = this->get_named_parameter(el.first).second;
    } else {
      prep = el.first;
    }
    value = prep + value;
  }

  for (long unsigned int i = 0; i < param->_string_appends.size(); ++i) {
    const std::pair<std::string, bool> &el = param->_string_appends.at(i);
    std::string prep("");
    if (el.second) {
      prep = this->get_named_parameter(el.first).second;
    } else {
      prep = el.first;
    }
    value = value + prep;
  }

  return value;
}
