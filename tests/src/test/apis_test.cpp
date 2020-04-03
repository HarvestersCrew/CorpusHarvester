#include "test/apis_test.h"

void discover_from_path_test() {

  std::filesystem::create_directory(ApiFactory::get_apis_folder_path() +
                                    "folder.json/");
  ofstream fake_json(ApiFactory::get_apis_folder_path() + "fake.json");
  fake_json << "blabla";
  fake_json.close();

  vector<shared_ptr<api_loader>> apis =
      ApiFactory::discover_from_path(ApiFactory::get_apis_folder_path());
  Assertion::assert_equals(__FUNCTION__, 4, apis.size());

  std::filesystem::remove(ApiFactory::get_apis_folder_path() + "folder.json/");
  std::filesystem::remove(ApiFactory::get_apis_folder_path() + "fake.json");
}

void apis_test() {
  std::cout << std::endl << "APIs tests : " << std::endl;
  Assertion::test(discover_from_path_test, "discover_from_path_test");
}
