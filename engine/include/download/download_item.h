#ifndef DOWNLOAD_ITEM_H
#define DOWNLOAD_ITEM_H

#include "download/api_parameter.h"
#include <map>
#include <sstream>
#include <string>
#include <utility>

/** Class to manage an item to download (base URL, parameters...) */
class download_item {
private:
  /** URL to use */
  std::string _url;
  /** Map of request parameters */
  std::map<const api_parameter_request *, std::string> _parameters;

public:
  /**
   * Creates the object with a given URL
   * @param url URL to assign
   */
  download_item(const std::string &url);

  /**
   * Gets a string representation of the object to print
   * @return std::string string to print
   */
  std::string to_string() const;

  /**
   * Sets a request parameter
   * @param key parameter to use
   * @param val value to use
   */
  void set_parameter(const api_parameter_request *key, const std::string &val);

  void set_url(const std::string &url);

  std::string get_url() const;
  const std::map<const api_parameter_request *, std::string> &
  get_parameters() const;

  std::map<std::string, std::string>
  get_position_parameters(const std::string &position) const;
};

#endif
