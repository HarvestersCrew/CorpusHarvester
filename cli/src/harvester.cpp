
#include <iostream>

#include "CommandLineInterface.h"

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
