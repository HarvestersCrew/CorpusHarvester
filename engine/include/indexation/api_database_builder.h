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

#define DB_BUILDER_ORDER_NONE " ORDER BY id"
#define DB_BUILDER_ORDER_API_ASC " ORDER BY source"
#define DB_BUILDER_ORDER_API_DESC " ORDER BY source DESC"
#define DB_BUILDER_ORDER_SIZE_ASC " ORDER BY size"
#define DB_BUILDER_ORDER_SIZE_DESC " ORDER BY size DESC"

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
  enum ordering_method { NONE, API_ASC, API_DESC, SIZE_ASC, SIZE_DESC };

private:
  ordering_method _order;
  unordered_map<ordering_method, string> _order_queries;

  unsigned int _page;

public:
  ApiDatabaseBuilder();

  virtual json serialize() const;
  virtual void deserialize(const json &j);

  virtual list<shared_ptr<File>> build(unsigned int number) const;

  virtual void add_request_parameter(long unsigned int request_id,
                                     const string &param_name,
                                     const string &param_value,
                                     const string &op);

  /**
   * @throw api_builder_feature_not_supported if order not supported
   */
  virtual void set_order(ordering_method order);
  virtual ordering_method get_order() const;

  virtual void set_page(unsigned int page);
  virtual unsigned int get_page() const;

  virtual void clear_all();
};

#endif
