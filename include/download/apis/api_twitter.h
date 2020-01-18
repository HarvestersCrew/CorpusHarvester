#include "download/api_abstract.h"

// Implementation of the Twitter API.
class ApiTwitter : public ApiAbstract {
public:
  ApiTwitter();
  virtual std::string get_name() const;
};
