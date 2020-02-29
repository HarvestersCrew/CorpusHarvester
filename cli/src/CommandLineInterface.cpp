
#include "CommandLineInterface.h"
#include "utils/utils.h"

CommandLineInterface::CommandLineInterface(
    const std::deque<std::string> arguments) {
  this->arguments = arguments;
}

void CommandLineInterface::show_help_menu() {
  // Create our corpus
  std::cout << "Possible Commands : " << std::endl;
  std::cout << "create : Create a corpus." << std::endl;
  std::cout << "--help : Show help menu." << std::endl;
}

void CommandLineInterface::create_api() {
  // TODO :: Creation of a new corpus
}

Corpus CommandLineInterface::create_corpus() {

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

  std::cout << "Creation of " << type << "'s corpus in progress..."
            << std::endl;

  // Download corresponding data
  download_manager dl;
  api_loader twitter(std::string("data/twitter.json"),
                     std::string("data/twitter.env.json"));
  std::list<File *> out = twitter.query_and_parse({{"q", type}}, dl);

  // Store the files
  Storage storage("/tmp/stored/");
  storage.store_files(out);

  // Index the downloaded data
  Indexer indexer("harvester", false);
  indexer.create_database(true);
  indexer.indexation(out);

  // Request files which has at least one retweet and one favorite
  SearchBuilder sb = indexer.get_search_builder();
  list<File *> tweets = sb.add_tag_condition("retweet", "100", ">")
                            ->logical_and()
                            ->add_condition("id", "50", "<")
                            ->build();

  // Create our corpus from the fetch data and save it
  std::string now = get_current_time("%d-%m-%Y %H:%M:%S");
  Corpus corpus("50 premiers avec retweets > 100", now, tweets, "");
  indexer.save_corpus(corpus);
  return corpus;
}

void CommandLineInterface::run() {

  // Get the first command and remove it form the deque array.
  std::string firstCommand = this->arguments.front();
  this->arguments.pop_front();

  // We print the help menu.
  if (firstCommand == "help" or firstCommand == "--help" or
      firstCommand == "-help") {
    this->show_help_menu();
  } else if (firstCommand == "create") {
    Corpus corpus = this->create_corpus();
    std::cout << corpus.to_string() << std::endl;
  } else {
    std::cout
        << "The command '" << firstCommand
        << "' doesn't exit. Please check the available commands with 'help'."
        << std::endl;
    exit(0);
  }
}