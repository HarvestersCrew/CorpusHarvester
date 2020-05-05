
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
      "append", "Append the result to the corpus specified by the id.", false);
  createCorpus.add_option(
      "number", "Number of results per page (default 0 = unlimited).", false);
  createCorpus.add_option(
      "page", "Works in conjuction with number, to paginate the result.",
      false);
  createCorpus.add_option(
      "order",
      "Specifies the return order (api_asc, api_desc, size_asc, size_desc).",
      false);
  createCorpus.add_option(
      "type", "Specifies the type of data (image, text, video).", false);

  // Get all the api names
  vector<string> apiNames = ApiFactory::get_api_names();
  string sourceHelp = "Name of the source we want. (";
  stringstream res;

  if (!apiNames.empty()) {
    // Convert all but the last element to avoid a trailing ","
    copy(apiNames.begin(), apiNames.end(),
         std::ostream_iterator<string>(res, ", "));

    // Now add the last element with no delimiter
    res << apiNames.back();
  }

  sourceHelp += res.str() + ").";
  // TODO :: Remove the source option
  //  createCorpus.add_option("source", sourceHelp, false);

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
  listFiles.add_option("page", "Page to use, starts at 0.", false);
  listFiles.add_option("number", "Number of results per page.", false);
  listFiles.add_option("api", "Files only from this API.", false);
  listFiles.add_option("type", "Type of files to find.", false);
  listFiles.add_option("order",
                       "Display in the specified order (name asc/desc or date "
                       "asc/desc), values to pass to be defined.",
                       false);

  // Get a corpus based on his id
  filesCommand.add_option("id", "Search a file based on his id.", false);

  //
  // Settings
  //
  cli_command &settingsCommand =
      this->parser.add_command("settings", "Settings function.");

  // Logger
  cli_command &loggerSetting =
      settingsCommand.add_command("logger", "Display the logger settings.");
  loggerSetting.add_option("level",
                           "Sets the logger level to one of these : debug, "
                           "info, warning, error, none.",
                           false);
  loggerSetting.add_option(
      "output", "Sets the logger output to one of these: stdout, file.", false);
  loggerSetting.add_option(
      "output_path", "Sets the logger output path to the given one.", false);
  loggerSetting.add_option("clear", "Deletes the logfile if existing.", true);

  // Storage
  cli_command &storageSetting =
      settingsCommand.add_command("storage", "Display the storage root.");
  storageSetting.add_option(
      "migrate", "Migrate the storage to this absolute path.", false);

  //
  // Web search
  //
  cli_command &webCommand =
      this->parser.add_command("web", "All commands related to web queries.");

  cli_command &downloadWeb =
      webCommand.add_command("download", "Retrieve new files from the web.");
  downloadWeb.add_option(
      "number",
      "Will download up to this number (not precise, around it) or stop before "
      "if specified queries start returning nothing. If it isn't specified, "
      "will only do a single query on each request.",
      false);
  downloadWeb.add_option(
      "type",
      "Will intersect the specified requests with the specified types, can "
      "appear multiple times (text, image).",
      false);
  downloadWeb.add_option(
      "source",
      "Specifies a request on this source name, can also appear many times.",
      false);
  downloadWeb.add_option("param_name",
                         "Specifies a parameter on the nearest source on the "
                         "left, can also appear many times.",
                         false);

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

  // Check if we have a value for the name
  itSubCommand = this->string_inputs.find("name");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the name of the corpus
    string corpusName = itSubCommand->second;

    // Add a filter for the title
    filters.insert(std::pair<std::string, std::string>("title", corpusName));
  }

  // We want the list of all the corpus in the database
  std::list<shared_ptr<Corpus>> corpusList =
      mr.get_corpuses(filters, orderingMethod);

  logger::info("Number of available corpus : " +
               std::to_string(corpusList.size()));

  for (const auto corpus : corpusList) {
    logger::info(corpus->header_string());
  }

  exit(0);
}

void CommandLineInterface::corpus_create() {

  string corpusName;
  map<string, string> params;
  map<string, string>::iterator itSubCommand;
  ManagerRequest mr;
  string::size_type sz;

  // Check if we have a value for the name
  itSubCommand = this->string_inputs.find("name");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    // Get the name of the corpus
    corpusName = itSubCommand->second;
  } else {
    logger::error("We have no name !");
    exit(-1);
  }

  // Check if we have a value for the id
  itSubCommand = this->string_inputs.find("id");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the value of the id
    string idString = itSubCommand->second;
    int id = 0;
    // Check if the id is valid
    try {
      id = std::stoi(idString, &sz);
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input id is not an integer ! Please check your input.");
      exit(-1);
    }
    // Check if the input and the transform have the same size
    if (sz != idString.length()) {
      logger::error("The input id contains a non valid character. Please, "
                    "check your input.");
      exit(-1);
    } else if (id < 0) {
      logger::error("The value of the id is invalid.");
      exit(-1);
    } else {
      // Add the id in the parameter
      params.insert({"id", to_string(id)});
    }
  }

  // Check if we have a value for the number parameter
  itSubCommand = this->string_inputs.find("number");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the value of the number
    string numberString = itSubCommand->second;
    int number = 0;
    // Check if the number is valid
    try {
      number = std::stoi(numberString, &sz);
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input number is not an integer ! Please check your input.");
      exit(-1);
    }
    // Check if the input and the transform have the same size
    if (sz != numberString.length()) {
      logger::error("The input number contains a non valid character. Please, "
                    "check your input.");
      exit(-1);
    } else if (number < 0) {
      logger::error("The value of the number is invalid.");
      exit(-1);
    } else {
      // Add the number in the parameter
      params.insert({"number", to_string(number)});
    }
  } else {
    // Add default value for the number
    params.insert({"number", "0"});
  }

  // Check if we have a value for the page parameter
  itSubCommand = this->string_inputs.find("page");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the value of the page
    string pageString = itSubCommand->second;
    int page = 0;
    // Check if the page is valid
    try {
      page = std::stoi(pageString, &sz);
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input page is not an integer ! Please check your input.");
      exit(-1);
    }
    // Check if the input and the transform have the same size
    if (sz != pageString.length()) {
      logger::error("The input page contains a non valid character. Please, "
                    "check your input.");
      exit(-1);
    } else if (page < 0) {
      logger::error("The value of the page is invalid.");
      exit(-1);
    } else {
      // Add the page in the parameter
      params.insert({"page", to_string(page)});
    }
  } else {
    // Add default value for the page
    params.insert({"page", "0"});
  }

  // Check if the user wants a specific order
  itSubCommand = this->string_inputs.find("order");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    string order = itSubCommand->second;

    if (order == "api_asc") {
      params.insert({"order", "api_asc"});
    } else if (order == "api_desc") {
      params.insert({"order", "api_desc"});
    } else if (order == "size_asc") {
      params.insert({"order", "size_asc"});
    } else if (order == "size_desc") {
      params.insert({"order", "size_desc"});
    } else {
      logger::error("The value " + order +
                    " for the order attribute is not valid. Please check "
                    "with the different values present : \n" +
                    "- api_asc \n" + +"- api_desc \n" + +"- size_asc \n" +
                    +"- size_desc \n");
      exit(-1);
    }
  }

  // Check if we have a value for the type
  itSubCommand = this->string_inputs.find("type");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    string type = itSubCommand->second;

    if (type == "image" || type == "video" || type == "text") {
      params.insert({"type", type});
    } else {
      logger::error("The value " + type +
                    " for the type attribute is not valid. Please check "
                    "with the different values present : \n" +
                    "- image \n" + +"- text \n" + +"- video \n");
      exit(-1);
    }
  }

  // Get the source available
  vector<string> apiNames = ApiFactory::get_api_names();
  // Check the source
  for (const pair<string, string> &param : this->unspecified_inputs) {
    if (param.first == "source") {
      if (find(apiNames.begin(), apiNames.end(), param.second) ==
          apiNames.end()) {
        logger::error(
            "Le nom de la source n'est pas valide !\n Source invalide : " +
            param.second);
        exit(-1);
      }
    }
  }

  // Create the corpus and show it
  int corpusID = mr.create_corpus(corpusName, params, this->unspecified_inputs);
  logger::info(to_string(corpusID));
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
  ManagerRequest mr;
  vector<string> apis = mr.get_apis();

  std::vector<string> fileType{"image", "video", "text"};

  map<string, string>::iterator itSubCommand;

  int page = 0;
  int number = 100;
  string api = "";
  string type = "";
  Corpus::ordering_method orderingMethod = Corpus::ordering_method::NONE;

  std::map<string, string> filters;

  // Check if we have the parameter page
  itSubCommand = this->string_inputs.find("page");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    // Get the value of the page parameter
    string pageString = itSubCommand->second;
    string::size_type sz;

    try {
      page = std::stoi(pageString, &sz);
      if (page < 0) {
        logger::error("The input page is invalid. The value need to be >= 0.");
        exit(-1);
      }
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input page is not an integer ! Please check your input.");
      exit(-1);
    }
  } else {
    logger::warning("The parameter `page` don't have a value. The default "
                    "value will be apply (page = 0).");
    page = 0;
  }
  filters.insert(std::pair<std::string, std::string>("page", to_string(page)));

  // Check if we have the parameter number
  itSubCommand = this->string_inputs.find("number");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    // Get the value of the page parameter
    string numberString = itSubCommand->second;
    string::size_type sz;

    try {
      number = std::stoi(numberString, &sz);
      if (number <= 0) {
        logger::error("The input number is invalid. The value need to be > 0.");
        exit(-1);
      }
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input number is not an integer ! Please check your input.");
      exit(-1);
    }
  } else {
    logger::warning("The parameter `number` don't have a value. The default "
                    "value will be apply (number = 100).");
    number = 100;
  }
  filters.insert(
      std::pair<std::string, std::string>("number", to_string(number)));

  // Check if we have the parameter api
  itSubCommand = this->string_inputs.find("api");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    // Get the value of the page parameter
    api = itSubCommand->second;

    // Check if the api name exists in our list of apis
    if (find(apis.begin(), apis.end(), api) == apis.end()) {
      logger::error("The name of the api is not valid !");
      logger::info("Apis available :");
      for (string api : apis) {
        logger::info(api);
      }
      exit(-1);
    } else {
      // We add the api in the filter
      filters.insert(std::pair<std::string, std::string>("api", api));
    }
  }

  // Check if we have the parameter type
  itSubCommand = this->string_inputs.find("type");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    // Get the value of the page parameter
    type = itSubCommand->second;

    // Check if the api name exists in our list of type
    if (find(fileType.begin(), fileType.end(), type) == fileType.end()) {
      logger::error("The type name is not valid !");
      logger::info("Types available :");
      for (string fileT : fileType) {
        logger::info(fileT);
      }
      exit(-1);
    } else {
      filters.insert(std::pair<std::string, std::string>("type", type));
    }
  }

  // Check if the user wants a specific order
  itSubCommand = this->string_inputs.find("order");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    string order = itSubCommand->second;

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

  list<shared_ptr<File>> files = mr.get_files(filters, orderingMethod);
  logger::info("Number of files available : " + std::to_string(files.size()));

  for (const auto file : files) {
    cout << file->to_string() << endl;
  }

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

void CommandLineInterface::setting_logger() {
  ManagerRequest mr;

  map<string, string>::iterator itSubCommand;
  string level = "";
  string output = "";
  string output_path = "";

  logger::debug("Settings logger.");

  // Check if we have a value for level parameter
  itSubCommand = this->string_inputs.find("level");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    logger::debug("We got a level");
    level = itSubCommand->second;
    mr.set_logger_level(level);
  }
  // Check if we have a value for output parameter
  itSubCommand = this->string_inputs.find("output");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    logger::debug("We got an output");
    output = itSubCommand->second;
    mr.set_logger_output(output);
  }
  // Check if we have a value for output_path parameter
  itSubCommand = this->string_inputs.find("output_path");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    logger::debug("We got an output_path");
    output_path = itSubCommand->second;
    mr.set_logger_output_path(output_path);
  }
  // Clear the logger if parameter
  if (this->bool_inputs.find("clear")->second) {
    logger::debug("Clear the logger.");
    mr.clear_logfile();
  }

  exit(0);
}

void CommandLineInterface::setting_storage() {
  ManagerRequest mr;

  // Check if we have the migrate parameter
  map<string, string>::iterator itSubCommand =
      this->string_inputs.find("migrate");
  if (itSubCommand != this->string_inputs.end()) {
    logger::debug("Migration of the storage path.");
    mr.migrate_storage(itSubCommand->second);
  }

  // Display the storage path
  cout << "Storage path : " << mr.get_storage_path() << endl;
  exit(0);
}

void CommandLineInterface::setting_manager() {
  logger::debug("Settings method.");
  if (std::find(this->commands.begin(), this->commands.end(), "logger") !=
      this->commands.end()) {
    this->setting_logger();
  } else if (std::find(this->commands.begin(), this->commands.end(),
                       "storage") != this->commands.end()) {
    this->setting_storage();
  }

  exit(0);
}

void CommandLineInterface::web_download() {
  ManagerRequest mr;
  map<string, string>::iterator itSubCommand;

  logger::debug("Download web.");
  int number = 0;
  string type = "";
  string source;
  //  map<string, string> params;

  // Number parameter
  itSubCommand = this->string_inputs.find("number");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {

    // Get the value of number
    string numberString = itSubCommand->second;
    string::size_type sz;

    try {
      number = std::stoi(numberString, &sz);
    } catch (const std::invalid_argument &ia) {
      logger::error(
          "The input number is not an integer ! Please check your input.");
      exit(-1);
    }

    // Check if the input and the transform have the same size
    if (sz != numberString.length()) {
      logger::error("The input number contains a non valid character. Please, "
                    "check your input.");
      exit(-1);
    }
  }

  // Check if we have a value for the type
  itSubCommand = this->string_inputs.find("type");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    string typeRaw = itSubCommand->second;

    if (typeRaw == "image" || typeRaw == "text") {
      type = "type";
    } else {
      logger::error("The value " + typeRaw +
                    " for the type attribute is not valid. Please check "
                    "with the different values present : \n" +
                    "- image \n" + +"- text \n");
      exit(-1);
    }
  }

  // Check if we have a value for the source
  itSubCommand = this->string_inputs.find("source");
  if (itSubCommand != this->string_inputs.end() && itSubCommand->second != "") {
    // Get the source
    source = itSubCommand->second;

    // Check the source
    vector<string> apiNames = ApiFactory::get_api_names();
    if (find(apiNames.begin(), apiNames.end(), source) == apiNames.end()) {
      logger::error("Le nom de la source n'est pas valide ! ");
      exit(-1);
    }
  }

  // TODO :: Manage the param value

  // TODO :: Call with all that the api manager request
  // TODO :: Need to check which function
}

void CommandLineInterface::web_manager() {
  logger::debug("Web method.");
  if (std::find(this->commands.begin(), this->commands.end(), "download") !=
      this->commands.end()) {
    this->web_download();
  }
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
  } else if (std::find(this->commands.begin(), this->commands.end(),
                       "settings") != this->commands.end()) {
    this->setting_manager();
  } else if (std::find(this->commands.begin(), this->commands.end(), "web") !=
             this->commands.end()) {
    this->web_manager();
  }
}
