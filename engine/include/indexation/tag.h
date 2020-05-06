#ifndef TAG_H
#define TAG_H

#include "database/database_item.h"
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sstream>

#define TAG_CREATE_STATEMENT                                                   \
  "CREATE TABLE IF NOT EXISTS Tag(id INTEGER NOT NULL AUTO_INCREMENT,file_id " \
  "INTEGER NOT NULL,name "                                                     \
  "TEXT NOT NULL,value TEXT NOT NULL,PRIMARY KEY (id),FOREIGN KEY (file_id) "  \
  "REFERENCES File(id));"
#define DROP_TAG_STATEMENT "DROP TABLE IF EXISTS Tag;"
#define INSERT_TAG_STATEMENT                                                   \
  "INSERT INTO Tag (file_id, name, value) VALUES(?, ?, ?)"

using std::string;

/**
 * Tag class describe a Tag table in the database
 */
class Tag : public DatabaseItem {

  /**
   * The id of the file this tag is linked to
   */
  int _file_id;

  /**
   * The name of the tag
   */
  string _name;

  /**
   * The value of the tag
   */
  string _value;

public:
  /**
   * Default constructor
   */
  Tag();

  /**
   * Creates a Tag object
   * @param name the name of the tag
   * @param value the value of the tag
   * @param id the id of the tag
   * @param file_id the id of the file this tag is linked to
   */
  Tag(string name, string value, int id = -1, int file_id = -1);

  /**
   * Default destructor
   */
  ~Tag();

  string to_string() const;

  bool insert();

  void fill_from_statement(sql::ResultSet *res);

  int get_file_id() const { return _file_id; }
  string get_name() const { return _name; }
  string get_value() const { return _value; }
  void set_value(string value) { _value = value; }

  void set_file_id(int file_id) { _file_id = file_id; }
};

#endif
