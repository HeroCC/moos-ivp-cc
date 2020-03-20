/****************************************************************/
/*   NAME:                                              */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: Neptune_Info.cpp                               */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "Neptune_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iNeptune application is used for communication with       ");
  blk("  SeeByte's Neptune marine vehicle platform via an NMEA link.   ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iNeptune file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iNeptune with the given process name         ");
  blk("      rather than iNeptune.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iNeptune.        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iNeptune Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iNeptune                              ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  Port = 10110              // Neptune NMEA Port (default 10110)");
  blk("  Host = localhost          // Neptune IP address / host (default localhost)");
  blk("  ReconnectInterval = 3     // Seconds, how often to try to reconnect to NMEA server");
  blk("  ValidateChecksum = true   // Should we validate incoming messages' checksum");
  blk("  MaximumTimeDifference = 3 // Seconds, -1 to ignore");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iNeptune INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NAV_SPEED, NAV_HEADING, NAV_DEPTH, NAV_LAT, NAV_LONG, NAV_ALTITUDE");
  blk("     -- For NMEA message $MONVG                                 ");
  blk("                                                                ");
  blk("  DEPLOY, IVPHELM_ALLSTOP, NEPTUNE_SURVEY_VISITED_POINT (from waypoint survey behavior)");
  blk("     -- For NMEA message $MOMIS                                 ");
  blk("                                                                ");
  blk("  Any other MOOS mailing could potentially be subscribed ");
  blk("  to with $MOREG, and returned with $MOVAL                      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  SENT_NMEA_MESSAGE       -- Outgoing NMEA message      ");
  blk("  INCOMING_NMEA           -- Incoming NMEA message             ");
  blk("  NEPTUNE_SURVEY_UPDATE   -- Survey points requested by Neptune      ");
  blk("  NEPTUNE_SURVEY_TRAVERSE -- Should survey behavior be active?      ");
  blk("  DEPLOY                  -- Deploys behaviors (via $MOHLM)      ");
  blk("  GIVEN_OBSTACLE          -- Region to avoid (via $MOAVD)      ");
  blk("  Any other message could be poked with $MOPOK      ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iNeptune", "gpl");
  exit(0);
}

