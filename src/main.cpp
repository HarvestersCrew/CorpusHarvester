#include <ApiTwitter.h>
#include <iostream>

int main(int argc, char const *argv[]) {
  std::cout << "Hello world!" << std::endl;
  ApiTwitter api;
  std::cout << api.get_required_settings_string() << std::endl;
  return 0;
}
