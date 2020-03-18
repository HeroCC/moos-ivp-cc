//
// Created by Conlan Cesar on 3/18/20.
//

#include <sstream>
#include <iomanip>
#include "NMEAUtils.h"

bool NMEAUtils::genNMEAChecksum(std::string nmeaString, std::string& checksum) {
  unsigned char xCheckSum = 0;
  std::string::iterator p;

  // Ensure we have a valid NMEA String
  if (nmeaString.at(0) != '$' || nmeaString.at(nmeaString.size() - 1) != '*') return false;

  // XOR every byte as a checksum
  for (p = nmeaString.begin(); p != nmeaString.end(); p++) {
    xCheckSum ^= *p;
  }

  std::ostringstream os;
  os.flags(std::ios::hex);
  os << (int) xCheckSum;
  std::string sExpected = os.str();

  if (sExpected.length() < 2)
    sExpected = "0" + sExpected;

  checksum = sExpected;

  return true;
}

bool NMEAUtils::genNMEATimestamp(time_t time, std::string& timeStr) {
  try {
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H%M%S");
    timeStr = ss.str();
    return true;
  } catch (...) {}
  return false;
}

double NMEAUtils::timeDifference(const std::string& t2, const time_t time) {
  struct std::tm* tm = std::localtime(&time); // Assume we have the same timezone
  std::istringstream ss(t2);
  ss >> std::get_time(tm, "%H%M%S"); // Override hour, minute, second
  time_t tx_unix_time = mktime(tm);

  return abs(time - tx_unix_time);
}

bool NMEAUtils::failsTimeCheck(const std::string& t2, double& diff, double maxDelta) {
  if (maxDelta < 0) {
    return false; // If time delta is below zero, consider it disabled
  }
  double df = timeDifference(t2);
  diff = df;
  return (df > maxDelta);
}