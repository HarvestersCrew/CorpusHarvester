#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include "indexation/file.h"

/**
 * This interface describe a request builder using the design patern Builder
 */
class RequestBuilder {

public:
  RequestBuilder() {}

  /**
   * Adds a condition to the request being built
   * @param condition_name the name of the condition
   * @param condition_value the value of the condition
   * @param op the logical operator between the name and the value
   * @return a pointer to itself
   */
  virtual RequestBuilder *add_condition(std::string condition_name,
                                        std::string conditionValue,
                                        std::string op) = 0;

  /**
   * Adds a logical And to the request being built
   * @return a pointer to itself
   */
  virtual RequestBuilder *logical_and() = 0;

  /**
   * Adds a logical Or to the request being built
   * @return a pointer to itself
   */
  virtual RequestBuilder *logical_or() = 0;

  /**
   * Finalizes the request being built, executes it and return the files
   * matching it
   * @return the list of all files matching the built request
   */
  virtual std::list<File *> build() = 0;

  /**
   * Clears the builder
   */
  virtual void clear() = 0;
};

#endif
