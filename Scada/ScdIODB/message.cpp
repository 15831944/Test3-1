//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Message.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Ascii and RTU message classes.
//
//
//**************************************************************************

#include "stdafx.h"
#include "message.h"

const BYTE CR = 0x0d;
const BYTE LF = 0x0a;

//*******************************************************************
/*
OMessage::OMessage()
: data(NULL)
{
   Clear();
}

OMessage::~OMessage()
{
   delete [] data;
}

void OMessage::Clear()
   {
   length = dataLength = 0;
   memset( buffer, 0xcd, sizeof( buffer ) );
   ok = EMPTY;
   timestamp.dwLowDateTime = timestamp.dwHighDateTime = 0;
   delete [] data;
   data = NULL;
   }

//*******************************************************************
OAsciiMessage::OAsciiMessage()
{
   lrc = 0;
   buffer[length++]=':';
}

OAsciiMessage::~OAsciiMessage()
{
}

// Build a message by adding bytes sequentially
void OAsciiMessage::Add( BYTE byte )
   {
   char buf[18];

   lrc += byte;
   itoa( byte, buf, 16 );  // convert value to 2 char hex number
   _strupr( buf );         // make it upper case!

   if( strlen(buf) < 2 )   // always insert 2 chars
      {
      buffer[length++] = '0';
      buffer[length++] = buf[0];
      }
   else
      {
      buffer[length++] = buf[0];
      buffer[length++] = buf[1];
      }
   }

//*******************************************************************
void OAsciiMessage::Finish()
   {
   // insert the LRC
   Add( -lrc );

   // and the termination
   buffer[length++] = CR;
   buffer[length++] = LF;
   ok = COMM_NO_ERROR;
   }

//*******************************************************************
BOOL OAsciiMessage::MsgDone()
   {
   return ( length > 9 && buffer[length-1] == LF );
   }

//*******************************************************************
// check the validity of a message
COMM_STATUS OAsciiMessage::IsOk()
   {
   if( ok != COMM_NO_ERROR )
      return ok;
   if( length < 9 )  // minimum is <: addr func lrc crlf>
      return (ok = BAD_MESSAGE);
   if( buffer[0] != ':'
    || buffer[length-2] != CR
    || buffer[length-1] != LF )
      return (ok = BAD_MESSAGE);
   // compute the LRC and compare with the message
   int sent = 0;
   char lrcString[3];
   lrcString[0] = buffer[length-4];
   lrcString[1] = buffer[length-3];
   lrcString[2] = 0;
   sscanf( lrcString, "%2x", &sent );

   // check error bit in function
   if( GetFunction() & 0x80 )
      return (ok = COMMAND_FAILURE);

   return COMM_NO_ERROR;
   }

//*******************************************************************
BYTE OAsciiMessage::GetAddress()
   {
   int number = 0;
   char string[3];
   string[0] = buffer[1];
   string[1] = buffer[2];
   string[2] = 0;
   sscanf( string, "%2x", &number );

   return (BYTE)number;
   }

//*******************************************************************
BYTE OAsciiMessage::GetFunction()
   {
   int number = 0;
   char string[3] = {0,0,0};
   string[0] = buffer[3];
   string[1] = buffer[4];
   sscanf( string, "%2x", &number );

   return (BYTE)number;
   }

//*******************************************************************
BOOL OAsciiMessage::GetBit( int offset )
   {
   if( !data )
      ConvertData();

   int byte = offset / sizeof(BYTE);
   int bit = offset % sizeof(BYTE);
   ASSERT( byte <= dataLength );   // is data byte within this msg?

   BOOL discrete = data[byte] & (1<<bit);
   return (discrete ? 0xFFFF : 0 ); // variant bool values, not TRUE/FALSE
   }

//*******************************************************************
WORD OAsciiMessage::GetWord( int offset )
   {
   if( !data )
      ConvertData();
   ASSERT( offset <= dataLength );

   WORD value = MAKEWORD(data[offset/2+1],data[offset/2]);
   return value;
   }

//*******************************************************************
void OAsciiMessage::ConvertData()
   {
   // find number of ascii bytes in the msg
   int number = 0;
   char string[3] = {0,0,0};
   string[0] = buffer[5];
   string[1] = buffer[6];
   sscanf( string, "%2x", &number );
   dataLength = number / 2;   // now data bytes
   ASSERT( dataLength >= 0 && dataLength < 254 );

   data = new BYTE[dataLength];

   int index=0;
   int msgindex = 7;
   while( index < dataLength )
      {
      int value;
      string[0] = buffer[msgindex++];
      string[1] = buffer[msgindex++];
      sscanf( string, "%2x", &value );
      data[index++] = (BYTE)value;
      }
   }

static BYTE CRCHI[] = {
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
   0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
   0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
   0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
   0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
   0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
   0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
   0x40 };
static BYTE CRCLO[] = {
   0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4,
   0x04, 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e, 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09,
   0x08, 0xc8, 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
   0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
   0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
   0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
   0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
   0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
   0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
   0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
   0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
   0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
   0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
   0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
   0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
   0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
   0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
   0x40 };

//*******************************************************************
ORtuMessage::ORtuMessage()
   {
   }

ORtuMessage::~ORtuMessage()
   {
   }

// Build a message by adding bytes sequentially
void ORtuMessage::Add( BYTE byte )
   {
   buffer[length++] = byte;
   }

//*******************************************************************
void ORtuMessage::Finish()
   {
   // compute the CRC and insert it
   BYTE hi = 0xFF;
   BYTE lo = 0xFF;
   for( int count=0; count < length; count++)
      {
      UINT index = hi ^ buffer[count];
      hi = lo ^ CRCHI[index];
      lo = CRCLO[index];
      }

   buffer[length++] = hi;
   buffer[length++] = lo;
   ok = COMM_NO_ERROR;
   }

//*******************************************************************
BOOL ORtuMessage::MsgDone()
   {
   if( length < 4 )  // minimum length
      return FALSE;

   if( buffer[1] & 0x80 )  // error bit in function code
      if( length == 5 )
         return TRUE;
      else
         return FALSE;

   switch( buffer[1] & 0x7F ) // regular function code
      {
      case 1:
      case 2:
      case 3:
      case 4:
      case 12:
      case 17:
      case 20:
      case 21:
         if( length == buffer[2]+5 )
            return TRUE;
         break;

      case 5:
      case 6:
      case 11:
      case 15:
      case 16:
         if( length == 8 )
            return TRUE;
         break;
      case 7:
         if( length == 5 )
            return TRUE;
         break;
      case 22:
         if( length == 10 )
            return TRUE;
         break;
      }
   return FALSE;
   }

//*******************************************************************
// check the validity of a message
COMM_STATUS ORtuMessage::IsOk()
   {
   if( ok != COMM_NO_ERROR )
      return ok;
   if( length < 4 )  // minimum is < addr func crc>
      return BAD_MESSAGE;
   // compute the CRC and compare with the message
   BYTE hi = 0xFF;
   BYTE lo = 0xFF;
   for( int count=0; count < length-2; count++)
      {
      UINT index = hi ^ buffer[count];
      hi = lo ^ CRCHI[index];
      lo = CRCLO[index];
      }
   if( buffer[length-2] != hi || buffer[length-1] != lo )
      return BAD_MESSAGE;

   // check error bit in function
   if( GetFunction() & 0x80 )
      return COMMAND_FAILURE;

   return COMM_NO_ERROR;
   }

//*******************************************************************
BYTE ORtuMessage::GetAddress()
   {
   ASSERT( buffer[0] >= 1 && buffer[0] <= 247 );
   return buffer[0];
   }

//*******************************************************************
BYTE ORtuMessage::GetFunction()
   {
   return buffer[1];
   }

//*******************************************************************
BOOL ORtuMessage::GetBit( int offset )
   {
   if( !data )
      ConvertData();

   int byte = offset / 8;  // size of BYTE
   int bit = offset % 8;
   ASSERT( byte <= dataLength );   // is data byte within this msg?

   BOOL discrete = data[byte] & (1<<bit);
   return (discrete ? 0xFFFF : 0 ); // variant bool values, not TRUE/FALSE
   }

//*******************************************************************
WORD ORtuMessage::GetWord( int offset )
   {
   if( !data )
      ConvertData();
   ASSERT( offset*2+1 < dataLength );

   WORD value = MAKEWORD(data[offset*2+1], data[offset*2]);
   return value;
   }

//*******************************************************************
void ORtuMessage::ConvertData()
   {
   // find number of ascii bytes in the msg
   dataLength = buffer[2];
   ASSERT( dataLength >= 0 && dataLength < 254 );

   data = new BYTE[dataLength];

   int index=0;
   int msgindex = 3; // data starts here
   while( index < dataLength )
      {
      data[index++] = buffer[msgindex];
      msgindex++;
      }
   }
*/