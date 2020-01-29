#ifndef CORPUSHARVESTER_CLENT_REQUEST_H
#define CORPUSHARVESTER_CLENT_REQUEST_H

#include <list>

#include "indexation/corpus.h"
#include "indexation/file.h"

class ClientRequest {
public:
  /**
   * Get all the corpus present in our database.
   * Allow the uœser to add specific filters in order to search in the database
   * the corpus. Different kind of filters is available :
   *      - Title : Corpus which contains the title.
   *      - Latest : Based on the input date, the latest corpus.
   *      - Oldest : Based on the input date, the oldest corpus.
   *
   * Also, we allow the possibility of the user to order the result of the data
   * by :
   *      - Date : Order the corpus by the date of the creation.
   *      - Alphabetical : Order the corpus by alphabetical.
   *
   * @param filters std::list<string> Filters for the corpus research.
   * @param orders std::list<string> Indicate how to order our corpus.
   *
   * @return std::list<Corpus> List of all the corpus.
   */
  virtual std::list<Corpus>
  visualisation_corpus(const std::list<std::string> &filters,
                       const std::list<std::string> &orders) = 0;

  /**
   * Get a corpus based on his name.
   *
   * @param name Corpus' name we want to visualized.
   *
   * @return Corpus Object
   */
  virtual std::optional<Corpus>
  visualisation_corpus(const std::string name) = 0;

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
   * @param filters std::list<string> Filters for the data research.
   * @param orders std::list<string> Indicate how to order our data.
   *
   * @return std::list<File> List of all the data.
   */
  virtual std::list<File>
  visualisation_data(const std::list<std::string> &filters,
                     const std::list<std::string> &orders) = 0;
};

#endif // CORPUSHARVESTER_CLENT_REQUEST_H
