/*=========================================================================

  Program:   Tracking Server Simulator 
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"
#include "igtlTimeStamp.h"

#include "ServerPhaseBase.h"
#include "ServerUndefinedPhase.h"
#include "ServerInitializationPhase.h"
//#include "ServerSimulatorPlanningPhase.h"
//#include "ServerSimulatorCalibrationPhase.h"
//#include "ServerSimulatorTargetingPhase.h"
//#include "ServerSimulatorMoveToTargetPhase.h"
//#include "ServerSimulatorManualPhase.h"
//#include "ServerSimulatorStopPhase.h"
//#include "ServerSimulatorEmergencyPhase.h"
#include "ServerStatus.h"
#include "SessionController.h"

typedef std::vector< ServerPhaseBase* > WorkphaseList;

enum {
  SESSION_ACTIVE,
  SESSION_INACTIVE
};

static int TimerInterval;
static int SessionStatus; // SESSION_*
static WorkphaseList PhaseList;
static ServerPhaseBase* CurrentWorkphase; // Current workphase


static void MonitorThread(void * ptr);
int Session(igtl::Socket * socket, WorkphaseList& PhaseList);

int main(int argc, char* argv[])
{

  SessionController * controller = new SessionController;

  //------------------------------------------------------------
  // Setup workphases
  controller->RegisterPhase(new ServerUndefinedPhase);
  controller->RegisterPhase(new ServerInitializationPhase);
  //controller->RegisterPhase(new ServerPlanningPhase);
  //controller->RegisterPhase(new ServerCalibrationPhase);
  //controller->RegisterPhase(new ServerTargetingPhase);
  //controller->RegisterPhase(new ServerMoveToTargetPhase);
  //controller->RegisterPhase(new ServerManualPhase);
  //controller->RegisterPhase(new ServerStopPhase);
  //controller->RegisterPhase(new ServerEmergencyPhase);

  //------------------------------------------------------------
  // Parse Arguments
  if (argc < 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> [<defect_1> [<defect_2> ... [<defect_N>]...]]"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <defect_X> : Defect to activate in 'PHASE:TYPE' format." << std::endl;
    std::cerr << std::endl;
    std::cerr << "  Available defect types:" <<std::endl;

    controller->PrintAvailableDefectTypes();
    std::cerr << std::endl;

    exit(0);
    }

  int    port     = atoi(argv[1]);

  //------------------------------------------------------------
  // Get a list of defects and set them to the workphase list.
  // Defects are listed as arguments in "PHASE:DEFECT_TYPE" format. 
  for (int i = 2; i < argc; i ++)
    {
    std::string astr = argv[i];
    std::size_t found = astr.find_first_of(":");
    if (found != std::string::npos)
      {
      std::string phase = astr.substr(0, found);
      std::string type  = astr.substr(found+1, std::string::npos);
      if (!controller->ActivateDefect(phase.c_str(), type.c_str()))
        {
        std::cerr << "ERROR: Illegal defect phase/type: " << phase << ": " << type << std::endl;
        }
      }
    }

  //------------------------------------------------------------
  // List active defects
  std::cerr << "MESSAGE: Defect status:" << std::endl;
  controller->PrintDefectStatus();

  //------------------------------------------------------------
  // Run the session controller

  controller->Run();

}


