
#include "CommandLineInterface.h"
#include "utils/utils.h"

CommandLineInterface::CommandLineInterface(
    const std::deque<std::string> arguments) {
  this->arguments = arguments;
}

void CommandLineInterface::showHelpMenu() {
  // Create our corpus
  std::cout << "Possible Commands : " << std::endl;
  std::cout << "create : Create a corpus." << std::endl;
  std::cout << "--help : Show help menu." << std::endl;
}

void CommandLineInterface::createApi() {
  // TODO :: Creation of a new corpus
}

Corpus CommandLineInterface::createCorpus() {

  // If we don't have any parameters.
  if (this->arguments.empty()) {
    std::cout << "Please specify the content's type of the corpus."
              << std::endl;
    exit(0);
  }

  // If the first command is help
  if (this->arguments.front() == "--help") {
    std::cout << "Please specify the content type of our future corpus."
              << std::endl;
    exit(0);
  }

  // We get the content's type of our data.
  std::string type = this->arguments.front();

  std::cout << "Creation " << type << "'s corpus in progress..." << std::endl;

  // Download corresponding data
  download_manager dl;
  api_loader twitter(std::string("data/twitter.json"));
  std::list<File *> out =
      twitter.query_and_parse({{"q", type},
                               {"Authorization", "Bearer "
                                                 "bearer_code"}},
                              dl);

  // Store the files
  Storage storage("/tmp/stored/");
  storage.store_files(out);

  // Index the downloaded data
  Indexer indexer("harvester", false);
  indexer.createDatabase(true);
  indexer.indexation(out);

  // Request files which has at least one retweet and one favorite
  SearchBuilder sb = indexer.getSearchBuilder();
  list<File *> tweets = sb.addTagCondition("retweet", "100", ">")
                            ->logicalAnd()
                            ->addCondition("id", "50", "<")
                            ->build();

  // Create our corpus from the fetch data and save it
  std::string now = get_current_time("%d-%m-%Y %H:%M:%S");
  Corpus corpus("50 premiers avec retweets > 100", now, tweets);
  indexer.saveCorpus(corpus);
  return corpus;
}

void CommandLineInterface::run() {

  // Get the first command and remove it form the deque array.
  std::string firstCommand = this->arguments.front();
  this->arguments.pop_front();

  // We print the help menu.
  if (firstCommand == "help" or firstCommand == "--help" or
      firstCommand == "-help") {
    this->showHelpMenu();
  } else if (firstCommand == "create") {
    Corpus corpus = this->createCorpus();
    std::cout << corpus.toString() << std::endl;
  } else {
    std::cout
        << "The command '" << firstCommand
        << "' doesn't exit. Please check the available commands with 'help'."
        << std::endl;
    exit(0);
  }
}
