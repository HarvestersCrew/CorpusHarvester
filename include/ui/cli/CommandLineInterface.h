
#ifndef CORPUSHARVESTER_CLIENTLINEINTERFACE_H
#define CORPUSHARVESTER_CLIENTLINEINTERFACE_H

#include "download/apis/api_twitter.h"
#include "download/download_manager.h"
#include "ui/api/ClientAction.h"
#include "utils/json.hpp"

#include <deque>
#include <iostream>
#include <string>

/**
 * CommandLineInterface class.
 *
 * Create a class that will build a request based on the client input.
 */
class CommandLineInterface : public ClientAction {
private:
  /**
   * Arguments of the user input.
   */
  std::deque<std::string> arguments;

  /**
   * Print in the user terminal all the possible actions.
   */
  void showHelpMenu();

public:
  /**
   * ClientLineInterface Constructor.
   *
   * @param arguments Vector sequence of all the arguments.
   */
  CommandLineInterface(const std::deque<std::string> arguments);

  /**
   * Execute the current command and print in the user terminal the output.
   */
  void run();
};

#endif // CORPUSHARVESTER_CLIENTLINEINTERFACE_H
