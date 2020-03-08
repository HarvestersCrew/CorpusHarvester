
#include "CommandLineInterface.h"
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
    std::cout << err.what() << std::endl;
    std::cout << this->parser;
    exit(0);
  }
}

void CommandLineInterface::create_api() {
  // TODO :: Creation of a new corpus
}

Corpus CommandLineInterface::create_corpus(const std::string name) {

  std::cout << "Creation of " << name << "'s corpus in progress..."
            << std::endl;

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
  Indexer indexer("harvester", false);
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

  Indexer indexer("harvester", 0);
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
      std::cout << corpus.to_string() << std::endl;

    } catch (std::logic_error &e) {
      std::cout << "No name provide for the corpus" << std::endl;
    }
  }
  // Show all the corpus
  else if (this->parser["--corpus"] == true) {
    std::cout << "List of all the corpus." << std::endl;
    std::list<Corpus *> corpusList = this->list_corpus();
    std::cout << "Number of available corpus : " << corpusList.size()
              << std::endl;

    for (const Corpus *corpus : corpusList) {
      std::cout << corpus->header_string() << std::endl;
    }
  }
}
