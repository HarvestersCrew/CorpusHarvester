#include "download/apis/api_twitter.h"

ApiTwitter::ApiTwitter() : ApiAbstract() {
  this->_settings["required"]["bearer"] = nullptr;
  this->_settings["required"]["query"] = nullptr;
}

std::string ApiTwitter::get_name() const { return "Twitter"; }
