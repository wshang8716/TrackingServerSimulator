/*========================================================================= 
  Program:   OpenIGTLink Communication Server: Tracking State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ServerTrackingState_h
#define __ServerTrackingState_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTrackingDataMessage.h"
#include "ServerStateBase.h"

class ServerTrackingState : public ServerStateBase
{
public:

  ServerTrackingState();
  ~ServerTrackingState();

  virtual const char* Name() { return "TRACKING"; };

  virtual int Initialize();
  virtual int MessageHandler(igtl::MessageHeader* headerMsg);
  virtual int TimerHandler(long timestamp); 

protected:

  igtl::TrackingDataMessage::Pointer TrackingMsg;

};

#endif //__ServerTrackingState_h
