#include "indexation/indexer.h"

list<string> Indexer::_stats_requests = {
    "SELECT COUNT(*) as stat FROM File;",
    "SELECT COUNT(*) as stat FROM Corpus;",
    "SELECT COUNT(*) as stat FROM File WHERE type=\"text\";",
    "SELECT COUNT(*) as stat FROM File WHERE type=\"image\";",
    "SELECT SUM(size) as stat FROM File;",
    "SELECT AVG(size) as stat FROM File WHERE type = \"text\";",
    "SELECT AVG(size) as stat FROM File WHERE type = \"image\";"};

Indexer::Indexer() {}

bool Indexer::insert_file(shared_ptr<File> file) const {
  bool inserted = file->insert();
  return inserted;
}

void Indexer::indexation(std::list<shared_ptr<File>> files) {
  for (shared_ptr<File> &file : files) {
    insert_file(file);
  }
}

void Indexer::save_corpus(Corpus &corpus) { corpus.insert(); }

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

SearchBuilder Indexer::get_search_builder() {
  SearchBuilder sb;
  return sb;
}

void Indexer::init_ith_member(int i, int value, db_statistics &stats) {
  if (i == 0) {
    stats.file_count = value;
  } else if (i == 1) {
    stats.corpus_count = value;
  } else if (i == 2) {
    stats.text_count = value;
  } else if (i == 3) {
    stats.image_count = value;
  } else if (i == 4) {
    stats.total_size = value;
  } else if (i == 5) {
    stats.average_text_size = value;
  } else if (i == 6) {
    stats.average_image_size = value;
  }
}

db_statistics Indexer::get_statistics() {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  db_statistics stats;
  auto con = PoolDB::borrow_from_pool();
  int i = 0;
  int value;
  for (string &request : _stats_requests) {
    prep_stmt = con->prepareStatement(request);
    res = prep_stmt->executeQuery();
    res->next();
    delete prep_stmt;
    value = res->getInt("stat");
    init_ith_member(i, value, stats);
    delete res;
    i++;
  }

  PoolDB::unborrow_from_pool(con);
  return stats;
}
