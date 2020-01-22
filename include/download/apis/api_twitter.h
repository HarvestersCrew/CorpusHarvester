#ifndef API_TWITTER_H
#define API_TWITTER_H

#include "download/api_abstract.h"
#include "utils/nlohmann/json.hpp"
#include <sstream>

// Implementation of the Twitter API.
class api_twitter : public api_abstract {
public:
  api_twitter();
  virtual std::string get_name() const;
  virtual std::string get_base_url() const;
  virtual nlohmann::json query(const download_manager &dl,
                               const std::string &query) const;
};

#endif
