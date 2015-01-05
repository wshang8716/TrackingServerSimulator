/*=========================================================================

  Program:   OpenIGTLink Communication Server: Initialization State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerInitializationState.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerInitializationState::ServerInitializationState() :
  ServerStateBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in INIT phase.");
}


ServerInitializationState::~ServerInitializationState()
{
}

int ServerInitializationState::Initialize()
{

  this->SockUtil->SendStatusMessage("STATE", igtl::StatusMessage::STATUS_OK, 0, this->Name());

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
    this->SetNextWorkState("STANDBY");
    return PHASE_CHANGE_REQUIRED;
    }
}


int ServerInitializationState::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerStateBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }

  // Implement message handling specific to this class here
  return NOT_PROCESSED;
}


int ServerInitializationState::TimerHandler(long timestamp)
{

  std::cerr << "ServerInitializationState::TimerHandler() is called" << std::endl;
  return 0;

}
