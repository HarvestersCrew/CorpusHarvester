#include "download/apis/api_twitter.h"

api_twitter::api_twitter() : api_abstract() {
  this->_settings["required"]["bearer"] = nullptr;
  this->_settings["required"]["query"] = nullptr;
}

std::string api_twitter::get_name() const { return "Twitter"; }
