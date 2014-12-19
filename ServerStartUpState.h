/*=========================================================================

  Program:   OpenIGTLink Communication Server: StartUp State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerStartUpState_h
#define __ServerStartUpState_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerStateBase.h"

class ServerStartUpState : public ServerStateBase
{
public:

  ServerStartUpState();
  ~ServerStartUpState();

  virtual const char* Name() { return "STARTUP"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

};

#endif //__ServerStartUpState_h
