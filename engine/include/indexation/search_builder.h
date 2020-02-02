#ifndef SEARCH_BUILDER_H
#define SEARCH_BUILDER_H

#include "indexation/file.h"
#include "indexation/request_builder.h"
#include "utils/utils.h"
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <list>
#include <regex>
#include <string>

using std::list;
using std::string;

class SearchBuilder : public RequestBuilder {

  /* Left part of the final statement to execute*/
  string _left_request;
  /* Right part of the final statement to execute*/
  string _right_request;
  /* SELECT clause that handles every conditions about File attributes*/
  string _firstSelect;
  /* Used to process the current clause */
  string _currentClause;
  /* List to store the values of the first SELECT clause*/
  list<string> _firstPreparedValues;
  /* List to store the values of the other clauses*/
  list<string> _preparedValues;
  /* List to store the values of the current clause*/
  list<string> _currentPreparedValues;
  /* Boolean used to know if the current clause to process is only composed of
   * conditions about File attributes */
  bool _currentClauseOnlyOnFile;
  bool _verbose;
  sql::Connection *_db;

private:
  void validCurrentClause();

public:
  SearchBuilder(sql::Connection *db, bool verbose = false);

  SearchBuilder *addCondition(string conditionName, string conditionValue,
                              string op);

  SearchBuilder *addTagCondition(string tagName, string tagValue, string op);

  SearchBuilder *logicalAnd();

  SearchBuilder *logicalOr();

  list<File *> build();

  void clear();
};

#endif
