#ifndef API_LOADER_H
#define API_LOADER_H

#include "download/api_parameter.h"
#include "download/download_manager.h"
#include "utils/exceptions.h"
#include "utils/nlohmann/json-schema.hpp"
#include "utils/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class api_loader {
private:
  std::string _url;
  std::string _method;
  std::string _name;
  std::vector<std::string> _path_to_results;
  std::vector<api_parameter_request *> _requests;
  std::vector<api_parameter_response *> _responses;

  void init(const nlohmann::json &j);

public:
  api_loader(const nlohmann::json &j);
  api_loader(const std::string &path);
  ~api_loader();
  std::string to_string() const;
  nlohmann::json query(const nlohmann::json &params,
                       const download_manager &dl) const;
};

#endif
