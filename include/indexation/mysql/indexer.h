#ifndef INDEXER_H
#define INDEXER_H

#include "indexation/mysql/database_item.h"
#include "indexation/mysql/file.h"

#include <cppconn/driver.h>

using std::list;
using std::string;

#define DROP_FILE_STATEMENT "DROP TABLE IF EXISTS File;"
#define DROP_TAG_STATEMENT "DROP TABLE IF EXISTS Tag;"

class Indexer {

  string _db_name;
  sql::Connection *_db;

private:
  void openDatabase();

  void insertDatabaseItem(DatabaseItem *item) const;

  list<File *> getFilesFromTag(string tag_name, string tag_value);

public:
  Indexer(string db_name);

  void create_database(bool drop_table);

  void indexation(list<File *> files);

  list<File *> fetchFromTag(string tag_name, string tag_value);
};

#endif