//
// Created by rere on 17/01/2020.
//

#ifndef CORPUSHARVESTER_CLIENT_REQUEST_H
#define CORPUSHARVESTER_CLIENT_REQUEST_H

#include <deque>
#include <string>

/**
 * Help command.
 */
void showHelpMenu();

/**
 * Create command.
 * @param parameters list of our parameters.
 */
void createCorpus(const std::deque<std::string>& parameters);

#endif //CORPUSHARVESTER_CLIENT_REQUEST_H
