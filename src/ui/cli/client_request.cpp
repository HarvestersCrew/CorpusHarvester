#include "ui/cli/client_request.h"

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
  download_manager dl;
  api_twitter api;
  api.insert_settings("settings/apis/twitter.json");
  auto res = api.query(dl, type);
  std::cout << res << std::endl;
}
