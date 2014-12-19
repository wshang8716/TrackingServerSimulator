/*=========================================================================

  Program:  OpenIGTLink Socket Utility Class
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __OpenIGTLinkSockUtil_h
#define __OpenIGTLinkSockUtil_h

#include "igtlSocket.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlMutexLock.h"

class OpenIGTLinkSockUtil
{

public:

  enum {
    ERROR = 0,
    SUCCESS = 1,
  };

public:
  OpenIGTLinkSockUtil();
  ~OpenIGTLinkSockUtil();

  void SetSocket(igtl::Socket* socket);
  igtl::Socket* GetSocket(igtl::Socket* socket) { return this->Socket; }

  // PushMessage() sends the specified OpenIGTLink message. The PushMessage() is thread-safe
  // -- it implements the semaphor internally, so that it can be called from multiple threads.
  virtual int PushMessage(igtl::MessageBase* message);
  
  int  ReceiveMessageHeader(igtl::MessageHeader* headerMsg, int timeout);

  // Check if the type and the name of the message match those specified.
  // Return 1 if they match.
  int  CheckMessageTypeAndName(igtl::MessageHeader* headerMsg,
                               const char* type, const char* name);

  int  SkipMesage(igtl::MessageHeader* headerMsg);


  // Send STRING message
  int  SendStringMessage(const char* name, const char* string);

  // Send STATUS message
  int  SendStatusMessage(const char* name, int Code, int SubCode,
                         const char * errorName=NULL, const char* statusString=NULL);
  // Send TRANSFORM message
  int  SendTransformMessage(const char* name, igtl::Matrix4x4& matrix);

  // Receive STATUS message
  int  ReceiveStatus(igtl::MessageHeader* header, int& code, int& subcode,
                     std::string& name, std::string& status);
  // Receive STRING message
  int  ReceiveString(igtl::MessageHeader* header, std::string& string);

  // Receive TRANSFORM message
  int  ReceiveTransform(igtl::MessageHeader* header, igtl::Matrix4x4& matrix);

  // Receive STT_TDATA message
  // ReceiveStartTracking saves the name of the coordinate system in 'cood' and
  // the resolution (millisecond) in 'res'
  int  ReceiveStartTracking(igtl::MessageHeader* header, std::string& coord, int& res);

  // Receive STP_TDATA message
  int  ReceiveStopTracking(igtl::MessageHeader* header);

  
  /// Check if the received messsage is STRING message, and if the message has
  /// the specfied name and content. If suffix=1 is specified, the function
  /// only checks if the message name begins with the string specified by 'name'.
  /// For example, "CMD_1234" will be accepted when name="CMD_" and suffix=1 are
  /// specfiied. 
  int  CheckAndReceiveStringMessage(igtl::MessageHeader* headerMsg,
                                    const char* name, const char* string, int suffix=0);
  int  CheckAndReceiveStatusMessage(igtl::MessageHeader* headerMsg,
                                    const char* name, int code, int suffix=0,
                                    const char* errorName=NULL);
  int  CheckAndReceiveTransformMessage(igtl::MessageHeader* headerMsg,
                                       const char* name, igtl::Matrix4x4& matrix,
                                       double err = 1.0e-10, int suffix=0);

  // Random matrix generator for testing
  void GetRandomTestMatrix(igtl::Matrix4x4& matrix);
  void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta);


  // Print matrix for debuging
  void PrintMatrix(std::string prefix, igtl::Matrix4x4& matrix);

  // Validate matrix (check if colums are normal and orthogonal)
  int  ValidateMatrix(igtl::Matrix4x4& matrix);

  // Compare two matrices. If there is any corresponding elements with error larger than 'tol', return 0.
  // Otherwise, it returns 1.
  int  CompareMatrices(igtl::Matrix4x4& matrix1, igtl::Matrix4x4& matrix2, double tol);


protected:

  igtl::Socket::Pointer Socket;
  igtl::MutexLock::Pointer Mutex;

};

#endif //__OpenIGTLinkSockUtil_h
