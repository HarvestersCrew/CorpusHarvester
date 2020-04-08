#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDownloadBuilder::build() {
  list<shared_ptr<File>> res;
  return res;
}
