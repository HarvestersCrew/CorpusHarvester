#include <string>
#include <unordered_map>
#include <utils/exceptions.h>

using api_settings = std::unordered_map<std::string, std::string>;

class ApiAbstract {
public:
  void start();

  virtual void insert_settings(const api_settings &settings);
  virtual void insert_settings(const std::string &key,
                               const std::string &value);

  virtual std::string get_name() const = 0;

  virtual const api_settings &get_required_settings() const;
  virtual std::string get_required_settings_string() const;

protected:
  api_settings _required_settings;
  api_settings _optionnal_settings;
  bool are_required_settings_filled() const;
};
