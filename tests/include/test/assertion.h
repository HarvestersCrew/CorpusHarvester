#ifndef ASSERTION_H
#define ASSERTION_H

#include <iostream>

#include "utils/exceptions.h"

class Assertion {

public:
  static void assert_contains(std::string function_name, int container_size,
                              std::string container[], std::string contained) {
    bool trouve = false;
    for (int i = 0; i < container_size; ++i) {
      if (container[i] == contained) {
        trouve = true;
        break;
      }
    }
    if (!trouve) {
      throw TestFailedException(function_name, contained, "");
    }
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

  static void assert_throw(std::string function_name,
                           std::string exception_name) {
    throw TestFailedException(function_name, exception_name, "nothing");
  }

  static void test(void (*func)(void), std::string function_name) {
    func();
    std::cout << "[OK] " << function_name << "()" << std::endl;
  }
};

#endif
