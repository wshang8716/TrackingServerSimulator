/*=========================================================================

  Program:   OpenIGTLink Communication Server: StandBy Stated
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerStandByState_h
#define __ServerStandByState_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerStateBase.h"

class ServerStandByState : public ServerStateBase
{
public:

  ServerStandByState();
  ~ServerStandByState();

  virtual const char* Name() { return "STANDBY"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

};

#endif //__ServerStandByState_h
