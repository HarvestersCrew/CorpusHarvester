
#ifndef CORPUSHARVESTER_CLIENTLINEINTERFACE_H
#define CORPUSHARVESTER_CLIENTLINEINTERFACE_H

#include "api/ClientAction.h"
#include "download/api_loader.h"
#include "download/download_manager.h"
#include "indexation/file.h"
#include "indexation/indexer.h"
#include "indexation/search_builder.h"
#include "storage/storage.h"

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
  void show_help_menu();

  /**
   * Create a new Corpus.
   *
   * @return Corpus created
   */
  Corpus create_corpus();

  /**
   * Create a new API.
   */
  void create_api();

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
