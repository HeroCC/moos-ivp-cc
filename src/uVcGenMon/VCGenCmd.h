/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: VCGenCmd.h                                           */
/*   DATE: Summer 2019                                          */
/****************************************************************/


#ifndef IVP_EXTEND_VCGENCMD_H
#define IVP_EXTEND_VCGENCMD_H

#include <string>

class VCGenCmd {

public:
    bool sanityCheck();
    float getTemperature();
    float getVoltage();
    long getClockSpeed(std::string clock="arm");
    std::string getThrottleHex();
    std::string getThrottleBinary();

private:
    std::string execVcgenCommand(std::string command, std::string vcPath="/usr/bin/vcgencmd");
    std::vector<std::string> getValue(const std::string &vcOutput);

    std::string hexStrToBinaryStr(const std::string& hex);
    const char* hexCharToBinary(char c);

};


#endif //IVP_EXTEND_VCGENCMD_H
