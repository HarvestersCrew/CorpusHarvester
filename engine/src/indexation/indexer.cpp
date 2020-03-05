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

void Indexer::close_database() {
  if (_db != nullptr) {
    _db->close();
    _db = nullptr;
  } else {
    print_if_verbose("Database is already closed", _verbose);
  }
}

void Indexer::insert_database_item(DatabaseItem *item) const {
  item->insert(_db);
  print_if_verbose("- Insert " + item->to_string() + " : OK", _verbose);
}

void Indexer::create_database(bool drop_table) {
  open_database();
  std::string drop_statements[] = {DROP_CORPUS_FILES_STATEMENT,
                                   DROP_CORPUS_STATEMENT, DROP_TAG_STATEMENT,
                                   DROP_FILE_STATEMENT, DROP_SETTING_STATEMENT};
  std::string create_statements[] = {
      CORPUS_CREATE_STATEMENT, FILE_CREATE_STATEMENT, TAG_CREATE_STATEMENT,
      CORPUS_FILES_CREATE_STATEMENT, SETTING_CREATE_STATEMENT};
  sql::Statement *stmt = _db->createStatement();
  if (drop_table) {
    for (auto &drop_statement : drop_statements) {
      stmt->execute(drop_statement);
    }
    print_if_verbose("- Drop tables : OK", _verbose);
  }

  stmt = _db->createStatement();
  for (auto &create_statement : create_statements) {
    stmt->execute(create_statement);
  }
  print_if_verbose("- Create tables : OK", _verbose);

  if (drop_table) {
    Setting::init_settings(_db);
    print_if_verbose("- Init Setting tables : OK", _verbose);
  }

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
