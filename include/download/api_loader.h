#ifndef API_LOADER_H
#define API_LOADER_H

#include "download/api_parameter.h"
#include "utils/json.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class api_loader {
private:
  std::string _url;
  std::string _method;
  std::string _name;
  std::vector<std::string> _path_to_results;
  std::vector<api_parameter_base *> _variables;
  void init(const nlohmann::json &j);

public:
  api_loader(const nlohmann::json &j);
  api_loader(const std::string &path);
  ~api_loader();
  std::string to_string() const;
};

#endif
