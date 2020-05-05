#include "database/pool_db.h"
#include "database/setting.h"
#include "test/api_parameter_test.h"
#include "test/api_request_builder_test.h"
#include "test/apis_test.h"
#include "test/cli_parser_test.h"
#include "test/database_test.h"
#include "test/indexation_test.h"
#include "test/logger_test.h"
#include "test/storage_test.h"
#include "utils/logger.h"

int main() {
  std::cout << std::endl << "Tests : " << std::endl;

  Setting::set_default_value(Setting::LOGGER_LEVEL,
                             std::to_string(logger::level::NONE));

  database_test();

  PoolDB::init(1);

  logger_test();
  storage_test();
  apis_test();
  api_parameter_test();
  api_request_builder_test();
  cli_parser_test();
  indexation_test();

  // PoolDB::drop();

  std::cout << std::endl
            << "Runs : "
            << Assertion::get_successes() + Assertion::get_failures()
            << " | Errors : " << Assertion::get_failures()
            << " | Successes : " << Assertion::get_successes() << std::endl;
  return 0;
}
