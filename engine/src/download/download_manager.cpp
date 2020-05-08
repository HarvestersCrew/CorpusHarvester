#include "download/download_manager.h"

download_manager::download_manager() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  this->curl = curl_easy_init();
}

download_manager::~download_manager() {
  curl_easy_cleanup(this->curl);
  curl_global_cleanup();
}

std::vector<char> download_manager::download(const download_item &dli) const {

  // IF GET TYPE, WHICH IS CURRENTLY ALWAYS
  std::string url = dli.get_url();
  if (dli.get_position_parameters("body").size() != 0) {
    url += "?" + this->build_body_query(dli);
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  std::map<std::string, std::string> request_headers =
      dli.get_position_parameters("header");
  if (request_headers.size() != 0) {
    struct curl_slist *list = NULL;
    for (auto el : request_headers) {
      std::stringstream header;
      header << el.first << ": " << el.second;
      list = curl_slist_append(list, header.str().c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
  }

  std::vector<char> read_buffer;

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   download_manager::write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
  curl_easy_perform(curl);

  long response_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

  if (response_code != 200) {
    throw download_no_200_exception(response_code, url);
    logger::debug(vec_to_string(read_buffer));
  }

  if (dli.get_truncate_before() > 0) {
    read_buffer.erase(read_buffer.begin(),
                      read_buffer.begin() + dli.get_truncate_before());
  }

  if (dli.get_truncate_after() > 0) {
    int first_to_erase = read_buffer.size() - dli.get_truncate_after();
    read_buffer.erase(read_buffer.begin() + first_to_erase, read_buffer.end());
  }

  return read_buffer;
}

size_t download_manager::write_callback(char *contents, size_t size,
                                        size_t nmemb, void *userp) {
  for (size_t i = 0; i < nmemb; ++i) {
    ((std::vector<char> *)userp)->push_back(contents[i]);
  }
  return size * nmemb;
}

std::string download_manager::build_body_query(const download_item &dli) const {
  std::stringstream ss;
  const std::map<std::string, std::string> query =
      dli.get_position_parameters("body");
  if (query.size() != 0) {
    bool first = true;

    for (auto el : query) {
      char *key = curl_easy_escape(this->curl, el.first.c_str(), 0);
      char *val = curl_easy_escape(this->curl, el.second.c_str(), 0);
      if (!first) {
        ss << "&";
      }
      ss << key << "=" << val;
      first = false;
    }
  }
  return ss.str();
}
