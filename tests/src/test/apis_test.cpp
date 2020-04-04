#include "test/apis_test.h"

void test_apis_get_apis() {

  std::filesystem::create_directory(ApiFactory::get_apis_folder_path() +
                                    "folder.json/");
  ofstream fake_json(ApiFactory::get_apis_folder_path() + "fake.json");
  fake_json << "blabla";
  fake_json.close();

  vector<shared_ptr<api_loader>> apis = ApiFactory::get_apis();
  Assertion::assert_equals(__FUNCTION__, APIS_TEST_CURRENT_APIS_NUMBER,
                           apis.size());

  std::filesystem::remove(ApiFactory::get_apis_folder_path() + "folder.json/");
  std::filesystem::remove(ApiFactory::get_apis_folder_path() + "fake.json");
}

void test_apis_get_names() {
  Assertion::assert_equals(__FUNCTION__, APIS_TEST_CURRENT_APIS_NUMBER,
                           ApiFactory::get_api_names().size());
}

void apis_test() {
  std::cout << std::endl << "APIs tests : " << std::endl;
  Assertion::test(test_apis_get_apis, "test_apis_get_apis");
  Assertion::test(test_apis_get_names, "test_apis_get_names");
}
