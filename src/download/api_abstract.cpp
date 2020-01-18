#include "download/api_abstract.h"

ApiAbstract::ApiAbstract() {
  this->_settings["required"] = nullptr;
  this->_settings["optional"] = nullptr;
}

template <class T>
void ApiAbstract::insert_settings(const std::string &key, const T &value) {
  if (!this->_settings.contains(key)) {
    throw ApiNoSettingCalledLikeThisException(key);
  }
  this->_settings[key] = value;
}

void ApiAbstract::insert_settings(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open provided file.");
  nlohmann::json j;
  in >> j;
  for (auto &[key, val] : j.items()) {
    this->insert_settings(key, val);
  }
}

void ApiAbstract::create_settings_file(const std::string &path) {
  std::ofstream out(path);
  if (out.fail())
    throw std::runtime_error("Can't open provided file.");
}

const nlohmann::json &ApiAbstract::get_settings() const {
  return this->_settings;
}

bool ApiAbstract::are_required_settings_filled() const {
  for (auto const &el : this->_settings["required"].items()) {
    if (el.value().is_null())
      return false;
  }
  return true;
}
