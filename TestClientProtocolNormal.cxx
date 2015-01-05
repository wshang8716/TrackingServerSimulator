/*=========================================================================

  Program:   Tracking Server Simulator (Test Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"

#include "TestClientProtocolNormal.h"


TestClientProtocolNormal::TestClientProtocolNormal()
{
}

TestClientProtocolNormal::~TestClientProtocolNormal()
{
}

TestClientProtocolNormal::ErrorPointType TestClientProtocolNormal::Test()
{
  int queryCounter = 0;
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  std::cerr << "MESSAGE: ===== Step 1: Initialization =====" << std::endl;
  this->SockUtil->SendStringMessage("CMD", "INIT");

  this->SockUtil->ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!this->SockUtil->CheckAndReceiveStatusMessage(headerMsg, "STATE", 1, 0, "INIT"))
    return Error(1,1);

  this->SockUtil->ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!this->SockUtil->CheckAndReceiveStatusMessage(headerMsg, "INIT", 1, 0, ""))
    return Error(1,1);

  std::cerr << "MESSAGE: ===== Step 2: Stand By =====" << std::endl;
  this->SockUtil->ReceiveMessageHeader(headerMsg, this->TimeoutShort);
  if (!this->SockUtil->CheckAndReceiveStatusMessage(headerMsg, "STATE", 1, 0, "STANDBY"))
    return Error(2,1);

  std::cerr << "MESSAGE: ===== Step 3: Tracking =====" << std::endl;
  this->SockUtil->SendStartTrackingDataMessage("TRACKING", 100, "Patient");

  this->SockUtil->ReceiveMessageHeader(headerMsg, this->TimeoutLong);
  if (!this->SockUtil->CheckAndReceiveStatusMessage(headerMsg, "STATE", 1, 0, "TRACKING"))
    return Error(3,1);
  
  int positionCount = 0;
  for (;;) // Can receive more than 1 transform message
    {
    this->SockUtil->ReceiveMessageHeader(headerMsg, this->TimeoutLong);
    if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "TDATA", "TRACKING"))
      {
      this->SockUtil->SkipMesage(headerMsg);
      positionCount ++;
      }
    else if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "TDATA", "TRACKING"))
      {
      return Error(3,2);
      }
    // Quit loop after receiving 100 TDATA messages
    if (positionCount > 100)
      {
      break;
      }
    }
  
  std::cerr << "MESSAGE: ===== Step 4: Stand by =====" << std::endl;
  this->SockUtil->SendStopTrackingDataMessage("TRACKING");

  for (;;) // Can receive more than 1 transform message
    {
    this->SockUtil->ReceiveMessageHeader(headerMsg, this->TimeoutLong);

    // Receive TDATA messages sent before STP_TDATA was received by the server
    if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "TDATA", "TRACKING"))
      {
      this->SockUtil->SkipMesage(headerMsg);
      }
    else if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "STATE", "STANDBY"))
      {
      if (this->SockUtil->CheckAndReceiveStatusMessage(headerMsg, "STATE", 1, 0, "STANDBY"))
        {
        break;
        }
      else
        {
        Error(4,2);
        }
      }
    else
      {
      return Error(4,1);
      }
    }
    
  return SUCCESS;
}
