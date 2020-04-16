#include "indexation/file.h"
// Put here to avoid circuling inclusion, do not put in header
#include "database/pool_db.h"

File::File(std::string path, std::string name, int size, std::string source,
           std::string format, int id)
    : DatabaseItem(id), _path(path), _name(name), _size(size), _source(source),
      _format(format) {}

File::File() : DatabaseItem(-1) {}

std::string File::to_string() const {
  std::ostringstream out;
  out << "File{_id=" << _id << ", _path=" << _path << ", _name=" << _name
      << ", _size=" << _size << ", _source=" << _source
      << ", _format=" << _format << ", _tags=[\n\t";
  for (const auto &tag : _tags) {
    out << tag->to_string() << "\n\t";
  }
  out << "]}";
  return out.str();
}

bool File::api_id_exists(sql::Connection *db) {
  sql::Statement *stmt;
  sql::ResultSet *res;
  std::string api_id = get_tag_value("_api_id");
  bool api_id_exists = false;
  if (api_id == "") {
    return api_id_exists;
  }
  stmt = db->createStatement();
  res = stmt->executeQuery(GET_FILE_API_ID);

  std::string curr_api_id = "";
  std::string curr_source = "";
  while (res->next()) {
    curr_source = res->getString("source");
    curr_api_id = res->getString("value");
    if (curr_api_id == api_id && curr_source == _source) {
      api_id_exists = true;
    }
  }
  delete stmt;
  delete res;
  return api_id_exists;
}

bool File::insert() {
  auto con = PoolDB::borrow_from_pool();
  bool aie = api_id_exists(con.get());
  if (!aie) {
    sql::PreparedStatement *prep_stmt;

    prep_stmt = con->prepareStatement(INSERT_FILE_STATEMENT);
    prep_stmt->setString(1, _path);
    prep_stmt->setString(2, _name);
    prep_stmt->setInt(3, _size);
    prep_stmt->setString(4, _source);
    prep_stmt->setString(5, _format);
    prep_stmt->execute();
    delete prep_stmt;

    this->_id = DatabaseItem::get_last_inserted_id(con.get());
    PoolDB::unborrow_from_pool(con);
    for (const auto &tag : _tags) {
      tag->set_file_id(this->_id);
      tag->insert();
    }
  }
  return !aie;
}

void File::fetch_tags(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  prep_stmt = db->prepareStatement("SELECT * FROM Tag WHERE file_id = ?;");
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    Tag tag = Tag();
    tag.fill_from_statement(db, res);
    _tags.push_back(std::make_unique<Tag>(tag));
  }
  delete res;
}

void File::fill_from_statement(sql::Connection *db, sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_path = res->getString("path");
  this->_name = res->getString("name");
  this->_size = res->getInt("size");
  this->_source = res->getString("source");
  this->_format = res->getString("format");
  fetch_tags(db);
}

std::string File::get_extraction_metadata() {
  // TEMPORARY
  return _name;
}

void File::add_tag(std::string name, std::string value) {
  _tags.push_back(std::make_unique<Tag>(Tag(name, value)));
}

void File::set_content(std::string content) {
  _content = content;
  this->set_size(content.size() + 1);
}

void File::set_bin_content(std::vector<char> content) {
  _bin_content = content;
  this->set_size(content.size());
}

void File::store(const std::string &path) const {
  if (!this->get_binary()) {

    std::ofstream outfile(path);
    outfile << this->get_content_str() << std::endl;
    outfile.close();

  } else {

    std::vector<char> vec_content = this->get_content_bin();
    char buf[vec_content.size()];
    for (size_t i = 0; i < vec_content.size(); ++i) {
      buf[i] = vec_content.at(i);
    }
    std::ofstream outfile(path, std::ios::out | std::ios::binary);
    outfile.write(buf, vec_content.size());
    outfile.close();
  }
}

std::string File::get_tag_value(std::string name) {
  std::string value = "";
  for (auto &tag : _tags) {
    if (tag->get_name() == name) {
      value = tag->get_value();
      break;
    }
  }
  return value;
}

std::optional<shared_ptr<File>> File::get_file_from_id(sql::Connection *db,
                                                       const int id) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  // Get the file based on the id
  prep_stmt = db->prepareStatement(GET_FILE_FROM_ID);
  prep_stmt->setInt(1, id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  // Define by default an empty optional for the corpus
  std::optional<shared_ptr<File>> file;

  // If we have got a value, we put it in the variable
  while (res->next()) {
    file.emplace(new File());
    file.value()->fill_from_statement(db, res);
  }
  delete res;

  // Based on the result, we return an optional
  return file;
}
