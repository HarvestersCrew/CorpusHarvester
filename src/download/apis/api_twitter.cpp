#include "download/apis/api_twitter.h"

ApiTwitter::ApiTwitter() : ApiAbstract() {
  this->_settings["required"]["api_key"] = nullptr;
}

std::string ApiTwitter::get_name() const { return "Twitter"; }
