#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "utils/json.hpp"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utils/utils.h>
#include <vector>

class download_manager {

private:
  static unsigned int const FILENAME_LENGTH = 10;
  CURL *curl;
  nlohmann::json urls;

public:
  download_manager();
  ~download_manager();
  bool add_url(const std::string &url);
  bool download_queue_to(const std::string &path);
};

#endif
