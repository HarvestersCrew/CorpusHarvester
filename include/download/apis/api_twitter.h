#include "download/api_abstract.h"
#include <sstream>

// Implementation of the Twitter API.
class api_twitter : public api_abstract {
public:
  api_twitter();
  virtual std::string get_name() const;
  virtual void add_job(download_manager &dl) const;
};
