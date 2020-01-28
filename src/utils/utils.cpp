#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>
#include <utils/utils.h>

using std::cout;
using std::endl;
using std::ostringstream;

std::string random_str(const unsigned int len) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist('a', 'z');
  std::string res;
  std::generate_n(std::back_inserter(res), len, [&] { return dist(mt); });
  return res;
}

void fillFileRandomly(File *file, bool tweet, bool isEven) {
  ostringstream isEvenStream;
  isEvenStream << isEven;
  file->addTag("isEven", isEvenStream.str());
  if (tweet) {
    file->addTag("type", "tweet");
  } else {
    file->addTag("type", "article");
  }
  int tweet_subject_length = 3;
  string tweet_subject_values[tweet_subject_length] = {"tank", "kitty",
                                                       "crisquare"};
  for (int i = 0; i < tweet_subject_length; ++i) {
    file->addTag("subject", tweet_subject_values[i]);
  }
}

void print(string toPrint, bool verbose) {
  if (verbose) {
    std::cout << toPrint << std::endl;
  }
}

void printSQLException(sql::SQLException &e) {
  cout << "# ERR: " << e.what();
  cout << " (MySQL error code: " << e.getErrorCode();
  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}