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

api_loader::~api_loader() {
  for (api_parameter_base *el : this->_requests)
    delete el;
  for (api_parameter_base *el : this->_responses)
    delete el;
}

void api_loader::set_parameter_request_default_value(const std::string &key,
                                                     const std::string &val) {
  for (api_parameter_request *p : this->_requests) {
    if (p->_api_name == key) {
      p->set_default_value(val);
      return;
    }
  }
  throw std::runtime_error("Default parameter \"" + key +
                           "\" not found in schema.");
}

void api_loader::init(const nlohmann::json &j) {

  nlohmann::json_schema::json_validator validator;
  nlohmann::json schema = json_from_file("data/api_schema.json");
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

  for (auto &el : j.at("path_to_results")) {
    this->_path_to_results.push_back(el.get<std::string>());
  }
  for (auto &el : j.at("request"))
    this->_requests.push_back(new api_parameter_request(el));
  for (auto &el : j.at("response"))
    this->_responses.push_back(new api_parameter_response(el));

  bool main_attribute_found = false;
  for (api_parameter_response *el : this->_responses) {
    if (el->_name == this->_response_main_item)
      main_attribute_found = true;
  }
  if (!main_attribute_found)
    throw api_missing_settings_exception();

  if (j.contains("response_url_prepends")) {
    for (const auto &el : j.at("response_url_prepends")) {
      std::string param_name = el.at("value").get<std::string>();
      if (el.at("is_parameter_name").get<bool>()) {
        if (!this->find_request_parameter(param_name).has_value() &&
            !this->find_response_parameter(param_name).has_value()) {
          throw api_no_setting_exception(param_name);
        }
      }
      this->response_url_prepends.push_back(
          std::make_pair(param_name, el.at("is_parameter_name").get<bool>()));
    }
  }
}

std::string api_loader::to_string() const {
  std::stringstream out;
  out << "name: " << this->_name << std::endl;
  out << "method: " << this->_method << std::endl;
  out << "url: " << this->_url << std::endl;
  out << "api_type: " << this->api_type_string() << std::endl;
  out << "main value name: " << this->_response_main_item << std::endl;

  out << "path to result: ";
  for (const std::string &el : this->_path_to_results) {
    out << "{" << el << "}";
  }
  out << std::endl;

  out << "response url prepend: " << std::endl;
  for (std::pair<std::string, bool> el : this->response_url_prepends) {
    out << "value: \"" << el.first << "\", is_parameter: " << el.second
        << std::endl;
  }

  out << std::endl << "Request parameters:";
  for (const api_parameter_request *el : this->_requests) {
    out << std::endl << el->to_string();
  }

  out << std::endl << "Response parameters:";
  for (const api_parameter_response *el : this->_responses) {
    out << std::endl << el->to_string();
  }

  return out.str();
}

std::string api_loader::api_type_string() const {
  switch (this->_api_type) {
  case api_loader::api_type::TEXT:
    return API_TYPE_TXT;
    break;
  case api_loader::api_type::IMAGE:
    return API_TYPE_IMG;
    break;
  default:
    throw std::runtime_error(
        "API type not yet recognized or implemented in string method");
    break;
  }
}

std::list<File *>
api_loader::query_and_parse(const nlohmann::json &params,
                            const download_manager &dl) const {

  download_item dl_item(this->_url);

  nlohmann::json relevant_parameters;

  std::list<File *> files;
  nlohmann::json results_array;

  for (api_parameter_request *el : this->_requests) {

    std::optional<std::string> val;
    val = std::nullopt;

    if (params.contains(el->_api_name)) {
      if (!el->is_value_valid(params.at(el->_api_name).get<std::string>()))
        throw std::runtime_error("Incompatible parameter.");
      val.emplace(params.at(el->_api_name).get<std::string>());
    } else if (el->_default_value.has_value())
      val.emplace(el->_default_value.value());
    else if (el->_required)
      throw std::runtime_error("Required parameter not filled: " +
                               el->_api_name);

    if (val.has_value()) {

      if (el->_relevant) {
        relevant_parameters[el->_name] = val.value();
      }

      if (el->_position == "body") {
        dl_item.set_parameter(el->_api_name, val.value());
      } else if (el->_position == "header") {
        dl_item.set_header(el->_api_name, val.value());
      } else
        throw std::runtime_error("Unknown position");
    }
  }

  nlohmann::json result = nlohmann::json::parse(dl.download(dl_item));

  results_array = result.get<nlohmann::json>();
  for (const std::string &el : this->_path_to_results) {
    results_array = results_array[el];
  }

  std::string content;
  std::string now;
  int id = 0;
  for (const auto &el : results_array) {
    now = get_current_time("%d-%m-%Y");
    std::ostringstream out;
    out << now << "_" << id;

    try {
      File *file = new File("", out.str(), 0, this->_name, "");
      for (const api_parameter_response *param : this->_responses) {
        if (param->_name == this->_response_main_item) {
          this->manage_main_value(el[param->_api_name], param, file, dl);
        } else {
          file->add_tag(param->_name,
                        param->json_value_to_string(el[param->_api_name]));
        }
      }
      for (const auto &[key, val] : relevant_parameters.items()) {
        file->add_tag(key, val.get<std::string>());
      }

      files.push_back(file);
    } catch (...) {
      std::exception_ptr e = std::current_exception();
      std::cerr << "Unexpected exception while parsing a result: "
                << e.__cxa_exception_type()->name() << std::endl;
    }

    id++;
  }

  return files;
}

void api_loader::manage_main_value(const nlohmann::json &result_to_manage,
                                   const api_parameter_response *param,
                                   File *file_to_save_to,
                                   const download_manager &dl) const {
  switch (this->_api_type) {
  case api_loader::api_type::TEXT:
    this->manage_text(param->json_value_to_string(result_to_manage),
                      file_to_save_to);
    break;
  case api_loader::api_type::IMAGE:
    this->manage_media(param->json_value_to_string(result_to_manage), param,
                       file_to_save_to, dl);
    break;
  default:
    throw std::runtime_error("Saving type " + this->api_type_string() +
                             " is unsupported currently.");
  }
}

void api_loader::manage_text(const std::string &api_result,
                             File *file_to_save_to) const {
  file_to_save_to->set_content(api_result);
  file_to_save_to->set_format("txt");
}

void api_loader::manage_media(const std::string &path_api,
                              const api_parameter_response *param,
                              File *file_to_save_to,
                              const download_manager &dl) const {
  if (param->_value_type != api_parameter_base::value_type::IMAGE_LINK) {
    throw std::runtime_error("Can't manage this API (" + this->_name +
                             ") media type.");
  }

  std::string url = this->response_url_prepend(path_api);
  std::vector<char> res = dl.download(url);
  file_to_save_to->set_bin_content(res);
  file_to_save_to->set_binary(true);

  int idx = path_api.rfind('.');
  std::string format;
  if (idx) {
    format = path_api.substr(idx + 1);
  }
  file_to_save_to->set_format(format);
}

std::optional<api_parameter_request *>
api_loader::find_request_parameter(const std::string &name) const {
  for (api_parameter_request *el : this->_requests) {
    if (el->_api_name == name) {
      return el;
    }
  }
  return {};
}

std::optional<api_parameter_response *>
api_loader::find_response_parameter(const std::string &name) const {
  for (api_parameter_response *el : this->_responses) {
    if (el->_api_name == name) {
      return el;
    }
  }
  return {};
}

std::string api_loader::response_url_prepend(std::string url) const {
  for (int i = this->response_url_prepends.size() - 1; i >= 0; --i) {
    const std::pair<std::string, bool> &el = this->response_url_prepends.at(i);
    std::string prep("");
    if (el.second) {
      throw std::runtime_error(
          "Using response value as prepend not yet implemented");
    } else {
      prep = el.first;
    }
    url = prep + url;
  }
  return url;
}
