#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include <list>
#include <sqlite3.h>
#include <string>

using std::list;
using std::string;

class SearchBuilder {

  string _statement;
  string _firstSelect;
  bool _currentClauseOnlyOnFile;
  string _currentClause;
  list<string> _firstPreparedValues;
  list<string> _preparedValues;
  bool _verbose;

private:
  void validCurrentClause(bool build);

public:
  SearchBuilder(bool verbose = false);

  SearchBuilder *fileTagEquals(string tag_name, string tag_value);

  SearchBuilder *fileColumnEquals(string column_name, string column_value);

  SearchBuilder *sqlAnd();

  SearchBuilder *sqlOr();

  list<File *> build(sql::Connection *db);

  void clear();
};

#endif