#include "indexation/indexer.h"

Indexer::Indexer(std::string db_name, bool verbose)
    : _db_name(db_name), _db(nullptr), _verbose(verbose) {}

void Indexer::open_database() {
  if (_db == nullptr) {
    sql::Driver *driver = get_driver_instance();
    _db = driver->connect("db", "root", "1234");
    _db->setSchema(_db_name);
  } else {
    print_if_verbose("Database is already opened", _verbose);
  }
}

void Indexer::insert_database_item(DatabaseItem *item) const {
  item->insert(_db);
  print_if_verbose("- Insert " + item->to_string() + " : OK", _verbose);
}

void Indexer::create_database(bool drop_table) {
  open_database();
  sql::Statement *stmt = _db->createStatement();
  if (drop_table) {
    stmt->execute(DROP_CORPUS_FILES_STATEMENT);
    stmt->execute(DROP_CORPUS_STATEMENT);
    stmt->execute(DROP_TAG_STATEMENT);
    stmt->execute(DROP_FILE_STATEMENT);
    print_if_verbose("- Drop tables : OK", _verbose);
  }

  stmt = _db->createStatement();
  stmt->execute(CORPUS_CREATE_STATEMENT);
  print_if_verbose("- Create Corpus table : OK", _verbose);
  stmt->execute(FILE_CREATE_STATEMENT);
  print_if_verbose("- Create File table : OK", _verbose);
  stmt->execute(TAG_CREATE_STATEMENT);
  print_if_verbose("- Create Tag table : OK", _verbose);
  stmt->execute(CORPUS_FILES_CREATE_STATEMENT);
  print_if_verbose("- Create CorpusFiles table : OK", _verbose);
  delete stmt;
}

void Indexer::indexation(std::list<File *> files) {
  open_database();
  for (File *file : files) {
    insert_database_item(file);
  }
}

void Indexer::save_corpus(Corpus &corpus) { corpus.insert(_db); }

std::list<File *> Indexer::fetch_from_tag(std::string tag_name,
                                          std::string tag_value) {
  open_database();
  SearchBuilder *sb = new SearchBuilder(_db, _verbose);
  std::list<File *> files =
      sb->add_tag_condition(tag_name, tag_value, "=")->build();
  return files;
}

std::list<File *> Indexer::fetch_from_attribute(std::string attribute,
                                                std::string value) {
  open_database();
  SearchBuilder *sb = new SearchBuilder(_db, _verbose);
  std::list<File *> files = sb->add_condition(attribute, value, "=")->build();
  return files;
}
