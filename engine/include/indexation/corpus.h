#ifndef CORPUSHARVESTER_CORPUS_H
#define CORPUSHARVESTER_CORPUS_H

#include <list>

#include "file.h"

#define CORPUS_CREATE_STATEMENT                                                \
  "CREATE TABLE IF NOT EXISTS Corpus(id INTEGER NOT NULL "                     \
  "AUTO_INCREMENT,title "                                                      \
  "TEXT NOT NULL,creation_date TEXT NOT NULL, PRIMARY KEY (id));"
#define INSERT_CORPUS_STATEMENT                                                \
  "INSERT INTO Corpus (title, creation_date) VALUES(?, ?)"
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

class Corpus : DatabaseItem {
private:
  /**
   * Title of the corpus.
   */
  std::string _title;

  /**
   * Date of the creation of the corpus.
   */
  std::string _creation_date;

  /**
   * List of all the data contained by the corpus.
   */
  std::list<File *> _files;

public:
  Corpus();

  Corpus(std::string title, std::string creation_date, list<File *> files,
         int id = -1);

  ~Corpus();

  void fetchFiles(sql::Connection *db);

  std::string toString() const;

  void insert(sql::Connection *db);

  void fillFromStatement(sql::Connection *db, sql::ResultSet *res);

  /**
   * Get the title of the corpus.
   *
   * @return std::string the title of the corpus.
   */
  std::string getTitle() const { return _title; }

  /**
   * Set a title to the corpus.
   *
   * @param title std::string the new name of the copus.
   */
  void setTitle(const std::string title) { _title = title; }
};

#endif // CORPUSHARVESTER_CORPUS_H
