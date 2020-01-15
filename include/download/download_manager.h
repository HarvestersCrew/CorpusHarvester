#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utils/utils.h>
#include <vector>

class DownloadManager {

private:
  static unsigned int const FILENAME_LENGTH = 10;
  std::vector<std::string> urls;
  CURL *curl;

public:
  DownloadManager();
  ~DownloadManager();
  bool add_url(const std::string &url);
  bool download_queue_to(const std::string &path);
};

#endif
