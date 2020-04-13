#ifndef CORPUSHARVESTER_CORPUS_H
#define CORPUSHARVESTER_CORPUS_H

#include <list>
#include <memory>

#include "file.h"
#include "storage/export_method.h"

#define CORPUS_CREATE_STATEMENT                                                \
  "CREATE TABLE IF NOT EXISTS Corpus(id INTEGER NOT NULL "                     \
  "AUTO_INCREMENT,title "                                                      \
  "TEXT NOT NULL,creation_date TIMESTAMP NOT NULL DEFAULT NOW(),filters TEXT " \
  "NOT NULL, PRIMARY "                                                         \
  "KEY (id));"
#define INSERT_CORPUS_STATEMENT                                                \
  "INSERT INTO Corpus (title, filters) VALUES(?, ?)"
#define DROP_CORPUS_STATEMENT "DROP TABLE IF EXISTS Corpus;"

#define CORPUS_FILES_CREATE_STATEMENT                                          \
  "CREATE TABLE IF NOT EXISTS CorpusFiles(corpus_id INTEGER NOT NULL, "        \
  "file_id INTEGER NOT NULL, FOREIGN KEY (corpus_id) "                         \
  "REFERENCES Corpus(id), FOREIGN KEY (file_id) REFERENCES File(id));"
#define INSERT_CORPUS_FILES_STATEMENT                                          \
  "INSERT INTO CorpusFiles (corpus_id, file_id) VALUES(?, ?)"
#define DROP_CORPUS_FILES_STATEMENT "DROP TABLE IF EXISTS CorpusFiles;"
#define GET_ALL_CORPUS "SELECT * FROM Corpus"
#define GET_CORPUS_FROM_NAME "SELECT * FROM Corpus WHERE title LIKE ?"
#define GET_CORPUS_FROM_ID "SELECT * FROM Corpus WHERE id = ?"
#define GET_ALL_CORPUS_FILES "SELECT * FROM CorpusFiles"
#define GET_CORPUS_FILES_STATEMENT                                             \
  "SELECT f.* FROM CorpusFiles cf, File f WHERE cf.corpus_id = ? and "         \
  "cf.file_id = f.id;"

using std::shared_ptr;

/**
 * Corpus class describes a Corpus table in the database
 */
class Corpus : DatabaseItem {
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
   * @param db the database
   */
  void fetch_files(sql::Connection *db);

  std::string to_string() const;

  std::string header_string() const;

  bool insert(sql::Connection *db);

  void fill_attribute_from_statement(sql::ResultSet *res);

  void fill_from_statement(sql::Connection *db, sql::ResultSet *res);

  std::string export_(ExportMethod *export_method);

  /**
   * Get the title of the corpus.
   * @return std::string the title of the corpus.
   */
  std::string get_title() const { return _title; }

  /**
   * Set a title to the corpus.
   * @param title std::string the new name of the copus.
   */
  void set_title(const std::string title) { _title = title; }

  bool has_file() { return !_files.empty(); }

  virtual int get_id() const { return this->_id; };

  /**
   * Get a list of all the corpus.
   *
   * @param db
   *
   * @return List of Corpus.
   */
  static std::list<shared_ptr<Corpus>> get_all_corpuses(sql::Connection *db);

  /**
   * Get a corpus based on his ID.
   *
   * @param db Database
   * @param id ID of the corpus
   *
   * @return Optional Contain a corpus if the we have a result.
   */
  static std::optional<shared_ptr<Corpus>>
  get_corpus_from_id(sql::Connection *db, const int id);

  /**
   * Search corpuses based on a string.
   *
   * @param db Database
   * @param str string to search
   *
   * @return List of found corpuses.
   */
  static std::list<shared_ptr<Corpus>>
  get_corpus_from_name(sql::Connection *db, const std::string str);
};

#endif // CORPUSHARVESTER_CORPUS_H
