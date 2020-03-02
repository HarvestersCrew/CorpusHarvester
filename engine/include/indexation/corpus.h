#ifndef CORPUSHARVESTER_CORPUS_H
#define CORPUSHARVESTER_CORPUS_H

#include <list>

#include "file.h"

#define CORPUS_CREATE_STATEMENT                                                \
  "CREATE TABLE IF NOT EXISTS Corpus(id INTEGER NOT NULL "                     \
  "AUTO_INCREMENT,title "                                                      \
  "TEXT NOT NULL,creation_date TEXT NOT NULL,filters TEXT NOT NULL, PRIMARY "  \
  "KEY (id));"
#define INSERT_CORPUS_STATEMENT                                                \
  "INSERT INTO Corpus (title, creation_date, filters) VALUES(?, ?, ?)"
#define DROP_CORPUS_STATEMENT "DROP TABLE IF EXISTS Corpus;"

#define CORPUS_FILES_CREATE_STATEMENT                                          \
  "CREATE TABLE IF NOT EXISTS CorpusFiles(corpus_id INTEGER NOT NULL, "        \
  "file_id INTEGER NOT NULL, FOREIGN KEY (corpus_id) "                         \
  "REFERENCES Corpus(id), FOREIGN KEY (file_id) REFERENCES File(id));"
#define INSERT_CORPUS_FILES_STATEMENT                                          \
  "INSERT INTO CorpusFiles (corpus_id, file_id) VALUES(?, ?)"
#define DROP_CORPUS_FILES_STATEMENT "DROP TABLE IF EXISTS CorpusFiles;"
#define GET_CORPUS_FILES_STATEMENT                                             \
  "SELECT f.* FROM CorpusFiles cf, File f WHERE cf.corpus_id = ? and "         \
  "cf.file_id = f.id;"

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
  std::list<File *> _files;

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
   * Creates a Corpus object
   * @param title the title of the corpus
   * @param creation_date the creation date of the corpus
   * @param files the corpus files
   * @param used_filters the corpus filters description
   * @param id the id of the corpus in the database
   */
  Corpus(std::string title, std::string creation_date, std::list<File *> files,
         std::string used_filters, int id = -1);

  /**
   * Default destructor
   */
  ~Corpus();

  /**
   * Fills the _files attribute fetching the files linked to this corpus in the
   * database
   * @param db the database
   */
  void fetch_files(sql::Connection *db);

  std::string to_string() const;

  void insert(sql::Connection *db);

  void fill_from_statement(sql::Connection *db, sql::ResultSet *res);

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
};

#endif // CORPUSHARVESTER_CORPUS_H
