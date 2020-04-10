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
   * Retrieves the files, stores them and index them
   * @param number number of elements to retrieve. 0 does a single pass with
   * the given settings
   * @throw api_factory_name_not_found checks before downloading anything if all
   * APIs are found
   */
  virtual list<shared_ptr<File>> build(unsigned int number) const;
};

#endif
