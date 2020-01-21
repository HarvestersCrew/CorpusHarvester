//
// Created by rere on 21/01/2020.
//

#ifndef CORPUSHARVESTER_DATA_H
#define CORPUSHARVESTER_DATA_H

class Data {
private:
  /**
   * Identification of the data.
   */
  std::string id;

  /**
   * Type of our data.
   * TODO :: Maybe in future version, put a enum ?
   */
  std::string type;

  /**
   * Path of the data.
   */
  std::string path;

public:
};

#endif // CORPUSHARVESTER_DATA_H
