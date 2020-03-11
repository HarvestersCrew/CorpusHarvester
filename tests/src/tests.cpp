#include "test/cli_parser_test.h"
#include "test/database_test.h"
#include "test/indexation_test.h"
#include "test/logger_test.h"
#include "test/storage_test.h"

int main() {
  std::cout << std::endl << "Tests : " << std::endl;
  logger_test();
  logger::set_level(logger::level::INFO);
  cli_parser_test();
  database_test();
  storage_test();
  indexation_test();
  return 0;
}
