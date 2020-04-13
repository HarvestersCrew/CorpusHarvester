#include "database/harvester_database.h"
#include "server/websocket_server.h"

using std::cout;
using std::endl;

int main(int argc, char **argv) {
  HarvesterDatabase::init();
  WebsocketServer::init();
  WebsocketServer::run();
}
