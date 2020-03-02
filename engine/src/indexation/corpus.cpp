#include "mysql_connection.h"
#include <cppconn/prepared_statement.h>

#include "indexation/corpus.h"
#include "indexation/file.h"

Corpus::Corpus() {}

Corpus::Corpus(string title, string creation_date, std::list<File *> files,
               string used_filters, int id)
    : DatabaseItem(id), _title(title), _creation_date(creation_date),
      _files(files), _used_filters(used_filters) {}

Corpus::Corpus(string title, string creation_date, int id)
    : DatabaseItem(id), _title(title), _creation_date(creation_date), _files(),
      _used_filters("") {}

Corpus::~Corpus() {
  for (auto &file : _files) {
    delete file;
  }
}

string Corpus::to_string() const {
  std::ostringstream out;
  out << "Corpus{_id=" << _id << ", _title=" << _title
      << ", _creation_date=" << _creation_date << ", _files=[\n";
  for (auto &file : _files) {
    out << file->to_string() << "\n";
  }
  out << "], _used_filters=" << _used_filters << "}";
  return out.str();
}

void Corpus::insert(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;

  prep_stmt = db->prepareStatement(INSERT_CORPUS_STATEMENT);
  prep_stmt->setString(1, _title);
  prep_stmt->setString(2, _creation_date);
  prep_stmt->setString(3, _used_filters);
  prep_stmt->execute();

  this->_id = DatabaseItem::get_last_inserted_id(db);
  for (const auto &file : _files) {
    prep_stmt = db->prepareStatement(INSERT_CORPUS_FILES_STATEMENT);
    prep_stmt->setInt(1, _id);
    prep_stmt->setInt(2, file->get_id());
    prep_stmt->execute();
  }
  delete prep_stmt;
}

void Corpus::fetch_files(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  prep_stmt = db->prepareStatement(GET_CORPUS_FILES_STATEMENT);
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    File *file = new File();
    file->fill_from_statement(db, res);
    _files.push_back(file);
  }
  delete res;
}

void Corpus::fill_attribute_from_statement(sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_title = res->getString("title");
  this->_creation_date = res->getString("creation_date");
  this->_used_filters = res->getString("filters");
}

void Corpus::fill_from_statement(sql::Connection *db, sql::ResultSet *res) {
  fill_attribute_from_statement(res);
  fetch_files(db);
}

std::list<Corpus *> Corpus::get_all_corpus(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  std::list<Corpus *> corpuses;

  prep_stmt = db->prepareStatement(GET_ALL_CORPUS);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    Corpus *corpus = new Corpus();
    corpus->fill_attribute_from_statement(res);
    corpuses.push_back(corpus);
  }
  delete res;
  return corpuses;
}