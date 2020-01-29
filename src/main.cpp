// #include "ui/cli/client_request.h"
// #include <iostream>

// /**
//  * Get the command of the user.
//  *
//  * @param argc
//  * @param argv
//  * @return
//  */
// int main(int argc, char **argv) {

//   // We have no command.
//   if (argc == 1) {
//     std::cout << "Please specify your command." << std::endl;
//     exit(0);
//   }

//   // Get all the parameters passed through the command line.
//   std::deque<std::string> cmdLineArgs(argv + 1, argv + argc);

//   // Get the first command and remove it form the deque array.
//   std::string firstCommand = cmdLineArgs.front();
//   cmdLineArgs.pop_front();

//   // We print the help menu.
//   if (firstCommand == "help" or firstCommand == "--help" or
//       firstCommand == "-help") {
//     showHelpMenu();
//   } else if (firstCommand == "create") {
//     createCorpus(cmdLineArgs);
//   } else {
//     std::cout
//         << "The command '" << firstCommand
//         << "' doesn't exit. Please check the available commands with 'help'."
//         << std::endl;
//     exit(0);
//   }
//   return 0;
// }
