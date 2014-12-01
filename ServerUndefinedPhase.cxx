/*=========================================================================

  Program:   OpenIGTLink Communication Server: Undefined Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerUndefinedPhase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerUndefinedPhase::ServerUndefinedPhase() :
  ServerPhaseBase()
{
}


ServerUndefinedPhase::~ServerUndefinedPhase()
{
}


int ServerUndefinedPhase::Initialize()
{
  return 1;
}


int ServerUndefinedPhase::MessageHandler(igtl::MessageHeader* headerMsg)
{

  if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0 &&
      strncmp(headerMsg->GetDeviceName(), "CMD", 3) == 0)
    {
    igtl::StringMessage::Pointer stringMsg;
    stringMsg = igtl::StringMessage::New();
    stringMsg->SetMessageHeader(headerMsg);
    stringMsg->AllocatePack();
    int r = this->Socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());
    if (r < 0)
      {
      std::cerr << "ERROR: Timeout." << std::endl;
      this->Socket->CloseSocket();
      exit(EXIT_FAILURE);
      }
    else if (r == 0)
      {
      std::cerr << "ERROR: Socket closed while reading a message." << std::endl;
      this->Socket->CloseSocket();
      exit(EXIT_FAILURE);
      }
    
    int c = stringMsg->Unpack(1);
    
    if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
      if (stringMsg->GetEncoding() == 3)
        {
        if (strcmp(stringMsg->GetString(), "INITIALIZE") == 0)
          {
          this->NextWorkphase = "Initialization";
          return 1;
          }
        }
      }
    else
      {
      std::cerr << "ERROR: Invalid CRC." << std::endl;
      this->NextWorkphase = "Unknown";
      }
    }
    
    // No phase change
    std::cerr << "ERROR: Could not initialize the server: Received an illeagal message." << std::endl;
    return 0;
}

int ServerUndefinedPhase::TimerHandler(long timestamp)
{
  std::cerr << "ServerUndefinedPhase::TimerHandler(): " << timestamp << std::endl;
  return 0;
}
