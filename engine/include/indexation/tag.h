#ifndef TAG_H
#define TAG_H

#include "indexation/database_item.h"
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

class Tag : public DatabaseItem {

  int _file_id;
  string _name;
  string _value;

public:
  Tag();

  Tag(string name, string value, int id = -1, int file_id = -1);

  ~Tag();

  string toString() const;

  void insert(sql::Connection *db);

  void fillFromStatement(sql::Connection *db, sql::ResultSet *res);

  int getFileId() const { return _file_id; }
  string getName() const { return _name; }
  string getValue() const { return _value; }

  void setFileId(int file_id) { _file_id = file_id; }
};

#endif
