/*=========================================================================

  Program:   OpenIGTLink Communication Server: Initialization State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerInitializationState_h
#define __ServerInitializationState_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerStateBase.h"

class ServerInitializationState : public ServerStateBase
{
public:

  ServerInitializationState();
  ~ServerInitializationState();

  virtual const char* Name() { return "INIT"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

};

#endif //__ServerInitializationState_h
