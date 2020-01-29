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

using std::list;
using std::string;
using std::unique_ptr;

/**
 * File class describes a File table in the database
 */
class File : public DatabaseItem {

  string _path;
  string _name;
  int _size;
  string _source;
  list<unique_ptr<Tag>> _tags;

public:
  File(string path, string name, int size, string source, int id = -1);

  File();

  ~File();

  string toString() const;

  void insert(sql::Connection *db);

  void fetchTags(sql::Connection *db);

  void fillFromStatement(sql::ResultSet *res);

  void addTag(string name, string value);

  string getPath() const { return _path; }
  string getName() const { return _name; }
  string getSource() const { return _source; }
  int getSize() const { return _size; }

  void setPath(string path) { _path = path; }
};

#endif
