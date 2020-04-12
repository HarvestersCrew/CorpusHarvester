#include "server/broadcast_log_output.h"

void BroadcastLogOutput::output(const std::string &msg) const {
  json j;
  j["message"] = msg;
  WebsocketServer::broadcast_json("logger", j);
}
