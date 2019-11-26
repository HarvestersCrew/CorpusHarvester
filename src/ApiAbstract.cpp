#include <ApiAbstract.h>

const api_settings &ApiAbstract::get_required_settings() const {
  return this->_required_settings;
}

std::string ApiAbstract::get_required_settings_string() const {
  std::string str = "";
  for (auto const &el : this->_required_settings) {
    str += el.first + ": " + el.second + "\n";
  }
  return str;
}