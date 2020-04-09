#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDownloadBuilder::build(unsigned int number) const {

  list<shared_ptr<File>> res;
  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);
  Storage storage(db);

  // Throws an exception if a request API name is not found
  for (const auto &el : this->get_requests()) {
    ApiFactory::get_api(el.first);
  }

  vector<pair<string, unordered_map<string, string>>> requests(
      this->get_requests());
  long unsigned int downloaded_this_turn;

  // For each specified request
  for (auto it = requests.begin(); it != requests.end();) {

    // Get the api_loader
    shared_ptr<api_loader> api = ApiFactory::get_api(it->first);

    // Do the query
    list<shared_ptr<File>> downloaded =
        api->query_and_parse(it->second, this->dl);

    // If the returned download is empty, it means there is an error or there is
    // nothing more to find, so we remove it from our local requests
    if (downloaded.size() > 0) {
      ++it;
    } else {
      it = requests.erase(it);
    }

    // Only choose the files not present in the DB
    list<shared_ptr<File>> partial_res;
    for (const auto &el : downloaded) {
      if (!el->api_id_exists(HarvesterDatabase::init())) {
        partial_res.push_back(el);
      }
    }

    // Store the files
    storage.store_files(partial_res);

    // Index the downloaded data
    indexer.indexation(partial_res);

    // Add freshly downloaded data to the return list
    res.splice(res.end(), partial_res);
  }

  logger::info("Downloaded " + to_string(res.size()) +
               " new files with user request");

  return res;
}
