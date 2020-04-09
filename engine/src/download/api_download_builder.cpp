#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDownloadBuilder::build(int number) const {

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

  for (auto it = requests.begin(); it != requests.end();) {

    shared_ptr<api_loader> api = ApiFactory::get_api(it->first);

    list<shared_ptr<File>> downloaded =
        api->query_and_parse(it->second, this->dl);

    if (downloaded.size() > 0) {
      ++it;
    } else {
      it = requests.erase(it);
    }

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

    res.splice(res.end(), partial_res);
  }

  logger::info("Downloaded " + to_string(res.size()) +
               " new files with user request");

  return res;
}
