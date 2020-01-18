#include "download/apis/api_twitter.h"

ApiTwitter::ApiTwitter() { this->_required_settings["api_key"] = nullptr; }

std::string ApiTwitter::get_name() const { return "Twitter"; }
