#ifndef API_DOWNLAD_BUILDER_H
#define API_DOWNLAD_BUILDER_H

#include "utils/api_request_builder.h"
#include <list>
#include <memory>
#include <vector>

using std::list;
using std::shared_ptr;
using std::vector;

class ApiDownloadBuilder : public ApiRequestBuilder {

public:
  ApiDownloadBuilder();
  list<shared_ptr<File>> build();
};

#endif
