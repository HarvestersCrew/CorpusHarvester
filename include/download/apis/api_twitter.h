#include "download/api_abstract.h"

// Implementation of the Twitter API.
class api_twitter : public api_abstract {
public:
  api_twitter();
  virtual std::string get_name() const;
};
