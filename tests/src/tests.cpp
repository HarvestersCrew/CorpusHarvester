#include "database/harvester_database.h"
#include "test/apis_test.h"
#include "test/cli_parser_test.h"
#include "test/database_test.h"
#include "test/indexation_test.h"
#include "test/logger_test.h"
#include "test/storage_test.h"

int main() {
  std::cout << std::endl << "Tests : " << std::endl;

  try {
    HarvesterDatabase::drop();
  } catch (const ClosedDatabaseException &e) {
  }

  // logger_test();
  logger::set_level(logger::level::NONE);
  database_test();
  logger::set_level(logger::level::NONE);
  storage_test();
  apis_test();
  cli_parser_test();
  indexation_test();

  try {
    HarvesterDatabase::drop();
  } catch (const ClosedDatabaseException &e) {
  }

  std::cout << std::endl
            << "Runs : "
            << Assertion::get_successes() + Assertion::get_failures()
            << " | Errors : " << Assertion::get_failures()
            << " | Successes : " << Assertion::get_successes() << std::endl;
  return 0;
}
