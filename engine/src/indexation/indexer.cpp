#include "indexation/indexer.h"

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
    printIfVerbose("Database is already opened", _verbose);
  }
}

void Indexer::insertDatabaseItem(DatabaseItem *item) const {
  item->insert(_db);
  printIfVerbose("- Insert " + item->toString() + " : OK", _verbose);
}

void Indexer::createDatabase(bool drop_table) {
  openDatabase();
  sql::Statement *stmt = _db->createStatement();
  if (drop_table) {
    stmt->execute(DROP_CORPUS_FILES_STATEMENT);
    stmt->execute(DROP_CORPUS_STATEMENT);
    stmt->execute(DROP_TAG_STATEMENT);
    stmt->execute(DROP_FILE_STATEMENT);
    printIfVerbose("- Drop tables : OK", _verbose);
  }

  stmt = _db->createStatement();
  stmt->execute(CORPUS_CREATE_STATEMENT);
  printIfVerbose("- Create Corpus table : OK", _verbose);
  stmt->execute(FILE_CREATE_STATEMENT);
  printIfVerbose("- Create File table : OK", _verbose);
  stmt->execute(TAG_CREATE_STATEMENT);
  printIfVerbose("- Create Tag table : OK", _verbose);
  stmt->execute(CORPUS_FILES_CREATE_STATEMENT);
  printIfVerbose("- Create CorpusFiles table : OK", _verbose);
  delete stmt;
}

void Indexer::indexation(list<File *> files) {
  openDatabase();
  for (File *file : files) {
    insertDatabaseItem(file);
  }
}

void Indexer::saveCorpus(Corpus &corpus) { corpus.insert(_db); }

list<File *> Indexer::fetchFromTag(string tagName, string tagValue) {
  openDatabase();
  SearchBuilder *sb = new SearchBuilder(_db, _verbose);
  list<File *> files = sb->addTagCondition(tagName, tagValue, "=")->build();
  return files;
}

list<File *> Indexer::fetchFromAttribute(string attribute, string value) {
  openDatabase();
  SearchBuilder *sb = new SearchBuilder(_db, _verbose);
  list<File *> files = sb->addCondition(attribute, value, "=")->build();
  return files;
}
