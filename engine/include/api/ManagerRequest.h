#ifndef CORPUSHARVESTER_CLENT_REQUEST_H
#define CORPUSHARVESTER_CLENT_REQUEST_H

#include <list>

#include "indexation/corpus.h"
#include "indexation/file.h"

class ManagerRequest {
private:
    /**
     * Private Constructor. We want to have a Singleton.
     */
    ManagerRequest() {}
    ManagerRequest(ManagerRequest const&); // Don't Implement
    void operator=(ManagerRequest const&); // Don't implement

public:
    /**
     * Singleton Design Pattern.
     */
    ManagerRequest(ManagerRequest const&) = delete;
    void operator=(ManagerRequest const&) = delete;

    static ManagerRequest& getInstance() {
        static ManagerRequest instance; // Guaranteed to be destroyed.
        return instance;
    }

  /**
   * Get all the corpus present in our database.
   * Allow the user to add specific filters in order to search in the database
   * the corpus. Different kind of filters is available :
   *      - 'title' : Corpus which contains the title.
   *      - 'latest' : Based on the input date, the latest corpus.
   *      - 'oldest' : Based on the input date, the oldest corpus.
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
  virtual std::list<Corpus>
  visualisation_corpus(const std::map<std::string, std::string> &filters,
                       const std::map<std::string, std::string> &orders) = 0;

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
   * @param filters Filters for the data research.
   * @param orders Indicate how to order our data.
   *
   * @return std::list<File> List of all the data.
   */
  virtual std::list<File>
  visualisation_data(const std::map<std::string, std::string> &filters,
                     const std::map<std::string, std::string> &orders) = 0;
};

#endif // CORPUSHARVESTER_CLENT_REQUEST_H
