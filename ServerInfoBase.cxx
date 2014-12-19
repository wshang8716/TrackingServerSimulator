/*=========================================================================

  Program:   OpenIGTLink Communication Server: Status
  Language:  C++

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ServerInfoBase.h"
#include "igtlMath.h"

ServerInfoBase::ServerInfoBase()
{
  igtl::IdentityMatrix(this->CalibrationMatrix);
  igtl::IdentityMatrix(this->TargetMatrix);
  this->FlagCalibration = 0;
  this->FlagTarget = 0;
}

ServerInfoBase::~ServerInfoBase()
{
}

void ServerInfoBase::SetCalibrationMatrix(igtl::Matrix4x4& matrix)
{
  this->FlagCalibration = 1;
  for (int i = 0; i < 4; i ++)
    {
    for (int j = 0; j < 4; j ++)
      {
      this->CalibrationMatrix[i][j] = matrix[i][j];
      }
    }
}

int ServerInfoBase::GetCalibrationMatrix(igtl::Matrix4x4& matrix)
{
  if (this->FlagCalibration)
    {
    for (int i = 0; i < 4; i ++)
      {
      for (int j = 0; j < 4; j ++)
        {
        matrix[i][j] = this->CalibrationMatrix[i][j];
        }
      }
    return 1;
    }
  else
    {
    return 0;
    }
}


void ServerInfoBase::SetTargetMatrix(igtl::Matrix4x4& matrix)
{
  this->FlagTarget = 1;

  for (int i = 0; i < 4; i ++)
    {
    for (int j = 0; j < 4; j ++)
      {
      this->TargetMatrix[i][j] = matrix[i][j];
      }
    }

}


int ServerInfoBase::GetTargetMatrix(igtl::Matrix4x4& matrix)
{
  if (this->FlagTarget)
    {
    for (int i = 0; i < 4; i ++)
      {
      for (int j = 0; j < 4; j ++)
        {
        matrix[i][j] = this->TargetMatrix[i][j];
        }
      }
    return 1;
    }
  else
    {
    return 0;
    }
}

