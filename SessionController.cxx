/*=========================================================================

  Program:   Tracking Server Simulator 
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "SessionController.h"
#include "OpenIGTLinkSockUtil.h"
#include "igtlOSUtil.h"
#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"

SessionController::SessionController()
{

  this->StateList.clear();
  this->Socket = NULL;
  this->TimerInterval = 100; // Default 100 ms
  this->SessionStatus = SESSION_INACTIVE;
  this->CurrentState = NULL;
  this->ThreadAlive = 0;
  this->PortNumber = 18944;
  this->SStatus = NULL;

}


SessionController::~SessionController()
{

  UnregisterAllStates();

}


int SessionController::RegisterState(ServerStateBase* phase)
{

  if (!this->SStatus)
    {
    std::cerr << "ERROR: ServerInfoBase object is not available." << std::endl;
    return this->StateList.size();
    }

  if (!phase)
    {
    std::cerr << "ERROR: State object is empty." << std::endl;
    return this->StateList.size();
    }
  
  this->StateList.push_back(phase);
  phase->SetServerInfo(this->SStatus);
  return this->StateList.size();

}


void SessionController::UnregisterAllStates()
{

  WorkphaseList::iterator wit;

  for (wit = this->StateList.begin(); wit != this->StateList.end(); wit ++)
    {
    delete *wit;
    }
  this->StateList.clear();

}


int SessionController::GetNumberOfStates()
{
  return this->StateList.size();
}


ServerStateBase* SessionController::GetState(int i)
{
  if (i >= 0 && i < this->StateList.size())
    {
    return this->StateList[i];
    }
  else
    {
    return NULL;
    }
}


ServerStateBase* SessionController::GetCurrentState()
{
  return this->CurrentState;
}


int SessionController::ActivateDefect(const char* phaseName, const char* type)
{

  WorkphaseList::iterator witer;

  for (witer = this->StateList.begin(); witer != this->StateList.end(); witer ++)
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

  for (wit = this->StateList.begin(); wit != this->StateList.end(); wit ++)
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
  for (wit = this->StateList.begin(); wit != this->StateList.end(); wit ++)
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
    if (controller->CurrentState)
      {
      controller->CurrentState->TimerHandler(time0);
      }
    
    // Get the end time
    timeStamp->GetTime();
    time1 = timeStamp->GetTimeStampInNanoseconds();
    
    // Elapsed time (ns)
    igtlUint64 elapsedTime = time1 - time0;
    igtlUint64 remainTime = intervalNano-elapsedTime;
    
    // Wait for the next cycle
    if (remainTime)
      {
      igtl::Sleep(remainTime/1000000);
      }
    }

  controller->ThreadAlive = false;
    
}


int SessionController::Session()
{

  ServerInfoBase * rs = new ServerInfoBase();
  OpenIGTLinkSockUtil * sockUtil = new OpenIGTLinkSockUtil();
  
  sockUtil->SetSocket(this->Socket);

  //------------------------------------------------------------
  // Set SockUtil and ServerInfo
  std::vector< ServerStateBase* >::iterator iter;
  for (iter = StateList.begin(); iter != StateList.end(); iter ++)
    {
    (*iter)->SetServerInfo(rs);
    (*iter)->SetSockUtil(sockUtil);
    }

  //------------------------------------------------------------
  // Set initial phase as the current phase;
  //WorkphaseList::iterator currentState = StateList.begin();
  this->CurrentState = StateList[0];
  
  int connect = 1;
  int fEnter = 1;
  
  //------------------------------------------------------------
  // loop
  while (connect && this->CurrentState)
    {
    int fStateChange = 0;

    if (fEnter)
      {
      fStateChange = this->CurrentState->Enter();
      fEnter = 0;
      }

    if (!fStateChange)
      {
      fStateChange = this->CurrentState->Process();
      }

    if (fStateChange)
      {
      // If Enter() or Process() returns 1, phase change has been requested.
      std::cerr << "MESSAGE: State change requested." << std::endl;
      std::string requestedWorkphase = this->CurrentState->GetNextWorkState();

      // Find the requested workphase
      std::vector<  ServerStateBase* >::iterator iter;
      for (iter = StateList.begin(); iter != StateList.end(); iter ++)
        {
        if (strcmp((*iter)->Name(), requestedWorkphase.c_str()) == 0)
          {
          // Change the current phase
          this->CurrentState = *iter;
          fEnter = 1;
          break;
          }
        }
      if (!fEnter)
        {
        // The next phase could not be found.
        // TODO: Error handling
        }
      }
    }
  
  return 1;
}



