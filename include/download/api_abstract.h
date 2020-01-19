#include "utils/exceptions.h"
#include "utils/json.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Class used to abstract a source of data, also known as APIs.
class api_abstract {
public:
  api_abstract();
  // Insert a setting with a key and a value.
  template <class T>
  void insert_settings(const std::string &key, const T &value);
  // Insert settings from a JSON file.
  void insert_settings(const std::string &path);

  // Generate a JSON file of settings for this API.
  void create_settings_file(const std::string &path) const;

  // String to define the name of the API.
  virtual std::string get_name() const = 0;

  // Getter of settings.
  const nlohmann::json &get_settings() const;

  // Check if all required settings are filled.
  bool are_required_settings_filled() const;

protected:
  // Settings to use API.
  nlohmann::json _settings;
};
