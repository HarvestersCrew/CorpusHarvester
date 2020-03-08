#include "download/download_item.h"

download_item::download_item(const std::string &url)
    : download_item(url, 0, 0) {}

download_item::download_item(const std::string &url, int truncate_before,
                             int truncate_after)
    : _truncate_before(truncate_before), _truncate_after(truncate_after) {
  this->set_url(url);
}

std::string download_item::to_string() const {
  std::stringstream ss;
  ss << "url: " << this->_url << std::endl;

  for (auto el : this->_parameters) {
    ss << el.first->_api_name << " (" << el.first->_position << "): \""
       << el.second << "\"" << std::endl;
  }

  return ss.str();
}

void download_item::set_parameter(
    const shared_ptr<const api_parameter_request> key, const std::string &val) {
  this->_parameters.insert_or_assign(key, val);
}

int download_item::get_truncate_before() const {
  return this->_truncate_before;
}
int download_item::get_truncate_after() const { return this->_truncate_after; }

void download_item::set_url(const std::string &url) { this->_url = url; }

std::string download_item ::get_url() const { return this->_url; }

const std::map<shared_ptr<const api_parameter_request>, std::string> &
download_item::get_parameters() const {
  return this->_parameters;
}

std::map<std::string, std::string>
download_item::get_position_parameters(const std::string &position) const {
  std::map<std::string, std::string> result;
  for (auto el : this->_parameters) {
    if (el.first->_position == position) {
      result.insert_or_assign(el.first->_api_name, el.second);
    }
  }
  return result;
}
