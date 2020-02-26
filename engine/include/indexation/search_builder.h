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

/**
 * SearchBuilder class provide a way to create an SQL statement with the buidler
 * design pattern using clauses method (set of ORs between ANDs)
 */
class SearchBuilder : public RequestBuilder {

  /**
   * Left part of the final statement to execute
   */
  string _left_request;

  /**
   * Right part of the final statement to execute
   */
  string _right_request;

  /**
   * SELECT clause that handles every conditions about File attributes
   */
  string _first_select;

  /**
   * Used to process the current clause
   */
  string _current_clause;

  /**
   * List to store the values of the first SELECT clause
   */
  list<string> _first_prepared_values;

  /**
   * List to store the values of the other clauses
   */
  list<string> _prepared_values;

  /**
   * List to store the values of the current clause
   */
  list<string> _current_prepared_values;

  /* Boolean used to know if the current clause to process is only composed of
   * conditions about File attributes */
  bool _current_clause_only_on_file;

  /**
   * A boolean to allow to print in the function of this class
   */
  bool _verbose;

  /**
   * A description of the filters applied in the search request
   */
  string _filters;

  /**
   * The database (as a connection)
   */
  sql::Connection *_db;

private:
  /**
   * Chose where to add the current clause
   */
  void valid_current_clause();

public:
  /**
   * Creates a SearchBuilder object
   * @param db the database
   * @param verbose true to allow prints in the function of this class, false
   * otherwise
   */
  SearchBuilder(sql::Connection *db, bool verbose = false);

  /**
   * Adds a condition to the request being built
   * @param condition_name the name of the condition
   * @param condition_value the value of the condition
   * @param op the logical operator between the name and the value
   * @return a pointer to itself
   */
  SearchBuilder *add_tag_condition(string tag_name, string tag_value,
                                   string op);

  SearchBuilder *add_condition(string condition_name, string condition_value,
                               string op);

  SearchBuilder *logical_and();

  SearchBuilder *logical_or();

  list<File *> build();

  void clear();

  string getFilters() { return _filters; }
};

#endif
