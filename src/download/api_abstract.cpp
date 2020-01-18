#include "download/api_abstract.h"

void ApiAbstract::start() {
  if (!this->are_required_settings_filled())
    throw ApiRequiredSettingsNotFilledException();
}

void ApiAbstract::insert_settings(const api_settings &settings) {
  for (auto const &el : settings) {
    this->insert_settings(el.first, el.second);
  }
}

void ApiAbstract::insert_settings(const std::string &key,
                                  const std::string &value) {
  for (auto &el : this->_required_settings) {
    if (!key.compare(el.first)) {
      el.second = value;
      return;
    }
  }
  for (auto &el : this->_optionnal_settings) {
    if (!key.compare(el.first)) {
      el.second = value;
      return;
    }
  }
  throw ApiNoSettingCalledLikeThisException();
}

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

bool ApiAbstract::are_required_settings_filled() const {
  for (auto const &el : this->_required_settings) {
    if (el.second.empty())
      return false;
  }
  return true;
}
