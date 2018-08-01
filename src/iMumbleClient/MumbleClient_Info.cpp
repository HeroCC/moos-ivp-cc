/****************************************************************/
/*   NAME: Conlan Cesar                                         */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: MumbleClient_Info.cpp                                */
/*   DATE: Summer 2018                                          */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "MumbleClient_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iMumbleClient application is used for connecting vehicles ");
  blk("  to a Mumble VoIP server                                       ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iMumbleClient file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iMumbleClient with the given process name         ");
  blk("      rather than iMumbleClient.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iMumbleClient.        ");
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
  blk("All configurations in this app are optional, defaults shown");
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iMumbleClient Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iMumbleClient                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  AUDIO_TX_KEY = SPEECH_BUTTON                                  ");
  blk("  SERVER_IP = localhost                                         ");
  blk("  SERVER_PORT = 64738                                           ");
  blk("  CLIENT_USERNAME = DefaultsToCommunityName                     ");
  blk("  CHANNEL_ID = -1                                               ");
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
  blu("iMumbleClient INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  SPEECH_BUTTON = true     ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  VOIP_AUDIO_TX = true/false when audio is being sent      ");
  blk("  VOIP_HEARING_AUDIO = status=TRUE,inChan=blue       ");
  blk("       Status is true/false when we are sending audio,"
      "       InChan is the name or ID of the channel we think we are in");
  blk("  VOIP_SENDING_AUDIO = status=TRUE,inChan=blue      ");
  blk("       Same as above                  ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iMumbleClient", "gpl");
  exit(0);
}

