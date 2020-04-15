#ifndef CORPUSHARVESTER_CLENT_REQUEST_H
#define CORPUSHARVESTER_CLENT_REQUEST_H

#include "database/harvester_database.h"
#include "database/setting.h"
#include "download/api_download_builder.h"
#include "download/api_loader.h"
#include "download/api_parameter.h"
#include "download/download_manager.h"
#include "indexation/api_database_builder.h"
#include "indexation/corpus.h"
#include "indexation/file.h"
#include "indexation/indexer.h"
#include "indexation/search_builder.h"
#include "storage/storage.h"
#include "utils/cli_parser.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using std::get;
using std::list;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class ManagerRequest {
private:
  ApiDownloadBuilder _dl_builder;
  ApiDatabaseBuilder _db_builder;

public:
  ManagerRequest() {}

  /*
   * ------------------------------------------
   * METHODS RELATING TO CORPUS MANAGEMENT
   * ------------------------------------------
   */

  /**
   * Get all the corpus present in our database.
   * Allow the user to add specific filters in order to search in the database
   * the corpus. Different kind of filters is available :
   *      - 'title' : Corpus which contains the title.
   *
   * @param filters Filters for the corpus research.
   * @param orders Indicate how to order our corpus.
   *
   * @return list<Corpus> List of all the corpus.
   */
  list<shared_ptr<Corpus>> get_corpuses(std::map<string, string> &filters,
                                        Corpus::ordering_method order) const;

  /**
   * @see get_corpuses above
   * Difference is that here a string for the order is accepted
   * "date_asc"
   * "date_desc"
   * "name_asc"
   * "name_desc"
   * "none"
   * By default it is "none"
   */
  list<shared_ptr<Corpus>> get_corpuses(std::map<string, string> &filters,
                                        const string &order) const;

  /**
   * Retrieves all the corpuses
   * @return List of pointers to corpuses
   */
  list<shared_ptr<Corpus>> get_corpuses() const;

  /**
   * Get corpuses based on a name.
   * @param name Corpus' name we want to visualized.
   * @return List of corpuses
   */
  list<shared_ptr<Corpus>> get_corpus_from_name(const string name) const;

  /**
   * Get a corpus from his id.
   * @param id of the corpus in the database.
   * @return The desired corpus.
   * @throw db_id_not_found if corpus wasn't found
   */
  shared_ptr<Corpus> get_corpus_from_id(const int id) const;

  /**
   * Create a corpus with files, a name and optional filters
   * @param name name of the corpus
   * @param files list of files
   * @param builder DB builder used (empty if none)
   * @return ID in the DB of the corpus
   */
  int create_corpus(const string &name, const list<shared_ptr<File>> &files,
                    const optional<ApiDatabaseBuilder> &builder);

  /**
   * Exports a given corpus ID with a certain format
   * @param id ID of the corpus
   * @param method Export method
   * @return Path to the exported corpus
   * @throw db_id_not_found if corpus wasn't found
   */
  string export_corpus(const int id, ExportMethod::methods method) const;

  /**
   * Exports a given corpus ID with a certain format
   * @param id ID of the corpus
   * @param method Export method as a string
   * "zip" available
   * "zip" by default
   * @return Path to the exported corpus
   * @throw db_id_not_found if corpus wasn't found
   * @throw manager_request_unhandled_exception if no exportation path is found
   * after the exportation, shouldn't happen
   */
  string export_corpus(const int id, const string &method) const;

  /*
   * ------------------------------------------
   * METHODS RELATING TO FILES
   * ------------------------------------------
   */

  /**
   * Get a file from his id.
   * @param id of the file in the database.
   * @return The desired file.
   */
  std::optional<shared_ptr<File>> get_file_from_id(const int id) const;

  /*
   * ------------------------------------------
   * METHODS RELATING TO WEB APIS INFORMATIONS
   * ------------------------------------------
   */

  /**
   * Returns a list of API names that are usable in the Harvester
   * @return vector of strings which are the API names
   */
  vector<string> get_apis();

  /**
   * Gets an immuable list of the API loaders loaded in memory
   */
  const vector<shared_ptr<const api_loader>> get_api_loaders() const;

  /**
   * Retrieves the list of parameters usable in a web request
   * @param api_name Name of the API to find
   * @return vector of parameters
   * @throw api_factory_name_not_found if the given API isn't found
   */
  const vector<shared_ptr<api_parameter_request>> &
  get_api_web_parameters(const string &api_name);

  /**
   * Retrieves the list of parameters usable in a DB request
   * @param api_name Name of the API to find
   * @return vector of parameters
   * @throw api_factory_name_not_found if the given API isn't found
   */
  const vector<shared_ptr<api_parameter_response>> &
  get_api_db_parameters(const string &api_name);

  /*
   * ------------------------------------------
   * METHODS RELATING TO API BUILDERS
   * ------------------------------------------
   */

  /**
   * Helper method not intended to be used by interfaces
   * It retrieves a reference to the wanted builder, in order to avoid all if /
   * else in subsequent methods
   * @param is_web true if it is the web builder, false if it is the DB builder
   */
  ApiRequestBuilder &api_builder_get_based_on_bool(bool is_web);

  /**
   * Resets one of the builders
   * @param is_web true if it is the web builder, false if it is the DB builder
   */
  void api_builder_clear(bool is_web);

  /**
   * Builder is capable of doing multiple OR requests based on the API name
   * ex.: (Twitter AND retweet < 10) OR (Twitter AND retweet > 100)
   * Adding a request is initialising a part of a multiple OR statement
   * Use api_builder_add_parameter with the returned index to add a parameter
   * @param is_web true if it is the web builder, false if it is the DB builder
   * @param api_name Name of the API to add
   * @return index of the newly inserted request
   * @throw api_factory_name_not_found if the given API is not found
   * @throw api_no_setting_exception if a parameter isn't found
   */
  long unsigned int api_builder_add_request(bool is_web,
                                            const string &api_name);

  /**
   * Inserts or replace a parameter value and operator in the request defined by
   * the ID
   * @param is_web true if it is the web builder, false if it is the DB builder
   * @param request_id Request ID
   * @param param_name Name of the API parameter
   * @param param_value Value to set
   * @param op Operator to use to search
   * @throw api_no_setting_exception if the parameter isn't found
   * @throw api_builder_request_not_found if the request ID isn't found
   * @throw api_builder_incompatible_operator If an operator is incompatible
   */
  void api_builder_add_request_parameter(bool is_web,
                                         long unsigned int request_id,
                                         const string &param_name,
                                         const string &param_value,
                                         const string &op);

  /**
   * Fetches the list of files from whatever source we want
   * @param is_web true if it is the web builder, false if it is the DB builder
   * @param number number of elements to retrieve, 0 means value not set and
   * outcome depends on the implementation
   * DatabaseBuilder -> 0 = everything that matches
   * DownloadBuilder -> 0 = single pass over the sources
   */
  list<shared_ptr<File>> api_builder_build(bool is_web, unsigned int number);

  /*
   * ------------------------------------------
   * METHODS RELATING TO HARVESTER SETTINGS
   * ------------------------------------------
   */

  /**
   * Retrieves current logger informations
   *
   * level: debug, info, warning, error, none
   *
   * method: stdout, file
   *
   * path: if the output method is file
   *
   * @return logger informations
   */
  tuple<string, string, string> get_logger_settings() const;

  /**
   * Set the new logger level
   * @param level debug, info, warning, error, none
   * @return new logger informations
   * @see get_logger_settings
   * @throw manager_request_invalid_parameter if the given value is invalid
   */
  tuple<string, string, string> set_logger_level(const string &level);

  /**
   * Set the new logger output
   * @param output stdout, file
   * @return new logger informations
   * @see get_logger_settings
   * @throw manager_request_invalid_parameter if the given value is invalid
   */
  tuple<string, string, string> set_logger_output(const string &output);

  /**
   * Set the new logger output path
   * @param path path to the log file
   * @return new logger informations
   * @see get_logger_settings
   * @throw logger_exception if the path is invalid
   */
  tuple<string, string, string>
  set_logger_output_path(const string &output_path);

  /**
   * Gets the storage path
   */
  string get_storage_path() const;
};

#endif // CORPUSHARVESTER_CLENT_REQUEST_H
