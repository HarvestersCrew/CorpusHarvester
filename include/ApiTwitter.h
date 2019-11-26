#include <ApiAbstract.h>

class ApiTwitter : public ApiAbstract {
public:
  virtual void init();
  virtual void init(const std::string &key, const std::string &value);
  virtual void init(const api_settings &settings);
  virtual std::string get_name() const;
};