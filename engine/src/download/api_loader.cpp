#include "download/api_loader.h"

api_loader::api_loader(const json &j) { this->init(j); }

api_loader::api_loader(const string &schema_path) {
  this->init(json_from_file(schema_path));
}

api_loader::api_loader(const string &schema_path,
                       const string &default_values_path)
    : api_loader(schema_path) {
  json default_values = json_from_file(default_values_path);
  for (auto &el : default_values.items()) {
    this->set_parameter_request_default_value(el.key(),
                                              el.value().get<string>());
  }
}

string api_loader::get_name() const { return this->_name; }

void api_loader::set_parameter_request_default_value(const string &key,
                                                     const string &val) {
  optional<shared_ptr<api_parameter_request>> param =
      this->find_request_parameter(key);
  if (!param.has_value()) {
    throw api_default_not_in_schema(key);
  }
  param.value()->set_default_value(val);
}

void api_loader::init(const json &j) {

  nlohmann::json_schema::json_validator validator;
  json schema = json_from_file(this->get_api_schema_full_path());
  validator.set_root_schema(schema);
  validator.validate(j);

  this->_name = j.at("name").get<string>();
  this->_method = j.at("method").get<string>();
  this->_url = j.at("url").get<string>();
  this->_response_main_item = j.at("response_main_item").get<string>();

  string given_api_type = j.at("api_type").get<string>();
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
    this->_path_to_results.emplace_back(el.get<string>());
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
  ss << Setting(Setting::STORAGE_ROOT).get_value() << API_LOADER_SCHEMA_NAME;
  return ss.str();
}

json api_loader::serialize() const {
  json j;
  j["name"] = this->_name;
  j["api_type"] = this->get_api_type_string();
  j["url"] = this->_url;
  j["method"] = this->_method;
  j["response_main_item"] = this->_response_main_item;
  j["path_to_results"] = this->_path_to_results;
  j["truncate_before"] = this->_truncate_before.value_or(0);
  j["truncate_after"] = this->_truncate_after.value_or(0);
  j["requests"] = json::array();
  j["responses"] = json::array();
  for (const auto &param : this->get_request_parameters()) {
    j["requests"].push_back(param->serialize());
  }
  for (const auto &param : this->get_response_parameters()) {
    j["responses"].push_back(param->serialize());
  }
  return j;
}

string api_loader::to_string() const {
  stringstream out;
  out << "name: " << this->_name << std::endl;
  out << "method: " << this->_method << std::endl;
  out << "url: " << this->_url << std::endl;
  out << "api_type: " << this->get_api_type_string() << std::endl;
  out << "main value name: " << this->_response_main_item << std::endl;
  out << "truncate before: " << this->_truncate_before.value_or(0) << std::endl;
  out << "truncate after: " << this->_truncate_after.value_or(0) << std::endl;

  out << "path to result: ";
  for (const string &el : this->_path_to_results) {
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

string api_loader::get_api_type_string() const {
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

list<shared_ptr<File>>
api_loader::query_and_parse(const unordered_map<string, string> &params,
                            const download_manager &dl) const {

  download_item dl_item(this->_url, this->_truncate_before.value_or(0),
                        this->_truncate_after.value_or(0));

  vector<pair<shared_ptr<const api_parameter_request>, string>>
      relevant_parameters;

  list<shared_ptr<File>> files;
  json results_array;

  for (const shared_ptr<const api_parameter_request> el : this->_requests) {

    optional<string> val;
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

  json result;
  try {
    result = json::parse(dl.download(dl_item));
  } catch (const json::exception &e) {
    logger::debug(
        "Exception thrown when parsing download result in api_loader");
    throw e;
  }

  results_array = result.get<json>();
  for (const string &el : this->_path_to_results) {
    results_array = results_array[el];
  }

  vector<response_item> parsed_responses;
  for (const auto &el : results_array) {
    try {
      parsed_responses.emplace_back(el, this->_responses);
    } catch (const json::exception &e) {
      logger::error("Aborting parsing an item");
      logger::error(e.what());
    }
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
      sp_file->set_type(this->get_api_type_string());

      for (const auto &response : el.get_parameters()) {
        if (response.first->_name == this->_response_main_item) {
          this->manage_main_value(el, sp_file, dl);
        } else if (response.first->_relevant) {
          sp_file->add_tag(response.first->_name, response.second);
        }
      }

      for (const pair<shared_ptr<const api_parameter_request>, string>
               &relevant : relevant_parameters) {
        sp_file->add_tag(relevant.first->_name, relevant.second);
      }

      files.push_back(sp_file);

    } catch (const std::exception &e) {
      stringstream ss;
      ss << "Unexpected exception while parsing a result: " << typeid(e).name();
      logger::error(ss.str());
      logger::debug(e.what());
    }
  }
}

void api_loader::manage_main_value(const response_item &result_to_manage,
                                   shared_ptr<File> file_to_save_to,
                                   const download_manager &dl) const {
  pair<shared_ptr<const api_parameter_response>, string> main_result =
      result_to_manage.get_named_parameter(this->_response_main_item);
  shared_ptr<const api_parameter_response> main_result_param =
      main_result.first;
  string main_result_value = main_result.second;
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

void api_loader::manage_text(const string &api_result,
                             shared_ptr<File> file_to_save_to) const {
  file_to_save_to->set_content(api_result);
  file_to_save_to->set_format("txt");
}

void api_loader::manage_media(
    const string &path_api,
    const shared_ptr<const api_parameter_response> param,
    shared_ptr<File> file_to_save_to, const download_manager &dl) const {
  if (param->_value_type != api_parameter_base::value_type::IMAGE_LINK) {
    throw std::runtime_error("Can't manage this API (" + this->_name +
                             ") media type.");
  }

  vector<char> res = dl.download(path_api);
  file_to_save_to->set_bin_content(res);
  file_to_save_to->set_binary(true);

  int idx = path_api.rfind('.');
  string format;
  if (idx) {
    format = path_api.substr(idx + 1);
  }
  file_to_save_to->set_format(format);
}

optional<shared_ptr<api_parameter_request>>
api_loader::find_request_parameter(const string &name) const {
  for (shared_ptr<api_parameter_request> el : this->_requests) {
    if (el->_name == name) {
      return el;
    }
  }
  return {};
}

optional<shared_ptr<api_parameter_response>>
api_loader::find_response_parameter(const string &name) const {
  for (shared_ptr<api_parameter_response> el : this->_responses) {
    if (el->_name == name) {
      return el;
    }
  }
  return {};
}

optional<shared_ptr<api_parameter_base>>
api_loader::find_relevant_parameter(const string &name) const {
  for (auto &el : get_relevant_parameters()) {
    if (el->_name == name)
      return el;
  }
  return {};
}

const vector<shared_ptr<api_parameter_request>> &
api_loader::get_request_parameters() const {
  return this->_requests;
}

const vector<shared_ptr<api_parameter_response>> &
api_loader::get_response_parameters() const {
  return this->_responses;
}

const vector<shared_ptr<api_parameter_base>>
api_loader::get_relevant_parameters() const {
  vector<shared_ptr<api_parameter_base>> resp;
  for (auto param : _responses) {
    if (param->_relevant)
      resp.push_back(param);
  }
  for (auto param : _requests) {
    if (param->_relevant)
      resp.push_back(param);
  }
  return resp;
}
