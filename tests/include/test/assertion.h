#ifndef ASSERTION_H
#define ASSERTION_H

#include "database/harvester_database.h"
#include "utils/exceptions.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <iostream>
#include <iterator>
#include <sstream>

class Assertion {

public:
  static int assert_contains(std::string function_name, int container_size,
                             std::string container[], std::string contained) {
    bool trouve = false;
    int i;
    for (i = 0; i < container_size; ++i) {
      if (container[i] == contained) {
        trouve = true;
        break;
      }
    }
    if (!trouve) {
      throw TestFailedException(function_name, contained, "");
    }
    return i;
  }

  static void assert_equals(std::string function_name, std::string expected,
                            std::string found) {
    if (expected != found) {
      throw TestFailedException(function_name, expected, found);
    }
  }

  static void assert_equals(std::string function_name, int expected,
                            int found) {
    if (expected != found) {
      throw TestFailedException(function_name, std::to_string(expected),
                                std::to_string(found));
    }
  }

  static void assert_equals(std::string function_name, bool expected,
                            bool found) {
    if (expected != found) {
      throw TestFailedException(function_name, std::to_string(expected),
                                std::to_string(found));
    }
  }

  static void assert_equals(std::string function_name, void *expected,
                            void *found) {
    if (expected != found) {
      std::stringstream ss_expected, ss_found;
      ss_expected << expected;
      ss_found << found;
      throw TestFailedException(function_name, ss_expected.str(),
                                ss_found.str());
    }
  }

  static void assert_not_equals(std::string function_name,
                                std::string not_expected, std::string found) {
    if (not_expected == found) {
      throw TestFailedException(function_name, not_expected, found);
    }
  }

  static void assert_true(std::string function_name, bool found) {
    if (!found) {
      throw TestFailedException(function_name, "true", std::to_string(found));
    }
  }

  static void assert_false(std::string function_name, bool found) {
    if (found) {
      throw TestFailedException(function_name, "false", std::to_string(found));
    }
  }

  template <class T>
  static void assert_nullptr(std::string function_name, T object) {
    if (object != nullptr) {
      throw TestFailedException(function_name, "nullptr", "A filled object");
    }
  }

  template <class T>
  static void assert_not_nullptr(std::string function_name, T object) {
    if (object == nullptr) {
      throw TestFailedException(function_name, "An filled object", "nullptr");
    }
  }

  static void assert_throw(std::string function_name,
                           std::string exception_name) {
    throw TestFailedException(function_name, exception_name, "nothing");
  }

  static void test(void (*func)(void), std::string function_name) {
    func();
    std::cout << "{{ OK }} " << function_name << "()" << std::endl;
  }

  static std::pair<int, int> test_all(void (*test_functions[])(void),
                                      std::string test_functions_name[],
                                      int function_count) {
    std::pair<int, int> successes_count = std::pair(0, 0);

    for (int i = 0; i < function_count; i++) {
      try {
        Assertion::test(test_functions[i], test_functions_name[i]);
        successes_count.first++;
      } catch (const TestFailedException &e) {
        std::cerr << e.what() << std::endl;
      } catch (const CommandException &e) {
        std::cerr << e.what() << std::endl;
      } catch (sql::SQLException &e) {
        print_sql_exception(e);
      } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
      }
      successes_count.second++;
    }
    HarvesterDatabase::close();
    return successes_count;
  }
};

#endif
