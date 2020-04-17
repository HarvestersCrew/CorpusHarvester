#ifndef CORPUSHARVESTER_CLENT_REQUEST_H
#define CORPUSHARVESTER_CLENT_REQUEST_H

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

  /**
   * Migrates the storage to a new path and returns the new path
   * @param new_path
   * @throw StorageMigrationException if the new path can't be used
   */
  string migrate_storage(const string &new_path) const;
};

#endif // CORPUSHARVESTER_CLENT_REQUEST_H
