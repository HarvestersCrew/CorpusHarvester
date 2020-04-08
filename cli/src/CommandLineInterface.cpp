
#include "CommandLineInterface.h"
#include "download/api_factory.h"
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

  // Get all the api names
  vector<string> apiNames = ApiFactory::get_api_names();
  string sourceHelp = "Name of the source we want. Available : \n";
  for (string apiName : apiNames) {
    sourceHelp += " - " + apiName + "\n";
  }

  createCorpus.add_option("source", sourceHelp, false);

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

void CommandLineInterface::run() {
  string source = "";
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
            ManagerRequest::getInstance().visualisation_corpus(corpusName);

        // Check if we have a corpus
        if (optionalCorpus.has_value()) {
          logger::info(optionalCorpus.value()->header_string());
        } else {
          logger::info("No corpus have been found for the name : " +
                       corpusName);
        }

      } else {
        logger::info("List of all the corpus.");

        std::map<std::string, std::string> filters, orders;

        std::list<Corpus *> corpusList =
            ManagerRequest::getInstance().visualisation_corpus(filters, orders);

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
          source = this->string_inputs.find("source")->second;

          // Check the source
          vector<string> apiNames = ApiFactory::get_api_names();
          if (find(apiNames.begin(), apiNames.end(), source) ==
              apiNames.end()) {
            logger::error("Le nom de la source n'est pas valide ! ");
            // TODO :: End the program ? Add confirmation user.
            source = "";
          } else {
            logger::debug("Source: " + source);
          }
        }

        // Get the name of the corpus
        string corpusName = itSubCommand->second;

        // Create the corpus and show it
        Corpus corpus =
            ManagerRequest::getInstance().create_corpus(corpusName, source);
        logger::info(corpus.to_string());

      } else {
        logger::error("We have no name !");
        exit(-1);
      }
    }
  }
}
