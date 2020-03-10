
#include "CommandLineInterface.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <typeinfo>

CommandLineInterface::CommandLineInterface(int argc, char **argv) {

  this->parser = argparse::ArgumentParser("Corpus Harvester");

  // Option for the creation of a corpus
  this->parser.add_argument("--create")
      .help("Create a new corpus")
      .default_value(false)
      .implicit_value(true);

  // Show the list of the corpus
  this->parser.add_argument("--corpus")
      .help("Show a corpus")
      .default_value(false)
      .implicit_value(true);

  // List of string at the end of the command to get the name of the corpus
  this->parser.add_argument("name").help("Name of the corpus").remaining();

  try {
    this->parser.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    logger::error(err.what());
    // TODO :: Need to change that
    std::cout << this->parser << std::endl;
    exit(0);
  }
}

void CommandLineInterface::create_api() {
  // TODO :: Creation of a new corpus
}

std::optional<Corpus *>
CommandLineInterface::search_corpus(const std::string name) {

  logger::debug("Search corpus : " + name);

  // Get the indexer
  Indexer indexer("harvester");
  indexer.create_database(false);

  sql::Connection *db = indexer.get_database();
  std::optional<Corpus *> corpus = Corpus::get_corpus_from_name(db, name);

  return corpus;
}

Corpus CommandLineInterface::create_corpus(const std::string name) {

  logger::info("Creation of " + name + "'s corpus in progress...");

  // Download corresponding data
  download_manager dl;
  api_loader twitter(std::string("data/twitter.json"),
                     std::string("data/twitter.env.json"));
  std::list<shared_ptr<File>> out = twitter.query_and_parse({{"q", name}}, dl);
  // api_loader tmdb(std::string("data/tmdb_poster.json"),
  //                 std::string("data/tmdb.env.json"));
  // std::list<shared_ptr<File>> out =
  //     tmdb.query_and_parse({{"query", "star wars"}}, dl);

  // Store the files
  Indexer indexer("harvester");
  indexer.create_database(true);
  Storage storage(indexer.get_database());
  storage.store_files(out);

  // Index the downloaded data
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
  indexer.close_database();

  return corpus;
}

std::list<Corpus *> CommandLineInterface::list_corpus() {

  Indexer indexer("harvester");
  sql::Connection *db = indexer.get_database();

  // TODO :: Problem here
  std::list<Corpus *> corpus = Corpus::get_all_corpuses(db);

  return corpus;
}

void CommandLineInterface::run() {

  // Creation of a corpus
  if (this->parser["--create"] == true) {

    std::string corpusName = "";

    try {
      auto names = this->parser.get<std::vector<std::string>>("name");
      for (auto &name : names)
        corpusName += name + " ";

      // Remove the last character, here a space
      corpusName.pop_back();

      // Create the corpus and show it
      Corpus corpus = this->create_corpus(corpusName);
      logger::info(corpus.to_string());

    } catch (std::logic_error &e) {
      logger::error("No name provide for the corpus.");
    }
  }
  // Show all the corpus
  else if (this->parser["--corpus"] == true) {

    // Check if we have a name else, we show all the corpus
    try {
      std::string corpusName = "";
      auto names = this->parser.get<std::vector<std::string>>("name");
      for (auto &name : names)
        corpusName += name + " ";

      // Remove the last character, here a space
      corpusName.pop_back();

      // Search for the corpus with the given name
      std::optional<Corpus *> optionalCorpus = this->search_corpus(corpusName);

      // Check if we have a corpus
      if (optionalCorpus.has_value()) {
        logger::info(optionalCorpus.value()->header_string());
      } else {
        logger::info("No corpus have been found for the name : " + corpusName);
      }

    } catch (std::logic_error &e) {
      logger::info("List of all the corpus.");

      std::list<Corpus *> corpusList = this->list_corpus();
      logger::info("Number of available corpus : " + corpusList.size());

      for (const Corpus *corpus : corpusList) {
        logger::info(corpus->header_string());
      }
    }
  }
}
