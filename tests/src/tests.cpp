#include "test/indexation_test.h"
#include "test/logger_test.h"
#include "test/storage_test.h"

int main() {
  std::cout << "Tests : " << std::endl;
  indexation_test();
  storage_test();
  logger_test();
  return 0;
}
