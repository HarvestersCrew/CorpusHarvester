#include "utils/api_request_builder.h"

ApiRequestBuilder::ApiRequestBuilder() {}

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

void ApiRequestBuilder::clear_all() {
  this->clear_requests();
  this->clear_types();
}

void ApiRequestBuilder::clear_requests() { this->_requests.clear(); }

long unsigned int ApiRequestBuilder::add_request(
    const string &api_name,
    const unordered_map<string, pair<string, string>> &params) {

  this->_requests.push_back(make_pair(ApiFactory::get_api(api_name), params));
  return this->_requests.size() - 1;
}

long unsigned int ApiRequestBuilder::add_request(const string &api_name) {
  return this->add_request(api_name,
                           unordered_map<string, pair<string, string>>());
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

const vector<
    pair<shared_ptr<api_loader>, unordered_map<string, pair<string, string>>>> &
ApiRequestBuilder::get_requests() const {
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
