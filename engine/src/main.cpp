
#include <iostream>

#include "ui/cli/CommandLineInterface.h"

/**
 * Get the command of the user.
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {

  // We have no command.
  if (argc == 1) {
    std::cout << "Please specify your command." << std::endl;
    exit(0);
  }

  // Get all the parameters passed through the command line.
  std::deque<std::string> cmdLineArgs(argv + 1, argv + argc);

  CommandLineInterface cli = CommandLineInterface(cmdLineArgs);
  cli.run();

  return 0;
}
