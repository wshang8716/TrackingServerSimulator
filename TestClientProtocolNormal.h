/*=========================================================================

  Program:   Tracking Server Simulator (Test Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __TestClientProtocolNormal_h
#define __TestClientProtocolNormal_h

#include "igtlSocket.h"
#include "TestClientProtocolBase.h"

class TestClientProtocolNormal : public TestClientProtocolBase
{
public:
  TestClientProtocolNormal();
  ~TestClientProtocolNormal();

  virtual const char* Name() { return "Normal Operation Test"; };

  virtual ErrorPointType Test();

};

#endif //__TestClientProtocolNormal_h


