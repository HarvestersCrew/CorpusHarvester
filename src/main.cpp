#include <download/apis/ApiTwitter.h>
#include <iostream>

int main(int argc, char const *argv[]) {
  ApiTwitter api;
  api.insert_settings("api_key", "test");
  api.start();
  std::cout << api.get_required_settings_string() << std::endl;
  return 0;
}
