#include "utils/exceptions.h"

api_no_setting_exception::api_no_setting_exception(std::string key) {
  std::ostringstream os;
  os << "Given setting key not existing in API: " << key;
  this->_msg = os.str();
}

const char *api_no_setting_exception::what() const throw() {
  return this->_msg.c_str();
}

const char *api_missing_settings_exception::what() const throw() {
  return "Missing setting to call an API.";
}

const char *api_unrecognized_settings_exception::what() const throw() {
  return this->_msg.c_str();
}

const char *download_no_200_exception::what() const throw() {
  return this->_msg.c_str();
}
