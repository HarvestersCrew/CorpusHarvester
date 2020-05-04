#include "download/api_download_builder.h"

ApiDownloadBuilder::ApiDownloadBuilder() : ApiRequestBuilder() {}

list<shared_ptr<File>> &ApiDownloadBuilder::build(unsigned int number) {

  if (this->get_requests().size() == 0) {
    throw api_builder_feature_not_supported(
        "DL builder can't download without specified requests");
  }

  _latest_build.clear();
  list<shared_ptr<File>> &res = _latest_build;
  Storage storage;

  long unsigned int all_dl_size = 0;

  // Convert the requests without operators
  auto requests = this->get_no_op_requests(this->get_usable_requests());

  unsigned int page = 0;

  // Inits the page number for page compatible APIs (if not given in the
  // parameters)
  for (auto &el : requests) {
    // If no base page was given, we either get the default value for the _page
    // parameter or set it to 1 (or nothing at all if there is no _page
    // parameter)
    if (el.second.find("_page") == el.second.end()) {

      if (el.first->find_request_parameter("_page").has_value()) {
        const auto page_param =
            el.first->find_request_parameter("_page").value();
        unsigned int page_to_set =
            stoi(page_param->get_default_value().value_or("1"));
        el.second.emplace("_page", std::to_string(page_to_set));
      }
    }
  }

  do {
    // For each specified request
    for (auto it = requests.begin(); it != requests.end();) {

      // Update page if present
      if (it->second.find("_page") != it->second.end()) {
        it->second.at("_page") =
            std::to_string(stoi(it->second.at("_page")) + page);
      }

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

      all_dl_size += downloaded.size();

      // Only choose the files not present in the DB
      list<shared_ptr<File>> partial_res;
      for (const auto &el : downloaded) {
        if (!el->api_id_exists()) {
          partial_res.push_back(el);
        }
      }

      // Store the files
      storage.store_files(partial_res);

      // Index the downloaded data
      for (auto it = partial_res.begin(); it != partial_res.end();) {
        try {
          (*it)->insert();
          ++it;
        } catch (const std::exception &e) {
          logger::error("Error while parsing a file");
          logger::debug(e.what());
          it = partial_res.erase(it);
        }
      }

      // Add freshly downloaded data to the return list
      res.splice(res.end(), partial_res);
    }

    ++page;

  } while (number > 0 && res.size() < number && requests.size() > 0);

  logger::info("Downloaded " + std::to_string(all_dl_size) + " and inserted " +
               std::to_string(res.size()) + " new files with user request");

  return res;
}

void ApiDownloadBuilder::add_request_parameter(long unsigned int request_id,
                                               const string &param_name,
                                               const string &param_value,
                                               const string &op) {
  if (this->_requests.size() <= request_id) {
    throw api_builder_request_not_found(request_id);
  }
  if (op != "=") {
    throw api_builder_incompatible_operator(op, "download");
  }
  shared_ptr<api_loader> &api = this->_requests[request_id].first;
  if (!api->find_request_parameter(param_name).has_value()) {
    throw api_no_setting_exception(param_name);
  }
  ApiRequestBuilder::add_request_parameter(request_id, param_name, param_value,
                                           op);
}
