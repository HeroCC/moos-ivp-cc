#include "LooseNinja.h"
#include "MBUtils.h"
#include "NMEAUtils.h"

#include <fcntl.h>

// The Neptune interface has a looser requirement for what a "valid" NMEA string is
// Reimplement the isValidNMEA check on our side so they don't have to rewrite code
bool LooseNinja::isValidNMEA(std::string nmeaString, bool strict) {
  nmeaString = findReplace(nmeaString, "\r\n", ""); // replace \r\n with \n (if it doesn't already)
  nmeaString = findReplace(nmeaString, "\n", ""); // now that the line ends with \n, replace it back to \r\n
  nmeaString += "\r\n";

  return SockNinja::isValidNMEA(nmeaString, strict && m_validate_checksum);
}

bool LooseNinja::isCheckSumChar(char c) {
  return SockNinja::isCheckSumChar(toupper(c));
}
