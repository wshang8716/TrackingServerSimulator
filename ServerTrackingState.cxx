/*=========================================================================

  Program:   OpenIGTLink Communication Server: Tracking State
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerTrackingState.h"
#include <string.h>
#include <stdlib.h>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"
#include "igtlTransformMessage.h"
#include <cmath>

ServerTrackingState::ServerTrackingState() :
  ServerStateBase()
{
  // Register Device-not-ready defect
  this->RegisterDefectType("DNR", "Device-not-ready in TRACKING phase.");

  this->TrackingMsg = igtl::TrackingDataMessage::New();
  this->TrackingMsg->SetDeviceName("TRACKING");

  igtl::TrackingDataElement::Pointer trackElement0;
  trackElement0 = igtl::TrackingDataElement::New();
  trackElement0->SetName("Channel 0");
  trackElement0->SetType(igtl::TrackingDataElement::TYPE_TRACKER);

  igtl::TrackingDataElement::Pointer trackElement1;
  trackElement1 = igtl::TrackingDataElement::New();
  trackElement1->SetName("Channel 1");
  trackElement1->SetType(igtl::TrackingDataElement::TYPE_6D);

  igtl::TrackingDataElement::Pointer trackElement2;
  trackElement2 = igtl::TrackingDataElement::New();
  trackElement2->SetName("Channel 2");
  trackElement2->SetType(igtl::TrackingDataElement::TYPE_5D);

  this->TrackingMsg->AddTrackingDataElement(trackElement0);
  this->TrackingMsg->AddTrackingDataElement(trackElement1);
  this->TrackingMsg->AddTrackingDataElement(trackElement2);

}


ServerTrackingState::~ServerTrackingState()
{
}


int ServerTrackingState::Initialize()
{

  this->SockUtil->SendStatusMessage("STATE", igtl::StatusMessage::STATUS_OK, 0, this->Name());
  return PHASE_CHANGE_NOT_REQUIRED;

}


int ServerTrackingState::MessageHandler(igtl::MessageHeader* headerMsg)
{

  int r = ServerStateBase::MessageHandler(headerMsg);
  if (r != NOT_PROCESSED)
    {
    return r;
    }
  
  if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "STP_TDATA", "TRACKING"))
    {
    this->SockUtil->ReceiveStopTracking(headerMsg);
    this->SetNextWorkState("STANDBY");
    return PHASE_CHANGE_REQUIRED;
    }
  else if (this->SockUtil->CheckMessageTypeAndName(headerMsg, "STRING", "CMD"))
    {
    std::string string;
    this->SockUtil->ReceiveString(headerMsg, string);
    if (string.compare("INIT") == 0)
      {
      this->SetNextWorkState("INIT");
      return PHASE_CHANGE_REQUIRED;
      }
    }

  return NOT_PROCESSED;
}


int ServerTrackingState::TimerHandler(long timestamp)
{
  std::cerr << "ServerTrackingState::TimerHandler() is called" << std::endl;

  igtl::Matrix4x4 matrix;

  static float phi0   = 0.0;
  static float theta0 = 0.0;
  static float phi1   = 0.0;
  static float theta1 = 0.0;
  static float phi2   = 0.0;
  static float theta2 = 0.0;

  igtl::TrackingDataElement::Pointer ptr;

  // Channel 0
  this->TrackingMsg->GetTrackingDataElement(0, ptr);
  this->SockUtil->GetRandomTestMatrix(matrix, phi0, theta0);
  ptr->SetMatrix(matrix);
  
  // Channel 1
  this->TrackingMsg->GetTrackingDataElement(1, ptr);
  this->SockUtil->GetRandomTestMatrix(matrix, phi1, theta1);
  ptr->SetMatrix(matrix);
  
  // Channel 2
  this->TrackingMsg->GetTrackingDataElement(2, ptr);
  this->SockUtil->GetRandomTestMatrix(matrix, phi2, theta2);
  ptr->SetMatrix(matrix);

  this->TrackingMsg->Pack();
  this->SockUtil->PushMessage(this->TrackingMsg);
  
  phi0 += 0.1;
  phi1 += 0.2;
  phi2 += 0.3;
  theta0 += 0.2;
  theta1 += 0.1;
  theta2 += 0.05;

  return 0;

}

