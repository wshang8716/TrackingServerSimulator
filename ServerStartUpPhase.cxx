/*=========================================================================

  Program:   OpenIGTLink Communication Server: StartUp Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerStartUpPhase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerStartUpPhase::ServerStartUpPhase() :
  ServerPhaseBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in START_UP phase.");
}


ServerStartUpPhase::~ServerStartUpPhase()
{
}

int ServerStartUpPhase::Initialize()
{
  return PHASE_CHANGE_NOT_REQUIRED;
}


int ServerStartUpPhase::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerPhaseBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }

  // Implement message handling specific to this class here
  return NOT_PROCESSED;

}


int ServerStartUpPhase::TimerHandler(long timestamp)
{

  std::cerr << "ServerStartUpPhase::TimerHandler() is called" << std::endl;
  return 0;

}
