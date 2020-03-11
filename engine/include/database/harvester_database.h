#ifndef HARVESTER_DATABASE_H
#define HARVESTER_DATABASE_H

#include "database/setting.h"
#include "indexation/corpus.h"
#include "indexation/file.h"
#include "indexation/tag.h"
#include "utils/logger.h"

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>

using std::string;

/**
 * HarvesterDatabase class is a database wrapper
 */
class HarvesterDatabase {

  /**
   * The database
   */
  static sql::Connection *_db;

public:
  /**
   * Opens the database if not already opened
   */
  static void open();

  /**
   * Checks if the database is empty
   * @return true if the datatbase is empty, false otherwise
   */
  static bool empty();

  /**
   * Drops the database to make it empty
   */
  static void drop();

  /**
   * Creates the database if it is empty
   */
  static void create();

  /**
   * Gets an initialized harvester database
   */
  static sql::Connection *init();

  /**
   * Gets the datatbase
   */
  static sql::Connection *get() { return _db; }

  /**
   * Closes the database if not already closed
   */
  static void close();
};

#endif
