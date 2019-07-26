/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: VcGenMon_Info.cpp                                    */
/*   DATE: Summer 2019                                          */
/****************************************************************/

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>
#include "VCGenCmd.h"

std::string VCGenCmd::execVcgenCommand(const std::string command, const std::string vcPath) {
  char buffer[128];
  std::string output;
  std::string realCommand = vcPath + " '" + command + "' 2>&1"; // Dump cerr into cout, otherwise cerr is lost
  FILE* pipe = popen(realCommand.c_str(), "r");
  if (!pipe) {
    std::cerr << "Failed to open pipe! Is " + command + " in your $PATH?";
  }
  try {
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
      output += buffer;
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    pclose(pipe);
    throw;
  }
  pclose(pipe);
  return output;
}

std::vector<std::string> VCGenCmd::getValue(const std::string &vcOutput) {
  std::stringstream ss(vcOutput);
  std::string splitted;
  std::vector<std::string> splitStrings;
  while (std::getline(ss, splitted, '=')) {
    splitStrings.push_back(splitted);
  }
  return splitStrings;
}

bool VCGenCmd::sanityCheck() {
  return !access("/usr/bin/vcgencmd", X_OK);
  // access() returns false if executable, true if not executable
}

float VCGenCmd::getTemperature() {
  float value = -1;
  std::vector<std::string> measuredValues = getValue(execVcgenCommand("measure_temp"));

  if (measuredValues.size() > 1)
    value = stof(measuredValues.at(1));

  return value;
}

float VCGenCmd::getVoltage() {
  float value = -1;
  std::vector<std::string> measuredValues = getValue(execVcgenCommand("measure_volts"));

  if (measuredValues.size() > 1)
    value = stof(measuredValues.at(1));

  return value;
}

std::string VCGenCmd::getThrottleHex() {
  std::string value = "0xFFFFF"; // 0x50000
  std::vector<std::string> measuredValues = getValue(execVcgenCommand("get_throttled"));

  if (measuredValues.size() > 1)
    value = measuredValues.at(1);

  return value.replace(0, 2, "");
}

std::string VCGenCmd::getThrottleBinary() {
  std::string hexVal = getThrottleHex();

  return hexStrToBinaryStr(hexVal);
}

long VCGenCmd::getClockSpeed(std::string clock) {
  long value = -1;
  std::vector<std::string> measuredValues = getValue(execVcgenCommand("measure_clock " + clock));

  if (measuredValues.size() > 1)
    value = std::stol(measuredValues.at(1));

  return value;
}

// Hex manipulations from https://stackoverflow.com/a/18311086/1709894 🙇
const char* VCGenCmd::hexCharToBinary(const char c) {
  switch(toupper(c)) {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default:  return "0000";
  }
}

std::string VCGenCmd::hexStrToBinaryStr(const std::string& hex) {
  std::string bin;
  for(unsigned i = 0; i != hex.length(); ++i)
    bin += hexCharToBinary(hex[i]);
  return bin;
}



