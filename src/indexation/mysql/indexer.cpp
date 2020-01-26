#include <iostream>
#include <sstream>

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#include "indexation/mysql/database_item.h"
#include "indexation/mysql/file.h"
#include "indexation/mysql/indexer.h"
#include "utils/utils.h"

using std::cout;
using std::endl;
using std::list;

Indexer::Indexer(string db_name, bool verbose)
    : _db_name(db_name), _db(nullptr), _verbose(verbose) {}

void Indexer::openDatabase() {
  if (_db == nullptr) {
    sql::Driver *driver = get_driver_instance();
    _db = driver->connect("db", "root", "1234");
    _db->setSchema(_db_name);
  } else {
    print("Database is already opened", _verbose);
  }
}

void Indexer::insertDatabaseItem(DatabaseItem *item) const {
  item->insert(_db);
  print("- Insert " + item->toString() + " : OK", _verbose);
}

list<File *> Indexer::getFilesFromTag(string tag_name, string tag_value) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  list<File *> files;

  prep_stmt = _db->prepareStatement(GET_FILES_FROM_TAG);
  prep_stmt->setString(1, tag_name);
  prep_stmt->setString(2, tag_value);
  res = prep_stmt->executeQuery();

  while (res->next()) {
    File *file = new File();
    file->fillFromStatement(res);
    file->fetchTags(_db);
    files.push_back(file);
  }
  delete prep_stmt;
  delete res;
  return files;
}

void Indexer::createDatabase(bool drop_table) {
  openDatabase();
  sql::Statement *stmt = _db->createStatement();
  if (drop_table) {
    stmt->execute(DROP_TAG_STATEMENT);
    stmt->execute(DROP_FILE_STATEMENT);
    print("- Drop tables : OK", _verbose);
  }

  stmt->execute(FILE_CREATE_STATEMENT);
  print("- Create File table : OK", _verbose);
  stmt->execute(TAG_CREATE_STATEMENT);
  print("- Create Tag table : OK", _verbose);
}

void Indexer::indexation(list<File *> files) {
  openDatabase();
  for (File *file : files) {
    insertDatabaseItem(file);
  }
}

list<File *> Indexer::fetchFromTag(string tag_name, string tag_value) {
  openDatabase();
  return getFilesFromTag(tag_name, tag_value);
}