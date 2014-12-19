/*=========================================================================

  Program:   OpenIGTLink Communication Server: StandBy State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerStandByState.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerStandByState::ServerStandByState() :
  ServerStateBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in START_UP phase.");
}


ServerStandByState::~ServerStandByState()
{
}


int ServerStandByState::Initialize()
{

  this->SockUtil->SendStatusMessage("STATE", igtl::StatusMessage::STATUS_OK, 0, this->Name());
  return PHASE_CHANGE_NOT_REQUIRED;

}


int ServerStandByState::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerStateBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }
  
  if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "STT_TDATA", "TRACKING"))
    {
    std::string coord;
    int res;
    this->SockUtil->ReceiveStartTracking(headerMsg, coord, res);
    this->ServerInfo->SetTrackingResolution(res);
    this->ServerInfo->SetTrackingCoordinate(coord.c_str());
    this->SetNextWorkState("STANDBY");
    return PHASE_CHANGE_REQUIRED;
    }
  else if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "STRING", "NAME"))
    {
    std::string string;
    this->SockUtil->ReceiveString(headerMsg, string);
    if (string.compare("INIT") == 0)
      {
      this->SetNextWorkState("INIT");
      return PHASE_CHANGE_REQUIRED;
      }
    }

  return NOT_PROCESSED;
}


int ServerStandByState::TimerHandler(long timestamp)
{

  std::cerr << "ServerStandByState::TimerHandler() is called" << std::endl;
  return 0;

}
