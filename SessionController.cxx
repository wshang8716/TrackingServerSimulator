/*=========================================================================

  Program:   Tracking Server Simulator 
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "SessionController.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"

SessionController::SessionController()
{

  this->PhaseList.clear();
  this->Socket = NULL;
  this->TimerInterval = 100; // Default 100 ms
  this->SessionStatus = SESSION_INACTIVE;
  this->CurrentWorkphase = NULL;
  this->ThreadAlive = 0;
  this->PortNumber = 18944;

}


SessionController::~SessionController()
{

  UnregisterAllPhases();

}


int SessionController::RegisterPhase(ServerPhaseBase* phase)
{

  if (phase)
    {
    this->PhaseList.push_back(phase);
    }

  return this->PhaseList.size();

}


void SessionController::UnregisterAllPhases()
{

  WorkphaseList::iterator wit;

  for (wit = this->PhaseList.begin(); wit != this->PhaseList.end(); wit ++)
    {
    delete *wit;
    }
  this->PhaseList.clear();

}


int SessionController::GetNumberOfPhases()
{
  return this->PhaseList.size();
}


ServerPhaseBase* SessionController::GetPhase(int i)
{
  if (i >= 0 && i < this->PhaseList.size())
    {
    return this->PhaseList[i];
    }
  else
    {
    return NULL;
    }
}


ServerPhaseBase* SessionController::GetCurrentPhase()
{
  return this->CurrentWorkphase;
}


int SessionController::ActivateDefect(const char* phaseName, const char* type)
{

  WorkphaseList::iterator witer;

  for (witer = this->PhaseList.begin(); witer != this->PhaseList.end(); witer ++)
    {
    if (strcmp(phaseName, (*witer)->Name()) == 0)
      {
      (*witer)->SetDefectStatus(type, 1);
      return 1;
      }
    }

  return 0;

}


void SessionController::PrintDefectStatus()
{
  WorkphaseList::iterator wit;

  for (wit = this->PhaseList.begin(); wit != this->PhaseList.end(); wit ++)
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
}


void SessionController::PrintAvailableDefectTypes()
{

  WorkphaseList::iterator wit;
  for (wit = this->PhaseList.begin(); wit != this->PhaseList.end(); wit ++)
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

}


int SessionController::Run()
{

  //------------------------------------------------------------
  // Set up a server
  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(this->PortNumber);

  if (r < 0)
    {
    std::cerr << "ERROR: Cannot create a server socket." << std::endl;
    exit(0);
    }

  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    this->Socket = serverSocket->WaitForConnection(1000);
    
    if (this->Socket.IsNotNull()) // if client connected
      {
      std::cerr << "MESSAGE: Client connected. Starting a session..." << std::endl;

      this->SessionStatus = SESSION_ACTIVE;
      igtl::MultiThreader::Pointer threader;
      threader = igtl::MultiThreader::New();
      threader->SpawnThread((igtl::ThreadFunctionType) &SessionController::MonitorThread, this);
      Session();

      this->SessionStatus = SESSION_INACTIVE; // This will stop the thread;

      // Wait until thread stops
      while (this->ThreadAlive)
        {
        igtl::Sleep(this->TimerInterval);
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  this->Socket->CloseSocket();

}


void SessionController::MonitorThread(void * ptr)
{
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  SessionController* controller = static_cast<SessionController*>(info->UserData);
  controller->ThreadAlive = true;

  igtl::TimeStamp::Pointer timeStamp = igtl::TimeStamp::New();
  igtlUint64 intervalNano = (igtlUint64)controller->TimerInterval * 1000000; // Convert from ms to ns
  igtlUint64 time0;
  igtlUint64 time1;

  while (controller->SessionStatus == SESSION_ACTIVE)
    {
    // Get the start time
    timeStamp->GetTime();
    time0 = timeStamp->GetTimeStampInNanoseconds();
    
    // Call timer handler for the current workhpase
    if (controller->CurrentWorkphase)
      {
      controller->CurrentWorkphase->TimerHandler(time0);
      }
    
    // Get the end time
    timeStamp->GetTime();
    time1 = timeStamp->GetTimeStampInNanoseconds();
    
    // Elapsed time
    igtlUint64 elapsedTime = time1 - time0;
    
    // Wait for the next cycle
    igtl::Sleep((intervalNano-elapsedTime)*1000000);
    }

  controller->ThreadAlive = false;
    
}


int SessionController::Session()
{

  RobotStatus * rs = new RobotStatus();

  //------------------------------------------------------------
  // Set socket and robot status
  std::vector< ServerPhaseBase* >::iterator iter;
  for (iter = PhaseList.begin(); iter != PhaseList.end(); iter ++)
    {
    //std::cerr << "MESSAGE: Setting up " << (*iter)->Name() << " phase." << std::endl;
    (*iter)->SetSocket(this->Socket);
    (*iter)->SetRobotStatus(rs);
    }

  //------------------------------------------------------------
  // Set undefined phase as the current phase;
  WorkphaseList::iterator currentPhase = PhaseList.begin();
  
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



