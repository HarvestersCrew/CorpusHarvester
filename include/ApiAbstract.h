#include <string>
#include <unordered_map>

using api_settings = std::unordered_map<std::string, std::string>;

class ApiAbstract {
public:
  virtual void init() = 0;
  virtual void init(const std::string &key, const std::string &value) = 0;
  virtual void init(const api_settings &settings) = 0;
  virtual std::string get_name() const = 0;
  const api_settings &get_required_settings() const;
  std::string get_required_settings_string() const;

protected:
  api_settings _required_settings;
  api_settings _optionnal_settings;
};