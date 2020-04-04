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
private:
  /** List of api_loader found */
  static optional<vector<shared_ptr<api_loader>>> apis;

  /**
   * Discovers and loads all compatible APIs from path
   * @param path Folder path to look into (non recursive)
   */
  static void discover_from_path(const string &path);

public:
  /**
   * Returns the path to the app APIs folder
   * @return string Path to the folder
   */
  static string get_apis_folder_path();

  /**
   * Returns a const ref to the found APIs but first discovers them if there the
   * optional isn't initialized
   */
  static const vector<shared_ptr<api_loader>> &get_apis();
};

#endif
