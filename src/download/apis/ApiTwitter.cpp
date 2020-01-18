#include "download/apis/ApiTwitter.h"

ApiTwitter::ApiTwitter() {
  this->_required_settings.insert(std::make_pair("api_key", ""));
}

std::string ApiTwitter::get_name() const { return "Twitter"; }
