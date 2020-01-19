#include "ui/cli/client_request.h"

void showHelpMenu() {
  // Create our corpus
  std::cout << "Possible Commands : " << std::endl;
  std::cout << "create : Create a corpus." << std::endl;
  std::cout << "--help : Show help menu." << std::endl;
}

void createCorpus(const std::deque<std::string> &parameters) {

  // If we don't have any parameters.
  if (parameters.empty()) {
    std::cout << "Please specify the content's type of the corpus."
              << std::endl;
    exit(0);
  }

  // If the first command is help
  if (parameters.front() == "--help") {
    std::cout << "Please specify the content type of our future corpus."
              << std::endl;
    exit(0);
  }

  // We get the content's type of our data.
  std::string type = parameters.front();

  // Create our corpus
  std::cout << "Creation " << type << "'s corpus in progress..." << std::endl;

  // TODO :: Call the twitter api for downloading the corresponding data.
  // TODO :; Call the indexation system and storage system.

  // api_twitter api;
  // api.insert_settings("settings/apis/twitter.json");
  // api.create_settings_file("settings/apis/twitter.json");
  // std::cout << api.get_settings() << std::endl;

  download_manager dl;
  dl.add_url("http://rpi.nocchi.me");
  dl.add_url("https://api.twitter.com/1.1/search/tweets.json?q=cat",
             {{"Authorization",
               "Bearer "
               "AAAAAAAAAAAAAAAAAAAAAFbXBwEAAAAAffCqNtEEVChsyKEOcep3UtaqDgM%"
               "3DDQyQagZBPQgrd02E4cnboMTDJlbfkObhWscuvXq3iXplX3TEXu"}});
  dl.download_queue_to("/tmp");
}
