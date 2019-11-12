#include <cstdlib>
#include <curl/curl.h>
#include <iostream>
#include <string>

void print_usage() {
  std::cout
      << "Usage: download <max_parallel_downloads> <urls_to_text_resource>"
      << std::endl;
}

size_t write_cb(char *data, size_t n, size_t l, void *userp) {
  for (int i = 0; i < l; ++i) {
    std::cout << data[i];
  }
  std::cout << std::endl;
  (void)data;
  (void)userp;
  return n * l;
}

void add_transfer(CURLM *cm, std::string &url) {
  CURL *eh = curl_easy_init();
  curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(eh, CURLOPT_URL, url.c_str());
  curl_easy_setopt(eh, CURLOPT_PRIVATE, url.c_str());
  curl_multi_add_handle(cm, eh);
}

int main(int argc, char **argv) {

  if (argc <= 2) {
    print_usage();
    return EXIT_FAILURE;
  }

  try {
    if (std::stoi(argv[1]) <= 0) {
      throw std::invalid_argument(
          "Max parallel downloads must be an int above 0.");
    }
  } catch (const std::invalid_argument &e) {
    print_usage();
    return EXIT_FAILURE;
  }

  const size_t MAX_DOWNLOADS = std::stoi(argv[1]);
  const size_t URL_COUNT = argc - 2;
  std::string urls[URL_COUNT];

  std::cout << "Max parallel downloads: " << MAX_DOWNLOADS << std::endl;
  std::cout << "Found " << URL_COUNT << " URL(s)." << std::endl;

  for (size_t i = 0; i < URL_COUNT; i++) {
    urls[i] = argv[2 + i];
    std::cout << "URL: " << urls[i] << std::endl;
  }

  CURLM *cm;
  CURLMsg *msg;
  unsigned int transfers = 0;
  int msgs_left = -1;
  int still_alive = 1;

  curl_global_init(CURL_GLOBAL_ALL);
  cm = curl_multi_init();

  /* Limit the amount of simultaneous connections curl should allow: */
  curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long)MAX_DOWNLOADS);

  for (transfers = 0; transfers < MAX_DOWNLOADS && transfers < URL_COUNT;
       transfers++)
    add_transfer(cm, urls[transfers]);

  do {
    curl_multi_perform(cm, &still_alive);

    while ((msg = curl_multi_info_read(cm, &msgs_left))) {
      if (msg->msg == CURLMSG_DONE) {
        char *url;
        CURL *e = msg->easy_handle;
        curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
        fprintf(stderr, "R: %d - %s <%s>\n", msg->data.result,
                curl_easy_strerror(msg->data.result), url);
        curl_multi_remove_handle(cm, e);
        curl_easy_cleanup(e);
      } else {
        fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
      }
      if (transfers < URL_COUNT)
        add_transfer(cm, urls[transfers++]);
    }

  } while (still_alive || (transfers < URL_COUNT));

  curl_multi_cleanup(cm);
  curl_global_cleanup();

  return EXIT_SUCCESS;

  exit(0);
}