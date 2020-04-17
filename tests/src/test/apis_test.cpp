#include "test/apis_test.h"

void test_apis_get_apis() {

  std::filesystem::create_directory(ApiFactory::get_apis_folder_path() +
                                    "folder.json/");
  ofstream fake_json(ApiFactory::get_apis_folder_path() + "fake.json");
  fake_json << "blabla";
  fake_json.close();

  unordered_map<string, shared_ptr<api_loader>> apis = ApiFactory::get_apis();
  int size = apis.size();

  std::filesystem::remove(ApiFactory::get_apis_folder_path() + "folder.json/");
  std::filesystem::remove(ApiFactory::get_apis_folder_path() + "fake.json");

  Assertion::assert_equals(__FUNCTION__, APIS_TEST_CURRENT_APIS_NUMBER, size);
}

void test_apis_get_names() {
  Assertion::assert_equals(__FUNCTION__, APIS_TEST_CURRENT_APIS_NUMBER,
                           ApiFactory::get_api_names().size());
}

void test_apis_get_api() {

  try {
    ApiFactory::get_api("random_name_not_used_at_all");
    Assertion::assert_throw(__FUNCTION__, "api_factory_name_not_found");
  } catch (const api_factory_name_not_found &e) {
  }

  auto api = ApiFactory::get_api("Twitter");
  Assertion::assert_equals(__FUNCTION__, "Twitter", api->get_name());
}

void test_apis_get_type() {
  auto images = ApiFactory::get_api_loaders_typed(api_loader::api_type::IMAGE);
  auto texts = ApiFactory::get_api_loaders_typed(api_loader::api_type::TEXT);
  for (const auto api : images) {
    Assertion::assert_equals(__FUNCTION__, api_loader::api_type::IMAGE,
                             api->get_api_type());
  }
  for (const auto api : texts) {
    Assertion::assert_equals(__FUNCTION__, api_loader::api_type::TEXT,
                             api->get_api_type());
  }
}

void apis_test() {
  std::cout << std::endl << "APIs tests : " << std::endl;
  Assertion::test(test_apis_get_apis, "test_apis_get_apis");
  Assertion::test(test_apis_get_names, "test_apis_get_names");
  Assertion::test(test_apis_get_api, "test_apis_get_api");
  Assertion::test(test_apis_get_type, "test_apis_get_type");
}
