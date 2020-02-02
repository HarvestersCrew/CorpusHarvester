#include "mysql_connection.h"
#include <cppconn/prepared_statement.h>

#include "indexation/corpus.h"
#include "indexation/file.h"

using std::ostringstream;
using std::string;

Corpus::Corpus() {}

Corpus::Corpus(string title, string creation_date, list<File *> files, int id)
    : DatabaseItem(id), _title(title), _creation_date(creation_date),
      _files(files) {}

Corpus::~Corpus() {
  for (auto &file : _files) {
    delete file;
  }
}

string Corpus::toString() const {
  ostringstream out;
  out << "Corpus{_id=" << _id << ", title=" << _title
      << ", _creation_date=" << _creation_date << "}";
  return out.str();
}

void Corpus::insert(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;

  prep_stmt = db->prepareStatement(INSERT_CORPUS_STATEMENT);
  prep_stmt->setString(1, _title);
  prep_stmt->setString(2, _creation_date);
  prep_stmt->execute();

  this->_id = DatabaseItem::getLastInsertedId(db);
  for (const auto &file : _files) {
    prep_stmt = db->prepareStatement(INSERT_CORPUS_FILES_STATEMENT);
    prep_stmt->setInt(1, _id);
    prep_stmt->setInt(2, file->getId());
    prep_stmt->execute();
  }
  delete prep_stmt;
}

void Corpus::fetchFiles(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  prep_stmt = db->prepareStatement(GET_CORPUS_FILES_STATEMENT);
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    File *file = new File();
    file->fillFromStatement(db, res);
    _files.push_back(file);
  }
  delete res;
}

void Corpus::fillFromStatement(sql::Connection *db, sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_title = res->getString("title");
  this->_creation_date = res->getString("creation_date");
  fetchFiles(db);
}
