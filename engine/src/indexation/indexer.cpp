#include "indexation/indexer.h"

Indexer::Indexer(sql::Connection *db) : _db(db) {}

bool Indexer::insert_file(shared_ptr<File> file) const {
  auto con = PoolDB::borrow_from_pool();
  bool inserted = file->insert(con.get());
  PoolDB::unborrow_from_pool(con);
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

void Indexer::save_corpus(Corpus &corpus) {
  auto con = PoolDB::borrow_from_pool();
  corpus.insert(con.get());
  PoolDB::unborrow_from_pool(con);
}

std::list<shared_ptr<File>> Indexer::fetch_from_tag(std::string tag_name,
                                                    std::string tag_value) {
  SearchBuilder sb = get_search_builder();
  std::list<shared_ptr<File>> files =
      sb.add_tag_condition(tag_name, tag_value, "=")->build();
  return files;
}

std::list<shared_ptr<File>> Indexer::fetch_from_attribute(std::string attribute,
                                                          std::string value) {
  SearchBuilder sb = get_search_builder();
  std::list<shared_ptr<File>> files =
      sb.add_condition(attribute, value, "=")->build();
  return files;
}
