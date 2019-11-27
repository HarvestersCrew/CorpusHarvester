#include <ApiAbstract.h>

class ApiTwitter : public ApiAbstract {
public:
  ApiTwitter();
  virtual std::string get_name() const;
};
