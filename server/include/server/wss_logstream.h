#ifndef WSS_LOGSTREAM_H
#define WSS_LOGSTREAM_H

#include "utils/logger.h"
#include <streambuf>
#include <string>

class WssLogstream : public std::streambuf {
private:
  std::string buffer;

protected:
  int overflow(int ch) override {
    buffer.push_back((char)ch);
    if (ch == '\n') {
      // End of line, write to logging output and clear buffer.
      logger::debug("WSS: " + buffer);
      buffer.clear();
    }
    return ch;
  }
};

#endif
