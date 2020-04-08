#ifndef API_DOWNLAD_BUILDER_H
#define API_DOWNLAD_BUILDER_H

#include "download/api_factory.h"
#include "download/api_loader.h"
#include "download/download_manager.h"
#include "indexation/file.h"
#include "utils/api_request_builder.h"
#include <list>
#include <memory>
#include <vector>

using std::list;
using std::shared_ptr;
using std::vector;

class ApiDownloadBuilder : public ApiRequestBuilder {

private:
  download_manager dl;

public:
  ApiDownloadBuilder();

  /**
   * Retrieves the file
   * @throw api_factory_name_not_found checks before downloading anything if all
   * APIs are found
   */
  list<shared_ptr<File>> build();
};

#endif
