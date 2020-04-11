#ifndef BROADCAST_LOG_OUTPUT_H
#define BROADCAST_LOG_OUTPUT_H

#include "utils/logger.h"
#include <iostream>

using std::endl;

class BroadcastLogOutput : public LoggerCustomOutput {
public:
  virtual void output(const std::string &msg) const;
};

#endif
