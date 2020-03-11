#include "indexation/indexer.h"

Indexer::Indexer(sql::Connection *db) : _db(db) {}

bool Indexer::insert_file(shared_ptr<File> file) const {
  bool inserted = file->insert(_db);
  if (inserted) {
    logger::debug("Insertion of " + file->to_string() + " : OK");
  } else {
    logger::debug("The file wasn't inserted");
  }
  return inserted;
}

void Indexer::indexation(std::list<shared_ptr<File>> files) {
  for (shared_ptr<File> &file : files) {
    insert_file(file);
  }
}

void Indexer::save_corpus(Corpus &corpus) { corpus.insert(_db); }

std::list<shared_ptr<File>> Indexer::fetch_from_tag(std::string tag_name,
                                                    std::string tag_value) {
  SearchBuilder *sb = new SearchBuilder(_db);
  std::list<shared_ptr<File>> files =
      sb->add_tag_condition(tag_name, tag_value, "=")->build();
  return files;
}

std::list<shared_ptr<File>> Indexer::fetch_from_attribute(std::string attribute,
                                                          std::string value) {
  SearchBuilder *sb = new SearchBuilder(_db);
  std::list<shared_ptr<File>> files =
      sb->add_condition(attribute, value, "=")->build();
  return files;
}