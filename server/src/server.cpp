#include "database/pool_db.h"
#include "server/websocket_server.h"
#include "utils/cli_parser.h"
#include <chrono>
#include <cppconn/exception.h>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>

using std::cout;
using std::endl;
using std::get;

#define POOL_DB_SERVER_SIZE 10
#define SERVER_PORT 9002
#define FILE_SERVER_PORT 9003

int main(int argc, char **argv) {

  cli_command cli_root(argv[0], "Harvester server");
  cli_root.add_option("port", "Port to be used by the websockets server",
                      false);
  cli_root.add_option("file_server_port", "Port to be used by the file server",
                      false);

  std::vector<string> all_args(argv + 1, argv + argc);
  unsigned int port = SERVER_PORT;
  unsigned int file_port = FILE_SERVER_PORT;

  try {
    const auto cli_res = cli_parser::parse(cli_root, all_args);

    if (get<1>(cli_res).find("port") != get<1>(cli_res).end()) {
      std::string port_str = get<1>(cli_res).at("port");
      try {
        port = std::stoul(port_str);
      } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid port argument, must be an unsigned int"
                  << std::endl;
        exit(-1);
      }
    }

    if (get<1>(cli_res).find("file_server_port") != get<1>(cli_res).end()) {
      std::string port_str = get<1>(cli_res).at("file_server_port");
      try {
        file_port = std::stoul(port_str);
      } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid port argument, must be an unsigned int"
                  << std::endl;
        exit(-1);
      }
    }

  } catch (const cli_parser_help_asked_exception &e) {
    exit(0);
  }

  bool db_ready = false;
  do {
    try {
      PoolDB::init(POOL_DB_SERVER_SIZE);
      db_ready = true;
    } catch (const sql::SQLException &e) {
      cout << "Failure connecting to DB, will retry in 10 seconds" << endl;
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  } while (!db_ready);

  WebsocketServer::init(port, file_port);
  WebsocketServer::run();
}
