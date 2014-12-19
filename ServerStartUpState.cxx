/*=========================================================================

  Program:   OpenIGTLink Communication Server: StartUp State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerStartUpState.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerStartUpState::ServerStartUpState() :
  ServerStateBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in START_UP phase.");
}


ServerStartUpState::~ServerStartUpState()
{
}

int ServerStartUpState::Initialize()
{
  return PHASE_CHANGE_NOT_REQUIRED;
}


int ServerStartUpState::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerStateBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }

  // Implement message handling specific to this class here
  return NOT_PROCESSED;

}


int ServerStartUpState::TimerHandler(long timestamp)
{

  std::cerr << "ServerStartUpState::TimerHandler() is called" << std::endl;
  return 0;

}
