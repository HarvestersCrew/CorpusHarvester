#ifndef API_FACTORY_H
#define API_FACTORY_H

#include "database/harvester_database.h"
#include "database/setting.h"
#include "download/api_loader.h"
#include "utils/exceptions.h"
#include "utils/logger.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define API_FACTORY_APIS_SUBFOLDER "apis/"

namespace fs = std::filesystem;
using std::make_pair;
using std::nullopt;
using std::optional;
using std::pair;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::to_string;
using std::unordered_map;
using std::vector;

class ApiFactory {
public:
  static string get_apis_folder_path();
  static vector<shared_ptr<api_loader>> discover_from_path(const string &path);
};

#endif
