//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Message.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  A message base class and Ascii and RTU subclasses.
//                Messages are built by a device, and sent by a port.
//                Ascii and RTU simply pack the data differently,
//                as well as calculating the CRC, etc.
//
//**************************************************************************

#ifndef __MESSAGE_H__               // Only Include Once
#define __MESSAGE_H__

//enum COMM_STATUS { EMPTY, COMM_NO_ERROR, TIMEOUT, BAD_MESSAGE, COMMAND_FAILURE };
////**************************************************************************
//class OMessage
//{
//enum { BUFFER_SIZE = 500 };
//public:
//   OMessage();
//   ~OMessage();
//   void Clear();
//   virtual COMM_STATUS IsOk()=0;
//   virtual BYTE GetAddress()=0;
//   virtual BYTE GetFunction()=0;
//   virtual BOOL GetBit( int offset )=0;    // offset of bit from start of group
//   virtual WORD GetWord( int offset )=0;   // word offset in group
//
//   virtual void Add( BYTE byte )=0;
//   virtual void Finish()=0;
//   virtual BOOL MsgDone()=0;
//
//   BYTE     buffer[BUFFER_SIZE];
//   BYTE*    data;
//   WORD     dataLength;
//   WORD     length;
//   FILETIME timestamp;
//   COMM_STATUS  ok;
//};

//**************************************************************************
//class OAsciiMessage : public OMessage
//{
//public:
//   OAsciiMessage();
//   ~OAsciiMessage();
//   virtual COMM_STATUS IsOk();
//   BYTE           GetAddress();
//   BYTE           GetFunction();
//   BOOL           GetBit( int offset );    // offset of bit from start of group
//   WORD           GetWord( int offset );   // word offset in group
//
//   void           Add( BYTE byte );
//   void           Finish();
//   BOOL           MsgDone();
//   void           ConvertData();
//private:
//   BYTE           lrc;
//};

//**************************************************************************
//class ORtuMessage : public OMessage
//{
//public:
//   ORtuMessage();
//   ~ORtuMessage();
//   virtual COMM_STATUS IsOk();
//   BYTE           GetAddress();
//   BYTE           GetFunction();
//   BOOL           GetBit( int offset );    // offset of bit from start of group
//   WORD           GetWord( int offset );   // word offset in group
//
//   void           Add( BYTE byte );
//   void           Finish();
//   BOOL           MsgDone();
//   void           ConvertData();
//};


#endif