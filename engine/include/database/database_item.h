#ifndef DATABASE_ITEM_H
#define DATABASE_ITEM_H

#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

/**
 * DatabaseItem class describes an object stored in the database
 */
class DatabaseItem {

protected:
  /**
   * The id of the object
   */
  int _id;

public:
  /**
   * Default constructor
   */
  DatabaseItem() {}

  /**
   * Creates a DatabaseItem object with the given id
   * @param id the id in the database of the object
   */
  DatabaseItem(int id) : _id(id) {}

  /**
   * Default destructor
   */
  virtual ~DatabaseItem() = default;

  /**
   * Converts the object in a string that describe it
   * @return a string describing the object
   */
  virtual std::string to_string() const = 0;

  /**
   * Inserts the object in the given database
   * @param db the database where the object is inserted
   * return true if the object was succesfully inserted
   */
  virtual bool insert(sql::Connection *db) = 0;

  /**
   * Fills the object reading the result of an SQL statement
   * @param db the database
   * @param the result of an SQL statement
   */
  virtual void fill_from_statement(sql::Connection *db,
                                   sql::ResultSet *res) = 0;

  /**
   * Gets the id of the last inserted object in the database
   * @param db please pass nullptr
   * @return the id of the last inserted object in the database
   */
  static int get_last_inserted_id(sql::Connection *db);

  virtual int get_id() const { return _id; };
  void set_id(int id) { _id = id; };
};

#endif // DATABASE_ITEM_H
