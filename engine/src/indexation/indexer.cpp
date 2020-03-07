#include "indexation/indexer.h"

Indexer::Indexer(std::string db_name, bool verbose)
    : _db_name(db_name), _db(nullptr), _verbose(verbose) {
  open_database();
}

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
    delete _db;
  } else {
    print_if_verbose("Database is already closed", _verbose);
  }
}

bool Indexer::insert_file(shared_ptr<File> file) const {
  bool inserted = file->insert(_db);
  if (inserted) {
    print_if_verbose("- Insertion of " + file->to_string() + " : OK", _verbose);
  } else {
    print_if_verbose("- The file wasn't inserted", _verbose);
  }
  return inserted;
}

void Indexer::create_database(bool drop_table) {
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

void Indexer::indexation(std::list<shared_ptr<File>> files) {
  open_database();
  for (shared_ptr<File> &file : files) {
    insert_file(file);
  }
}

void Indexer::save_corpus(Corpus &corpus) { corpus.insert(_db); }

std::list<shared_ptr<File>> Indexer::fetch_from_tag(std::string tag_name,
                                                    std::string tag_value) {
  SearchBuilder *sb = new SearchBuilder(_db, _verbose);
  std::list<shared_ptr<File>> files =
      sb->add_tag_condition(tag_name, tag_value, "=")->build();
  return files;
}

std::list<shared_ptr<File>> Indexer::fetch_from_attribute(std::string attribute,
                                                          std::string value) {
  SearchBuilder *sb = new SearchBuilder(_db, _verbose);
  std::list<shared_ptr<File>> files =
      sb->add_condition(attribute, value, "=")->build();
  return files;
}