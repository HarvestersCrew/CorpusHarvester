#include "download/apis/api_twitter.h"

api_twitter::api_twitter() : api_abstract() {
  this->_settings["required"]["bearer"] = nullptr;
}

std::string api_twitter::get_name() const { return "Twitter"; }

std::string api_twitter::get_base_url() const {
  return "https://api.twitter.com/1.1/search/tweets.json";
}

nlohmann::json api_twitter::query(const download_manager &dl,
                                  const std::string &query) const {
  if (!this->are_required_settings_filled())
    throw api_missing_settings_exception();
  std::stringstream url;
  url << this->get_base_url() << "?result_type=mixed&q=" << query;
  nlohmann::json headers;
  headers["Authorization"] =
      "Bearer " + this->_settings["required"]["bearer"].get<std::string>();
  std::string response = dl.download(url.str(), headers);
  nlohmann::json json = nlohmann::json::parse(response);
  nlohmann::json result;
  for (auto &el : json["statuses"]) {
    result += {{"id", el["id"].get<int64_t>()},
               {"date", el["created_at"].get<std::string>()},
               {"text", el["text"].get<std::string>()},
               {"retweet", el["retweet_count"].get<int>()},
               {"favorite", el["favorite_count"].get<int>()},
               {"lang", el["lang"].get<std::string>()}};
  }
  return result;
}
