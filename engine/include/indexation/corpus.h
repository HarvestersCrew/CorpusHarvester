#ifndef CORPUSHARVESTER_CORPUS_H
#define CORPUSHARVESTER_CORPUS_H

#include "database/pool_db.h"
#include "file.h"
#include "indexation/file.h"
#include "storage/export_method.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json.hpp"
#include <cppconn/prepared_statement.h>
#include <filesystem>
#include <list>
#include <memory>
#include <mysql_connection.h>
#include <optional>
#include <string>
#include <unordered_map>

#define CORPUS_CREATE_STATEMENT                                                \
  "CREATE TABLE IF NOT EXISTS Corpus(id INTEGER NOT NULL "                     \
  "AUTO_INCREMENT,title "                                                      \
  "TEXT NOT NULL,creation_date TIMESTAMP NOT NULL DEFAULT NOW(),filters TEXT " \
  "NOT NULL, extraction_path TEXT DEFAULT NULL, PRIMARY "                      \
  "KEY (id));"
#define INSERT_CORPUS_STATEMENT                                                \
  "INSERT INTO Corpus (title, filters) VALUES(?, ?)"
#define UPDATE_CORPUS_EXTRACTION_PATH                                          \
  "UPDATE Corpus SET extraction_path = ? WHERE id = ?;"
#define DROP_CORPUS_STATEMENT "DROP TABLE IF EXISTS Corpus;"

#define CORPUS_FILES_CREATE_STATEMENT                                          \
  "CREATE TABLE IF NOT EXISTS CorpusFiles(corpus_id INTEGER NOT NULL, "        \
  "file_id INTEGER NOT NULL, FOREIGN KEY (corpus_id) "                         \
  "REFERENCES Corpus(id) ON DELETE CASCADE, FOREIGN KEY (file_id) REFERENCES " \
  "File(id), UNIQUE "                                                          \
  "KEY(file_id, corpus_id));"
#define INSERT_CORPUS_FILES_STATEMENT                                          \
  "INSERT IGNORE INTO CorpusFiles (corpus_id, file_id) VALUES(?, ?)"
#define DROP_CORPUS_FILES_STATEMENT "DROP TABLE IF EXISTS CorpusFiles;"
#define GET_ALL_CORPUS "SELECT * FROM Corpus"
#define GET_CORPUS_FROM_NAME "SELECT * FROM Corpus WHERE title LIKE ?"
#define GET_CORPUS_FROM_ID "SELECT * FROM Corpus WHERE id = ?"
#define GET_ALL_CORPUS_FILES "SELECT * FROM CorpusFiles"
#define GET_CORPUS_FILES_STATEMENT                                             \
  "SELECT f.* FROM CorpusFiles cf, File f WHERE cf.corpus_id = ? and "         \
  "cf.file_id = f.id;"
#define CORPUS_ORDER_BY_TITLE_ASC " ORDER BY title ASC"
#define CORPUS_ORDER_BY_TITLE_DESC " ORDER BY title DESC"
#define CORPUS_ORDER_BY_DATE_ASC " ORDER BY creation_date ASC"
#define CORPUS_ORDER_BY_DATE_DESC " ORDER BY creation_date DESC"
#define DELETE_CORPUS_BY_ID "DELETE FROM Corpus WHERE id = ?;"

using nlohmann::json;
using std::list;
using std::optional;
using std::shared_ptr;
using std::string;
using std::unordered_map;

struct corpus_statistics {
  int file_count;
  int text_count;
  int image_count;
  int total_size;
};

/**
 * Corpus class describes a Corpus table in the database
 */
class Corpus : DatabaseItem {
public:
  enum ordering_method { NONE, NAME_ASC, NAME_DESC, DATE_ASC, DATE_DESC };

private:
  /**
   * The title of the corpus.
   */
  std::string _title;

  /**
   * The date of the creation of the corpus.
   */
  std::string _creation_date;

  /**
   * The list of all the data contained by the corpus.
   */
  std::list<shared_ptr<File>> _files;

  /**
   * A description of the filters that were used to create this corpus
   */
  std::string _used_filters;

  /**
   * The path where the corpus archive is stored
   */
  std::optional<std::string> _extraction_path;

  static unordered_map<ordering_method, string> _ordering_queries;

public:
  /**
   * Default constructor
   */
  Corpus();

  /**
   * Creates a Corpus object without files
   * @param title the title of the corpus
   * @param id the id of the corpus in the database
   */
  Corpus(std::string title, int id = -1);

  /**
   * Creates a Corpus object
   * @param title the title of the corpus
   * @param files the corpus files
   * @param used_filters the corpus filters description
   * @param id the id of the corpus in the database
   */
  Corpus(std::string title, std::list<shared_ptr<File>> files,
         std::string used_filters, int id = -1);

  /**
   * Fills the _files attribute fetching the files linked to this corpus in the
   * database
   */
  void fetch_files();

  std::string to_string() const;

  json serialize() const;

  /**
   * Gets some information about a corpus
   */
  std::string header_string() const;

  bool delete_();

  bool insert();

  void fill_from_statement(sql::ResultSet *res);

  void export_(ExportMethod::methods method);

  void add_files(const list<shared_ptr<File>> &files);

  void set_extraction_path(optional<string> path);
  optional<std::string> get_extraction_path() const { return _extraction_path; }

  unsigned int get_extraction_size() const;

  std::string get_title() const { return _title; }
  void set_title(const std::string title);
  virtual int get_id() const { return this->_id; };

  /**
   * Gets statistics of corpus
   * @return corpus_statistics structure
   */
  corpus_statistics get_statistics() const;

  /**
   * Deletes if it exists the extracted archive
   */
  void delete_associated_extraction();

  /**
   * Gets a list of all the corpus.
   *
   * @return List of Corpus.
   */
  static std::list<shared_ptr<Corpus>> get_all_corpuses(ordering_method order);

  /**
   * Gets a corpus based on his ID.
   *
   * @param id ID of the corpus
   *
   * @return Resulting corpus
   * @throw db_id_not_found if corpus wasn't found
   */
  static shared_ptr<Corpus> get_corpus_from_id(const int id);

  /**
   * Deletes a corpus based on his ID
   * @param id the ID of the corpus
   * @throw db_id_not_found if corpus wasn't found
   * @throw StorageFileDeletionException if something went wrong during deletion
   * @throw ExtractionPathMissingException if given path is empty
   */
  static void delete_from_id(const int id);

  /**
   * Searches corpuses based on a string.
   *
   * @param str string to search
   *
   * @return List of found corpuses.
   */
  static std::list<shared_ptr<Corpus>>
  get_corpus_from_name(const std::string str, ordering_method order);
};

#endif // CORPUSHARVESTER_CORPUS_H
