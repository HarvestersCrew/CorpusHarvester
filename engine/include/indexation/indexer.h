#ifndef INDEXER_H
#define INDEXER_H

#include "indexation/corpus.h"
#include "indexation/database_item.h"
#include "indexation/file.h"
#include "indexation/search_builder.h"
#include "indexation/setting.h"
#include "utils/utils.h"

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <sstream>

/**
 * Indexer class provides methods to manage the indexation of the files in the
 * database
 */
class Indexer {

  /**
   * The name of the database
   */
  std::string _db_name;

  /**
   * The database (as a connection)
   */
  sql::Connection *_db;

  /**
   * A boolean to allow prints in the function of this class
   */
  bool _verbose;

public:
  /**
   * Creates an Indexer object
   * @param db_name the name of the database
   * @param verbose true to allow prints in the function of this class, false
   * otherwise
   */
  Indexer(std::string db_name, bool verbose = false);

  /**
   * Inserts the given item in the database
   * @param item the item to insert in the database
   * return true if the item was succesfully inserted
   */
  bool insert_database_item(DatabaseItem *item) const;

  /**
   * Opens the database if not already opened
   */
  void open_database();

  /**
   * Closes the database if not already closed
   */
  void close_database();

  /**
   * Creates the database
   * @param drop_table if true, drop the tables existing in the database before
   * creating the new ones
   */
  void create_database(bool drop_table);

  /**
   * Indexes the given files in the database
   * @param files the files to index
   */
  void indexation(std::list<File *> files);

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
  std::list<File *> fetch_from_tag(std::string tag_name, std::string tag_value);

  /**
   * Filters the files in the database where the given attribute (one column in
   * the database) has the given value
   * @param tag_name the name of the tag
   * @param tag_value the value of the tag
   * return a list of all the files matching the condition
   */
  std::list<File *> fetch_from_attribute(std::string attribute,
                                         std::string value);

  /**
   * Creates a search builder to request files from the database used by this
   * indexer
   * @return a search builder
   */
  SearchBuilder get_search_builder() {
    SearchBuilder sb(_db, _verbose);
    return sb;
  }

  sql::Connection *get_database() { return _db; }
};

#endif
