//
// Created by rere on 17/01/2020.
//

#ifndef CORPUSHARVESTER_CLIENT_REQUEST_H
#define CORPUSHARVESTER_CLIENT_REQUEST_H

#include "download/apis/api_twitter.h"
#include "download/download_manager.h"
#include "utils/json.hpp"
#include <deque>
#include <iostream>
#include <string>

/**
 * Help command.
 */
void showHelpMenu();

/**
 * Create command.
 * @param parameters list of our parameters.
 */
void createCorpus(const std::deque<std::string> &parameters);

#endif // CORPUSHARVESTER_CLIENT_REQUEST_H
