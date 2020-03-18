//
// Created by Conlan Cesar on 3/18/20.
//

#ifndef IVP_EXTEND_NMEAUTILS_H
#define IVP_EXTEND_NMEAUTILS_H

#include <string>


class NMEAUtils {
  public:
    static bool genNMEAChecksum(std::string nmeaString, std::string& checksum);
    static bool genNMEATimestamp(time_t time, std::string &timeStr);

    static bool failsTimeCheck(const std::string &t2, double &diff, double maxDelta = 3);
    static double timeDifference(const std::string &t2, time_t time = std::time(nullptr));
};


#endif //IVP_EXTEND_NMEAUTILS_H
