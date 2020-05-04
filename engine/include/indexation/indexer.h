#ifndef INDEXER_H
#define INDEXER_H

#include "database/pool_db.h"
#include "indexation/corpus.h"
#include "indexation/file.h"
#include "indexation/search_builder.h"
#include "utils/exceptions.h"
#include "utils/logger.h"
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <memory>
#include <sstream>

using std::shared_ptr;

typedef struct {
  int file_count;
  int corpus_count;
  int text_count;
  int image_count;
  int total_size;
} db_statistics;

/**
 * Indexer class provides methods to manage the indexation of the files in the
 * database
 */
class Indexer {

public:
  /** A list containing the available stat requests */
  static list<string> _stats_requests;

  /**
   * Creates an Indexer object that uses the given database
   */
  Indexer();

  /**
   * Inserts the given file in the database
   * @param file the file to insert in the database
   * return true if the file was succesfully inserted
   */
  bool insert_file(shared_ptr<File> file) const;

  /**
   * Indexes the given files in the database
   * @param files the files to index
   */
  void indexation(std::list<shared_ptr<File>> files);

  /**
   * Saves the given corpus in the database
   * @param the corpus to save
   */
  void save_corpus(Corpus &corpus);

  /**
   * Filters the files in the database that own the single given tag
   * @param tag_name the name of the tag
   * @param tag_value the value of the tag
   * return a list of all the files matching the condition
   */
  std::list<shared_ptr<File>> fetch_from_tag(std::string tag_name,
                                             std::string tag_value);

  /**
   * Filters the files in the database where the given attribute (one column in
   * the database) has the given value
   * @param tag_name the name of the tag
   * @param tag_value the value of the tag
   * return a list of all the files matching the condition
   */
  std::list<shared_ptr<File>> fetch_from_attribute(std::string attribute,
                                                   std::string value);

  /**
   * Creates a search builder to request files from the database used by this
   * indexer
   * @return a search builder
   */
  SearchBuilder get_search_builder();

  void init_ith_member(int i, int32_t value, db_statistics *stats);

  /**
   * Computes every needed statistics from the database
   * @param stats the struct to fill with statistics
   */
  void get_statistics(db_statistics *stats);
};

#endif
