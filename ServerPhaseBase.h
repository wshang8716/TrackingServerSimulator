/*=========================================================================

  Program:   OpenIGTLink Communication Server: Initialization Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerPhaseBase_h
#define __ServerPhaseBase_h

#include <string>
#include <map>

#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerInfoBase.h"
#include "OpenIGTLinkSockUtil.h"


class ServerPhaseBase
{
public:

  // Return values for MessageHandler()
  enum {
    NOT_PROCESSED = 0,
    PHASE_CHANGE_NOT_REQUIRED,
    PHASE_CHANGE_REQUIRED
  };

public:
  ServerPhaseBase();
  virtual ~ServerPhaseBase();

  virtual const char* Name()=0;

  // Enter() will be called when the workphase is switched from another
  // workphase. Enter() calls Initialize() which implements actual
  // initialization process for this workphase.
  // Returns 1 if there is any workphase change request. Otherwise returns 0.
  //virtual int Enter(const char* queryID);
  virtual int Enter();

  // Process() will be called by the main session loop.
  // Process() receives a message and calls MessageHander() to perform workphase-specific
  // message handling. 
  // Returns 1 if there is any workphase change request. Otherwise returns 0.
  virtual int Process();

  // Initialization process. This must be implemented in child classes.
  // MessageHandler() returns one of the following values:
  //   NOT_PROCESSED:             The message was not processed in MessageHandler().
  //   PHASE_CHANGE_NOT_REQUIRED: The message was processed, but no phase change is
  //                              required.
  //   PHASE_CHANGE_REQUIRED:     The message was processed, and a phaes change is
  //                              required.
  // Those returned values allow calling MessageHandler() hierarchically.
  virtual int Initialize() = 0;

  // MessageHandler() defines workphase-specific message handling.
  // The function must be implemented in the child classes.
  // MessageHanlder returns one of the values listed above (see comment for
  // Initialize()).
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);

  // TimerHandler() defines a process that needs to be called periodically
  // during the TCP/IP session.
  // The function must be defined in the child classes.
  virtual int TimerHandler(long timestamp) = 0; 

  void SetNextWorkPhase(const char* phase)  { this->NextWorkphase = phase; }
  std::string GetNextWorkPhase() { return this->NextWorkphase; };
  //std::string GetQueryID() { return this->QueryID; };

  // Enable/disable defects. Specify s=1 when enabled.
  int SetDefectStatus(const char * type, int s);

  // Return 0 if disabled, 1 if enabled, or -1 if the specified type is not available.
  int GetDefectStatus(const char * type);

  std::list< std::string > GetDefectTypeList();
  std::string GetDefectTypeDescription(const char * type);

  void SetServerInfo(ServerInfoBase* rs) { this->ServerInfo = rs; }
  ServerInfoBase* GetServerInfo() { return this->ServerInfo; }

  void SetSockUtil(OpenIGTLinkSockUtil* su) { this->SockUtil = su; }

protected:

  // Check if a CMD message (workphase change) has been received.
  // Return 1, if workphase change has been requested.
  int CheckWorkphaseChange(igtl::MessageHeader* headerMsg);

  // Check if there is any messages that must be accepted 
  // regardless of current workhpase.
  int CheckCommonMessage(igtl::MessageHeader* headerMsg);

  // Register defect type. 
  int RegisterDefectType(const char* name, const char* desc);

  std::string NextWorkphase;
  //std::string QueryID;

  std::map< std::string, int > DefectStatus;
  std::map< std::string, std::string > DefectDescription;

  ServerInfoBase* ServerInfo;
  OpenIGTLinkSockUtil* SockUtil;

};

#endif //__ServerPhaseBase_h
