#ifndef BROADCAST_LOG_OUTPUT_H
#define BROADCAST_LOG_OUTPUT_H

#include "server/websocket_server.h"
#include "utils/logger.h"
#include "utils/nlohmann/json.hpp"

using nlohmann::json;
using std::endl;

class BroadcastLogOutput : public LoggerCustomOutput {
public:
  virtual void output(const std::string &msg) const;
};

#endif
