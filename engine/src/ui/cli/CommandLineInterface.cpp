
#include "ui/cli/CommandLineInterface.h"

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

  // Create our corpus
  std::cout << "Creation " << type << "'s corpus in progress..." << std::endl;

  // TODO :: Call the twitter api for downloading the corresponding data.
  // TODO :; Call the indexation system and storage system.
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
    // TODO :: Print the corpus
  } else {
    std::cout
        << "The command '" << firstCommand
        << "' doesn't exit. Please check the available commands with 'help'."
        << std::endl;
    exit(0);
  }
}