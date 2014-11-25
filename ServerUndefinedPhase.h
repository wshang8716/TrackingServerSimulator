/*=========================================================================

  Program:   OpenIGTLink Communication Server: Undefined Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerUndefinedPhase_h
#define __ServerUndefinedPhase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "ServerPhaseBase.h"

class ServerUndefinedPhase : public ServerPhaseBase
{
public:

  ServerUndefinedPhase();
  ~ServerUndefinedPhase();

  virtual const char* Name() { return "UNDEFINED"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);

protected:

};

#endif //__ServerUndefinedPhase_h
