
#ifndef CORPUSHARVESTER_CLIENTLINEINTERFACE_H
#define CORPUSHARVESTER_CLIENTLINEINTERFACE_H

/**
 * ClientLineInterface class.
 *
 * Create a class that will build a request based on the client input.
 */
class ClientLineInterface {
private:
  /**
   * Arguments of the user input.
   */
  std::deque<std::string> arguments;

public:
  /**
   * ClientLineInterface Constructor.
   *
   * @param arguments Vector sequence of all the arguments.
   */
  ClientLineInterface(const std::deque<std::string> arguments);

  /**
   * Execute the current command and print in the user terminal the output.
   */
  void run() const;
};

#endif // CORPUSHARVESTER_CLIENTLINEINTERFACE_H
