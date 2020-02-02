
#include "api/ManagerRequest.h"


std::list<Corpus>
ManagerRequest::visualisation_corpus(
    const std::map<std::string, std::string> &filters,
    const std::map<std::string, std::string> &orders) {

    // Define the string available for the research
    std::string title, latest, oldest;
    std::string date, alphabetical;

    std::map<std::string, std::string>::iterator it;

    if (filters.empty() and orders.empty()) {
      // TODO :: return all the Corpus present in the database
      // Maybe merge with the request bellow in order to have one call.
    }

    // Get the filters available
    if (!filters.empty()) {
        // Check if we have a value for the title
        it = filters.find('title')
        if (it != filters.end()) {
            title = it->second;
        }

        // Check if we have a value for the latest
        it = filters.find('latest')
        if (it != filters.end()) {
            latest = it->second;
        }

        // Check if we have a value for the oldest
        it = filters.find('oldest')
        if (it != filters.end()) {
            oldest = it->second;
        }
    }

    // Get the orders available
    if (!orders.empty()) {
        // Check if we have a value for the date
        it = orders.find('date')
        if (it != orders.end()) {
            date = it->second;
        }

        // Check if we have a value for the latest
        it = orders.find('alphabetical')
        if (it != orders.end()) {
            alphabetical = it->second;
        }
    }


    // TODO :: Do the corresponding request

}
