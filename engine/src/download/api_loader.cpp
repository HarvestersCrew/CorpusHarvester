#include "download/api_loader.h"

api_loader::api_loader(const nlohmann::json &j) { this->init(j); }

api_loader::api_loader(const std::string &schema_path) {
  this->init(json_from_file(schema_path));
}

api_loader::api_loader(const std::string &schema_path,
                       const std::string &default_values_path)
    : api_loader(schema_path) {
  nlohmann::json default_values = json_from_file(default_values_path);
  for (auto &el : default_values.items()) {
    this->set_parameter_request_default_value(el.key(),
                                              el.value().get<std::string>());
  }
}

std::string api_loader::get_name() const { return this->_name; }

void api_loader::set_parameter_request_default_value(const std::string &key,
                                                     const std::string &val) {
  for (const shared_ptr<api_parameter_request> p : this->_requests) {
    if (p->_name == key) {
      p->set_default_value(val);
      return;
    }
  }
  throw api_default_not_in_schema(key);
}

void api_loader::init(const nlohmann::json &j) {

  nlohmann::json_schema::json_validator validator;
  nlohmann::json schema = json_from_file(this->get_api_schema_full_path());
  validator.set_root_schema(schema);
  validator.validate(j);

  this->_name = j.at("name").get<std::string>();
  this->_method = j.at("method").get<std::string>();
  this->_url = j.at("url").get<std::string>();
  this->_response_main_item = j.at("response_main_item").get<std::string>();

  std::string given_api_type = j.at("api_type").get<std::string>();
  if (given_api_type == API_TYPE_TXT) {
    this->_api_type = api_loader::api_type::TEXT;
  } else if (given_api_type == API_TYPE_IMG) {
    this->_api_type = api_loader::api_type::IMAGE;
  } else {
    throw api_unrecognized_settings_exception("api_type", given_api_type);
  }

  this->_truncate_before = {};
  if (j.contains("truncate_before")) {
    this->_truncate_before = j.at("truncate_before").get<int>();
  }

  this->_truncate_after = {};
  if (j.contains("truncate_after")) {
    this->_truncate_after = j.at("truncate_after").get<int>();
  }

  for (auto &el : j.at("path_to_results")) {
    this->_path_to_results.emplace_back(el.get<std::string>());
  }
  for (auto &el : j.at("request"))
    this->_requests.emplace_back(make_shared<api_parameter_request>(el));
  for (auto &el : j.at("response"))
    this->_responses.emplace_back(make_shared<api_parameter_response>(el));

  bool main_attribute_found = false;
  for (const shared_ptr<const api_parameter_response> el : this->_responses) {
    if (el->_name == this->_response_main_item)
      main_attribute_found = true;
  }
  if (!main_attribute_found)
    throw api_missing_settings_exception(this->_response_main_item);
}

string api_loader::get_api_schema_full_path() const {
  stringstream ss;
  ss << Setting(Setting::STORAGE_ROOT, HarvesterDatabase::init()).get_value()
     << API_LOADER_SCHEMA_NAME;
  return ss.str();
}

std::string api_loader::to_string() const {
  std::stringstream out;
  out << "name: " << this->_name << std::endl;
  out << "method: " << this->_method << std::endl;
  out << "url: " << this->_url << std::endl;
  out << "api_type: " << this->get_api_type_string() << std::endl;
  out << "main value name: " << this->_response_main_item << std::endl;
  out << "truncate before: " << this->_truncate_before.value_or(0) << std::endl;
  out << "truncate after: " << this->_truncate_after.value_or(0) << std::endl;

  out << "path to result: ";
  for (const std::string &el : this->_path_to_results) {
    out << "{" << el << "}";
  }
  out << std::endl;

  out << std::endl << "Request parameters:";
  for (const shared_ptr<const api_parameter_request> el : this->_requests) {
    out << std::endl << el->to_string();
  }

  out << std::endl << "Response parameters:";
  for (const shared_ptr<const api_parameter_response> el : this->_responses) {
    out << std::endl << el->to_string();
  }

  return out.str();
}

std::string api_loader::get_api_type_string() const {
  switch (this->_api_type) {
  case api_loader::api_type::TEXT:
    return API_TYPE_TXT;
    break;
  case api_loader::api_type::IMAGE:
    return API_TYPE_IMG;
    break;
  }
  throw api_filetype_incompatible(std::to_string(this->_api_type));
}

api_loader::api_type api_loader::get_api_type() const {
  return this->_api_type;
}

std::list<shared_ptr<File>>
api_loader::query_and_parse(const unordered_map<string, string> &params,
                            const download_manager &dl) const {

  download_item dl_item(this->_url, this->_truncate_before.value_or(0),
                        this->_truncate_after.value_or(0));

  std::vector<std::pair<shared_ptr<const api_parameter_request>, std::string>>
      relevant_parameters;

  std::list<shared_ptr<File>> files;
  nlohmann::json results_array;

  for (const shared_ptr<const api_parameter_request> el : this->_requests) {

    std::optional<std::string> val;
    val = std::nullopt;

    if (params.find(el->_name) != params.end()) {
      if (!el->is_value_valid(params.at(el->_name)))
        throw api_parameter_incompatible_value(el->get_value_type_string(),
                                               el->get_name());
      val.emplace(params.at(el->_name));
    } else if (el->_default_value.has_value())
      val.emplace(el->_default_value.value());
    else if (el->_required)
      throw api_missing_settings_exception(el->_name);

    if (val.has_value()) {

      if (el->_relevant) {
        relevant_parameters.emplace_back(std::make_pair(el, val.value()));
      }

      if (el->_position == "body" || el->_position == "header") {
        dl_item.set_parameter(el, val.value());
      } else
        throw api_unrecognized_settings_exception("position", el->_position);
    }
  }

  nlohmann::json result = nlohmann::json::parse(dl.download(dl_item));

  results_array = result.get<nlohmann::json>();
  for (const std::string &el : this->_path_to_results) {
    results_array = results_array[el];
  }

  std::vector<response_item> parsed_responses;
  for (const auto &el : results_array) {
    parsed_responses.emplace_back(el, this->_responses);
  }

  this->manage_parsed_responses(parsed_responses, files, relevant_parameters,
                                dl);
  return files;
}

void api_loader::manage_parsed_responses(
    const vector<response_item> &parsed_responses,
    list<shared_ptr<File>> &files,
    vector<pair<shared_ptr<const api_parameter_request>, string>>
        &relevant_parameters,
    const download_manager &dl) const {

  for (const response_item &el : parsed_responses) {

    try {

      shared_ptr<File> sp_file = make_shared<File>();
      sp_file->set_source(this->_name);

      for (const auto &response : el.get_parameters()) {
        if (response.first->_name == this->_response_main_item) {
          this->manage_main_value(el, sp_file, dl);
        } else if (response.first->_relevant) {
          sp_file->add_tag(response.first->_name, response.second);
        }
      }

      for (const std::pair<shared_ptr<const api_parameter_request>, std::string>
               &relevant : relevant_parameters) {
        sp_file->add_tag(relevant.first->_name, relevant.second);
      }

      files.push_back(sp_file);

    } catch (const std::exception &e) {
      stringstream ss;
      ss << "Unexpected exception while parsing a result: " << typeid(e).name();
      logger::error(ss.str());
    }
  }

  logger::info("'" + this->get_name() + "' downloaded " +
               std::to_string(parsed_responses.size()) + " and " +
               std::to_string(parsed_responses.size() - files.size()) +
               " were already present");
}

void api_loader::manage_main_value(const response_item &result_to_manage,
                                   shared_ptr<File> file_to_save_to,
                                   const download_manager &dl) const {
  std::pair<shared_ptr<const api_parameter_response>, std::string> main_result =
      result_to_manage.get_named_parameter(this->_response_main_item);
  shared_ptr<const api_parameter_response> main_result_param =
      main_result.first;
  std::string main_result_value = main_result.second;
  switch (this->_api_type) {
  case api_loader::api_type::TEXT:
    this->manage_text(main_result_value, file_to_save_to);
    break;
  case api_loader::api_type::IMAGE:
    this->manage_media(main_result_value, main_result_param, file_to_save_to,
                       dl);
    break;
  default:
    throw api_filetype_incompatible(std::to_string(this->_api_type));
  }
}

void api_loader::manage_text(const std::string &api_result,
                             shared_ptr<File> file_to_save_to) const {
  file_to_save_to->set_content(api_result);
  file_to_save_to->set_format("txt");
}

void api_loader::manage_media(
    const std::string &path_api,
    const shared_ptr<const api_parameter_response> param,
    shared_ptr<File> file_to_save_to, const download_manager &dl) const {
  if (param->_value_type != api_parameter_base::value_type::IMAGE_LINK) {
    throw std::runtime_error("Can't manage this API (" + this->_name +
                             ") media type.");
  }

  std::vector<char> res = dl.download(path_api);
  file_to_save_to->set_bin_content(res);
  file_to_save_to->set_binary(true);

  int idx = path_api.rfind('.');
  std::string format;
  if (idx) {
    format = path_api.substr(idx + 1);
  }
  file_to_save_to->set_format(format);
}

std::optional<shared_ptr<api_parameter_request>>
api_loader::find_request_parameter(const std::string &name) const {
  for (shared_ptr<api_parameter_request> el : this->_requests) {
    if (el->_name == name) {
      return el;
    }
  }
  return {};
}

std::optional<shared_ptr<api_parameter_response>>
api_loader::find_response_parameter(const std::string &name) const {
  for (shared_ptr<api_parameter_response> el : this->_responses) {
    if (el->_name == name) {
      return el;
    }
  }
  return {};
}

const std::vector<shared_ptr<api_parameter_request>> &
api_loader::get_request_parameters() const {
  return this->_requests;
}
