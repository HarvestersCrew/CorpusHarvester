#ifndef API_FACTORY_H
#define API_FACTORY_H

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
using std::const_pointer_cast;
using std::make_pair;
using std ::make_shared;
using std::nullopt;
using std::optional;
using std::pair;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::to_string;
using std::unordered_map;
using std::vector;

/**
 * Class used to get automatically load API files and retrieve informations
 * about them
 *
 * get_apis() returns a map of loaded APIs with their name in first and the
 * pointer to its api_loader in second
 *
 * get_api_names() returns only the names of the loaded APIs, useful for user
 * interfaces
 *
 * get_api(string api_name) retrieves the api_loader of the given name or throws
 * an exception
 */
class ApiFactory {
private:
  /** Map of name and api_loader found */
  static optional<unordered_map<string, shared_ptr<api_loader>>> _apis;

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
  static const unordered_map<string, shared_ptr<api_loader>> &get_apis();

  /**
   * Returns an immuable list of API loaders
   */
  static const vector<shared_ptr<const api_loader>> get_api_loaders();

  /**
   * Returns an immuable list of API following a given type
   * @param t type of APIs to retrieve
   */
  static const vector<shared_ptr<const api_loader>>
  get_api_loaders_typed(api_loader::api_type t);

  /**
   * Returns a list of the API names
   */
  static vector<string> get_api_names();

  /**
   * Returns the pointer to an api_loader given its name
   * @throw api_factory_name_not_found if the given API is not found in the list
   */
  static const shared_ptr<api_loader> get_api(const string &name);
};

#endif
