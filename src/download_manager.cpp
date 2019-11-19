#include <download_manager.h>

DownloadManager::DownloadManager() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  this->curl = curl_easy_init();
}

DownloadManager::~DownloadManager() {
  curl_easy_cleanup(this->curl);
  curl_global_cleanup();
}

bool DownloadManager::add_url(const std::string &url) {
  this->urls.push_back(url);
  return true;
}

bool DownloadManager::download_queue_to(const std::string &path) {
  FILE *f;
  long unsigned int i = 0;
  while (i < this->urls.size()) {

    auto url = this->urls.at(i);
    std::stringstream path_with_filename;
    path_with_filename << path << "/" << random_str(this->FILENAME_LENGTH);
    f = fopen(path_with_filename.str().c_str(), "w");
    if (!f) {
      std::cout << "Can't write to " << path_with_filename.str()
                << " file, skipping " << url << "..." << std::endl;
      ++i;
      continue;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    curl_easy_perform(curl);
    fclose(f);
    std::cout << "Downloaded " << url << " to " << path_with_filename.str()
              << std::endl;
    this->urls.erase(this->urls.begin() + i);
  }
  return true;
}
