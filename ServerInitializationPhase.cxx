/*=========================================================================

  Program:   OpenIGTLink Communication Server: Initialization Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerInitializationPhase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerInitializationPhase::ServerInitializationPhase() :
  ServerPhaseBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in START_UP phase.");
}


ServerInitializationPhase::~ServerInitializationPhase()
{
}

int ServerInitializationPhase::Initialize()
{

  this->SockUtil->SendStatusMessage("STATE", igtl::StatusMessage::STATUS_OK, 0, this->Name());

  // Send Status after waiting for 2 seconds (mimicking initialization process)
  igtl::Sleep(2000); // wait for 2000 msec

  if (this->GetDefectStatus("DNR"))
    {
    // Device-not-ready defect is active
    this->SockUtil->SendStatusMessage(this->Name(), igtl::StatusMessage::STATUS_NOT_READY, 0);
    return PHASE_CHANGE_NOT_REQUIRED;
    }
  else
    {
    // Normal
    this->SockUtil->SendStatusMessage(this->Name(), 1, 0);
    this->SetNextWorkPhase("STANDBY");
    return PHASE_CHANGE_REQUIRED;
    }
}


int ServerInitializationPhase::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerPhaseBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }

  // Implement message handling specific to this class here
  return NOT_PROCESSED;
}


int ServerInitializationPhase::TimerHandler(long timestamp)
{

  std::cerr << "ServerInitializationPhase::TimerHandler() is called" << std::endl;
  return 0;

}
