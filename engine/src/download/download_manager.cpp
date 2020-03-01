#include "download/download_manager.h"

download_manager::download_manager() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  this->curl = curl_easy_init();
}

download_manager::~download_manager() {
  curl_easy_cleanup(this->curl);
  curl_global_cleanup();
}

void download_manager::download_to(const std::string &path,
                                   const std::string &url) const {
  this->download_to(path, url, nullptr);
}
void download_manager::download_to(const std::string &path,
                                   const std::string &url,
                                   const nlohmann::json &headers) const {
  std::ofstream out;
  out.open(path);
  if (!out)
    throw std::runtime_error("Can't open provided file.");
  out << this->download(url, headers);
  out.close();
}

std::string download_manager::download(const std::string &url) const {
  return this->download(url, nullptr);
}

std::string download_manager::download(const std::string &url,
                                       const nlohmann::json &headers) const {
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  if (!headers.is_null() && headers.is_object()) {
    struct curl_slist *list = NULL;
    for (const auto &[key, val] : headers.items()) {
      std::stringstream header;
      header << key << ": " << val.get<std::string>();
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
    throw download_no_200_exception();
  }

  return vec_to_string(read_buffer);
}

size_t download_manager::write_callback(char *contents, size_t size,
                                        size_t nmemb, void *userp) {
  for (size_t i = 0; i < nmemb; ++i) {
    ((std::vector<char> *)userp)->push_back(contents[i]);
  }
  return size * nmemb;
}
