
#include "ui/cli/CommandLineInterface.h"

CommandLineInterface::CommandLineInterface(
    const std::deque<std::string> arguments) {
  this->arguments = arguments;
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
    //    createCorpus(cmdLineArgs);
  } else {
    std::cout
        << "The command '" << firstCommand
        << "' doesn't exit. Please check the available commands with 'help'."
        << std::endl;
    exit(0);
  }
}

void CommandLineInterface::showHelpMenu() {
  // Create our corpus
  std::cout << "Possible Commands : " << std::endl;
  std::cout << "create : Create a corpus." << std::endl;
  std::cout << "--help : Show help menu." << std::endl;
}
