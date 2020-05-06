#include "indexation/file.h"
// Put here to avoid circuling inclusion, do not put in header
#include "database/pool_db.h"

File::File(string path, string name, int size, string source, string format,
           string type, int id)
    : DatabaseItem(id), _path(path), _name(name), _size(size), _source(source),
      _format(format), _type(type) {}

File::File() : DatabaseItem(-1) {}

string File::to_string() const {
  std::ostringstream out;
  out << "File{_id=" << _id << ", _path=" << _path << ", _name=" << _name
      << ", _size=" << _size << ", _source=" << _source
      << ", _format=" << _format << ", _type=" << _type << ", _tags=[\n\t";
  for (const auto &tag : _tags) {
    out << tag->to_string() << "\n\t";
  }
  out << "]}";
  return out.str();
}

bool File::api_id_exists() {
  sql::Statement *stmt;
  sql::ResultSet *res;
  string api_id = get_tag_value("_api_id");
  bool api_id_exists = false;
  if (api_id == "") {
    return api_id_exists;
  }

  auto con = PoolDB::borrow_from_pool();
  stmt = con->createStatement();
  res = stmt->executeQuery(GET_FILE_API_ID);
  PoolDB::unborrow_from_pool(con);

  string curr_api_id = "";
  string curr_source = "";
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
  bool aie = api_id_exists();
  if (!aie) {
    sql::PreparedStatement *prep_stmt;
    auto con = PoolDB::borrow_from_pool();

    prep_stmt = con->prepareStatement(INSERT_FILE_STATEMENT);
    prep_stmt->setString(1, _path);
    prep_stmt->setString(2, _name);
    prep_stmt->setInt(3, _size);
    prep_stmt->setString(4, _source);
    prep_stmt->setString(5, _format);
    prep_stmt->setString(6, _type);
    prep_stmt->execute();
    delete prep_stmt;

    this->_id = DatabaseItem::get_last_inserted_id(con);
    PoolDB::unborrow_from_pool(con);
    for (const auto &tag : _tags) {
      tag->set_file_id(this->_id);
      tag->insert();
    }
  }
  return !aie;
}

void File::fetch_tags() {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  auto con = PoolDB::borrow_from_pool();

  prep_stmt = con->prepareStatement("SELECT * FROM Tag WHERE file_id = ?;");
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    Tag tag = Tag();
    tag.fill_from_statement(res);
    _tags.push_back(std::make_unique<Tag>(tag));
  }
  delete res;
  PoolDB::unborrow_from_pool(con);
}

void File::fill_from_statement(sql::ResultSet *res) {
  this->_id = res->getInt("id");
  this->_path = res->getString("path");
  this->_name = res->getString("name");
  this->_size = res->getInt("size");
  this->_source = res->getString("source");
  this->_format = res->getString("format");
  this->_type = res->getString("type");
  fetch_tags();
}

string File::get_metadata_titles() { return "name,size,format,type,is_binary"; }

string File::get_metadata_values() {
  return _name + "," + std::to_string(_size) + "," + _format + "," + _type +
         "," + std::to_string(_binary);
}

void File::fill_extraction_tags(ExtractionApiTags &extraction_tags) {
  std::list<Tag> extraction_file_tags = extraction_tags.at(_source);
  for (auto &tag : _tags) {
    for (auto &extraction_tag : extraction_file_tags) {
      if (tag->get_name() == extraction_tag.get_name()) {
        extraction_tag.set_value(tag->get_value());
        break;
      }
    }
  }
  extraction_tags[_source] = extraction_file_tags;
}

void File::add_tag(string name, string value) {
  _tags.push_back(std::make_unique<Tag>(Tag(name, value)));
}

void File::set_content(string content) {
  _content = content;
  this->set_size(content.size() + 1);
}

void File::set_bin_content(std::vector<char> content) {
  _bin_content = content;
  this->set_size(content.size());
}

void File::store(const string &path) const {
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

string File::get_tag_value(string name) {
  string value = "";
  for (auto &tag : _tags) {
    if (tag->get_name() == name) {
      value = tag->get_value();
      break;
    }
  }
  return value;
}

std::optional<shared_ptr<File>> File::get_file_from_id(const int id) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;
  auto con = PoolDB::borrow_from_pool();

  // Get the file based on the id
  prep_stmt = con->prepareStatement(GET_FILE_FROM_ID);
  prep_stmt->setInt(1, id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;
  PoolDB::unborrow_from_pool(con);

  // Define by default an empty optional for the corpus
  std::optional<shared_ptr<File>> file;

  // If we have got a value, we put it in the variable
  while (res->next()) {
    file.emplace(new File());
    file.value()->fill_from_statement(res);
  }
  delete res;

  // Based on the result, we return an optional
  return file;
}

json File::serialize() const {
  json j = json::object();
  j["id"] = this->_id;
  j["path"] = this->_path;
  j["name"] = this->_name;
  j["size"] = this->_size;
  j["source"] = this->_source;
  j["format"] = this->_format;
  j["type"] = this->_type;
  j["tags"] = json::object();
  for (const auto &tag : _tags) {
    j.at("tags")[tag->get_name()] = tag->get_value();
  }
  return j;
}
