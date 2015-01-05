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

#include "ServerStateBase.h"
#include "ServerInitializationState.h"
#include "ServerStartUpState.h"
#include "ServerStandByState.h"
#include "ServerTrackingState.h"

#include "ServerInfoBase.h"
#include "SessionController.h"


int main(int argc, char* argv[])
{

  SessionController * controller = new SessionController;

  // Must create ServerInfoBase object before registering phases. 
  ServerInfoBase * sinfo = new ServerInfoBase;
  controller->SetServerInfo(sinfo);

  //------------------------------------------------------------
  // Setup workphases
  controller->RegisterState(new ServerStartUpState);
  controller->RegisterState(new ServerInitializationState);
  controller->RegisterState(new ServerStandByState);
  controller->RegisterState(new ServerTrackingState);

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


