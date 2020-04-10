#ifndef API_DOWNLAD_BUILDER_H
#define API_DOWNLAD_BUILDER_H

#include "download/api_factory.h"
#include "download/api_loader.h"
#include "download/download_manager.h"
#include "indexation/file.h"
#include "indexation/indexer.h"
#include "storage/storage.h"
#include "utils/api_request_builder.h"
#include "utils/logger.h"
#include <list>
#include <memory>
#include <string>
#include <vector>

using std::list;
using std::shared_ptr;
using std::stoi;
using std::vector;

/**
 * Downloads a batch request
 * Prepare the requests by preparing this object with ApiRequestBuilder
 * interface
 * Then retrieves the file by calling the build() method
 *
 * For now, only specified requests are supported, no request by type
 */
class ApiDownloadBuilder : public ApiRequestBuilder {

private:
  download_manager dl;

public:
  ApiDownloadBuilder();

  virtual long unsigned int
  add_request(const string &api_name,
              const unordered_map<string, pair<string, string>> &params);

  virtual list<shared_ptr<File>> build(unsigned int number) const;

  virtual void add_request_parameter(long unsigned int request_id,
                                     const string &param_name,
                                     const string &param_value,
                                     const string &op);
};

#endif
