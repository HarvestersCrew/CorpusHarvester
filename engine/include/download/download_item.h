#ifndef DOWNLOAD_ITEM_H
#define DOWNLOAD_ITEM_H

#include <map>
#include <sstream>
#include <string>

/** Class to manage an item to download (base URL, parameters...) */
class download_item {
private:
  /** URL to use */
  std::string _url;
  /** Map of query parameters */
  std::map<std::string, std::string> _parameters;
  /** Map of header parameters */
  std::map<std::string, std::string> _headers;

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
   * Sets a query parameter
   * @param key key to use
   * @param val value to use
   */
  void set_parameter(const std::string &key, const std::string &val);

  /**
   * Sets a header parameter
   * @param key key to use
   * @param val value to use
   */
  void set_header(const std::string &key, const std::string &val);

  void set_url(const std::string &url);

  std::string get_url() const;
  const std::map<std::string, std::string> &get_parameters() const;
  const std::map<std::string, std::string> &get_headers() const;
};

#endif
