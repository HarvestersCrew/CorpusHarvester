#include "indexation/corpus.h"

unordered_map<Corpus::ordering_method, string> Corpus::_ordering_queries = {
    {Corpus::ordering_method::DATE_ASC, CORPUS_ORDER_BY_DATE_ASC},
    {Corpus::ordering_method::DATE_DESC, CORPUS_ORDER_BY_DATE_DESC},
    {Corpus::ordering_method::NAME_ASC, CORPUS_ORDER_BY_TITLE_ASC},
    {Corpus::ordering_method::NAME_DESC, CORPUS_ORDER_BY_TITLE_DESC},
    {Corpus::ordering_method::NONE, ""}};

Corpus::Corpus() {}

Corpus::Corpus(std::string title, std::list<shared_ptr<File>> files,
               std::string used_filters, int id)
    : DatabaseItem(id), _title(title), _files(files),
      _used_filters(used_filters) {}

Corpus::Corpus(std::string title, int id)
    : DatabaseItem(id), _title(title), _files(), _used_filters(" ") {}

std::string Corpus::header_string() const {
  std::ostringstream out;
  out << "Corpus{_id=" << _id << ", _title=" << _title
      << ", _creation_date=" << _creation_date
      << ", _extraction_path=" << _extraction_path.value_or("none") << "}";
  return out.str();
}

std::string Corpus::to_string() const {
  std::ostringstream out;
  out << header_string() << ", _files=[\n";
  for (auto &file : _files) {
    out << file->to_string() << "\n";
  }
  out << "], _used_filters=" << _used_filters << "}";
  return out.str();
}

bool Corpus::insert() {
  sql::PreparedStatement *prep_stmt;
  auto con = PoolDB::borrow_from_pool();

  prep_stmt = con->prepareStatement(INSERT_CORPUS_STATEMENT);
  prep_stmt->setString(1, _title);
  prep_stmt->setString(2, _used_filters);
  prep_stmt->execute();

  this->_id = DatabaseItem::get_last_inserted_id(con);
  for (const auto &file : _files) {
    prep_stmt = con->prepareStatement(INSERT_CORPUS_FILES_STATEMENT);
    prep_stmt->setInt(1, _id);
    prep_stmt->setInt(2, file->get_id());
    prep_stmt->execute();
  }
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);
  return true;
}

void Corpus::fetch_files() {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  auto con = PoolDB::borrow_from_pool();

  prep_stmt = con->prepareStatement(GET_CORPUS_FILES_STATEMENT);
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    shared_ptr<File> file = std::make_shared<File>();
    file->fill_from_statement(res);
    _files.push_back(file);
  }
  PoolDB::unborrow_from_pool(con);
  delete res;
}

void Corpus::fill_from_statement(sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_title = res->getString("title");
  this->_creation_date = res->getString("creation_date");
  this->_used_filters = res->getString("filters");
  if (res->isNull("extraction_path")) {
    this->_extraction_path = std::nullopt;
  } else {
    this->_extraction_path = res->getString("extraction_path");
  }
  fetch_files();
}

void Corpus::set_title(const std::string title) {
  _title = title;
  if (_id != -1) {
    auto con = PoolDB::borrow_from_pool();
    sql::PreparedStatement *prep_stmt =
        con->prepareStatement("UPDATE Corpus SET title = ? WHERE id = ?;");
    prep_stmt->setString(1, title);
    prep_stmt->execute();
    delete prep_stmt;
    PoolDB::unborrow_from_pool(con);
  }
}

std::list<shared_ptr<Corpus>> Corpus::get_all_corpuses(ordering_method order) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  std::list<shared_ptr<Corpus>> corpuses;
  auto con = PoolDB::borrow_from_pool();

  prep_stmt =
      con->prepareStatement(GET_ALL_CORPUS + _ordering_queries.at(order));
  res = prep_stmt->executeQuery();
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);

  while (res->next()) {
    shared_ptr<Corpus> corpus(new Corpus());
    corpus->fill_from_statement(res);
    corpuses.push_back(corpus);
  }
  delete res;
  return corpuses;
}

shared_ptr<Corpus> Corpus::get_corpus_from_id(const int id) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  auto con = PoolDB::borrow_from_pool();

  // Get the corpus based on the id
  prep_stmt = con->prepareStatement(GET_CORPUS_FROM_ID);
  prep_stmt->setInt(1, id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);

  // Define by default an empty optional for the corpus
  shared_ptr<Corpus> corpus;

  // If we have got a value, we put it in the variable
  while (res->next()) {
    corpus = std::make_shared<Corpus>();
    corpus->fill_from_statement(res);
  }
  delete res;

  if (corpus == nullptr) {
    throw db_id_not_found();
  }

  // Based on the result, we return an optional
  return corpus;
}

std::list<shared_ptr<Corpus>>
Corpus::get_corpus_from_name(const std::string str, ordering_method order) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  std::list<shared_ptr<Corpus>> corpuses;
  auto con = PoolDB::borrow_from_pool();

  prep_stmt =
      con->prepareStatement(GET_CORPUS_FROM_NAME + _ordering_queries.at(order));
  prep_stmt->setString(1, "%" + str + "%");
  res = prep_stmt->executeQuery();
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);

  while (res->next()) {
    shared_ptr<Corpus> corpus(new Corpus());
    corpus->fill_from_statement(res);
    corpuses.push_back(corpus);
  }
  delete res;
  return corpuses;
}

void Corpus::export_(ExportMethod::methods method) {
  if (!_extraction_path || !std::filesystem::exists(_extraction_path.value())) {
    string new_path =
        ExportMethod::compressed_export(_files, std::to_string(_id), method);
    set_extraction_path(new_path);
  }
}

void Corpus::set_extraction_path(optional<string> path) {
  _extraction_path = path;
  if (_id != -1) {
    sql::PreparedStatement *prep_stmt;
    auto con = PoolDB::borrow_from_pool();
    prep_stmt = con->prepareStatement(UPDATE_CORPUS_EXTRACTION_PATH);
    if (_extraction_path) {
      prep_stmt->setString(1, _extraction_path.value());
    } else {
      prep_stmt->setNull(1, 0);
    }
    prep_stmt->setInt(2, _id);
    prep_stmt->executeQuery();
    delete prep_stmt;
    PoolDB::unborrow_from_pool(con);
  }
}
