#ifndef DOWNLOAD_ITEM_H
#define DOWNLOAD_ITEM_H

#include "download/api_parameter.h"
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

using std::shared_ptr;

/** Class to manage an item to download (base URL, parameters...) */
class download_item {
private:
  /** URL to use */
  std::string _url;
  /** Map of request parameters */
  std::map<shared_ptr<const api_parameter_request>, std::string> _parameters;
  /** Number of characters to truncate at the start of the response */
  int _truncate_before;
  /** Number of characters to truncate at the end of the response */
  int _truncate_after;

public:
  /**
   * Creates the object with a given URL
   * @param url URL to assign
   */
  download_item(const std::string &url);

  /**
   * Creates the object with a given URL and specifies a truncate before and
   * after
   * @param url URL to assign
   * @param truncate_before number of characters to erase at start of response
   * @param truncate_after number of characters to erase at end of response
   */
  download_item(const std::string &url, int truncate_before,
                int truncate_after);

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
  void set_parameter(const shared_ptr<const api_parameter_request> key,
                     const std::string &val);

  void set_url(const std::string &url);

  int get_truncate_before() const;
  int get_truncate_after() const;

  std::string get_url() const;

  const std::map<shared_ptr<const api_parameter_request>, std::string> &
  get_parameters() const;

  std::map<std::string, std::string>
  get_position_parameters(const std::string &position) const;
};

#endif
