/*=========================================================================

  Program:   Tracking Server Simulator (Test Client)
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlClientSocket.h"
#include "TestClientProtocolNormal.h"
#include "OpenIGTLinkSockUtil.h"

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <string>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <test>     : Test Protocol# (1-10)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  int    test     = atoi(argv[3]);

  if (test <= 0 || test > 10)
    {
    std::cerr << "Invalid test" << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Call Test
  TestClientProtocolBase* testProtocol = NULL;

  switch (test)
    {
    case 1:
      {
      testProtocol = (TestClientProtocolNormal*) new TestClientProtocolNormal();
      break;
      }
    default:
      break;
    }

  if (testProtocol)
    {
    OpenIGTLinkSockUtil * sockUtil = new OpenIGTLinkSockUtil();    
    sockUtil->SetSocket(socket);

    // Set timeout values (ms)
    testProtocol->SetTimeoutShort(1000);
    testProtocol->SetTimeoutMedium(5000);
    testProtocol->SetTimeoutMedium(10000);

    testProtocol->SetSockUtil(sockUtil);
    testProtocol->Exec();
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();
}

