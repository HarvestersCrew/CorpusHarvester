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

class Indexer {

  string _db_name;
  sql::Connection *_db;
  bool _verbose;

private:
  void insertDatabaseItem(DatabaseItem *item) const;

  list<File *> getFilesFromTag(string tagName, string tagValue);

  list<File *> getFilesFromAttribute(string attribute, string value);

public:
  Indexer(string db_name, bool verbose = false);

  void openDatabase();

  void createDatabase(bool drop_table);

  void indexation(list<File *> files);

  void saveCorpus(Corpus &corpus);

  list<File *> fetchFromTag(string tagName, string tagValue);

  list<File *> fetchFromAttribute(string attribute, string value);

  sql::Connection *getDatabase() { return _db; }
};

#endif
