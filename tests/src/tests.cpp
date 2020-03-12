#include "test/cli_parser_test.h"
#include "test/database_test.h"
#include "test/indexation_test.h"
#include "test/logger_test.h"
#include "test/storage_test.h"

int main() {
  std::cout << std::endl << "Tests : " << std::endl;

  int function_count = 5;
  std::pair<int, int> (*test_functions[])(void) = {logger_test, cli_parser_test,
                                                   database_test, storage_test,
                                                   indexation_test};
  int success_count = 0;
  int test_count = 0;
  std::pair<int, int> test_results;
  success_count += test_results.first;
  test_count += test_results.second;

  for (int i = 0; i < function_count; i++) {
    test_results = test_functions[i]();
    success_count += test_results.first;
    test_count += test_results.second;
    if (i == 0) { // Reset logger level after logger_test
      logger::set_output(logger::output::STDOUT);
      logger::set_level(logger::level::NONE);
    }
  }

  std::cout << std::endl
            << "Runs : " << test_count
            << " | Errors : " << test_count - success_count
            << " | Success : " << success_count << std::endl;

  return 0;
}
