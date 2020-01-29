
#ifndef CORPUSHARVESTER_CLIENTACTION_H
#define CORPUSHARVESTER_CLIENTACTION_H

#include "indexation/corpus.h"

class ClientAction {
private:
  /**
   * Show a help menu that indicate to the user all the possible actions
   * available.
   */
  virtual void showHelpMenu() = 0;

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
  virtual Corpus createCorpus() = 0;

  /**
   * Create a new api for downloading new data.
   * TODO :: See for the parameter
   */
  virtual void createApi() = 0;
};

#endif // CORPUSHARVESTER_CLIENTACTION_H
