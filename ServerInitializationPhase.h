/*=========================================================================

  Program:   OpenIGTLink Communication Server: Initialization Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerInitializationPhase_h
#define __ServerInitializationPhase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerPhaseBase.h"

class ServerInitializationPhase : public ServerPhaseBase
{
public:

  ServerInitializationPhase();
  ~ServerInitializationPhase();

  virtual const char* Name() { return "Initialization"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

};

#endif //__ServerInitializationPhase_h
