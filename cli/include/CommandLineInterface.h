
#ifndef CORPUSHARVESTER_CLIENTLINEINTERFACE_H
#define CORPUSHARVESTER_CLIENTLINEINTERFACE_H

#include "api/ManagerRequest.h"
#include "argparse.hpp"
#include "download/api_loader.h"
#include "download/download_manager.h"
#include "indexation/file.h"
#include "indexation/indexer.h"
#include "indexation/search_builder.h"
#include "storage/storage.h"
#include "utils/cli_parser.h"
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>

/**
 * CommandLineInterface class.
 *
 * Create a class that will build a request based on the client input.
 */
class CommandLineInterface {
private:
  /**
   * Parser we will use to create our request.
   */
  cli_command parser;

  /**
   * List of all the command set.
   */
  vector<string> commands;

  /**
   * Map with all options and values.
   */
  map<string, string> string_inputs;

  /**
   * Map indicates if there are options
   */
  map<string, bool> bool_inputs;

  /**
   * Print in the user terminal all the possible actions.
   */
  void show_help_menu();

  /**
   * Manage the api request.
   */
  void api_manager();

  /**
   * Manage the corpus request.
   */
  void corpus_manager();

  /**
   * Show all the corpus created
   *
   * @return list of corpus
   */
  std::list<Corpus *> list_corpus();

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
  CommandLineInterface(int argc, char **argv);

  /**
   * Execute the current command and print in the user terminal the output.
   */
  void run();
};

#endif // CORPUSHARVESTER_CLIENTLINEINTERFACE_H
