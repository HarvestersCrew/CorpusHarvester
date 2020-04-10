#ifndef API_DATABASE_BUILDER_H
#define API_DATABASE_BUILDER_H

#include "database/harvester_database.h"
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

class ApiDatabaseBuilder : public ApiRequestBuilder {
public:
  ApiDatabaseBuilder();

  /**
   * Adds a new request
   * @param api_name Name of the API to use
   * @param params List of parameters to use for this request
   * @throw api_factory_name_not_found if the given API is not found
   * @throw api_no_setting_exception if a parameter isn't found
   */
  virtual void
  add_request(const string &api_name,
              const unordered_map<string, pair<string, string>> &params);

  /**
   * Fetches the list of files from the DB
   * @param number number of elements to retrieve, 0 means everything that
   * matches is retrieved
   */
  virtual list<shared_ptr<File>> build(unsigned int number) const;
};

#endif
