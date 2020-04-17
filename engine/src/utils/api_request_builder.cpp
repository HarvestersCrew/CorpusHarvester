#include "utils/api_request_builder.h"

ApiRequestBuilder::ApiRequestBuilder() {}

json ApiRequestBuilder::serialize() const {

  json j;

  j["requests"] = json::array();
  j["types"] = json::array();

  for (const auto &el : this->_types) {
    j["types"].push_back(el);
  }

  for (const auto &request : this->_requests) {
    j["requests"].push_back(
        {{"api", request.first->get_name()}, {"params", json::array()}});
    for (const auto &param : request.second) {
      j["requests"][j["requests"].size() - 1]["params"].push_back(
          {{"name", param.first},
           {"value", param.second.first},
           {"op", param.second.second}});
    }
  }

  return j;
}

void ApiRequestBuilder::deserialize(const json &j) {
  this->clear_requests();
  this->clear_types();

  for (const auto &type : j["types"]) {
    this->add_type((api_loader::api_type)type.get<int>());
  }

  for (auto it = j["requests"].begin(); it != j["requests"].end(); ++it) {
    string api_name = it->at("api").get<string>();
    int idx = this->add_request(api_name);
    for (auto param = it->at("params").begin(); param != it->at("params").end();
         ++param) {
      string param_name = param->at("name");
      string param_value = param->at("value");
      string op = param->at("op");
      this->add_request_parameter(idx, param_name, param_value, op);
    }
  }
}

void ApiRequestBuilder::clear_types() { this->_types.clear(); }

void ApiRequestBuilder::add_type(const api_loader::api_type t) {
  if (t >= api_loader::api_type::TEXT && t <= api_loader::api_type::IMAGE) {
    this->_types.insert(t);
  } else {
    throw api_request_builder_invalid_type(t);
  }
}

const unordered_set<api_loader::api_type> &
ApiRequestBuilder::get_types() const {
  return this->_types;
}

void ApiRequestBuilder::clear_requests() { this->_requests.clear(); }

long unsigned int ApiRequestBuilder::add_request(const string &api_name) {
  this->_requests.push_back(
      make_pair(ApiFactory::get_api(api_name),
                unordered_map<string, pair<string, string>>()));
  return this->_requests.size() - 1;
}

void ApiRequestBuilder::add_request_parameter(long unsigned int request_id,
                                              const string &param_name,
                                              const string &param_value,
                                              const string &op) {
  if (this->_requests.size() <= request_id) {
    throw api_builder_request_not_found(request_id);
  }

  auto &params = this->_requests[request_id].second;
  if (params.find(param_name) == params.end()) {
    params.emplace(param_name, make_pair(param_value, op));
  } else {
    auto &param = params[param_name];
    param.first = param_value;
    param.second = op;
  }
}

vector<pair<shared_ptr<api_loader>, unordered_map<string, string>>>
ApiRequestBuilder::get_no_op_requests() const {

  vector<pair<shared_ptr<api_loader>, unordered_map<string, string>>> requests;
  for (const auto &request : this->get_requests()) {

    unordered_map<string, string> params;
    for (const auto &param : request.second) {
      params.emplace(param.first, param.second.first);
    }

    requests.push_back(make_pair(request.first, params));
  }

  return requests;
}

const vector<api_builder_request> &ApiRequestBuilder::get_requests() const {
  return this->_requests;
}

string ApiRequestBuilder::to_string() const {
  stringstream res;

  res << "Requests: " << endl;
  for (const auto &request : this->get_requests()) {
    res << request.first->get_name() << ": {";
    for (const auto &param : request.second) {
      res << param.first << param.second.second << param.second.first << ",";
    }
    res << "}";
  }

  return res.str();
}

vector<api_builder_request> ApiRequestBuilder::get_usable_requests() const {

  vector<api_builder_request> res;

  if (this->get_requests().size() != 0) {
    res = vector<api_builder_request>(this->get_requests());
  } else {
    for (auto api : ApiFactory::get_api_loaders()) {
      res.push_back(
          make_pair(api, unordered_map<string, pair<string, string>>()));
    }
  }

  if (this->get_types().size() != 0) {
    auto types = this->get_types();
    for (auto it = res.begin(); it != res.end();) {
      auto api = it->first;
      if (types.find(api->get_api_type()) == types.end()) {
        it = res.erase(it);
      } else {
        ++it;
      }
    }
  }

  return res;
}
