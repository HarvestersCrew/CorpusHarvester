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
#include <memory>
#include <mutex>
#include <queue>
#include <set>

#define HARVESTER_DATABASE_NAME "harvester"
#define HARVESTER_DATABASE_DB_POOL 1

using sql::Connection;
using std::lock_guard;
using std::mutex;
using std::queue;
using std::set;
using std::shared_ptr;
using std::string;

/**
 * HarvesterDatabase class is a database wrapper
 */
class HarvesterDatabase {

  friend void test_open_pool();
  friend void test_close_pool();
  friend void test_reassign_free_borrowed_pool();

  /**
   * The database
   */
  static sql::Connection *_db;

  static queue<shared_ptr<Connection>> _available_pool;
  static set<shared_ptr<Connection>> _borrowed_pool;
  static mutex _pool_mut;

public:
  /**
   * Opens the database if not already opened
   */
  static void open();

  /**
   * Opens the pool of connection
   */
  static void open_pool(unsigned int nbr);

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

  /**
   * Closes the DB pool
   */
  static void close_pool();

  /**
   * Borrows a connection from the pool
   */
  static shared_ptr<Connection> borrow_from_pool();

  /**
   * Gives back to the pool a connection
   * Not mandatory as they are automatically freed, but better to make the
   * program quicker
   */
  static void unborrow_from_pool(shared_ptr<Connection> &ptr);
};

#endif
