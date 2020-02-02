#include <iostream>
#include <list>
#include <string>

using std::cout;
using std::endl;
using std::list;
using std::string;

class Test {
  list<string> test;
  list<string> test2;

public:
  void run() {
    test.push_back("t1");
    test.push_back("t2");
    test.push_back("t3");
    cout << "Debut : " << endl;
    for (string str : test) {
      test2.push_back(str);
    }
    test.clear();
  }

  void print() {
    cout << "Debut test : " << endl;
    for (string str : test) {
      cout << str << endl;
    }
    cout << "Debut test2 : " << endl;
    for (string str : test2) {
      cout << str << endl;
    }
  }
};

int main(int argc, char const *argv[]) {
  Test test = Test();
  test.print();
  test.run();
  test.print();
  return 0;
}
