#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <curl/curl.h>
#include <fstream>
#include <iostream>
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
   * Download the given URL to a file at path
   * @param path file to store the result into*
   * @param url url to download
   */
  void download_to(const std::string &path, const std::string &url) const;

  /**
   * Download the given URL to a file at path with a list of headers as a JSON
   * @param path file to store the result into*
   * @param url url to download
   * @param headers headers to use during the request
   */
  void download_to(const std::string &path, const std::string &url,
                   const nlohmann::json &headers) const;

  /**
   * Retrieves the given URL
   * @param url url to download
   */
  std::vector<char> download(const std::string &url) const;

  /**
   * Retrieves the given URL with a list of headers as a JSON
   * @param url url to download
   * @param headers headers to use during the request
   */
  std::vector<char> download(const std::string &url,
                             const nlohmann::json &headers) const;

  /**
   * Callback used to write content to buffer, model following the cURL callback
   * example
   */
  static size_t write_callback(char *contents, size_t size, size_t nmemb,
                               void *userp);
};

#endif
