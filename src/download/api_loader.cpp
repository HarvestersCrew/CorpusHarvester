#include "download/api_loader.h"

api_loader::api_loader(const nlohmann::json &j) { this->init(j); }

api_loader::api_loader(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open provided file: " + path);
  nlohmann::json j;
  in >> j;
  this->init(j);
}

api_loader::~api_loader() {
  for (api_parameter_base *el : this->_requests)
    delete el;
  for (api_parameter_base *el : this->_responses)
    delete el;
}

void api_loader::init(const nlohmann::json &j) {

  nlohmann::json_schema::json_validator validator;
  std::ifstream schema_stream("data/api_schema.json");
  nlohmann::json schema;
  schema_stream >> schema;
  validator.set_root_schema(schema);
  validator.validate(j);

  this->_name = j.at("name").get<std::string>();
  this->_method = j.at("method").get<std::string>();
  this->_url = j.at("url").get<std::string>();
  for (auto &el : j.at("path_to_results")) {
    this->_path_to_results.push_back(el.get<std::string>());
  }
  for (auto &el : j.at("request"))
    this->_requests.push_back(new api_parameter_request(el));
  for (auto &el : j.at("response"))
    this->_responses.push_back(new api_parameter_response(el));
}

std::string api_loader::to_string() const {
  std::stringstream out;
  out << "name: " << this->_name << std::endl;
  out << "method: " << this->_method << std::endl;
  out << "url: " << this->_url << std::endl;

  out << "path to result: ";
  for (const std::string &el : this->_path_to_results) {
    out << "{" << el << "}";
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

nlohmann::json api_loader::query(const nlohmann::json &params,
                                 const download_manager &dl) const {
  std::stringstream url;
  url << this->_url;

  nlohmann::json headers;
  std::vector<std::string> body;

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
      throw std::runtime_error("Required parameter not filled");

    if (val.has_value()) {
      if (el->_position == "body") {
        body.push_back(el->_api_name + "=" + val.value());
      } else if (el->_position == "header") {
        headers[el->_api_name] = val.value();
      } else
        throw std::runtime_error("Unknown position");
    }
  }

  if (body.size() > 0) {
    url << "?";
    url << body.at(0);
    for (long unsigned int i = 1; i < body.size(); ++i)
      url << "&" << body.at(i);
  }

  nlohmann::json result =
      nlohmann::json::parse(dl.download(url.str(), headers));

  return result;
}

nlohmann::json api_loader::parse(const std::string &path) const {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open provided file: " + path);
  nlohmann::json j;
  in >> j;
  return this->parse(j);
}

nlohmann::json api_loader::parse(const nlohmann::json &result) const {
  nlohmann::json results_array = result.get<nlohmann::json>();
  nlohmann::json parsed_array = nlohmann::json::array();
  for (const std::string &el : this->_path_to_results) {
    results_array = results_array[el];
  }
  for (const auto &el : results_array) {
    nlohmann::json parsed_el;
    for (const api_parameter_response *param : this->_responses) {
      parsed_el[param->_name] = el[param->_api_name];
    }
    parsed_array += parsed_el;
  }
  return parsed_array;
}
