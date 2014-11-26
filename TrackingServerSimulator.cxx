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

typedef std::vector< ServerPhaseBase* > WorkphaseList;

enum {
  SESSION_ACTIVE,
  SESSION_INACTIVE
};

static int SessionStatus; // SESSION_*
static WorkphaseList PhaseList;
static ServerPhaseBase* CurrentWorkphase; // Current workphase

static void MonitorThread(void * ptr);
int Session(igtl::Socket * socket, WorkphaseList& PhaseList);

int main(int argc, char* argv[])
{


  SessionStatus = SESSION_INACTIVE;

  //------------------------------------------------------------
  // Setup workphases
  PhaseList.push_back(new ServerUndefinedPhase);
  PhaseList.push_back(new ServerInitializationPhase);
  //PhaseList.push_back(new ServerPlanningPhase);
  //PhaseList.push_back(new ServerCalibrationPhase);
  //PhaseList.push_back(new ServerTargetingPhase);
  //PhaseList.push_back(new ServerMoveToTargetPhase);
  //PhaseList.push_back(new ServerManualPhase);
  //PhaseList.push_back(new ServerStopPhase);
  //PhaseList.push_back(new ServerEmergencyPhase);

  CurrentWorkphase = NULL;

  std::cerr << std::endl;

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

    WorkphaseList::iterator wit;
    for (wit = PhaseList.begin(); wit != PhaseList.end(); wit ++)
      {
      std::list< std::string > typeList;
      typeList = (*wit)->GetDefectTypeList();
      std::list< std::string >::iterator tit;
      for (tit = typeList.begin(); tit != typeList.end(); tit ++)
        {
        std::cerr << "    " << (*wit)->Name() << ":" << (*tit) << "  :  "
                  << (*wit)->GetDefectTypeDescription((*tit).c_str()) << std::endl;
        }
      }
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
      WorkphaseList::iterator witer;
      for (witer = PhaseList.begin(); witer != PhaseList.end(); witer ++)
        {
        if (phase.compare((*witer)->Name()) == 0)
          {
          (*witer)->SetDefectStatus(type.c_str(), 1);
          }
        }
      }
    }

  //------------------------------------------------------------
  // List active defects
  std::cerr << "MESSAGE: Defect status:" << std::endl;
  WorkphaseList::iterator wit;
  for (wit = PhaseList.begin(); wit != PhaseList.end(); wit ++)
    {
    std::list< std::string > typeList;
    typeList = (*wit)->GetDefectTypeList();
    std::list< std::string >::iterator tit;
    for (tit = typeList.begin(); tit != typeList.end(); tit ++)
      {
      std::string status = (*wit)->GetDefectStatus((*tit).c_str())? "ON" : "OFF";
      std::cerr << "MESSAGE:    " << (*wit)->Name() << ":" << (*tit) << "  :  "
                << status << std::endl;
      }
    }

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  if (r < 0)
    {
    std::cerr << "ERROR: Cannot create a server socket." << std::endl;
    exit(0);
    }

  igtl::Socket::Pointer socket;
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      std::cerr << "MESSAGE: Client connected. Starting a session..." << std::endl;

      SessionStatus = SESSION_ACTIVE;
      igtl::MultiThreader::Pointer threader;
      threader = igtl::MultiThreader::New();
      threader->SpawnThread((igtl::ThreadFunctionType) &MonitorThread, &PhaseList);
      Session(socket, PhaseList);
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  socket->CloseSocket();

}

void MonitorThread(void * ptr)
{
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  WorkphaseList* PhaseList = static_cast<WorkphaseList *>(info->UserData);
  while (SessionStatus == SESSION_ACTIVE)
    {
    if (CurrentWorkphase != NULL)
      {
      CurrentWorkphase->TimerHandler(10); // TODO: Give a correct time stamp.
      igtl::Sleep(100); // wait for 100 ms
      }
    }
}


int Session(igtl::Socket * socket, WorkphaseList& PhaseList)
{
  RobotStatus * rs = new RobotStatus();

  //------------------------------------------------------------
  // Set socket and robot status
  std::vector< ServerPhaseBase* >::iterator iter;
  for (iter = PhaseList.begin(); iter != PhaseList.end(); iter ++)
    {
    //std::cerr << "MESSAGE: Setting up " << (*iter)->Name() << " phase." << std::endl;
    (*iter)->SetSocket(socket);
    (*iter)->SetRobotStatus(rs);
    }

  //------------------------------------------------------------
  // Set undefined phase as the current phase;
  std::vector<  ServerPhaseBase* >::iterator currentPhase = PhaseList.begin();

  int connect = 1;

  //------------------------------------------------------------
  // loop
  while (connect)
    {
    if ((*currentPhase)->Process())
      {
      // If Process() returns 1, phase change has been requested.
      std::string requestedWorkphase = (*currentPhase)->GetNextWorkPhase();
      std::string queryID = (*currentPhase)->GetQueryID();
      
      // Find the requested workphase
      std::vector<  ServerPhaseBase* >::iterator iter;
      for (iter = PhaseList.begin(); iter != PhaseList.end(); iter ++)
        {
        if (strcmp((*iter)->Name(), requestedWorkphase.c_str()) == 0)
          {
          // Change the current phase
          currentPhase = iter;
          (*currentPhase)->Enter(queryID.c_str()); // Initialization process
          }
        }
      }
    }
  
  return 1;
}



