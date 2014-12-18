/*=========================================================================

  Program:   OpenIGTLink Communication Server: StandBy Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerStandByPhase_h
#define __ServerStandByPhase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerPhaseBase.h"

class ServerStandByPhase : public ServerPhaseBase
{
public:

  ServerStandByPhase();
  ~ServerStandByPhase();

  virtual const char* Name() { return "STARTUP"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

};

#endif //__ServerStandByPhase_h
