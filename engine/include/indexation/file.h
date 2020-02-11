#ifndef FILE_H
#define FILE_H

#include "indexation/database_item.h"
#include "indexation/tag.h"
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_connection.h>
#include <sstream>

#define FILE_CREATE_STATEMENT                                                  \
  "CREATE TABLE IF NOT EXISTS File(id INTEGER NOT NULL AUTO_INCREMENT,path "   \
  "TEXT NOT NULL,name TEXT NOT NULL,size INTEGER, source TEXT NOT NULL, "      \
  "PRIMARY KEY (id));"
#define GET_FILES_FROM_TAG                                                     \
  "SELECT f.* FROM File f, Tag t WHERE f.id = t.file_id AND t.name "           \
  "= "                                                                         \
  "? AND "                                                                     \
  "t.value = ?;"
#define INSERT_FILE_STATEMENT                                                  \
  "INSERT INTO File (path, name, size, source) VALUES(?, ?, ?, ?)"
#define DROP_FILE_STATEMENT "DROP TABLE IF EXISTS File;"

using std::list;
using std::string;
using std::unique_ptr;

/**
 * File class describes a File table in the database
 */
class File : public DatabaseItem {

  /**
   * The path of the file
   */
  string _path;

  /**
   * The name of the file
   */
  string _name;

  /**
   * The size of the file
   */
  int _size;

  /**
   * The source of the file
   */
  string _source;

  /**
   * The content of the file (not a database attribute)
   */
  string _content;

  /**
   * The tags describing this file
   */
  list<unique_ptr<Tag>> _tags;

public:
  /**
   * Default constructor
   */
  File();

  /**
   * Creates a File object
   * @param path the path of the file
   * @param name the name of the file
   * @param size the size of the file
   * @param source the source of the file
   * @param id the id of the file in the database
   */
  File(string path, string name, int size, string source, int id = -1);

  /**
   * Default destructor
   */
  ~File();

  string to_string() const;

  void insert(sql::Connection *db);

  void fill_from_statement(sql::Connection *db, sql::ResultSet *res);

  /**
   * Fills the _tags attribute fetching the tags linked to this file in the
   * database
   * @param db the database
   */
  void fetch_tags(sql::Connection *db);

  /**
   * Adds a tag in the list of tags
   * @param name the name of the tag
   * @param value the value of the tag
   */
  void add_tag(string name, string value);

  string get_path() const { return _path; }
  string get_name() const { return _name; }
  string get_source() const { return _source; }
  string get_content() const { return _content; }
  int get_size() const { return _size; }

  void set_path(string path) { _path = path; }
  void set_content(string content) { _content = content; }
};

#endif
