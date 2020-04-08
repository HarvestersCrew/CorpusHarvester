
#include "api/ManagerRequest.h"
#include "argparse.hpp"
#include "download/api_loader.h"
#include "download/download_manager.h"
#include "indexation/corpus.h"
#include "indexation/file.h"
#include "indexation/indexer.h"
#include "indexation/search_builder.h"
#include "storage/storage.h"
#include "utils/cli_parser.h"
#include "utils/logger.h"
#include "utils/utils.h"

#include <iostream>
#include <list>
#include <map>

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

Corpus ManagerRequest::create_corpus(string name, string source) {

  logger::info("Creation of " + name + "'s corpus in progress...");

  ApiDownloadBuilder dl_builder;

  if (source == "twitter") {
    logger::info("[*] Source Twitter OK");
    dl_builder.add_request("Twitter",
                           unordered_map<string, string>({{"q", name}}));
  } else if (source == "tmdb") {
    logger::info("[*] Source TMDB OK");
    // TODO :: Use the proper dl_builder
  } else {
    logger::info("[*] Default source used !");
    dl_builder.add_request("Twitter",
                           unordered_map<string, string>({{"q", name}}));
  }

  std::list<shared_ptr<File>> out = dl_builder.build();

  // Store the files
  sql::Connection *db = HarvesterDatabase::init();
  Storage storage(db);
  storage.store_files(out);

  // Index the downloaded data
  Indexer indexer(db);
  indexer.indexation(out);

  // Request files which has at least one retweet and one favorite
  SearchBuilder sb = indexer.get_search_builder();
  std::list<shared_ptr<File>> tweets =
      sb.add_tag_condition("retweet", "100", ">")
          ->logical_and()
          ->add_condition("id", "50", "<")
          ->build();

  // Create our corpus from the fetch data and save it
  std::string now = get_current_time("%d-%m-%Y %H:%M:%S");
  Corpus corpus("50 premiers avec retweets > 100", now, tweets, "");
  indexer.save_corpus(corpus);
  HarvesterDatabase::close();

  return corpus;
}
