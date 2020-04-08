#ifndef ASSERTION_H
#define ASSERTION_H

#include "utils/exceptions.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <iostream>
#include <sstream>
#include <typeinfo>

class Assertion {

public:
  static int successes;
  static int failures;

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
                           std::string exception_name, int line = -1) {
    throw TestFailedException(function_name, exception_name, "nothing", line);
  }

  static void test(void (*func)(void), std::string function_name) {
    try {
      try {
        func();
        std::cout << "{{ OK }} " << function_name << "()" << std::endl;
        ++Assertion::successes;
      } catch (const std::exception &e) {
        throw TestFailedException(function_name, "no exception",
                                  typeid(e).name());
      }
    } catch (const TestFailedException &e) {
      std::cerr << e.what() << std::endl;
      ++Assertion::failures;
    }
  }

  static int get_successes() { return Assertion::successes; }
  static int get_failures() { return Assertion::failures; }
};

#endif
