#ifndef CORPUSHARVESTER_CLENT_REQUEST_H
#define CORPUSHARVESTER_CLENT_REQUEST_H

#include "argparse.hpp"
#include "database/harvester_database.h"
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
#include <string>
#include <unordered_map>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class ManagerRequest {
private:
  /**
   * Private Constructor. We want to have a Singleton.
   */
  ManagerRequest() {}

  static ApiDownloadBuilder _dl_builder;
  static ApiDatabaseBuilder _db_builder;

public:
  /**
   * Get all the corpus present in our database.
   * Allow the user to add specific filters in order to search in the database
   * the corpus. Different kind of filters is available :
   *      - 'title' : Corpus which contains the title.
   *
   * Also, we allow the possibility of the user to order the result of the data
   * by :
   *      - 'date' : Order the corpus by the date of the creation.
   *      - 'alphabetical' : Order the corpus by alphabetical.
   *
   * @param filters Filters for the corpus research.
   * @param orders Indicate how to order our corpus.
   *
   * @return std::list<Corpus> List of all the corpus.
   */
  static std::list<Corpus *>
  visualisation_corpus(std::map<std::string, std::string> &filters,
                       std::map<std::string, std::string> &orders);

  /**
   * Get a corpus based on his name.
   *
   * @param name Corpus' name we want to visualized.
   *
   * @return Corpus Object
   */
  static std::optional<Corpus *> visualisation_corpus(const std::string name);

  /**
   * Get all the data present in our database.
   * Allow the user to add specific filters in order to search in the
   * database the data. Different kind of filters is available :
   *      - Type : Type of the data.
   *      - Sources : Source where the data come from.
   *      - Latest : Based on the input data, the latest data.
   *      - Oldest : Based on the input data, the oldest data.
   *
   * Allow the possibility to order the result by :
   *      - Date : Order the data by the date of the creation.
   *      - Alphabetical : Order the data by alphabetical.
   *
   * @param filters Filters for the data research.
   * @param orders Indicate how to order our data.
   *
   * @return std::list<File> List of all the data.
   */
  static std::list<File>
  visualisation_data(const std::map<std::string, std::string> &filters,
                     const std::map<std::string, std::string> &orders);

  /**
   * Create a corpus based on the name of it.
   *
   * @param name of the corpus
   * @param sources list of the source we want to create the corpus
   * @param params Map of the parameter for the new corpus.
   *
   * @return Created corpus.
   */
  static Corpus create_corpus(std::string name,
                              std::vector<std::string> sources,
                              std::map<std::string, std::string> &params);

  /*
   * Methods relating to web APIs informations
   */

  /**
   * Returns a list of API names that are usable in the Harvester
   * @return vector of strings which are the API names
   */
  static vector<string> get_apis();

  /**
   * Retrieves the list of parameters usable in a web request
   * @param api_name Name of the API to find
   * @return vector of parameters
   * @throw api_factory_name_not_found if the given API isn't found
   */
  static vector<shared_ptr<api_parameter_request>>
  get_api_web_parameters(const string &api_name);
};

#endif // CORPUSHARVESTER_CLENT_REQUEST_H
