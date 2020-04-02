
#include "CommandLineInterface.h"
#include "utils/cli_parser.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <typeinfo>

CommandLineInterface::CommandLineInterface(int argc, char **argv)
    : parser(argv[0], "Corpus Harvester") {

  // Corpus function
  cli_command &corpusCommand =
      this->parser.add_command("corpus", "Corpus function.");

  // Creation of the corpus
  cli_command &createCorpus =
      corpusCommand.add_command("create", "Create a corpus with a given name.");

  // Name of the corpus
  createCorpus.add_option("name", "Name of the new corpus.", false);
  createCorpus.add_option("image", "Set if you want video in your corpus.",
                          true);
  createCorpus.add_option("video", "Set if you want video in your corpus.",
                          true);
  createCorpus.add_option("text", "Set if you want video in your corpus.",
                          true);

  createCorpus.add_option("source", "Name of the source we want.", false);

  // List of corpus
  cli_command &listCorpus = corpusCommand.add_command(
      "list", "List of all the corpus with potentially a name.");

  listCorpus.add_option("name", "Corpus name that you want to search.", false);

  // Transform our array to a vector of string
  std::vector<string> allArgs(argv + 1, argv + argc);

  // Get and store all the parameter
  try {
    std::tie(this->commands, this->string_inputs, this->bool_inputs) =
        cli_parser::parse(this->parser, allArgs);
  } catch (const cli_parser_bad_parse_exception &e) {
    exit(EXIT_FAILURE);
  } catch (const cli_parser_help_asked_exception &e) {
    exit(EXIT_FAILURE);
  }
}

void CommandLineInterface::create_api() {
  // TODO :: Creation of a new api
}

std::optional<Corpus *>
CommandLineInterface::search_corpus(const std::string name) {

  logger::debug("Search corpus : " + name);

  // Get the indexer
  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);

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

std::list<Corpus *> CommandLineInterface::list_corpus() {

  sql::Connection *db = HarvesterDatabase::init();
  Indexer indexer(db);

  // TODO :: Problem here
  std::list<Corpus *> corpus = Corpus::get_all_corpuses(db);

  return corpus;
}

void CommandLineInterface::run() {

  map<string, string>::iterator itSubCommand;

  // Check if we have the corpus command
  if (std::find(this->commands.begin(), this->commands.end(), "corpus") !=
      this->commands.end()) {
    logger::debug("Corpus method.");

    // Check if we want to list the corpus
    if (std::find(this->commands.begin(), this->commands.end(), "list") !=
        this->commands.end()) {

      // Check if we have a value for the name
      itSubCommand = this->string_inputs.find("name");

      if (itSubCommand != this->string_inputs.end() &&
          itSubCommand->second != "") {

        // Get the name of the corpus
        string corpusName = itSubCommand->second;

        // Search for the corpus with the given name
        std::optional<Corpus *> optionalCorpus =
            this->search_corpus(corpusName);

        // Check if we have a corpus
        if (optionalCorpus.has_value()) {
          logger::info(optionalCorpus.value()->header_string());
        } else {
          logger::info("No corpus have been found for the name : " +
                       corpusName);
        }

      } else {
        logger::info("List of all the corpus.");

        std::list<Corpus *> corpusList = this->list_corpus();
        logger::info("Number of available corpus : " + corpusList.size());

        for (const Corpus *corpus : corpusList) {
          logger::info(corpus->header_string());
        }
      }
    }

    // Check if the user want to create a corpus.
    if (std::find(this->commands.begin(), this->commands.end(), "create") !=
        this->commands.end()) {

      // Check if we have a value for the name
      itSubCommand = this->string_inputs.find("name");

      if (itSubCommand != this->string_inputs.end() &&
          itSubCommand->second != "") {

        if (this->bool_inputs.find("image")->second) {
          // TODO :: We got the "image" label
          logger::debug("We have the image label");
        }

        if (this->bool_inputs.find("video")->second) {
          // TODO :: We got the "video" label
          logger::debug("We have the video label");
        }

        if (this->bool_inputs.find("text")->second) {
          // TODO :: We got the "text" label
          logger::debug("We have the text label");
        }

        // Check if we have a value for the source
        map<string, string>::iterator itSource =
            this->string_inputs.find("source");

        if (itSource != this->string_inputs.end() && itSource->second != "") {
          logger::debug("We have a source");
          string source = this->string_inputs.find("source")->second;

          logger::debug("Source: " + source);

          // TODO :: Use the source
        }

        // Get the name of the corpus
        string corpusName = itSubCommand->second;

        // Create the corpus and show it
        Corpus corpus = this->create_corpus(corpusName);
        logger::info(corpus.to_string());

      } else {
        logger::error("We have no name !");
        exit(-1);
      }
    }
  }
}
