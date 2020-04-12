#include "api/ManagerRequest.h"
#include "websocket_server.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char **argv) {
  HarvesterDatabase::init();
  WebsocketServer::init();
  WebsocketServer::run();
}
