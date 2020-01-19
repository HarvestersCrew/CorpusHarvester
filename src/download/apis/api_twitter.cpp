#include "download/apis/api_twitter.h"

api_twitter::api_twitter() : api_abstract() {
  this->_settings["required"]["bearer"] = nullptr;
  this->_settings["required"]["query"] = nullptr;
}

std::string api_twitter::get_name() const { return "Twitter"; }

void api_twitter::add_job(download_manager &dl) const {
  api_abstract::add_job(dl);
  std::stringstream url, token;
  url << "https://api.twitter.com/1.1/search/tweets.json?q="
      << this->_settings["required"]["query"].get<std::string>();
  token << "Bearer "
        << this->_settings["required"]["bearer"].get<std::string>();
  dl.add_url(url.str(), {{"Authorization", token.str()}});
}
