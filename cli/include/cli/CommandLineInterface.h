
#ifndef CORPUSHARVESTER_CLIENTLINEINTERFACE_H
#define CORPUSHARVESTER_CLIENTLINEINTERFACE_H

#include "api/ManagerRequest.h"
#include "download/api_loader.h"
#include "download/download_manager.h"
#include "indexation/file.h"
#include "indexation/indexer.h"
#include "indexation/search_builder.h"
#include "storage/storage.h"
#include "utils/cli_parser.h"
#include "utils/exceptions.h"
#include <deque>
#include <iostream>
#include <memory>
#include <string>

using std::shared_ptr;

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

  //
  // API MANAGER
  //

  /**
   * Manage the api request.
   */
  void api_manager();

  /**
   * Create a new API.
   */
  void create_api();

  //
  // CORPUS MANAGER
  //

  /**
   * Manage the corpus request.
   */
  void corpus_manager();

  /**
   * See a corpus by his id
   * Method : ./bin/cli corpus id --id "1234"
   */
  void corpus_by_id();

  /**
   * See a list of corpus.
   * Method : ./bin/cli corpus list
   */
  void corpus_list();

  /**
   * Create a new corpus.
   * Method : ./bin/cli corpus create --name "Name of my corpus"
   */
  void corpus_create();

  //
  // FILES MANAGER
  //

  /**
   * Manage the files request.
   */
  void files_manager();

  /**
   * List of files present in the database.
   * Method : ./bin/cli files list
   */
  void files_list();

  /**
   * Get a file by his id
   * Method : ./bin/cli files id --id "mon_id"
   */
  void files_by_id();

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
