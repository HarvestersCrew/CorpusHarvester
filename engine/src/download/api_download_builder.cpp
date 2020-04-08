#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDownloadBuilder::build() {
  list<shared_ptr<File>> res;

  // Throws an exception if a request API name is not found
  for (const auto &el : this->get_requests()) {
    ApiFactory::get_api(el.first);
  }

  for (const auto &el : this->get_requests()) {
    shared_ptr<api_loader> api = ApiFactory::get_api(el.first);
    res.splice(res.end(), api->query_and_parse(el.second, this->dl));
  }

  return res;
}
