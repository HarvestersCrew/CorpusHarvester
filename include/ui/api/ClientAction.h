//
// Created by rere on 21/01/2020.
//

#ifndef CORPUSHARVESTER_CLIENTACTION_H
#define CORPUSHARVESTER_CLIENTACTION_H

class ClientAction {
private:
  /**
   * Show a help menu that indicate to the user all the possible actions
   * available.
   */
  virtual void showHelpMenu() = 0;
};

#endif // CORPUSHARVESTER_CLIENTACTION_H
