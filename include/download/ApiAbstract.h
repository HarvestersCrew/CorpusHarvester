#include <string>
#include <unordered_map>
#include <utils/exceptions.h>

// Declaring this type as map of strings as keys and values.
using api_settings = std::unordered_map<std::string, std::string>;

// Class used to abstract a source of data, also known as APIs.
class ApiAbstract {
public:
  // Provides an abstracted way to check if the API is ready to be used.
  void start();

  // Replaces the current settings with a copy of the one given.
  void insert_settings(const api_settings &settings);
  // Insert a setting with a key and a value.
  void insert_settings(const std::string &key, const std::string &value);

  // String to define the name of the API.
  virtual std::string get_name() const = 0;

  // Getter of required settings.
  const api_settings &get_required_settings() const;
  // Get required settings as a string to display.
  std::string get_required_settings_string() const;

protected:
  // Required settings to use API.
  api_settings _required_settings;
  // Optionnal settings to use API.
  api_settings _optionnal_settings;
  // Check if all required settings are filled.
  bool are_required_settings_filled() const;
};
