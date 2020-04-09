#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> ApiDownloadBuilder::build(unsigned int number) const {

  list<shared_ptr<File>> res;
  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);
  Storage storage(db);

  vector<pair<shared_ptr<api_loader>, unordered_map<string, string>>> requests(
      this->get_requests());

  // Throws an exception if a request API name is not found
  for (const auto &el : this->get_requests()) {
    // If no page is specified in the parameters, we will check if the API has a
    // page parameter and load its default value (or 1) to init the page turning
    // if (el.second.find("_page") == el.second.end()) {
    //   if(el.f)
    // }
  }

  unsigned int page = 0;

  do {
    // For each specified request
    for (auto it = requests.begin(); it != requests.end();) {

      // Do the query
      list<shared_ptr<File>> downloaded =
          it->first->query_and_parse(it->second, this->dl);

      // If the returned download is empty, it means there is an error or there
      // is nothing more to find, so we remove it from our local requests
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

    ++page;

  } while (number > 0 && res.size() < number && requests.size() > 0);

  logger::info("Downloaded " + to_string(res.size()) +
               " new files with user request");

  return res;
}
