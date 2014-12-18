/*=========================================================================

  Program:   OpenIGTLink Communication Server: Tracking Phase
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerTrackingPhase_h
#define __ServerTrackingPhase_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTrackingDataMessage.h"
#include "ServerPhaseBase.h"

class ServerTrackingPhase : public ServerPhaseBase
{
public:

  ServerTrackingPhase();
  ~ServerTrackingPhase();

  virtual const char* Name() { return "STARTUP"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

  igtl::TrackingDataMessage::Pointer TrackingMsg;

};

#endif //__ServerTrackingPhase_h
