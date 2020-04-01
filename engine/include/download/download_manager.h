#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "download/download_item.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utils/utils.h>
#include <vector>

/**
 * Wrapper class to ease HTTP downloads
 */
class download_manager {

private:
  CURL *curl;

public:
  /**
   * Initializes the cURL object
   */
  download_manager();

  /**
   * Cleans up the cURL object
   */
  ~download_manager();

  /**
   * Retrieves the given URL with a list of headers as a JSON
   * @param dli download item to use
   */
  std::vector<char> download(const download_item &dli) const;

  /**
   * Callback used to write content to buffer, model following the cURL callback
   * example
   */
  static size_t write_callback(char *contents, size_t size, size_t nmemb,
                               void *userp);

  std::string build_body_query(const download_item &dli) const;
};

#endif
