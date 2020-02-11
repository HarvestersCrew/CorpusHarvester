#ifndef ASSERTION_H
#define ASSERTION_H

#include <iostream>

#include "utils/exceptions.h"

using std::cout;
using std::endl;
using std::string;

class Assertion {

public:
  static void assert_contains(string functionName, int container_size,
                              string container[], string contained) {
    bool trouve = false;
    for (int i = 0; i < container_size; ++i) {
      if (container[i] == contained) {
        trouve = true;
        break;
      }
    }
    if (!trouve) {
      throw TestFailedException(functionName, contained, "");
    }
  }

  static void assert_equals(string functionName, string expected,
                            string found) {
    if (expected != found) {
      throw TestFailedException(functionName, expected, found);
    }
  }

  static void assert_equals(string functionName, int expected, int found) {
    if (expected != found) {
      throw TestFailedException(functionName, std::to_string(expected),
                                std::to_string(found));
    }
  }

  static void test(void (*func)(void), string functionName) {
    func();
    cout << "[OK] " << functionName << "()" << endl;
  }
};

#endif
