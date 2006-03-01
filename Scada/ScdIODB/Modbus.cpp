//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Modbus.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  Modbus device protocol
//
//  Description:  The following classes:
//                OAnalogData - tag scaling and alarming
//                OTag - base tag; name, value, qual, scaling
//                OModbusTag - modbus specific tag; location, simulation
//                OTagGroup - a collection of tags and other OTagGroups
//                ODevice - a OTagGroup with 2 sets of device parameters
//                ODeviceParams -
//                OPort - COM port parameters and fns to send/receive msgs
//
// Note use of Unicode for OLE, and BYTE for protocol messages
//**************************************************************************

#include "stdafx.h"
#include "message.h"
#include "OPCGroup.h"
#include <math.h>
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(OAnalogData, CObject,    2 | VERSIONABLE_SCHEMA)
IMPLEMENT_SERIAL(OTag,        CObject,    1 | VERSIONABLE_SCHEMA)
IMPLEMENT_SERIAL(OModbusTag,  OTag,       1 | VERSIONABLE_SCHEMA)
//IMPLEMENT_SERIAL(ODeviceParams, CObject,  1 | VERSIONABLE_SCHEMA)
IMPLEMENT_SERIAL(OTagGroup,   CObject,    1 | VERSIONABLE_SCHEMA)
IMPLEMENT_SERIAL(ODevice,     OTagGroup,  1 | VERSIONABLE_SCHEMA)
//IMPLEMENT_SERIAL(OPort,       CObject,    1 | VERSIONABLE_SCHEMA)

const BYTE CR = 0x0d;
const BYTE LF = 0x0a;

const BYTE FN_READ_OUTPUT_COILS   = 1;  // 0X
const BYTE FN_READ_INPUT_COILS    = 2;  // 1X
const BYTE FN_READ_HOLDING_REGS   = 3;  // 4X
const BYTE FN_READ_INPUT_REGS     = 4;  // 3X
const BYTE FN_WRITE_COIL          = 5;  // 0X
const BYTE FN_WRITE_REGISTER      = 6;  // 4X
const BYTE FN_READ_EXCEPTION      = 7;  // 4X
const BYTE FN_READ_COMM_EVENT_CTR = 11; // Communication stats
const BYTE FN_READ_COMM_EVENT_LOG = 12; // Communication info
const BYTE FN_WRITE_COILS         = 15; // 0X
const BYTE FN_WRITE_REGISTERS     = 16; // 4X
const BYTE FN_READ_DEVICE_ID      = 17; // Device specific

//*******************************************************************
OAnalogData::OAnalogData()
: EngUnits()
   {
   InitValue = MinEngRange = 0.0;
   MaxEngRange = 10.0;
   MinRawRange = 0.0;
   MaxRawRange = 1000.0;
   Deadband = LogDeadband = 0.0;
   Options = 0;
   }

OAnalogData::OAnalogData(const OAnalogData& source)
   {
   Assign( source );
   }

OAnalogData& OAnalogData::operator=(const OAnalogData& source)
   {
   if( this == &source )
      return *this;
   Assign( source );
   return *this;
   }

void OAnalogData::Assign(const OAnalogData& source)
   {
   EngUnits          = source.EngUnits;
   InitValue         = source.InitValue;
   MinEngRange       = source.MinEngRange;
   MaxEngRange       = source.MaxEngRange;
   MinRawRange       = source.MinRawRange;
   MaxRawRange       = source.MaxRawRange;
   Deadband          = source.Deadband;
   LogDeadband       = source.LogDeadband;
   Options           = source.Options;
   }

BOOL OAnalogData::operator==(const OAnalogData& source) const
   {
   if(EngUnits          == source.EngUnits &&
      InitValue         == source.InitValue &&
      MinEngRange       == source.MinEngRange &&
      MaxEngRange       == source.MaxEngRange &&
      MinRawRange       == source.MinRawRange &&
      MaxRawRange       == source.MaxRawRange &&
      Deadband          == source.Deadband &&
      LogDeadband       == source.LogDeadband &&
      Options           == source.Options )
      return TRUE;

   return FALSE;
   }

//*******************************************************************
BOOL OAnalogData::Scale(VARIANT& value)
{
ASSERT( value.vt == VT_R8 );

   if (Options & ANALOG_SQRT)
      value.dblVal = ((sqrt(value.dblVal) - MinRawRange) *
                        (MaxEngRange - MinEngRange)) /
                         sqrt(MaxRawRange - MinRawRange) +
                         MinEngRange;
   else
      value.dblVal = ((value.dblVal - MinRawRange)/
                       (MaxRawRange - MinRawRange)) *
                       (MaxEngRange - MinEngRange) +
                        MinEngRange;

   return TRUE;
}

//*******************************************************************
void OTempTag::Release()
{
   OTag::Release();
   if( !InUse() )
      delete this;
}

//*******************************************************************

long OTag::s_idno=0;

OTag::OTag()
: name(), description()
   {
   idno=++s_idno;
   timestamp.dwLowDateTime=timestamp.dwHighDateTime=0;
   quality = OPC_QUALITY_UNCERTAIN;
   nativeType = VT_EMPTY;
   active = TRUE;
   enableProcessing=FALSE;
   pProcess=new OAnalogData;
   refCount=0;
   pGroup = NULL;
   }

OTag::OTag(const OTag& source)
: name(source.name), description(source.description)
   {
   timestamp.dwLowDateTime = source.timestamp.dwLowDateTime;
   timestamp.dwHighDateTime = source.timestamp.dwHighDateTime;
   quality                 = source.quality;
   nativeType              = source.nativeType;
   active                  = source.active;
   enableProcessing        = source.enableProcessing;
   pProcess                = new OAnalogData(*source.pProcess);
   value = source.value;
   refCount=0;
   }

OTag::~OTag()
   {
   delete pProcess;
   ASSERT( refCount == 0 );
   }

OTag& OTag::operator=(const OTag& source)
   {
   if( this == &source )
      return *this;
   name                    = source.name;
   description             = source.description;
   nativeType              = source.nativeType;
   active                  = source.active;
   timestamp.dwLowDateTime = source.timestamp.dwLowDateTime;
   timestamp.dwHighDateTime = source.timestamp.dwHighDateTime;
   enableProcessing        = source.enableProcessing;
   delete pProcess;
   pProcess             = new OAnalogData(*source.pProcess);
   value = source.value;
   return *this;
   }

//*******************************************************************
void OTag::AddRef()
{
   InterlockedIncrement(&refCount);
   pGroup->AddRef();
}

//*******************************************************************
void OTag::Release()
{
   InterlockedDecrement(&refCount);
   pGroup->Release();
}

//*******************************************************************
// Always convert to Bool or double internally if scaling
BOOL OTag::SetValue( const VARIANT& v )
   {
   if( enableProcessing )
      {
      if( nativeType == VT_BOOL )
         {
         ASSERT( v.vt == VT_BOOL );
         value = v;
         return TRUE;
         }

      value.vt = VT_R8;
      switch( v.vt )
         {
         case VT_I1 :   value.dblVal = v.cVal;  break;
         case VT_I2 :   value.dblVal = v.iVal;  break;
         case VT_I4 :   value.dblVal = v.lVal;  break;
         case VT_UI1:   value.dblVal = v.bVal;  break;
         case VT_UI2:   value.dblVal = v.uiVal;  break;
         case VT_UI4:   value.dblVal = v.ulVal;  break;
         case VT_R4 :   value.dblVal = v.fltVal;  break;
         case VT_R8 :   value.dblVal = v.dblVal;  break;
         default: ASSERT( FALSE );
         }
      // perform scaling, etc
      pProcess->Scale( value );
      }
   else
      {
      value = v;  // just copy the data
      }
   return TRUE;
   }

//*******************************************************************
// Tags are added alphabetically for display
BOOL OTagGroup::AddTag( OModbusTag* pTag )
   {
   POSITION pos = tags.GetHeadPosition();

   if( pos == NULL )    // list is empty
      {
      tags.AddHead( pTag );
      return TRUE;
      }

   while( pos )        // iterate over all objects
      {
      POSITION temp = pos;
      OTag* pTestTag = tags.GetNext( temp );

      // same as: if( pTag->name == pTestTag->name ) ignoring case
      if( pTag->name.CompareNoCase( pTestTag->name ) == -1
       && pTag != pTestTag )
         break;
      pos = temp;
      }

   if( pos )     // found a position
      tags.InsertBefore( pos, (CObject*)pTag );
   else              // past the end
      tags.AddTail( pTag );
   return TRUE;
   }

//*******************************************************************
OTag* OTagGroup::FindTag( const CString& target)
   {
   int delimiter = target.Find( _T('.') );
   if( delimiter == -1 )   // tag name if no delimiter
      {
      POSITION pos = tags.GetHeadPosition();
      while( pos )
         {
         OTag* pTag = tags.GetNext( pos );
         if( pTag->name.CompareNoCase( target ) == 0 )
            return pTag;
         }
      }
   else
      {
      CString group( target.Left( delimiter ) );
      POSITION pos = groups.GetHeadPosition();
      while( pos )
         {
         OTagGroup* pGroup = groups.GetNext( pos );
         if( pGroup->name.CompareNoCase( group ) == 0 )
            return pGroup->FindTag( target.Mid( delimiter+1 ));
         }
      }

   return NULL;
   }

CString OTagGroup::FindTagName(CString target)
{
   int delimiter = target.Find( _T('.') );
   BOOL bFoundTag = TRUE;
   if( delimiter == -1 )   // tag name if no delimiter
   {
	   if(bFoundTag)
	   {
		  POSITION pos = tags.GetHeadPosition();
		  while( pos )
		  {
			OModbusTag* pTag = tags.GetNext( pos );
			if( pTag->name.CompareNoCase( target ) == 0 )
			{
				bFoundTag = FALSE;
				return GetPathName() + target;
			}
		  }
		}
	   if(bFoundTag)
	   {
		   CString branchName = target;
		   POSITION pos = groups.GetHeadPosition();
		   while( pos )
		   {
			 OTagGroup* pBranch = groups.GetNext( pos );
             if( pBranch->name.CompareNoCase( branchName ) == 0 )
                return target;
		   }
	   }
    }
   else
   {
      CString branchName( target.Left( delimiter ) );
      POSITION pos = groups.GetHeadPosition();
      while( pos )
      {
         OTagGroup* pBranch = groups.GetNext( pos );
         if( pBranch->name.CompareNoCase( branchName ) == 0 )
            return pBranch->FindTagName( target.Mid( delimiter+1 ) );
      }
   }
   return "";
}
//*******************************************************************
OModbusTag::OModbusTag()
: OTag(), m_IsOutput(false)//, offset(0)
  , simType( RAMP )
   {
   }

OModbusTag::OModbusTag(const OModbusTag& source)
: OTag(source), m_IsOutput(source.m_IsOutput)//, offset(source.offset)
  , simType(source.simType)
   {
   }

OModbusTag& OModbusTag::operator=(const OModbusTag& source)
   {
   if( this == &source )
      return *this;
   OTag::operator=( source );
   m_IsOutput  = source.m_IsOutput;
   //offset = source.offset;
   simType = source.simType;
   return *this;
   }

//CString OModbusTag::LocationString() const
//   {
//   CString s;
//   WORD loc;
//   switch( range )
//      {
//      case OUTPUT_BOOL:     loc = 0; break;
//      case INPUT_BOOL:      loc = 10000; break;
//      case OUTPUT_VALUE: loc = 40000; break;
//      case INPUT_VALUE:  loc = 30000; break;
//      default: ASSERT( FALSE );
//      }
//   loc += offset+1;  // user sees 1 based
//   s.Format(_T("%d"), loc );
//   return s;
//   }

LPTSTR OModbusTag::TypeString() const
   {
   switch(value.vt)
      {
      case VT_BOOL: return m_IsOutput ? _T("Bool_Out"): _T("Bool_In");
      case VT_I1:   return m_IsOutput ? _T("I1_Out"): _T("I1_In");
      case VT_I2:   return m_IsOutput ? _T("I2_Out"): _T("I2_In");
      case VT_I4:   return m_IsOutput ? _T("I4_Out"): _T("I4_In");
      case VT_UI1:  return m_IsOutput ? _T("U1_Out"): _T("U1_In");
      case VT_UI2:  return m_IsOutput ? _T("U2_Out"): _T("U2_In");
      case VT_UI4:  return m_IsOutput ? _T("U4_Out"): _T("U4_In");
      case VT_R4:   return m_IsOutput ? _T("R4_Out"): _T("R4_In");
      case VT_R8:   return m_IsOutput ? _T("R8_Out"): _T("R8_In");
      default:      return m_IsOutput ? _T("??_Out"): _T("??_In");
        ASSERT( FALSE );
      }
   return _T("");
   }

//*******************************************************************
BOOL ODevice::IdDevice()
   {
   //pCurrent->pPort->Initialize( );   // Only first call will do anything
   //
   //OMessage* msg = NULL;
   //if( pCurrent->pPort->dataBits == 8 )        
   //   msg = new ORtuMessage;
   //else
   //   msg = new OAsciiMessage;
   //msg->Add( pCurrent->address );
   //msg->Add( 17 );   // Report ID function
   //msg->Finish();
   //ASSERT( msg->IsOk() );
   //
   //m_Sent++;
   //msg = pCurrent->pPort->Message( msg, pCurrent->timeout );
   //if( msg->IsOk() )
   //   {
   //   m_Rcvd++;
   //   pCurrent->pPort->m_Rcvd++;
   //   }
   //else
   //   {
   //   // lock
   //   m_Messages++;
   //   pCurrent->pPort->m_Messages++;  // need lock!
   //   delete msg;
   //   return FALSE;
   //   }
   //ASSERT( msg->GetAddress() == pCurrent->address );
   //ASSERT( msg->GetFunction() == 17 );
   //delete msg;
   return TRUE;
   }

//*******************************************************************
const double pi = 3.1415926535;

void sim( OTagList& tags )
{
   CSLock wait( &tagCS );   // protect data
   double t = GetTickCount()/6000.0;
   POSITION pos = tags.GetHeadPosition();
   while( pos )
      {
      OModbusTag* pTag = tags.GetNext( pos );
      pTag->quality = OPC_QUALITY_GOOD;
      if( !pTag->active || pTag->m_IsOutput)
         continue;

      switch( pTag->value.vt )
        {
          case VT_EMPTY:
          case VT_NULL:
             pTag->value.vt = VT_R8;
             pTag->value.dblVal = 0.0;
             break;
          case VT_BOOL:
             switch(pTag->simType)
                {
                case OModbusTag::SIN:
                case OModbusTag::RAMP:
                   pTag->value.boolVal = ((int)t)&1 ? VARIANT_TRUE : VARIANT_FALSE; break;
                case OModbusTag::RANDOM:
                   pTag->value.boolVal = rand()&1 ? VARIANT_TRUE : VARIANT_FALSE; break;
                }
             break;
          case VT_UI1: // uchar
             switch(pTag->simType)
                {
                case OModbusTag::SIN:
                   pTag->value.bVal = (UCHAR)(50*sin(pi*t/10)+50); break;
                case OModbusTag::RAMP:
                   pTag->value.bVal = (pTag->value.bVal+1)%100; break;
                case OModbusTag::RANDOM:
                   pTag->value.bVal = rand()%100; break;
                }
             break;
          case VT_I2 :   // short
             switch(pTag->simType)
                {
                case OModbusTag::SIN:
                   pTag->value.iVal = (short)(50*sin(pi*t/10)+50); break;
                case OModbusTag::RAMP:
                   pTag->value.iVal = (pTag->value.iVal+1)%100; break;
                case OModbusTag::RANDOM:
                   pTag->value.iVal = rand()%100; break;
                }
             break;
          case VT_I4 :  // long
             switch(pTag->simType)
                {
                case OModbusTag::SIN:
                   pTag->value.lVal = (long)(50*sin(pi*t/10)+50); break;
                case OModbusTag::RAMP:
                   pTag->value.lVal = (pTag->value.lVal+1)%100; break;
                case OModbusTag::RANDOM:
                   pTag->value.lVal = rand()%100; break;
                }
             break;
          case VT_R4 :
             switch(pTag->simType)
                {
                case OModbusTag::SIN:
                   pTag->value.fltVal = (float)sin(pi*t/10); break;
                case OModbusTag::RAMP:
                   pTag->value.fltVal = (float)(long(t)%100); break;
                case OModbusTag::RANDOM:
                   pTag->value.fltVal = (float)((rand()%300)/3.0); break;
                }
             break;
          case VT_R8 :
             switch(pTag->simType)
                {
                case OModbusTag::SIN:
                   pTag->value.dblVal = sin(pi*t/10); break;
                case OModbusTag::RAMP:
                   pTag->value.dblVal = (long)(t)%100; break;
                case OModbusTag::RANDOM:
                   pTag->value.dblVal = (rand()%300)/3.0; break;
                }
             break;
          default: ASSERT( FALSE );
        }
      CoFileTimeNow( &pTag->timestamp );
      }
}

//*******************************************************************
const int ADJACENT_SPAN = 25;
const int MAX_SPAN = 124;

DWORD FileTimeDiff( FILETIME& first, FILETIME& last )
   {
   DWORDLONG earlierTime = first.dwHighDateTime;
   earlierTime = earlierTime<<32 | (DWORDLONG)first.dwLowDateTime;
   DWORDLONG laterTime = last.dwHighDateTime;
   laterTime = laterTime<<32 | (DWORDLONG)last.dwLowDateTime;
   DWORDLONG diff = laterTime - earlierTime;
   return (DWORD)(diff / 10000); // 100nSec -> mSec
   }

//*******************************************************************
BOOL ODevice::UpdateTags( OTagList& tags, DWORD mSec )
   {
   if( simulate )
       {
       sim( tags );
       return TRUE;
       }

   //pCurrent->pPort->Initialize( );   // Only first call will do anything

   FILETIME now;
   CoFileTimeNow( &now );

   // for each tag in the list
   POSITION pos = tags.GetHeadPosition();
   while( pos )
      {  // maybe reuse a msg, instead of new/delete each time
      POSITION savedPos = pos;
      OModbusTag* pTag = tags.GetNext( pos );
      
      
//      OModbusTag::TYPE range = pTag->range;
//
//      // search for tags to group with this one
//      BOOL done = FALSE;
//      int count=1;
//      WORD groupSize = 1;
//      WORD groupStart = pTag->offset;
//      OModbusTag* pLastTag = pTag;
//      while( pos && !done)
//         {
//         OModbusTag* pNextTag = tags.GetNext( pos );
//         // Can this tag be combined in a message?
//         if( pNextTag->offset - pLastTag->offset < ADJACENT_SPAN
//          && pNextTag->offset - groupStart < MAX_SPAN
//          && pNextTag->range == range)
//            {
//            groupSize = pNextTag->offset - groupStart + 1;
//            count++;
//            }
//         else
//            done = TRUE;
//         pLastTag = pNextTag;
//         }
//
//      // create a packet for it
//      OMessage* msg = CreateReadPacket( pTag, groupSize+1 );
//
//      // Wait for the device's delay period?
//      DWORD diff = FileTimeDiff(pCurrent->lastMessage, now );
//      if( diff < pCurrent->interPollDelay )
//         Sleep( pCurrent->interPollDelay - diff );
//      // send the message to the port and get response
//      m_Sent++;
//      msg = pCurrent->pPort->Message( msg, pCurrent->timeout );
//      pCurrent->lastMessage = now;
//      if( msg->IsOk() == COMM_NO_ERROR )
//         {
//         m_Rcvd++;
//         pCurrent->pPort->m_Rcvd++;
//         }
//      else
//         {
//         // lock
//         if( msg->IsOk() == TIMEOUT )
//            {
//            m_Timeouts++;
//            pCurrent->pPort->m_Timeouts++;  // need lock!
//
//            // set quality for remaining tags
//            CSLock wait( &tagCS );   // protect data
//            pos = savedPos;
//            while( pos )
//               {
//               pTag = tags.GetNext( pos );
//               pTag->quality = OPC_QUALITY_BAD | OPC_QUALITY_COMM_FAILURE;
//               }
//            delete msg;
//            return FALSE;
//            }
//         else
//            {
//            m_Messages++;
//            pCurrent->pPort->m_Messages++;  // need lock!
//            }
//         }
//
//
//      // extract the data from the packet
//      VARIANT value;
//      pos = savedPos;
//      while( pos && count-- )
//         {
//         CSLock wait( &tagCS );   // protect data
//         pTag = tags.GetNext( pos );
//         if( msg->IsOk() == COMM_NO_ERROR )
//            {
//            switch( pTag->range )
//               {
//               case OModbusTag::OUTPUT_BOOL:
//               case OModbusTag::INPUT_BOOL :
//                  {
//                  value.vt = VT_BOOL;
//                  value.boolVal = msg->GetBit( pTag->offset - groupStart );
//                  break;
//                  }
//               case OModbusTag::OUTPUT_VALUE:
//               case OModbusTag::INPUT_VALUE :
//                  value.vt = pTag->nativeType;
//                  switch( pTag->nativeType )
//                     {
//                     default:
//                     case VT_EMPTY: ASSERT( FALSE );
//                     case VT_I2:
//                        value.iVal = msg->GetWord( pTag->offset - groupStart );
//                        break;
//                     case VT_I4:
//                        value.lVal = MAKELONG(msg->GetWord( pTag->offset - groupStart ),
//                                              msg->GetWord( pTag->offset - groupStart + 1 ));
//                        if( pCurrent->wordSwap )
//                           value.lVal = MAKELONG(HIWORD(value.lVal),LOWORD(value.lVal));
//                        break;
//					 case VT_R8:
//                     case VT_R4:
//                        DWORD temp = MAKELONG(msg->GetWord( pTag->offset - groupStart ),
//                                              msg->GetWord( pTag->offset - groupStart + 1 ));
//                        if( pCurrent->wordSwap )
//                           temp = MAKELONG(HIWORD(temp),LOWORD(temp));
//                        void* pfloat = &temp;
//                        value.fltVal = *(float*)pfloat;
//                        break;
//                     }
//                  break;
//               default: ASSERT( FALSE );
//               }
//            pTag->SetValue( value );
//            pTag->quality = OPC_QUALITY_GOOD;
//            }
//         else  // message had an error
//            {
//            pTag->quality = OPC_QUALITY_BAD | OPC_QUALITY_COMM_FAILURE;
//            }
//         pTag->timestamp = msg->timestamp;
//         }
//      delete msg;
      }
   return TRUE;
   }

//*******************************************************************
//OMessage* ODevice::CreateReadPacket( OModbusTag* pTag, WORD number )
//   {
//   OMessage* msg = NULL;
//   if( pCurrent->pPort->dataBits == 8 )
//      msg = new ORtuMessage;
//   else
//      msg = new OAsciiMessage;
//   msg->Add( pCurrent->address );
//   switch( pTag->range )
//      {
//      case OModbusTag::OUTPUT_BOOL:      msg->Add( FN_READ_OUTPUT_COILS ); break;
//      case OModbusTag::INPUT_BOOL :      msg->Add( FN_READ_INPUT_COILS ); break;
//      case OModbusTag::OUTPUT_VALUE:  msg->Add( FN_READ_HOLDING_REGS ); break;
//      case OModbusTag::INPUT_VALUE :  msg->Add( FN_READ_INPUT_REGS ); break;
//      }
//   msg->Add( HIBYTE(pTag->offset) );
//   msg->Add( LOBYTE(pTag->offset) );
//   msg->Add(  HIBYTE(number) );
//   msg->Add(  LOBYTE(number) );
//   msg->Finish();
//   ASSERT( msg->IsOk() );
//   return msg;
//   }
//
////*******************************************************************
//OMessage* ODevice::CreateWritePacket( OModbusTag* pTag )
//   {
//   WORD number=0;
//   OMessage* msg = NULL;
//   if( pCurrent->pPort->dataBits == 8 )
//      msg = new ORtuMessage;
//   else
//      msg = new OAsciiMessage;
//   msg->Add( pCurrent->address );
//   switch( pTag->range )
//      {
//      case OModbusTag::OUTPUT_BOOL:
//         msg->Add( FN_WRITE_COIL );
//         if( pTag->value.boolVal )
//            number = 0xFF00;
//         break;
//      case OModbusTag::OUTPUT_VALUE:
//         msg->Add( FN_WRITE_REGISTER );
//         number = pTag->value.iVal;
//         break;
//      }
//   msg->Add( HIBYTE(pTag->offset) );
//   msg->Add( LOBYTE(pTag->offset) );
//   msg->Add(  HIBYTE(number) );
//   msg->Add(  LOBYTE(number) );
//   msg->Finish();
//   ASSERT( msg->IsOk() );
//   return msg;
//   }
//
//*******************************************************************
BOOL ODevice::WriteTag( OModbusTag* pTag )
   {
   bool DoChg=true;
   //if (pTag->value.vt==VT_R8 && !_finite(pTag->value.dblVal) ||
   //    pTag->value.vt==VT_R4 && !_finite(pTag->value.fltVal))
   //  {
   //  DoChg=false;
   //  }
   
   if (pTag->value.vt==VT_R8 && !_finite(pTag->value.dblVal))
     pTag->value.dblVal=0.0;
   if (pTag->value.vt==VT_R4 && !_finite(pTag->value.fltVal))
     pTag->value.fltVal=0.0;

   if (DoChg)
     {
     pTag->value.ChangeType( pTag->nativeType );

     if( pTag->value.vt==VT_R8 )
        pTag->value.ChangeType( VT_R4 );
     };

   pTag->quality = OPC_QUALITY_GOOD;

//   if( simulate )
//      return TRUE;
//
   BOOL result = TRUE;
//   WORD number=0;
//   OMessage* msg = NULL;
//   if( pCurrent->pPort->dataBits == 8 )
//      msg = new ORtuMessage;
//   else
//      msg = new OAsciiMessage;
//
//   msg->Add( pCurrent->address );
//   switch( pTag->range )
//      {
//      case OModbusTag::OUTPUT_BOOL:
//         msg->Add( FN_WRITE_COIL );
//         if( pTag->value.boolVal )
//            number = 0xFF00;
//         break;
//      case OModbusTag::OUTPUT_VALUE:
//         msg->Add( FN_WRITE_REGISTER );
//         if( pCurrent->wordSwap )
//            pTag->value.lVal = MAKELONG(HIWORD(pTag->value.lVal),LOWORD(pTag->value.lVal));
//         number = pTag->value.iVal;
//         break;
//      }
//   msg->Add( HIBYTE(pTag->offset) );
//   msg->Add( LOBYTE(pTag->offset) );
//   msg->Add(  HIBYTE(number) );
//   msg->Add(  LOBYTE(number) );
//   msg->Finish();
//   ASSERT( msg->IsOk() );
//
//   // Wait for the device's delay period?
//   FILETIME now;
//   CoFileTimeNow( &now );
//   DWORD diff = FileTimeDiff(pCurrent->lastMessage, now );
//   if( diff < pCurrent->interPollDelay )
//      Sleep( pCurrent->interPollDelay - diff );
//   msg = pCurrent->pPort->Message( msg, pCurrent->timeout );
//   pCurrent->lastMessage = now;
//   if( msg && msg->IsOk() )
//      {
//      m_Rcvd++;
//      pCurrent->pPort->m_Rcvd++;
//      pTag->quality = OPC_QUALITY_GOOD;
//      result = TRUE;
//      }
//   else
//      {
//      m_Messages++;
//      pCurrent->pPort->m_Messages++;
//      pTag->quality = OPC_QUALITY_BAD;
//      return FALSE;
//      }
//
//   // Write 2nd word if necessary
//   if( pTag->range==OModbusTag::OUTPUT_VALUE
//    && (pTag->value.vt==VT_I4 || pTag->value.vt==VT_R4) )
//   {
//      msg->Clear();
//      msg->Add( pCurrent->address );
//      msg->Add( FN_WRITE_REGISTER );
//      msg->Add( HIBYTE(pTag->offset+1) );
//      msg->Add( LOBYTE(pTag->offset+1) );
//      number = HIWORD(pTag->value.lVal);
//      msg->Add(  HIBYTE(number) );
//      msg->Add(  LOBYTE(number) );
//      msg->Finish();
//      ASSERT( msg->IsOk() );
//
//      // Wait for the device's delay period?
//      FILETIME now;
//      CoFileTimeNow( &now );
//      DWORD diff = FileTimeDiff(pCurrent->lastMessage, now );
//      if( diff > pCurrent->interPollDelay )
//         Sleep( diff - pCurrent->interPollDelay );
//      msg = pCurrent->pPort->Message( msg, pCurrent->timeout );
//      pCurrent->lastMessage = now;
//      if( msg && msg->IsOk() )
//         {
//         m_Rcvd++;
//         pCurrent->pPort->m_Rcvd++;
//         pTag->quality = OPC_QUALITY_GOOD;
//         result = TRUE;
//         }
//      else
//         {
//         m_Messages++;
//         pCurrent->pPort->m_Messages++;
//         pTag->quality = OPC_QUALITY_BAD;
//         }
//   }
//   delete msg;
   return result;
   }
//
//*******************************************************************
//OPort::OPort()
//   {
//   InitializeCriticalSection( &lock );
//   enable = TRUE;
//   reInitialize = TRUE;
//   baud = 9600;
//   dataBits = 8;
//   parity = NOPARITY;
//   flow = HARDWARE;
//   stop = ONESTOPBIT;
//   hPort = hLog = NULL;
//   m_Timeouts = m_Sent_sec = m_Rcvd_sec = m_Sent = m_Rcvd = 0;
//   m_Parity = m_Overruns = m_Messages = m_Framing = 0;
//
//   cts.ReadIntervalTimeout = 0;
//   cts.ReadTotalTimeoutMultiplier = 1;
//   cts.ReadTotalTimeoutConstant = 500;
//   cts.WriteTotalTimeoutMultiplier = 1;
//   cts.WriteTotalTimeoutConstant = 500;
//   }
//
//OPort::~OPort()
//   {
//   DeleteCriticalSection( &lock );
//   CloseHandle( hPort );
//   }
//
////*******************************************************************
//BOOL OPort::Initialize( )
//   {
//   CSLock temp( &lock );
//   if( hPort == NULL )
//   {
//      hPort = CreateFile( name,
//                         GENERIC_READ | GENERIC_WRITE,
//                         0,
//                         NULL,   // security
//                         OPEN_EXISTING,
//                         FILE_ATTRIBUTE_NORMAL,
//                         NULL);
//      if( hPort == INVALID_HANDLE_VALUE )
//         {
//         TRACE_MSG
//         return FALSE;
//         }
//      reInitialize = TRUE;
//   }
//
//   if( reInitialize == FALSE )
//      return TRUE;
//
//   DCB dcb;
//   if( !GetCommState( hPort, &dcb ) )
//      {
//      TRACE_MSG
//      return FALSE;
//      }
//   dcb.BaudRate = baud;
//   dcb.fBinary = TRUE;
//   dcb.fParity = (parity==NOPARITY ? FALSE : TRUE);
//   dcb.fNull = FALSE;
//   dcb.ByteSize = (UCHAR)dataBits;
//   dcb.Parity = (UCHAR)parity;
//   dcb.StopBits = (UCHAR)stop;
//   dcb.fOutxCtsFlow = (flow==HARDWARE ? TRUE : FALSE);
//   dcb.fRtsControl = (flow==HARDWARE ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE);
//
//   if( !SetCommState( hPort, &dcb ) )
//      {
//      TRACE_MSG
//      return FALSE;
//      }
//
//   reInitialize = FALSE;
//   return TRUE;
//   }
//
////*******************************************************************
//// Blocking call to send message and recieve response
//OMessage* OPort::Message( OMessage* msg, DWORD timeout )
//   {
//   CSLock temp( &lock );
//
//   // if this message has a new timeout value, set it first
//   if( cts.ReadTotalTimeoutConstant != timeout )
//   {
//      cts.ReadTotalTimeoutConstant = timeout;
//      cts.WriteTotalTimeoutConstant = timeout;
//      if( !SetCommTimeouts( hPort, &cts ) )
//      {
//         msg->Clear();
//         msg->ok = TIMEOUT;
//         return msg;
//      }
//   }
//
//   m_Sent++;
//
//   DWORD written = 0;
//   if( !WriteFile(hPort, msg->buffer, msg->length, &written, NULL ) )
//      {
//      msg->Clear();
//      msg->ok = TIMEOUT;
//      return msg;
//      }
//   if( msg->length != written )
//      {
//      msg->Clear();
//      msg->ok = TIMEOUT;
//      return msg;
//      }
//
//   msg->Clear();
//   DWORD read = 0;
//   BYTE input;
//   while( !msg->MsgDone() )
//      {
//      if( !ReadFile(hPort, &input, 1, &read, NULL) || (read != 1) )
//         {
//         msg->ok = TIMEOUT;
//         return msg;
//         }
//      msg->buffer[msg->length++] = input;
//      }
//
//   CoFileTimeNow( &msg->timestamp );
//   msg->ok = COMM_NO_ERROR;
//
//   return msg;
//   }

//*******************************************************************
void OAnalogData::Serialize(CArchive& ar)
   {
   if (ar.IsStoring())
      {
      ar << InitValue;
      ar << MinEngRange;
      ar << MaxEngRange;
      ar << MinRawRange;
      ar << MaxRawRange;
      ar << EngUnits;
      ar << Deadband;
      ar << LogDeadband;

      ar << Options;
      }
   else
      {
      UINT version = ar.m_nObjectSchema;
      ar >> InitValue;
      ar >> MinEngRange;
      ar >> MaxEngRange;
      ar >> MinRawRange;
      ar >> MaxRawRange;
      ar >> EngUnits;
      ar >> Deadband;
      ar >> LogDeadband;

      ar >> Options;
      }
   }

void OTag::Serialize(CArchive& ar)
   {
   if (ar.IsStoring())
      {
      ar << name;
      ar << description;
      ar << enableProcessing;
      ar << nativeType;
      ar << active;
      ar.SerializeClass(RUNTIME_CLASS(OAnalogData));
      pProcess->Serialize(ar);
      }
   else
      {
      UINT version = ar.m_nObjectSchema;
      ASSERT_KINDOF( CDoc, ar.m_pDocument );
      ar >> name;
      ar >> description;
      ar >> enableProcessing;
      ar >> nativeType;
      ar >> active;

      value.ChangeType( nativeType );
      ar.SerializeClass(RUNTIME_CLASS(OAnalogData));
      pProcess->Serialize(ar);
      }
   }

void OModbusTag::Serialize(CArchive& ar)
   {
   if (ar.IsStoring())
      {
      ar << (BOOL)m_IsOutput;
      //ar << offset;
      ar << simType;
      }
   else
      {
      UINT version = ar.m_nObjectSchema;
      BOOL temp;
      ar >> temp; m_IsOutput = temp;
      //ar >> offset;
      ar >> simType;
      }
   ar.SerializeClass(RUNTIME_CLASS(OTag));
   OTag::Serialize(ar);
   }

//void ODeviceParams::Serialize(CArchive& ar)
//   {
//   if (ar.IsStoring())
//      {
//      ar << portName;
//      ar << address;
//      ar << timeout;
//      ar << (WORD)format;
//      ar << wordSwap;
//      }
//   else
//      {
//      UINT version = ar.m_nObjectSchema;
//      WORD temp;
//      ar >> portName;
//      ar >> address;
//      ar >> timeout;
//      ar >> temp; format = (OFORMAT)temp;
//      ar >> wordSwap;
//      CDoc* pDoc = (CDoc*)ar.m_pDocument;
//      // get pointer to the named port
//      POSITION pos = pDoc->ports.GetHeadPosition();
//      while( pos )
//         {
//         OPort* pScanPort = pDoc->ports.GetNext( pos );
//         if( portName == pScanPort->name )
//            pPort = pScanPort;
//         }
//      }
//   }

void OTagGroup::SetTagDevices()
   {
   // set tag device pointers
   POSITION pos = tags.GetHeadPosition();
   while( pos )
      {
      OTag* pTag = tags.GetNext( pos );
      pTag->pGroup = this;
      }
   // set tag device pointers in sub-groups
   pos = groups.GetHeadPosition();
   while( pos )
      {
      OTagGroup* pGroup = groups.GetNext( pos );
      pGroup->SetTagDevices();
      }
   }

void OTagGroup::Serialize(CArchive& ar)
   {
   if (ar.IsStoring())
      {
      ar << name;
      ar << description;
      ar << nextIndex;
      tags.Serialize(ar);
      groups.Serialize(ar);
      }
   else
      {
      UINT version = ar.m_nObjectSchema;
      ar >> name;
      ar >> description;                     
      ar >> nextIndex;
      tags.Serialize(ar);
      groups.Serialize(ar);

      // set parent pointers
      POSITION pos = groups.GetHeadPosition();
      while( pos )
         {
         OTagGroup* pGroup = groups.GetNext( pos );
         pGroup->parent = this;
         }
      }
   }

void ODevice::Serialize(CArchive& ar)
   {
   if (ar.IsStoring())
      {
      ar << backup;
      ar << simulate;
      //ar.SerializeClass(RUNTIME_CLASS(ODeviceParams));
      //primary.Serialize(ar);
      }
   else
      {
      UINT version = ar.m_nObjectSchema;
      ar >> backup;
      ar >> simulate;
      //ar.SerializeClass(RUNTIME_CLASS(ODeviceParams));
      //primary.Serialize(ar);
      }
   ar.SerializeClass(RUNTIME_CLASS(OTagGroup));
   OTagGroup::Serialize(ar);
   }

//void OPort::Serialize(CArchive& ar)
//   {
//   if (ar.IsStoring())
//      {
//      ar << name;
//      ar << enable;
//      ar << baud;
//      ar << dataBits;
//      ar << parity;
//      ar << (WORD)flow;
//      ar << stop;
//      }
//   else
//      {
//      UINT version = ar.m_nObjectSchema;
//      WORD temp;
//      ar >> name;
//      ar >> enable;
//      ar >> baud;
//      ar >> dataBits;
//      ar >> parity;
//      ar >> temp; flow = (FLOW)temp;
//      ar >> stop;
//      }
//   }
//
