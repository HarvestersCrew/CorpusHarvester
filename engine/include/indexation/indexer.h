#ifndef INDEXER_H
#define INDEXER_H

#include "indexation/corpus.h"
#include "indexation/database_item.h"
#include "indexation/file.h"
#include "indexation/search_builder.h"
#include "utils/utils.h"

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <sstream>

using std::list;
using std::string;

/**
 * Indexer class provides methods to manage the indexation of the files in the
 * database
 */
class Indexer {

  /**
   * The name of the database
   */
  string _db_name;

  /**
   * The database (as a connection)
   */
  sql::Connection *_db;

  /**
   * A boolean to allow prints in the function of this class
   */
  bool _verbose;

private:
  /**
   * Inserts the given item in the database
   * @param item the item to insert in the database
   */
  void insertDatabaseItem(DatabaseItem *item) const;

public:
  /**
   * Creates an Indexer object
   * @param db_name the name of the database
   * @param verbose true to allow prints in the function of this class, false
   * otherwise
   */
  Indexer(string db_name, bool verbose = false);

  /**
   * Opens the database if not already opened
   */
  void openDatabase();

  /**
   * Creates the database
   * @param drop_table if true, drop the tables existing in the database before
   * creating the new ones
   */
  void createDatabase(bool drop_table);

  /**
   * Indexes the given files in the database
   * @param files the files to index
   */
  void indexation(list<File *> files);

  /**
   * Saves the given corpus in the database
   * @param the corpus to save
   */
  void saveCorpus(Corpus &corpus);

  /**
   * Filters the files in the database that own the single given tag
   * @param tagName the name of the tag
   * @param tagValue the value of the tag
   * return a list of all the files matching the condition
   */
  list<File *> fetchFromTag(string tagName, string tagValue);

  /**
   * Filters the files in the database where the given attribute (one column in
   * the database) has the given value
   * @param tagName the name of the tag
   * @param tagValue the value of the tag
   * return a list of all the files matching the condition
   */
  list<File *> fetchFromAttribute(string attribute, string value);

  sql::Connection *getDatabase() { return _db; }
};

#endif
