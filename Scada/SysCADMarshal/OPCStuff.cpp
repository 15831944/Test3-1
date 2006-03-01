//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "opcstuff.h"

//#include "..\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



LPCTSTR OPCQualityAsString(long q)
  {
  // NotGood
  static CString X;
  switch (q & OPC_STATUS_MASK)
    {
    //
    case OPC_QUALITY_BAD             : X="QUALITY_BAD";             break;
    // STATUS_MASK Values for Quality = BAD
    case OPC_QUALITY_CONFIG_ERROR    : X="QUALITY_CONFIG_ERROR";    break;
    case OPC_QUALITY_NOT_CONNECTED   : X="QUALITY_NOT_CONNECTED";   break;
    case OPC_QUALITY_DEVICE_FAILURE  : X="QUALITY_DEVICE_FAILURE";  break;
    case OPC_QUALITY_SENSOR_FAILURE  : X="QUALITY_SENSOR_FAILURE";  break;
    case OPC_QUALITY_LAST_KNOWN      : X="QUALITY_LAST_KNOWN";      break;
    case OPC_QUALITY_COMM_FAILURE    : X="QUALITY_COMM_FAILURE";    break;
    case OPC_QUALITY_OUT_OF_SERVICE  : X="QUALITY_OUT_OF_SERVICE";  break;
    //
    case OPC_QUALITY_UNCERTAIN       : X="QUALITY_UNCERTAIN";       break;
    // STATUS_MASK Values for Quality = UNCERTAIN
    case OPC_QUALITY_LAST_USABLE     : X="QUALITY_LAST_USABLE";     break;
    case OPC_QUALITY_SENSOR_CAL      : X="QUALITY_SENSOR_CAL";      break;
    case OPC_QUALITY_EGU_EXCEEDED    : X="QUALITY_EGU_EXCEEDED";    break;
    case OPC_QUALITY_SUB_NORMAL      : X="QUALITY_SUB_NORMAL";      break;
    //
    case OPC_QUALITY_GOOD            : X="QUALITY_GOOD";            break;
    // STATUS_MASK Values for Quality = GOOD
    case OPC_QUALITY_LOCAL_OVERRIDE  : X="QUALITY_LOCAL_OVERRIDE";  break;
    }

  switch (q & OPC_LIMIT_MASK)
    {
    case OPC_LIMIT_OK     : break;
    case OPC_LIMIT_LOW    : X+= "LIMIT_LOW"; break;
    case OPC_LIMIT_HIGH   : X+= "LIMIT_HIGH"; break;
    case OPC_LIMIT_CONST  : X+= "LIMIT_CONST"; break;
    }

  return X;
  }

LPCTSTR OPCQualityAsShortString(long q)
  {
  // NotGood
  static CString X;
  switch (q & OPC_STATUS_MASK)
    {
    //
    case OPC_QUALITY_BAD             : X="BAD";             break;
    // STATUS_MASK Values for Quality = BAD
    case OPC_QUALITY_CONFIG_ERROR    : X="CONFIG_ERROR";    break;
    case OPC_QUALITY_NOT_CONNECTED   : X="NOT_CONNECTED";   break;
    case OPC_QUALITY_DEVICE_FAILURE  : X="DEVICE_FAILURE";  break;
    case OPC_QUALITY_SENSOR_FAILURE  : X="SENSOR_FAILURE";  break;
    case OPC_QUALITY_LAST_KNOWN      : X="LAST_KNOWN";      break;
    case OPC_QUALITY_COMM_FAILURE    : X="COMM_FAILURE";    break;
    case OPC_QUALITY_OUT_OF_SERVICE  : X="OUT_OF_SERVICE";  break;
    //
    case OPC_QUALITY_UNCERTAIN       : X="UNCERTAIN";       break;
    // STATUS_MASK Values for Quality = UNCERTAIN
    case OPC_QUALITY_LAST_USABLE     : X="LAST_USABLE";     break;
    case OPC_QUALITY_SENSOR_CAL      : X="SENSOR_CAL";      break;
    case OPC_QUALITY_EGU_EXCEEDED    : X="EGU_EXCEEDED";    break;
    case OPC_QUALITY_SUB_NORMAL      : X="SUB_NORMAL";      break;
    //
    case OPC_QUALITY_GOOD            : X="GOOD";            break;
    // STATUS_MASK Values for Quality = GOOD
    case OPC_QUALITY_LOCAL_OVERRIDE  : X="LOCAL_OVERRIDE";  break;
    }

  switch (q & OPC_LIMIT_MASK)
    {
    case OPC_LIMIT_OK     : break;
    case OPC_LIMIT_LOW    : X+= "LOW"; break;
    case OPC_LIMIT_HIGH   : X+= "HIGH"; break;
    case OPC_LIMIT_CONST  : X+= "CONST"; break;
    }

  return X;
  }
