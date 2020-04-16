
#include "cli/CommandLineInterface.h"
#include "database/pool_db.h"
#include <chrono>
#include <iostream>
#include <thread>

/**
 * Get the command of the user.
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {

  bool db_ready = false;
  do {
    try {
      PoolDB::init(10);
      db_ready = true;
    } catch (const sql::SQLException &e) {
      std::cout << "Failure connecting to DB, will retry in 10 seconds"
                << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  } while (!db_ready);

  CommandLineInterface cli = CommandLineInterface(argc, argv);
  cli.run();
}
