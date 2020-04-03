#include "download/api_factory.h"

string ApiFactory::get_apis_folder_path() {
  stringstream ss;
  ss << Setting(Setting::STORAGE_ROOT, HarvesterDatabase::init()).get_value();
  ss << API_FACTORY_APIS_SUBFOLDER;
  return ss.str();
}

vector<shared_ptr<api_loader>>
ApiFactory::discover_from_path(const string &path) {
  vector<shared_ptr<api_loader>> apis;
  logger::debug("Discovering APIs from " + path);

  if (!fs::exists(path)) {
    logger::warning("Can't discover APIs, aborting - missing folder: " + path);
    return apis;
  }

  /** Map of APIs: key is the stem ("twitter", "tmdb"...)
   * First of pair is path to the API schema
   * Second of pair is an optional path to default values
   */
  unordered_map<string, pair<optional<string>, optional<string>>> apis_found;

  for (const auto &el : fs::directory_iterator(path)) {
    //   File is a JSON
    if (el.is_regular_file() && el.path().extension() == ".json") {

      string second_ext(el.path().stem().extension());

      // File is a json
      if (second_ext == "") {
        if (apis_found.find(el.path().stem()) == apis_found.end()) {
          apis_found.emplace(el.path().stem(), make_pair(el.path(), nullopt));
        } else {
          apis_found.at(el.path().stem()).first = el.path();
        }
      }

      // File is a env.json
      else if (second_ext == ".env") {
        string real_stem(fs::path(el.path().stem()).stem());
        if (apis_found.find(real_stem) == apis_found.end()) {
          apis_found.emplace(real_stem, make_pair(nullopt, el.path()));
        } else {
          apis_found.at(real_stem).second = el.path();
        }
      }
    }
  }

  // Keeping only pairs with a schema
  for (auto it = apis_found.begin(); it != apis_found.end();) {
    if (it->second.first.has_value()) {
      ++it;
    } else {
      logger::warning("API discover: ignoring '" + it->second.second.value() +
                      "' as no appropriated schema for it was found");
      it = apis_found.erase(it);
    }
  }

  for (const auto &el : apis_found) {

    try {
      if (el.second.second.has_value()) {
        const auto &inserted = apis.emplace_back(
            new api_loader(el.second.first.value(), el.second.second.value()));
        logger::debug("API '" + inserted->get_name() +
                      "' loaded with its provided default values");
      } else {
        const auto &inserted =
            apis.emplace_back(new api_loader(el.second.first.value()));
        logger::debug("API '" + inserted->get_name() + "' loaded");
      }
    } catch (const api_unrecognized_settings_exception &e) {
      logger::error("API discover: unrecognized setting for " +
                    el.second.first.value() + ", skipping it");
      logger::error(e.what());
    } catch (const api_missing_settings_exception &e) {
      logger::error("API discover: main attribute for " +
                    el.second.first.value() + " not found, skipping it");
      logger::error(e.what());
    } catch (const api_default_not_in_schema &e) {
      logger::error("API discover: invalid default values for " +
                    el.second.second.value() + ", skipping it");
      logger::error(e.what());
    } catch (const std::exception &e) {
      logger::error("API discover: invalid JSON schema for " +
                    el.second.first.value() + ", skipping it");
      logger::error(e.what());
    }
  }

  logger::info("API discover: " + to_string(apis.size()) + " APIs were loaded");
  return apis;
}
