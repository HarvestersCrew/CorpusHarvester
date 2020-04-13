
#include "cli/CommandLineInterface.h"
#include <iostream>

/**
 * Get the command of the user.
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {

  CommandLineInterface cli = CommandLineInterface(argc, argv);
  cli.run();
}
