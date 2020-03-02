#include "indexation/file.h"

File::File(std::string path, std::string name, int size, std::string source,
           std::string format, int id)
    : DatabaseItem(id), _path(path), _name(name), _size(size), _source(source),
      _format(format) {}

File::File() : DatabaseItem(0) {}

File::~File() {
  for (auto &tag : _tags) {
    delete tag;
  }
}

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

void File::insert(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  std::list<File *> files;

  prep_stmt = db->prepareStatement(INSERT_FILE_STATEMENT);
  prep_stmt->setString(1, _path);
  prep_stmt->setString(2, _name);
  prep_stmt->setInt(3, _size);
  prep_stmt->setString(4, _source);
  prep_stmt->setString(5, _format);
  prep_stmt->execute();
  delete prep_stmt;

  this->_id = DatabaseItem::get_last_inserted_id(db);
  for (const auto &tag : _tags) {
    tag->set_file_id(this->_id);
    tag->insert(db);
  }
}

void File::fetch_tags(sql::Connection *db) {
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  prep_stmt = db->prepareStatement("SELECT * FROM Tag WHERE file_id = ?;");
  prep_stmt->setInt(1, _id);
  res = prep_stmt->executeQuery();
  delete prep_stmt;

  while (res->next()) {
    Tag *tag = new Tag();
    tag->fill_from_statement(db, res);
    _tags.push_back(tag);
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

void File::add_tag(std::string name, std::string value) {
  _tags.push_back(new Tag(name, value));
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
