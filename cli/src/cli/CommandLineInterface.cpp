
#include "cli/CommandLineInterface.h"
#include "download/api_factory.h"
#include "utils/cli_parser.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <typeinfo>

CommandLineInterface::CommandLineInterface(int argc, char **argv)
    : parser(argv[0], "Corpus Harvester") {

  //
  // Corpus function
  //
  cli_command &apiCommand = this->parser.add_command("apis", "Api function.");

  apiCommand.add_command("list", "Api list");

  apiCommand.add_option("name", "Name of the api.", false);
  apiCommand.add_option("db", "See all the parameters for the database.", true);
  apiCommand.add_option("web", "See all the parameters for the web request.",
                        true);

  //
  // Corpus function
  //
  cli_command &corpusCommand =
      this->parser.add_command("corpus", "Corpus function.");

  // Creation of the corpus
  cli_command &createCorpus =
      corpusCommand.add_command("create", "Create a corpus with a given name.");

  // Name of the corpus
  createCorpus.add_option("name", "Name of the new corpus.", false);
  createCorpus.add_option(
      "n_element", "Number of element you wanted in your corpus.", false);
  createCorpus.add_option("image", "Set if you want image in your corpus.",
                          true);
  createCorpus.add_option("video", "Set if you want video in your corpus.",
                          true);
  createCorpus.add_option("text", "Set if you want text in your corpus.", true);

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
  listCorpus.add_option("order", "Set the order of the output corpus.", false);

  // Get a corpus based on his id
  corpusCommand.add_option("id", "Id of a specific corpus.", false);

  //
  // Files search
  //
  cli_command &filesCommand =
      this->parser.add_command("files", "Files function.");

  // List of files
  cli_command &listFiles =
      filesCommand.add_command("list", "List of all the files.");

  // Get a corpus based on his id
  filesCommand.add_option("id", "Search a file based on his id.", false);

  // Transform our array to a vector of string
  std::vector<string> allArgs(argv + 1, argv + argc);

  // Get and store all the parameter
  try {
    std::tie(this->commands, this->string_inputs, this->bool_inputs,
             this->unspecified_inputs) =
        cli_parser::parse(this->parser, allArgs);
  } catch (const cli_parser_bad_parse_exception &e) {
    exit(EXIT_FAILURE);
  } catch (const cli_parser_help_asked_exception &e) {
    exit(EXIT_FAILURE);
  }
}

void CommandLineInterface::corpus_by_id() {
  logger::debug("Search corpus by his ID");

  // Check if we have a value for the id
  map<string, string>::iterator itSubCommand = this->string_inputs.find("id");

  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    logger::debug("We got an id");

    // Get the value of the id
    string idString = this->string_inputs.find("id")->second;
    string::size_type sz;
    int id = 0;

    try {
      id = std::stoi(idString, &sz);
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input id is not an integer ! Please check your input.");
      exit(-1);
    }

    // Check if the input and the transform have the same size
    if (sz == idString.length()) {
      logger::debug("The id is OK");

      // Search the corpus in our database
      ManagerRequest managerRequest;

      try {
        shared_ptr<Corpus> corpus = managerRequest.get_corpus_from_id(id);
        cout << corpus->header_string() << endl;
      } catch (const db_id_not_found &e) {
        logger::info("No corpus have been found for the id : " +
                     std::to_string(id));
      }
      exit(0);

    } else {
      logger::error("The input id contains a non valid character. Please, "
                    "check your input.");
      exit(-1);
    }

  } else {
    logger::debug("we have no id");
    exit(-1);
  }
}

void CommandLineInterface::corpus_list() {

  Corpus::ordering_method orderingMethod = Corpus::ordering_method::NONE;
  map<string, string>::iterator itSubCommand;
  map<string, string>::iterator itOrderCommand;
  ManagerRequest mr;

  // Check if we have a value for the name
  itSubCommand = this->string_inputs.find("name");

  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the name of the corpus
    string corpusName = itSubCommand->second;

    // Search for the corpus with the given name
    std::list<shared_ptr<Corpus>> corpuses =
        mr.get_corpus_from_name(corpusName);

    // Check if we have a corpus
    if (corpuses.size() > 0) {
      for (const auto &corpus : corpuses) {
        logger::info(corpus->header_string());
      }
    } else {
      logger::info("No corpus have been found for the name : " + corpusName);
    }

  } else {
    logger::info("List of all the corpus.");

    std::map<std::string, std::string> filters;

    // Check if the user wants a specific order
    itOrderCommand = this->string_inputs.find("order");
    if (itOrderCommand != this->string_inputs.end() &&
        itOrderCommand->second != "") {

      string order = itOrderCommand->second;

      if (order == "date_asc") {
        orderingMethod = Corpus::ordering_method::DATE_ASC;
      } else if (order == "date_desc") {
        orderingMethod = Corpus::ordering_method::DATE_DESC;
      } else if (order == "name_asc") {
        orderingMethod = Corpus::ordering_method::NAME_ASC;
      } else if (order == "name_desc") {
        orderingMethod = Corpus::ordering_method::NAME_DESC;
      } else {
        logger::error("The value " + order +
                      " for the order attribute is not valid. Please check "
                      "with the different values present : \n" +
                      "- date_asc \n" + +"- date_desc \n" + +"- name_asc \n" +
                      +"- name_desc \n");
        exit(-1);
      }
    }

    // Get the corpus from the database
    std::list<shared_ptr<Corpus>> corpusList =
        mr.get_corpuses(filters, orderingMethod);

    logger::info("Number of available corpus : " +
                 std::to_string(corpusList.size()));

    for (const auto corpus : corpusList) {
      logger::info(corpus->header_string());
    }
  }
}

void CommandLineInterface::corpus_create() {

  string source = "";
  vector<string> sources;
  map<string, string> params;
  map<string, string>::iterator itSubCommand;
  ManagerRequest mr;

  // Check if we have a value for the name
  itSubCommand = this->string_inputs.find("name");

  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

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
    map<string, string>::iterator itSource = this->string_inputs.find("source");

    if (itSource != this->string_inputs.end() && itSource->second != "") {
      // Get the source
      source = this->string_inputs.find("source")->second;

      // Check the source
      vector<string> apiNames = ApiFactory::get_api_names();
      if (find(apiNames.begin(), apiNames.end(), source) == apiNames.end()) {
        logger::error("Le nom de la source n'est pas valide ! ");
        // TODO :: End the program ? Add user confirmation.
        source = "";
        sources.push_back(source);
      } else {
        logger::debug("Source: " + source);
        sources.push_back(source);
      }
    }

    // Get the name of the corpus
    string corpusName = itSubCommand->second;

    // Create the corpus and show it
    // Corpus corpus =
    //     ManagerRequest::create_corpus(corpusName, sources, params);
    // logger::info(corpus.to_string());

  } else {
    logger::error("We have no name !");
    exit(-1);
  }
}

void CommandLineInterface::corpus_manager() {

  logger::debug("Corpus method.");

  // Check if we want to list the corpus
  if (std::find(this->commands.begin(), this->commands.end(), "list") !=
      this->commands.end()) {
    this->corpus_list();
  }

  // Check if the user want to create a corpus.
  if (std::find(this->commands.begin(), this->commands.end(), "create") !=
      this->commands.end()) {
    this->corpus_create();
  }

  // Check if the user want to get a specific corpus
  map<string, string>::iterator itSubCommand = this->string_inputs.find("id");
  if (itSubCommand != this->string_inputs.end()) {
    this->corpus_by_id();
  }
}

void CommandLineInterface::files_list() {
  // TODO ::
  exit(0);
}

void CommandLineInterface::files_by_id() {
  logger::debug("Search files by his ID");

  // Check if we have a value for the id
  map<string, string>::iterator itSubCommand = this->string_inputs.find("id");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the value of the id
    string idString = this->string_inputs.find("id")->second;
    string::size_type sz;
    int id = 0;

    try {
      id = std::stoi(idString, &sz);
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input id is not an integer ! Please check your input.");
      exit(-1);
    }

    // Check if the input and the transform have the same size
    if (sz == idString.length()) {
      logger::debug("The id is OK");

      // Search the corpus in our database
      ManagerRequest managerRequest;
      std::optional<shared_ptr<File>> file =
          managerRequest.get_file_from_id(id);

      // Check the answer
      if (file.has_value()) {
        cout << file.value()->to_string() << endl;
      } else {
        logger::info("No file have been found for the id : " +
                     std::to_string(id));
      }
      exit(0);

    } else {
      logger::error("The input id contains a non valid character. Please, "
                    "check your input.");
      exit(-1);
    }

  } else {
    logger::debug("we have no id");
    exit(-1);
  }
}

void CommandLineInterface::files_manager() {

  logger::debug("Files method.");

  // Check if we want to list the corpus
  if (std::find(this->commands.begin(), this->commands.end(), "list") !=
      this->commands.end()) {
    this->files_list();
  } else {
    map<string, string>::iterator itSubCommand = this->string_inputs.find("id");
    if (itSubCommand != this->string_inputs.end()) {
      this->files_by_id();
    }
  }
}

void CommandLineInterface::api_list() {
  logger::debug("List request.");

  ManagerRequest mr;
  vector<string> apis = mr.get_apis();

  std::cout << "Apis availables :" << std::endl;
  for (string api : apis) {
    std::cout << api << std::endl;
  }
  exit(0);
}

void CommandLineInterface::detail_api() {
  logger::debug("Detail api.");

  ManagerRequest mr;

  vector<string> apis = mr.get_apis();
  string apiNameUser = "";

  // Check if we have a value for the name
  map<string, string>::iterator itSubCommand = this->string_inputs.find("name");

  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    logger::debug("We got a name");

    // Get the value of the name
    apiNameUser = this->string_inputs.find("name")->second;

    // Check if the name exist in our list of apis
    if (find(apis.begin(), apis.end(), apiNameUser) == apis.end()) {
      logger::error("The name of the api is not valid !");
      logger::info("Apis available :");
      for (string api : apis) {
        logger::info(api);
      }
      exit(-1);
    }
  } else {
    // We have no name
    logger::error("Please precise the name of the api");
    exit(-1);
  }

  // Search for eventually subcommand
  if (this->bool_inputs.find("db")->second) {
    logger::debug("DB");
    if (apiNameUser == "") {
      for (string api : apis) {
        std::cout << "Database parameter for : " << api << "\n" << std::endl;
        vector<shared_ptr<api_parameter_response>> db_parameters =
            mr.get_api_db_parameters(api);
        for (shared_ptr<api_parameter_response> parameter : db_parameters) {
          std::cout << parameter.get()->to_string() << std::endl;
        }
        std::cout << "\n" << std::endl;
      }
    } else {
      std::cout << "Database parameter for : " << apiNameUser << "\n"
                << std::endl;
      // Get specific parameter from the name
      vector<shared_ptr<api_parameter_response>> db_parameters =
          mr.get_api_db_parameters(apiNameUser);
      for (shared_ptr<api_parameter_response> parameter : db_parameters) {
        std::cout << parameter.get()->to_string() << std::endl;
      }
      std::cout << "\n" << std::endl;
    }
  }

  if (this->bool_inputs.find("web")->second) {
    logger::debug("Web");
    if (apiNameUser == "") {
      for (string api : apis) {
        std::cout << "Web request parameter for : " << api << "\n" << std::endl;
        vector<shared_ptr<api_parameter_request>> db_parameters =
            mr.get_api_web_parameters(api);
        for (shared_ptr<api_parameter_request> parameter : db_parameters) {
          std::cout << parameter.get()->to_string() << std::endl;
        }
        std::cout << "\n" << std::endl;
      }
    } else {
      std::cout << "Web request for : " << apiNameUser << "\n" << std::endl;
      // Get specific parameter from the name
      vector<shared_ptr<api_parameter_request>> db_parameters =
          mr.get_api_web_parameters(apiNameUser);
      for (shared_ptr<api_parameter_request> parameter : db_parameters) {
        std::cout << parameter.get()->to_string() << std::endl;
      }
      std::cout << "\n" << std::endl;
    }
  }

  if (!this->bool_inputs.find("db")->second &&
      !this->bool_inputs.find("web")->second) {
    cout << "Please precise the action you want to have :" << endl;
    cout << "--db : See the database parameter of the api." << endl;
    cout << "--web : See the web parameter of the api." << endl;
  }

  exit(0);
}

void CommandLineInterface::api_manager() {
  logger::debug("Apis method.");
  if (std::find(this->commands.begin(), this->commands.end(), "list") !=
      this->commands.end()) {
    this->api_list();
  }

  // Check if we have a name
  map<string, string>::iterator itSubCommand = this->string_inputs.find("name");
  if (itSubCommand != this->string_inputs.end()) {
    this->detail_api();
  }

  exit(0);
}

void CommandLineInterface::run() {

  // Check if we have the corpus command
  if (std::find(this->commands.begin(), this->commands.end(), "corpus") !=
      this->commands.end()) {
    this->corpus_manager();
  } else if (std::find(this->commands.begin(), this->commands.end(), "files") !=
             this->commands.end()) {
    this->files_manager();
  } else if (std::find(this->commands.begin(), this->commands.end(), "apis") !=
             this->commands.end()) {
    this->api_manager();
  }
}
