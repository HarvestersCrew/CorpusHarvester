#include "utils/exceptions.h"

ApiNoSettingCalledLikeThisException::ApiNoSettingCalledLikeThisException(
    std::string key) {
  std::ostringstream os;
  os << "Given setting key not existing in API: " << key;
  this->_msg = os.str();
}

const char *ApiNoSettingCalledLikeThisException::what() const throw() {
  return this->_msg.c_str();
}
