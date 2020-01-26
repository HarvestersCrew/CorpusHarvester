#ifndef FILE_H
#define FILE_H

#include <cppconn/connection.h>
#include <cppconn/resultset.h>

#include "indexation/mysql/database_item.h"
#include "indexation/mysql/tag.h"

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

class File : public DatabaseItem {

  string _path;
  string _name;
  int _size;
  string _source;
  list<unique_ptr<Tag>> _tags;

private:
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
  int getSize() const { return _size; }
};

#endif