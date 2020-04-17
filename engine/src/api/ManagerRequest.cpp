#include "api/ManagerRequest.h"

/*
 * ------------------------------------------
 * METHODS RELATING TO CORPUS MANAGEMENT
 * ------------------------------------------
 */

list<shared_ptr<Corpus>>
ManagerRequest::get_corpuses(std::map<string, string> &filters,
                             Corpus::ordering_method order) const {

  std::map<string, string>::iterator it;

  list<shared_ptr<Corpus>> corpuses;

  // Get the filters available
  if (!filters.empty()) {
    // Check if we have a value for the title
    it = filters.find("title");
    if (it != filters.end()) {
      string title = it->second;

      logger::debug("Search corpus : " + title);
      corpuses = Corpus::get_corpus_from_name(title, order);

      if (corpuses.size() == 0) {
        logger::info("No corpus have been found for the name : " + title);
      }
    }

  } else {
    logger::debug("Get all corpuses : ");
    corpuses = Corpus::get_all_corpuses(order);
  }

  return corpuses;
}

list<shared_ptr<Corpus>>
ManagerRequest::get_corpuses(std::map<string, string> &filters,
                             const string &order) const {
  Corpus::ordering_method order_parsed = Corpus::ordering_method::NONE;
  if (order == "date_asc") {
    order_parsed = Corpus::ordering_method::DATE_ASC;
  } else if (order == "date_desc") {
    order_parsed = Corpus::ordering_method::DATE_DESC;
  } else if (order == "name_asc") {
    order_parsed = Corpus::ordering_method::NAME_ASC;
  } else if (order == "name_desc") {
    order_parsed = Corpus::ordering_method::NAME_DESC;
  } else if (order != "none") {
    logger::warning("Ordering method for corpuses unsupported, using 'none'");
  }
  return this->get_corpuses(filters, order_parsed);
}

list<shared_ptr<Corpus>> ManagerRequest::get_corpuses() const {
  std::map<string, string> empty;
  return this->get_corpuses(empty, Corpus::ordering_method::NONE);
}

list<shared_ptr<Corpus>>
ManagerRequest::get_corpus_from_name(string name) const {
  logger::debug("Search corpus : " + name);
  logger::warning("Use the get_corpuses(std::map<string, string> &filters, "
                  "Corpus::ordering_method order). Add the possibility to "
                  "order the corpus and avoid future maintenance.");
  std::map<string, string> search{{"title", name}};
  return this->get_corpuses(search, Corpus::ordering_method::NONE);
}

shared_ptr<Corpus> ManagerRequest::get_corpus_from_id(const int id) const {
  logger::debug("Search corpus with id = " + std::to_string(id));
  return Corpus::get_corpus_from_id(id);
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
  corpus.insert();
  return corpus.get_id();
}

string ManagerRequest::export_corpus(const int id,
                                     ExportMethod::methods method) const {
  const auto corpus = this->get_corpus_from_id(id);
  corpus->export_(method);
  if (!corpus->get_extraction_path()) {
    throw manager_request_unhandled_exception(
        "No corpus exportation path even though there should be one");
  }
  return corpus->get_extraction_path().value();
}

string ManagerRequest::export_corpus(const int id, const string &method) const {
  ExportMethod::methods method_parsed = ExportMethod::methods::ZIP;
  if (method != "zip") {
    logger::warning("Export method not supported, using zip");
  }
  return this->export_corpus(id, method_parsed);
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

/*
 * ------------------------------------------
 * METHODS RELATING TO HARVESTER SETTINGS
 * ------------------------------------------
 */

tuple<string, string, string> ManagerRequest::get_logger_settings() const {
  tuple<string, string, string> res;
  get<0>(res) = logger::_level_strings.at(logger::get_level());
  get<1>(res) = logger::_output_strings.at(logger::get_output());
  get<2>(res) = logger::get_output_path();
  return res;
}

tuple<string, string, string>
ManagerRequest::set_logger_level(const string &level) {
  bool found = false;
  for (const auto &assoc : logger::_level_strings) {
    if (assoc.second == level) {
      found = true;
      logger::set_level(assoc.first);
      logger::save_to_db();
      break;
    }
  }
  if (!found)
    throw manager_request_invalid_parameter();
  return this->get_logger_settings();
}

tuple<string, string, string>
ManagerRequest::set_logger_output(const string &output) {
  bool found = false;
  for (const auto &assoc : logger::_output_strings) {
    if (assoc.second == output) {
      found = true;
      logger::set_output(assoc.first);
      logger::save_to_db();
      break;
    }
  }
  if (!found)
    throw manager_request_invalid_parameter();
  return this->get_logger_settings();
}

tuple<string, string, string>
ManagerRequest::set_logger_output_path(const string &output_path) {
  logger::set_output_path(output_path);
  logger::save_to_db();
  return this->get_logger_settings();
}

string ManagerRequest::get_storage_path() const {
  return Storage().get_root_folder_name();
}

string ManagerRequest::migrate_storage(const string &new_path) const {
  Storage storage;
  storage.migrate(new_path);
  return storage.get_root_folder_name();
}
