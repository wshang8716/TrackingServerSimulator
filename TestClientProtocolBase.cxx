/*=========================================================================

  Program:   Tracking Server Simulator (Test Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "TestClientProtocolBase.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

TestClientProtocolBase::TestClientProtocolBase()
{
  this->TimeoutShort  = DEFULT_TIMEOUT_SHORT;
  this->TimeoutMedium = DEFULT_TIMEOUT_MEDIUM;
  this->TimeoutLong   = DEFULT_TIMEOUT_LONG;
  this->SockUtil = NULL;
}


TestClientProtocolBase::~TestClientProtocolBase()
{
}


int TestClientProtocolBase::Exec()
{
  if (this->SockUtil)
    {
    ErrorPointType errorPoint = this->Test();
    if (errorPoint == SUCCESS)
      {
      std::cerr << "MESSAGE: The test has been completed successfully." << std::endl;
      return 1;
      }
    else
      {
      std::cerr << "ERROR: Test failed at check point #" << GetStep(errorPoint) << "." << GetPoint(errorPoint) << std::endl;
      return 0;
      }
    }
  else
    {
    std::cerr << "ERROR: Socket is not available."  << std::endl;
    return 0;
    }
}


