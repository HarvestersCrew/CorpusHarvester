#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDownloadBuilder::build(int number) const {
  list<shared_ptr<File>> res;

  // Throws an exception if a request API name is not found
  for (const auto &el : this->get_requests()) {
    ApiFactory::get_api(el.first);
  }

  for (const auto &el : this->get_requests()) {
    shared_ptr<api_loader> api = ApiFactory::get_api(el.first);

    list<shared_ptr<File>> partial_res =
        api->query_and_parse(el.second, this->dl);

    // Store the files
    sql::Connection *db = HarvesterDatabase::init();
    Storage storage(db);
    storage.store_files(partial_res);

    // Index the downloaded data
    Indexer indexer(db);
    indexer.indexation(partial_res);

    res.splice(res.end(), partial_res);
  }

  logger::info("Downloaded " + to_string(res.size()) +
               " new files with user request");

  return res;
}
