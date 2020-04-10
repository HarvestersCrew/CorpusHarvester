#include "api/ManagerRequest.h"

ApiDownloadBuilder ManagerRequest::_dl_builder = ApiDownloadBuilder();
ApiDatabaseBuilder ManagerRequest::_db_builder = ApiDatabaseBuilder();

std::list<Corpus *> ManagerRequest::visualisation_corpus(
    std::map<std::string, std::string> &filters,
    std::map<std::string, std::string> &orders) {

  // Define the string available for the research
  std::string title, latest, oldest;
  std::string date, alphabetical;

  std::map<std::string, std::string>::iterator it;

  std::list<Corpus *> corpuses;

  // Get the indexer
  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);

  // Get the filters available
  if (!filters.empty()) {
    // Check if we have a value for the title
    it = filters.find("title");
    if (it != filters.end()) {
      title = it->second;

      logger::debug("Search corpus : " + title);
      std::optional<Corpus *> optionalCorpus =
          Corpus::get_corpus_from_name(db, title);

      if (optionalCorpus.has_value()) {
        corpuses.push_back(optionalCorpus.value());
      } else {
        logger::info("No corpus have been found for the name : " + title);
      }
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

std::optional<Corpus *> ManagerRequest::visualisation_corpus(std::string name) {
  logger::debug("Search corpus : " + name);

  // Get the indexer
  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);

  std::optional<Corpus *> corpus = Corpus::get_corpus_from_name(db, name);

  return corpus;
}

Corpus
ManagerRequest::create_corpus(std::string name,
                              std::vector<std::string> sources,
                              std::map<std::string, std::string> &params) {

  // TODO :: At the moment, we manage only one source.
  string source = sources.front();

  logger::info("Creation of " + name + "'s corpus in progress...");

  ApiDownloadBuilder dl_builder;

  // if (source == "twitter") {
  //   logger::info("[*] Source Twitter OK");
  //   dl_builder.add_request("Twitter",
  //                          unordered_map<string, string>({{"q", name}}));
  // } else if (source == "TheMovieDB_Synopsis") {
  //   logger::info("[*] Source TMDB OK");
  //   // TODO :: Use the proper dl_builder
  // } else {
  //   logger::info("[*] Default source used !");
  //   dl_builder.add_request("Twitter",
  //                          unordered_map<string, string>({{"q", name}}));
  // }

  dl_builder.add_request(source, unordered_map<string, pair<string, string>>(
                                     {{"query", make_pair(name, "=")}}));

  dl_builder.build(0);

  Indexer indexer(HarvesterDatabase::init());

  ApiDatabaseBuilder db_builder;
  db_builder.add_request("Twitter", unordered_map<string, pair<string, string>>(
                                        {{"retweet", make_pair("30", ">")}}));
  std::list<shared_ptr<File>> tweets = db_builder.build(0);

  // Create our corpus from the fetch data and save it
  std::string now = get_current_time("%d-%m-%Y %H:%M:%S");
  Corpus corpus("tweets avec 0 retweet", now, tweets, "");
  indexer.save_corpus(corpus);
  HarvesterDatabase::close();

  return corpus;
}

/*
 * Methods relating to web APIs informations
 */

vector<string> ManagerRequest::get_apis() {
  return ApiFactory::get_api_names();
}

vector<shared_ptr<api_parameter_request>>
ManagerRequest::get_api_web_parameters(const string &api_name) {
  return ApiFactory::get_api(api_name)->get_request_parameters();
}
