#include "database/harvester_database.h"
#include "server/websocket_server.h"
#include <chrono>
#include <cppconn/exception.h>
#include <iostream>
#include <thread>

using std::cout;
using std::endl;

int main(int argc, char **argv) {
  bool db_ready = false;

  do {
    try {
      HarvesterDatabase::init();
      db_ready = true;
    } catch (const sql::SQLException &e) {
      std::cout << "Failure connecting to DB, will retry in 10 seconds"
                << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  } while (!db_ready);

  WebsocketServer::init();
  WebsocketServer::run();
}
