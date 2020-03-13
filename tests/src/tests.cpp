#include "database/harvester_database.h"
#include "test/cli_parser_test.h"
#include "test/database_test.h"
#include "test/indexation_test.h"
#include "test/logger_test.h"
#include "test/storage_test.h"

int main() {
  std::cout << std::endl << "Tests : " << std::endl;
  logger_test();
  logger::set_level(logger::level::NONE);
  cli_parser_test();
  database_test();
  storage_test();
  indexation_test();
  HarvesterDatabase::init();
  HarvesterDatabase::drop();
  std::cout << std::endl
            << "Runs : "
            << Assertion::get_successes() + Assertion::get_failures()
            << " | Errors : " << Assertion::get_failures()
            << " | Successes : " << Assertion::get_successes() << std::endl;
  return 0;
}
