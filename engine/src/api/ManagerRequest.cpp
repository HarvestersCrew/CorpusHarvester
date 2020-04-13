#include "api/ManagerRequest.h"

/*
 * ------------------------------------------
 * METHODS RELATING TO CORPUS MANAGEMENT
 * ------------------------------------------
 */

std::list<shared_ptr<Corpus>> ManagerRequest::visualisation_corpus(
    std::map<std::string, std::string> &filters,
    std::map<std::string, std::string> &orders) {

  // Define the string available for the research
  std::string title, latest, oldest;
  std::string date, alphabetical;

  std::map<std::string, std::string>::iterator it;

  std::list<shared_ptr<Corpus>> corpuses;

  // Get the indexer
  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);

  // Get the filters available
  if (!filters.empty()) {
    // Check if we have a value for the title
    it = filters.find("title");
    if (it != filters.end()) {
      title = it->second;

      // logger::debug("Search corpus : " + title);
      // std::optional<shared_ptr<Corpus>> optionalCorpus =
      //     Corpus::get_corpus_from_title(db, title);

      // if (optionalCorpus.has_value()) {
      //   corpuses.push_back(optionalCorpus.value());
      // } else {
      //   logger::info("No corpus have been found for the name : " + title);
      // }
    }

  } else {
    logger::debug("Get all corpuses : ");
    // TODO :: Problem here
    corpuses = Corpus::get_all_corpuses(db);
  }

  // Get the orders available
  if (!orders.empty()) {
    // Check if we have a value for the date
    it = orders.find("date");
    if (it != orders.end()) {
      date = it->second;
    }

    // Check if we have a value for the latest
    it = orders.find("alphabetical");
    if (it != orders.end()) {
      alphabetical = it->second;
    }
  }

  return corpuses;
}

list<shared_ptr<Corpus>> ManagerRequest::get_all_corpuses() const {
  return Corpus::get_all_corpuses(HarvesterDatabase::init());
}

list<shared_ptr<Corpus>>
ManagerRequest::get_corpus_from_name(std::string name) {
  logger::debug("Search corpus : " + name);
  return Corpus::get_corpus_from_name(HarvesterDatabase::init(), name);
}

std::optional<shared_ptr<Corpus>>
ManagerRequest::get_corpus_from_id(const int id) {
  logger::debug("Search corpus with id = " + std::to_string(id));
  return Corpus::get_corpus_from_id(HarvesterDatabase::init(), id);
}

int ManagerRequest::create_corpus(const string &name,
                                  const list<shared_ptr<File>> &files,
                                  const optional<ApiDatabaseBuilder> &builder) {
  Corpus corpus;
  if (builder.has_value()) {
    corpus = Corpus(name, files, builder.value().serialize().dump());
  } else {
    corpus = Corpus(name, files, "");
  }
  corpus.insert(HarvesterDatabase::init());
  return corpus.get_id();
}

/*
 * ------------------------------------------
 * METHODS RELATING TO WEB APIS INFORMATIONS
 * ------------------------------------------
 */

vector<string> ManagerRequest::get_apis() {
  return ApiFactory::get_api_names();
}

const vector<shared_ptr<const api_loader>>
ManagerRequest::get_api_loaders() const {
  return ApiFactory::get_api_loaders();
}

const vector<shared_ptr<api_parameter_request>> &
ManagerRequest::get_api_web_parameters(const string &api_name) {
  return ApiFactory::get_api(api_name)->get_request_parameters();
}

const vector<shared_ptr<api_parameter_response>> &
ManagerRequest::get_api_db_parameters(const string &api_name) {
  return ApiFactory::get_api(api_name)->get_response_parameters();
}

/*
 * ------------------------------------------
 * METHODS RELATING TO API BUILDERS
 * ------------------------------------------
 */

ApiRequestBuilder &ManagerRequest::api_builder_get_based_on_bool(bool is_web) {
  if (is_web) {
    return this->_dl_builder;
  } else {
    return this->_db_builder;
  }
}

void ManagerRequest::api_builder_clear(bool is_web) {
  ApiRequestBuilder &builder = this->api_builder_get_based_on_bool(is_web);
  builder.clear_requests();
  builder.clear_types();
}

long unsigned int
ManagerRequest::api_builder_add_request(bool is_web,
                                        const std::string &api_name) {
  ApiRequestBuilder &builder = this->api_builder_get_based_on_bool(is_web);
  return builder.add_request(api_name);
}

void ManagerRequest::api_builder_add_request_parameter(
    bool is_web, long unsigned int request_id, const string &param_name,
    const string &param_value, const string &op) {
  ApiRequestBuilder &builder = this->api_builder_get_based_on_bool(is_web);
  builder.add_request_parameter(request_id, param_name, param_value, op);
}

list<shared_ptr<File>> ManagerRequest::api_builder_build(bool is_web,
                                                         unsigned int number) {
  ApiRequestBuilder &builder = this->api_builder_get_based_on_bool(is_web);
  return builder.build(number);
}
