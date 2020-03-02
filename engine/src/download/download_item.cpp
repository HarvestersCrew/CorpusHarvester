#include "download/download_item.h"

download_item::download_item(const std::string &url) { this->set_url(url); }

std::string download_item::to_string() const {
  std::stringstream ss;
  ss << "url: " << this->_url << std::endl;

  ss << std::endl << "query parameters:" << std::endl;
  for (auto el : this->_parameters) {
    ss << el.first << ": \"" << el.second << "\"" << std::endl;
  }

  ss << std::endl << "header parameters:" << std::endl;
  for (auto el : this->_headers) {
    ss << el.first << ": \"" << el.second << "\"" << std::endl;
  }

  return ss.str();
}

void download_item::set_parameter(const std::string &key,
                                  const std::string &val) {
  this->_parameters.insert_or_assign(key, val);
}

void download_item::set_header(const std::string &key, const std::string &val) {
  this->_headers.insert_or_assign(key, val);
}

void download_item::set_url(const std::string &url) { this->_url = url; }

std::string download_item ::get_url() const { return this->_url; }

const std::map<std::string, std::string> &
download_item::get_parameters() const {
  return this->_parameters;
}

const std::map<std::string, std::string> &download_item::get_headers() const {
  return this->_headers;
}
