#include "download/api_loader.h"

api_loader::api_loader(const nlohmann::json &j) { this->init(j); }

api_loader::api_loader(const std::string &path) {
  std::ifstream in(path);
  if (in.fail())
    throw std::runtime_error("Can't open provided file.");
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
