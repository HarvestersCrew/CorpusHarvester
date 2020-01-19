#include "download/download_manager.h"

download_manager::download_manager() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  this->curl = curl_easy_init();
}

download_manager::~download_manager() {
  curl_easy_cleanup(this->curl);
  curl_global_cleanup();
}

bool download_manager::add_url(const std::string &url) {
  this->urls.push_back(url);
  return true;
}

bool download_manager::download_queue_to(const std::string &path) {
  FILE *f;
  for (std::vector<std::string>::iterator it = this->urls.begin();
       it != this->urls.end();) {

    std::stringstream path_with_filename;
    path_with_filename << path << "/" << random_str(this->FILENAME_LENGTH);
    f = fopen(path_with_filename.str().c_str(), "w");
    if (!f) {
      std::cout << "Can't write to " << path_with_filename.str()
                << " file, skipping " << *it << "..." << std::endl;
      ++it;
      continue;
    }

    curl_easy_setopt(curl, CURLOPT_URL, (*it).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    curl_easy_perform(curl);
    fclose(f);
    std::cout << "Downloaded " << *it << " to " << path_with_filename.str()
              << std::endl;
    this->urls.erase(it);
  }
  return true;
}
