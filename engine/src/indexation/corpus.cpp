#include "indexation/corpus.h"

unordered_map<Corpus::ordering_method, string> Corpus::_ordering_queries = {
    {Corpus::ordering_method::DATE_ASC, CORPUS_ORDER_BY_DATE_ASC},
    {Corpus::ordering_method::DATE_DESC, CORPUS_ORDER_BY_DATE_DESC},
    {Corpus::ordering_method::NAME_ASC, CORPUS_ORDER_BY_NAME_ASC},
    {Corpus::ordering_method::NAME_DESC, CORPUS_ORDER_BY_NAME_DESC},
    {Corpus::ordering_method::NONE, ""}};

Corpus::Corpus() {}

Corpus::Corpus(std::string title, std::list<shared_ptr<File>> files,
               std::string used_filters, int id)
    : DatabaseItem(id), _title(title), _files(files),
      _used_filters(used_filters) {}

Corpus::Corpus(std::string title, int id)
    : DatabaseItem(id), _title(title), _files(), _used_filters("") {}

std::string Corpus::header_string() const {
  std::ostringstream out;
  out << "Corpus{_id=" << _id << ", _title=" << _title
      << ", _creation_date=" << _creation_date << "}";
  return out.str();
}

std::string Corpus::to_string() const {
  std::ostringstream out;
  out << "Corpus{_id=" << _id << ", _title=" << _title
      << ", _creation_date=" << _creation_date << ", _files=[\n";
  for (auto &file : _files) {
    out << file->to_string() << "\n";
  }
  out << "], _used_filters=" << _used_filters << "}";
  return out.str();
}

bool Corpus::insert(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;

  prep_stmt = db->prepareStatement(INSERT_CORPUS_STATEMENT);
  prep_stmt->setString(1, _title);
  prep_stmt->setString(2, _used_filters);
  prep_stmt->execute();

  this->_id = DatabaseItem::get_last_inserted_id(db);
  for (const auto &file : _files) {
    prep_stmt = db->prepareStatement(INSERT_CORPUS_FILES_STATEMENT);
    prep_stmt->setInt(1, _id);
    prep_stmt->setInt(2, file->get_id());
    prep_stmt->execute();
  }
  delete prep_stmt;
  return true;
}

void Corpus::fetch_files(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  prep_stmt = db->prepareStatement(GET_CORPUS_FILES_STATEMENT);
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    shared_ptr<File> file = std::make_shared<File>();
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

std::list<shared_ptr<Corpus>> Corpus::get_all_corpuses(sql::Connection *db,
                                                       ordering_method order) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  std::list<shared_ptr<Corpus>> corpuses;

  prep_stmt =
      db->prepareStatement(GET_ALL_CORPUS + _ordering_queries.at(order));
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    shared_ptr<Corpus> corpus(new Corpus());
    corpus->fill_attribute_from_statement(res);
    corpuses.push_back(corpus);
  }
  delete res;
  return corpuses;
}

std::optional<shared_ptr<Corpus>>
Corpus::get_corpus_from_id(sql::Connection *db, const int id) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  // Get the corpus based on the id
  prep_stmt = db->prepareStatement(GET_CORPUS_FROM_ID);
  prep_stmt->setInt(1, id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  // Define by default an empty optional for the corpus
  std::optional<shared_ptr<Corpus>> corpus;

  // If we have got a value, we put it in the variable
  while (res->next()) {
    corpus.emplace(new Corpus());
    corpus.value()->fill_attribute_from_statement(res);
  }
  delete res;

  // Based on the result, we return an optional
  return corpus;
}

std::list<shared_ptr<Corpus>>
Corpus::get_corpus_from_name(sql::Connection *db, const std::string str,
                             ordering_method order) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  std::list<shared_ptr<Corpus>> corpuses;

  prep_stmt =
      db->prepareStatement(GET_CORPUS_FROM_NAME + _ordering_queries.at(order));
  prep_stmt->setString(1, "%" + str + "%");
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    shared_ptr<Corpus> corpus(new Corpus());
    corpus->fill_attribute_from_statement(res);
    corpuses.push_back(corpus);
  }
  delete res;
  return corpuses;
}

string Corpus::export_(ExportMethod *export_method) {
  return export_method->compressed_export(_files, _title);
}
