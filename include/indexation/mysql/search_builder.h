#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include <list>
#include <sqlite3.h>
#include <string>

using std::list;
using std::string;

class SearchBuilder {

  /* Final statement to execute*/
  string _statement;
  /* SELECT clause that handles every conditions about File attributes*/
  string _firstSelect;
  /* Used to process the current clause */
  string _currentClause;
  /* List to store the values of the first SELECT clause*/
  list<string> _firstPreparedValues;
  /* List to store the values of the other clauses*/
  list<string> _preparedValues;
  /* Boolean used to know if the current clause to process is only composed of
   * conditions about File attributes */
  bool _currentClauseOnlyOnFile;
  bool _verbose;

private:
  void validCurrentClause(bool build);

public:
  SearchBuilder(bool verbose = false);

  SearchBuilder *fileTagEquals(string tag_name, string tag_value, string op);

  SearchBuilder *fileColumnEquals(string column_name, string column_value,
                                  string op);

  SearchBuilder *sqlAnd();

  SearchBuilder *sqlOr();

  list<File *> build(sql::Connection *db);

  void clear();
};

#endif