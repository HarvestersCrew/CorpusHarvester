
#ifndef CORPUSHARVESTER_CLIENTACTION_H
#define CORPUSHARVESTER_CLIENTACTION_H

#include "indexation/corpus.h"

class ClientAction {
private:
  /**
   * Create a new corpus with data in the system.
   * Call the Download system and the Indexer system for the files.
   * Call the Database for the creation of the corpus.
   *
   * @param parameters List of parameters for the creation of the corpus.
   *
   * TODO :: Need to see if the parameters will be fixed in order to be more
   * flexible in the different interfaces (cli and UI).
   */
  virtual Corpus create_corpus(const std::string name) = 0;

  /**
   * Search for a particular corpus.
   *
   * @param name
   * @return
   */
  virtual std::optional<Corpus *> search_corpus(const std::string name) = 0;

  /**
   * Create a new api for downloading new data.
   * TODO :: See for the parameter
   */
  virtual void create_api() = 0;
};

#endif // CORPUSHARVESTER_CLIENTACTION_H
