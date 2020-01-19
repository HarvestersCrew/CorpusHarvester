#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "utils/json.hpp"
#include <curl/curl.h>
#include <fstream>
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
  void download_to(const std::string &path, const std::string &url) const;
  void download_to(const std::string &path, const std::string &url,
                   const nlohmann::json &headers) const;
  std::string download(const std::string &url) const;
  std::string download(const std::string &url,
                       const nlohmann::json &headers) const;
  static size_t write_callback(void *contents, size_t size, size_t nmemb,
                               void *userp);
};

#endif
