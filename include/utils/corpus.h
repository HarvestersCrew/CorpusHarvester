#ifndef CORPUSHARVESTER_CORPUS_H
#define CORPUSHARVESTER_CORPUS_H

#include <list>

#include "data.h"

class Corpus {
private:
  /**
   * Title of the corpus.
   */
  std::string title;

  /**
   * Date of the creation of the corpus.
   */
  std::string date_creation;

  /**
   * List of all the data contained by the corpus.
   */
  std::list<Data> data;

public:
  /**
   * Get the title of the corpus.
   *
   * @return std::string the title of the corpus.
   */
  std::string getTitle() const;

  /**
   * Set a title to the corpus.
   *
   * @param title std::string the new name of the copus.
   */
  void setTitle(const std::string title);
};

#endif // CORPUSHARVESTER_CORPUS_H
