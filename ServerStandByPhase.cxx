/*=========================================================================

  Program:   OpenIGTLink Communication Server: StandBy Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerStandByPhase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerStandByPhase::ServerStandByPhase() :
  ServerPhaseBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in START_UP phase.");
}


ServerStandByPhase::~ServerStandByPhase()
{
}


int ServerStandByPhase::Initialize()
{

  this->SendStatusMessage("STATE", igtl::StatusMessage::STATUS_OK, 0, this->Name());
  return PHASE_CHANGE_NOT_REQUIRED;

}


int ServerStandByPhase::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerPhaseBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }
  
  if (this->CheckMessageTypeAndName(headerMsg, "STT_TDATA", "TRACKING"))
    {
    std::string coord;
    int res;
    this->ReceiveStartTracking(headerMsg, coord, res);
    this->SStatus->SetTrackingResolution(res);
    this->SStatus->SetTrackingCoordinate(coord.c_str());
    this->SetNextWorkPhase("STANDBY");
    return PHASE_CHANGE_REQUIRED;
    }
  else if (this->CheckMessageTypeAndName(headerMsg, "STRING", "NAME"))
    {
    std::string string;
    this->ReceiveString(headerMsg, string);
    if (string.compare("INIT") == 0)
      {
      this->SetNextWorkPhase("INIT");
      return PHASE_CHANGE_REQUIRED;
      }
    }

  return NOT_PROCESSED;
}


int ServerStandByPhase::TimerHandler(long timestamp)
{

  std::cerr << "ServerStandByPhase::TimerHandler() is called" << std::endl;
  return 0;

}
