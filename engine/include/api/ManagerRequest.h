#ifndef CORPUSHARVESTER_CLENT_REQUEST_H
#define CORPUSHARVESTER_CLENT_REQUEST_H

#include "database/harvester_database.h"
#include "download/api_download_builder.h"
#include "indexation/api_database_builder.h"
#include "indexation/corpus.h"
#include "indexation/file.h"
#include <list>
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class ManagerRequest {
private:
  /**
   * Private Constructor. We want to have a Singleton.
   */
  ManagerRequest() {}

public:
  /**
   * Get the instance of the Manager Request.
   *
   * @return ManagerRequest instance
   */
  static ManagerRequest &getInstance() {
    static ManagerRequest instance;
    return instance;
  }

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
  std::list<Corpus *>
  visualisation_corpus(std::map<std::string, std::string> &filters,
                       std::map<std::string, std::string> &orders);

  /**
   * Get a corpus based on his name.
   *
   * @param name Corpus' name we want to visualized.
   *
   * @return Corpus Object
   */
  std::optional<Corpus *> visualisation_corpus(const std::string name);

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
  std::list<File>
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
  Corpus create_corpus(std::string name, std::vector<std::string> sources,
                       std::map<std::string, std::string> &params);
};

#endif // CORPUSHARVESTER_CLENT_REQUEST_H
