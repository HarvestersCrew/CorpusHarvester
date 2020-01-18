#include "utils/exceptions.h"
#include "utils/json.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Class used to abstract a source of data, also known as APIs.
class ApiAbstract {
public:
  // Insert a setting with a key and a value.
  template <class T>
  void insert_settings(const std::string &key, const T &value);
  // Insert settings from a JSON file.
  void insert_settings(const std::string &path);

  void create_settings_file(const std::string &path);

  // String to define the name of the API.
  virtual std::string get_name() const = 0;

  // Getter of required settings.
  const nlohmann::json &get_required_settings() const;

protected:
  // Required settings to use API.
  nlohmann::json _required_settings;
  // Optionnal settings to use API.
  nlohmann::json _optionnal_settings;
  // Check if all required settings are filled.
  bool are_required_settings_filled() const;
};
