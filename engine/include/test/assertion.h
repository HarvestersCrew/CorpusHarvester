#ifndef ASSERTION_H
#define ASSERTION_H

#include <iostream>

#include "utils/exceptions.h"

using std::cout;
using std::endl;
using std::string;

class Assertion {

public:
  static void assertEquals(string functionName, string expected, string found) {
    if (expected != found) {
      throw TestFailedException(functionName, expected, found);
    }
  }

  static void assertEquals(string functionName, int expected, int found) {
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