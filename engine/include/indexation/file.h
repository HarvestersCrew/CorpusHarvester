#ifndef FILE_H
#define FILE_H

#include "database/database_item.h"
#include "indexation/tag.h"
#include "utils/nlohmann/json.hpp"
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <fstream>
#include <memory>
#include <mysql_connection.h>
#include <sstream>
#include <vector>

#define FILE_CREATE_STATEMENT                                                  \
  "CREATE TABLE IF NOT EXISTS File(id INTEGER NOT NULL AUTO_INCREMENT,path "   \
  "TEXT NOT NULL,name TEXT NOT NULL,size INTEGER, source TEXT NOT NULL, "      \
  "format TEXT NOT NULL, type TEXT NOT NULL, PRIMARY KEY (id));"
#define GET_FILES_FROM_TAG                                                     \
  "SELECT f.* FROM File f, Tag t WHERE f.id = t.file_id AND t.name "           \
  "= "                                                                         \
  "? AND "                                                                     \
  "t.value = ?;"
#define GET_FILE_API_ID                                                        \
  "SELECT f.source, t.value FROM File f, Tag t WHERE f.id = t.file_id AND "    \
  "t.name = "                                                                  \
  "'_api_id'"
#define INSERT_FILE_STATEMENT                                                  \
  "INSERT INTO File (path, name, size, source, format, type) VALUES(?, ?, ?, " \
  "?, ?, ?)"
#define DROP_FILE_STATEMENT "DROP TABLE IF EXISTS File;"
#define GET_FILE_FROM_ID "SELECT * FROM File WHERE id = ?"

using nlohmann::json;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

/**
 * File class describes a File table in the database
 */
class File : public DatabaseItem {

  /** The path of the file */
  string _path;

  /** The name of the file */
  string _name;

  /** The size of the file */
  int _size;

  /** The source of the file */
  string _source;

  /** The format of the file */
  string _format;

  /** The content of the file (not a database attribute) */
  string _content;

  /** The type of the file */
  string _type;

  /** Binary content of file */
  std::vector<char> _bin_content;

  /** Whether the content is binary */
  bool _binary = false;

  /** The tags describing this file */
  std::list<unique_ptr<Tag>> _tags;

public:
  /**
   * Default constructor
   */
  File();

  /**
   * Creates a File object
   * @param path the path of the file
   * @param name the name of the file
   * @param size the size of the file
   * @param source the source of the file
   * @param format the format of the file
   * @param type the type of the file
   * @param id the id of the file in the database
   */
  File(string path, string name, int size, string source, string format,
       string type, int id = -1);

  string to_string() const;

  /**
   * Checks if the api Id of this file exists
   */
  bool api_id_exists();

  bool insert();

  void fill_from_statement(sql::ResultSet *res);

  /**
   * Fills the _tags attribute fetching the tags linked to this file in the
   * database
   */
  void fetch_tags();

  /**
   * Gets the metadata with the corresponding format for extraction
   * return the formated metadata
   */
  string get_extraction_metadata();

  /**
   * Adds a tag in the list of tags
   * @param name the name of the tag
   * @param value the value of the tag
   */
  void add_tag(string name, string value);

  string get_tag_value(string name);

  string get_full_path() const { return _path + "/" + _name + _format; }
  string get_path() const { return _path; }
  string get_name() const { return _name; }
  string get_source() const { return _source; }
  string get_format() const { return _format; }
  bool get_binary() const { return _binary; }
  std::vector<char> get_content_bin() const { return _bin_content; }
  string get_content_str() const { return _content; }
  int get_size() const { return _size; }

  void set_format(string format) { _format = format; }
  void set_path(string path) { _path = path; }
  void set_name(string name) { _name = name; }
  void set_source(string source) { _source = source; }
  void set_binary(bool bin) { _binary = bin; }
  void set_content(string content);
  void set_bin_content(std::vector<char> content);
  void set_size(int size) { _size = size; }

  void store(const string &path) const;

  /**
   * Get a file based on his id
   *
   * @param id Id of the file.
   *
   * @return The desired file.
   */
  static std::optional<shared_ptr<File>> get_file_from_id(const int id);

  /**
   * Serialize a file to a JSON string
   */
  virtual json serialize() const;
};

#endif
