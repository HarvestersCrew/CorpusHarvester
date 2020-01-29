#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include <list>
#include <string>

#include "indexation/file.h"

using std::list;
using std::string;

/**
 * This interface describe a request builder using the design patern Builder
 */
class RequestBuilder {

public:
  RequestBuilder() {}

  /**
   * addCondition() adds a condition to the request being built
   * @param conditionName the name of the condition
   * @param conditionValue the value of the condition
   * @param op the logical operator between the name and the value
   * @return a pointer to itself
   */
  virtual RequestBuilder *addCondition(string conditionName,
                                       string conditionValue, string op) = 0;

  /**
   * logicalAnd() adds a logical And to the request being built
   * @return a pointer to itself
   */
  virtual RequestBuilder *logicalAnd() = 0;

  /**
   * logicalOr() adds a logical Or to the request being built
   * @return a pointer to itself
   */
  virtual RequestBuilder *logicalOr() = 0;

  /**
   * build() finalizes the request being built, executes it and return the files
   * matching it
   * @return the list of all files matching the built request
   */
  virtual list<File *> build() = 0;

  /**
   * clear() clears the builder
   */
  virtual void clear() = 0;
};

#endif
