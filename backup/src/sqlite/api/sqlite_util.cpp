#include <iostream>
#include <string.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

using std::cerr;
using std::endl;

/*
* Prints an error message and exits the program if the caught code isn't the expected one
*
* error : the caught error
* expected_code : the expected
* error_label : the label of the error message
* error_message : the error message
*
* returns 1 if an error occured, 0 otherwise
*/
int manage_errors(const int error, const int expected_code, const char *error_label, const char *error_message) {
    if (error != expected_code) {
        cerr << "Code n°" << error << " was caught while expecting code n°" << expected_code << " :" << endl << error_label << " : {" << error_message << "}" << endl;
        // exit(0);
        return 1;
    }
    return 0;
}