/*=========================================================================

  Program:   Tracking Server Simulator 
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __SessionController_h
#define __SessionController_h

#include "igtlSocket.h"
#include "ServerPhaseBase.h"


class SessionController
{
public:

  enum {
    SESSION_ACTIVE,
    SESSION_INACTIVE
  };

public:

  SessionController();
  ~SessionController();

public:

  // Register a new phase and return the number of registered phases.
  int  RegisterPhase(ServerPhaseBase*);

  // Unregister the all phases (The all objects will be deleted)
  void UnregisterAllPhases();

  int  GetNumberOfPhases();

  ServerPhaseBase* GetPhase(int i);
  
  ServerPhaseBase* GetCurrentPhase();
  

  // Activate defect (for testing); returns 1 if successful.
  int  ActivateDefect(const char* phaseName, const char* type);

  void PrintDefectStatus();

  void PrintAvailableDefectTypes();

  // Set Port Number;
  void SetPortNumber(int port) { this->PortNumber = port; }

  // Set an interval for the timer
  void SetTimerInterval(int intv) { this->TimerInterval = intv; }

  void SetServerInfo(ServerInfoBase* status) { this->SStatus = status; }

  int  Run();

  static void MonitorThread(void * ptr);

protected:

  int  Session();

protected:

  typedef std::vector< ServerPhaseBase* > WorkphaseList;

  igtl::Socket::Pointer Socket;

  int PortNumber;
  int TimerInterval;
  int SessionStatus; // SESSION_*

  WorkphaseList PhaseList;
  ServerPhaseBase* CurrentPhase; // Current workphase

  bool ThreadAlive;

  ServerInfoBase* SStatus;

};



#endif //__SessionController_h

