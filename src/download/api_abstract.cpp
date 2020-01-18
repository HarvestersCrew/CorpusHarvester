#include "download/api_abstract.h"

template <class T>
void ApiAbstract::insert_settings(const std::string &key, const T &value) {
  if (this->_required_settings.contains(key)) {
    this->_required_settings[key] = value;
    return;
  }
  if (this->_optionnal_settings.contains(key)) {
    this->_optionnal_settings[key] = value;
    return;
  }
  throw ApiNoSettingCalledLikeThisException(key);
}

void ApiAbstract::insert_settings(const std::string &path) {
  std::ifstream in(path);
  nlohmann::json j;
  in >> j;
  for (auto &[key, val] : j.items()) {
    this->insert_settings(key, val);
  }
}

const nlohmann::json &ApiAbstract::get_required_settings() const {
  return this->_required_settings;
}

bool ApiAbstract::are_required_settings_filled() const {
  for (auto const &el : this->_required_settings.items()) {
    if (el.value().is_null())
      return false;
  }
  return true;
}
