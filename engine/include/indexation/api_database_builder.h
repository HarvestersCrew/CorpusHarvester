#ifndef API_DATABASE_BUILDER_H
#define API_DATABASE_BUILDER_H

#include "database/pool_db.h"
#include "download/api_loader.h"
#include "indexation/indexer.h"
#include "utils/api_request_builder.h"
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::list;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::vector;

/**
 * Class to handle database searches
 *
 * Requests only: will only retrieve what is specified by the request
 * Type only: will only retrive files belonging to API of these types
 * Both: will only query requests of given types
 */
class ApiDatabaseBuilder : public ApiRequestBuilder {
public:
  ApiDatabaseBuilder();

  virtual list<shared_ptr<File>> build(unsigned int number) const;

  virtual void add_request_parameter(long unsigned int request_id,
                                     const string &param_name,
                                     const string &param_value,
                                     const string &op);
};

#endif
