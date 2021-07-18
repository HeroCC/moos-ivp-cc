#ifndef NEPTUNE_LOOSENINJA_H
#define NEPTUNE_LOOSENINJA_H

#include "SockNinja.h"

class LooseNinja : public SockNinja {

  // Inherit Constructor
  using SockNinja::SockNinja;

  // Overrides
  protected:
    bool isValidNMEA(std::string str, bool strict) override;
    bool isCheckSumChar(char) override;

  // Custom
  public:
    bool m_validate_checksum = true;

};

#endif