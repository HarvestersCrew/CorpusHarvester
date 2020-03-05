#include "download/response_item.h"

response_item::response_item(
    const nlohmann::json &single_entry,
    const std::vector<api_parameter_response *> &params) {
  this->parse(single_entry, params);
}

std::string response_item::to_string() const {
  std::stringstream out;
  for (auto el : this->_parameters) {
    out << el.first->_api_name << ": \"" << el.second << "\"" << std::endl;
  }
  return out.str();
}

void response_item::parse(const nlohmann::json &single_entry,
                          const std::vector<api_parameter_response *> &params) {
  for (api_parameter_response *el : params) {
    std::string value =
        el->json_value_to_string(single_entry.at(el->_api_name));
    this->_parameters.insert_or_assign(el, value);
  }
  for (auto &el : this->_parameters) {
    if (el.first->_string_prepends.size() != 0) {
      el.second = this->string_prepend(el.first, el.second);
    }
  }
}

const std::map<const api_parameter_response *, std::string> &
response_item::get_parameters() const {
  return this->_parameters;
}

std::pair<const api_parameter_response *, std::string>
response_item::get_named_parameter(const std::string &name) const {
  for (const std::pair<const api_parameter_response *, std::string> &el :
       this->_parameters) {
    if (el.first->_name == name) {
      return std::make_pair(el.first, el.second);
    }
  }
  throw api_missing_settings_exception();
}

std::string response_item::string_prepend(const api_parameter_response *param,
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
  return value;
}
