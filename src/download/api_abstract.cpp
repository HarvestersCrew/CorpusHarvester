#include "download/api_abstract.h"

api_abstract::api_abstract() {
  this->_settings["required"] = nullptr;
  this->_settings["optional"] = nullptr;
}

template <class T>
void api_abstract::insert_settings(const std::string &key, const T &value) {
  if (this->_settings["required"].contains(key)) {
    this->_settings["required"][key] = value;
    return;
  }
  if (this->_settings["optional"].contains(key)) {
    this->_settings["optional"][key] = value;
    return;
  }
  throw api_no_setting_exception(key);
}

void api_abstract::insert_settings(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open provided file.");
  nlohmann::json j;
  in >> j;
  for (auto &[key, val] : j["optional"].items()) {
    this->insert_settings(key, val);
  }
  for (auto &[key, val] : j["required"].items()) {
    this->insert_settings(key, val);
  }
  in.close();
}

void api_abstract::create_settings_file(const std::string &path) const {
  std::ofstream out;
  out.open(path);
  if (!out)
    throw std::runtime_error("Can't open provided file.");
  out << this->_settings;
  out.close();
}

const nlohmann::json &api_abstract::get_settings() const {
  return this->_settings;
}

bool api_abstract::are_required_settings_filled() const {
  for (auto const &el : this->_settings["required"].items()) {
    if (el.value().is_null())
      return false;
  }
  return true;
}
