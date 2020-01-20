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

  if (!headers.is_null() && (headers.is_array() || headers.is_object())) {
    struct curl_slist *list = NULL;
    for (auto &[key, val] : headers.items()) {
      std::stringstream header;
      header << key << ": " << val.get<std::string>();
      list = curl_slist_append(list, header.str().c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
  }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                   download_manager::write_callback);
  std::string readBuffer;
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
  curl_easy_perform(curl);
  return readBuffer;
}

size_t download_manager::write_callback(void *contents, size_t size,
                                        size_t nmemb, void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}
