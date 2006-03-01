//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Modbus.h
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

#ifndef __MODBUS_H__             // Only Include Once
#define __MODBUS_H__


class OAnalogData;
class OTag;
class OModbusTag;
class OMessage;
class OTagGroup;
class ODevice;
//class OPort;

typedef CTypedPtrList<CObList, OModbusTag*>  OTagList;
typedef CTypedPtrList<CObList, OTagGroup*>   OGroupList;
//typedef CTypedPtrList<CObList, OPort*>       OPortList;

//*******************************************************************
// Use to Initialize a critical section and automatically delete it
// when the object goes out of scope (including exceptions, etc).
class CSObject
{
public:
   CSObject(CRITICAL_SECTION* lock)
      : pLock(lock) {InitializeCriticalSection(pLock);}
   ~CSObject()
      {DeleteCriticalSection(pLock);}
private:
   CRITICAL_SECTION* pLock;
};

//*******************************************************************
// Use to enter a critical section and automatically leave it
// when the object goes out of scope (including exceptions, etc).
class CSLock
{
public:
   CSLock(CRITICAL_SECTION* lock)
      : pLock(lock) {EnterCriticalSection(pLock);}
   ~CSLock()
      {LeaveCriticalSection(pLock);}
private:
   CRITICAL_SECTION* pLock;
};

//*******************************************************************
class OAnalogData : public CObject
{
   DECLARE_SERIAL(OAnalogData)
public:
   OAnalogData();
   OAnalogData(const OAnalogData&);
   OAnalogData& operator=(const OAnalogData&);
   BOOL         operator==(const OAnalogData&) const;
   BOOL         Scale(VARIANT& value);

   double      InitValue;
   double      MinEngRange;
   double      MaxEngRange;
   double      MinRawRange;
   double      MaxRawRange;
   CString     EngUnits;
   double      Deadband;
   double      LogDeadband;

   enum {LOLO=1, LOW=2, HIGH=4, HIHI=8, MINOR_DEV=16,
         MAJOR_DEV=32, RATE=64, ANALOG_SQRT=128};
   WORD        Options;

   virtual void  Serialize(CArchive& ar);
private:
   void        Assign(const OAnalogData&);
};

//*******************************************************************
class OTag : public CObject
{
   DECLARE_SERIAL(OTag)
public:
                  OTag();
                  OTag(const OTag&);
   virtual        ~OTag();
   OTag&          operator=(const OTag&);
   BOOL           SetValue( const VARIANT& );
   virtual void AddRef();
   virtual void Release();
   BOOL  InUse() { return refCount>0 ? TRUE : FALSE; }

//private:
   static long    s_idno;
   long           idno;
   CString        name;
   CString        description;
   VARTYPE        nativeType;
   COleVariant    value;
   BOOL           active;     // FALSE when manually overriding
   FILETIME       timestamp;
   WORD           quality;
   BOOL           enableProcessing;
   OAnalogData*   pProcess;
   OTagGroup*     pGroup;    // When writing, tag needs to find its device

protected:
   LONG           refCount;   // reference counting
   virtual void   Serialize(CArchive& ar);
};

//*******************************************************************
class OModbusTag : public OTag
{
   DECLARE_SERIAL(OModbusTag)
public:
                  OModbusTag();
                  OModbusTag(const OModbusTag&);
   virtual        ~OModbusTag() {;}
   OModbusTag&    operator=(const OModbusTag&);
//   CString        LocationString() const;
   LPTSTR         TypeString() const;
//
////private:
//   enum TYPE {INPUT_COIL, OUTPUT_COIL, INPUT_REGISTER, OUTPUT_REGISTER};
//   TYPE           range;
//   WORD           offset; // zero based offset (user sees 1 based)
   //enum TYPE {INPUT_BOOL, OUTPUT_BOOL, INPUT_VALUE, OUTPUT_VALUE};
   //TYPE           range;
   BOOL           m_IsOutput;
   enum SIMULATION { SIN, RAMP, RANDOM };
   WORD           simType;

   virtual void   Serialize(CArchive& ar);
};

//*******************************************************************
// This tag is generated on the fly for a client
class OTempTag : public OModbusTag
{
public:
   virtual void Release();
};

//*******************************************************************
//class ODeviceParams : public CObject
//{
//   DECLARE_SERIAL(ODeviceParams)
//public:
//      ODeviceParams()
//         :pPort(NULL), address(1), interPollDelay(0), timeout(1000),
//         format(FORMAT_BINARY), wordSwap( FALSE ), portName(_T("COM1"))
//         {
//            lastMessage.dwLowDateTime=lastMessage.dwHighDateTime=0;
//         }
//
//   OPort*   pPort;
//   CString  portName;         // ex. "COM2"
//   BYTE     address;          // legal 1-247
//   DWORD    timeout;          // in mSec
//   DWORD    interPollDelay;   // in mSec
//   enum OFORMAT {FORMAT_BINARY, FORMAT_BCD };
//   OFORMAT  format;
//   WORD     wordSwap;
//   FILETIME lastMessage;
//
//   virtual void   Serialize(CArchive& ar);
//};

//*******************************************************************
class OTagGroup : public CObject
{
   DECLARE_SERIAL(OTagGroup)
public:
            OTagGroup() : name(_T("Group")), nextIndex(1), parent(NULL)
               {
               hItem = 0;
               refCount=0;
               }
   virtual  ~OTagGroup()
               {
               while( !groups.IsEmpty() )
                  delete groups.RemoveTail();
               while( !tags.IsEmpty() )
                  delete tags.RemoveTail();
               ASSERT( refCount == 0 );
               }
   BOOL  AddTag( OModbusTag* pTag );
   void  DeleteTag(OModbusTag* pTag)
            {
            ASSERT_KINDOF( OModbusTag, pTag );

            POSITION pos = tags.Find( pTag );
            ASSERT( pos );
            tags.RemoveAt( pos );
            delete pTag;
            }
   virtual void AddRef()   {InterlockedIncrement(&refCount); parent->AddRef();}
   virtual void Release()  {InterlockedDecrement(&refCount); parent->Release();}
   BOOL    InUse() { return refCount>0 ? TRUE : FALSE; }
   OTag*   FindTag( const CString& target);
   virtual ODevice* GetDevice() {return parent->GetDevice();}
   virtual CString  GetPathName() { if(parent)
                                        return CString(parent->GetPathName() + name + _T(".")  );
                                    else
                                        return _T("");}
   virtual CString FindTagName(CString name);
   void     SetTagDevices();

//private:

   CString        name;
   CString        description;
   OTagList       tags;
   OGroupList     groups;
   OTagGroup*     parent;
   WORD           nextIndex;  // to be used naming tags
   HTREEITEM      hItem;      // item in the tree control
   LONG           refCount;   // reference counting

   virtual void   Serialize(CArchive& ar);
};

//*******************************************************************
class ODevice : public OTagGroup
{
   DECLARE_SERIAL(ODevice)
public:
            ODevice()
               : OTagGroup(), //primary(), pCurrent(&primary),
                 backup( FALSE ), simulate( FALSE)
              {                                       
               name = _T("Device");
               //m_Messages = m_Rcvd = m_Rcvd_sec = 0;
               //m_Sent = m_Sent_sec = m_Timeouts = 0;
               }
   virtual   ~ODevice() {;}
   virtual   ODevice* GetDevice() {return this;}
   virtual   CString  GetPathName() { return CString(name + _T(".")); }
   virtual void AddRef()   {InterlockedIncrement(&refCount);}
   virtual void Release()  {InterlockedDecrement(&refCount);}
   BOOL      IdDevice();
   BOOL      UpdateTags( OTagList& tags, DWORD mSec );
   //OMessage* CreateReadPacket( OModbusTag* tag, WORD number = 1 );
   //OMessage* CreateWritePacket( OModbusTag* tag );
   BOOL      WriteTag( OModbusTag* tag );

//private:
   BOOL           backup;
   //ODeviceParams  primary;
   //ODeviceParams* pCurrent;
   BOOL           simulate;

   //int      m_Messages;
   //int      m_Rcvd;
   //int      m_Rcvd_sec;
   //int      m_Sent;
   //int      m_Sent_sec;
   //int      m_Timeouts;

   virtual void   Serialize(CArchive& ar);
};

//*******************************************************************
//class OPort : public CObject
//{
//   DECLARE_SERIAL(OPort)
//public:
//   OPort();
//   ~OPort();
//
//   BOOL             Initialize();
//   OMessage*        Message( OMessage* msg, DWORD timeout );
//
////private:
//   CString          name;        // used as file name to open port
//   BOOL             enable;
//   DWORD            baud;        // the value, eg 9600
//   WORD             dataBits;    // 7 or 8
//   WORD             parity;      // EVENPARITY, etc.
//   enum FLOW { HARDWARE, XON_XOFF, NONE };
//   FLOW             flow;
//   WORD             stop;        // defined constants
//   CRITICAL_SECTION lock;
//   HANDLE           hPort;        // file handle for port
//   HANDLE           hLog;         // log file handle
//   BOOL             reInitialize; // if settings have changed
//   COMMTIMEOUTS     cts;
//
//// statistics are read-only
//   int      m_Timeouts;
//   int      m_Sent_sec;
//   int      m_Rcvd_sec;
//   int      m_Sent;
//   int      m_Rcvd;
//   int      m_Parity;
//   int      m_Overruns;
//   int      m_Messages;
//   int      m_Framing;
//
//   virtual void   Serialize(CArchive& ar);
//};

#endif