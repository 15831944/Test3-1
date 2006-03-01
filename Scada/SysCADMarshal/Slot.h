// Slot.h: interface for the XSlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLOT_H__45CCC708_5BB4_4E0D_A45C_646FE33D9374__INCLUDED_)
#define AFX_SLOT_H__45CCC708_5BB4_4E0D_A45C_646FE33D9374__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "codeblock.h"
#include "slotcommon.h"
#include "slotconnect.h"

// =======================================================================
//
//
//
// =======================================================================

enum eSlotCfgEdits {SlotCfgNull, SlotCfgUpdate, SlotCfgAdd, SlotCfgDelete} ;

class CSlotCfgStrings : public CSlotValueSet
  {
  public:
    CSlotCfgStrings();
    ~CSlotCfgStrings();
    CSlotCfgStrings(const CSlotCfgStrings & V);
    CSlotCfgStrings & operator =(const CSlotCfgStrings & V);

    eSlotCfgEdits m_eDoWhat;

    long        m_lNo;
    CString     m_sTag;
    CString     m_sConnects;
    CString     m_sType;
    CString     m_sDriver;
    CString     m_sModifier;
    CString     m_sRange;
    CString     m_sLinkValues;
    CString     m_sDesc;
  };

// -----------------------------------------------------------------------

class CSlotSpan 
  {
  public:
    double    m_dSpan, m_dOffset;
    bool      m_bValid;

    CSlotSpan () { m_bValid=false; m_dSpan=m_dOffset=0; };
  };

class CSlotRange
  {
  public:
    double    m_dMin, m_dMax;
    bool      m_bValid;

    CSlotRange() { m_bValid=false; m_dMin=m_dMax=0; };
    double Range() { return m_dMax-m_dMin; };
  };

class CSlotClamp
  {
  public:
    double    m_dLo, m_dHi;
    bool      m_bValid;

    CSlotClamp() { m_bValid=false; m_dLo=m_dHi=0; };
  };

// =======================================================================
//
//
//
// =======================================================================

#pragma pack (push, 1)
struct CSlotState
  {
  USHORT m_nError;
  USHORT m_bHold:1;
  CSlotState()
    {
    m_nError=0;
    m_bHold=0;
    }
  };
#pragma pack (pop)

class CSlotValue : public CFullValue
  {
  public:
    CSlotValue();
    CSlotValue(const CSlotValue & V);
    CSlotValue & operator =(const CSlotValue & V);

    long        m_lSlot;
    DWORD       m_dwTransactionID;
    long        m_lValueChanges;   // Number of times the value has changed
    long        m_lValuesIgnored;  // Number of times the value has 'changed' but been ignored because values are the 'same'
    long        m_lCfgSequence;    // Number of times the Configuration has changed

    CSlotState  m_State;
    long      & StateLong() { return *(long*)&m_State; };

  DEFINE_SPARES(CSlotValue);
  };

// -----------------------------------------------------------------------
class CLinkCfg;
class CSlotCfg : public CSlotValue, public CSlotCfgStrings
  {
  public:
	  CSlotCfg();
	  ~CSlotCfg();
    CSlotCfg(const CSlotCfg & V);
    CSlotCfg & operator =(const CSlotCfg & V);
    
    //CString     m_sTag;
    //CString     m_sDesc;
    WORD        m_wNativeType;
    bool        m_bRead;
    bool        m_bWrite;
    bool        m_bLocal;
    bool        m_bConnected;

    //CString     m_sConnects;     //tag connection ( for display and for reconnection)
 
    OPCHANDLE   m_hServer;
    CString     m_sOPCClient;
    CString     m_sOPCTag;
    long        m_lDevice;

    bool        m_bInUse;          // true if Used in Current Config
    bool        m_bInFilter;

    CLinkCfg  * m_pLink;

    void SetError(USHORT Error, bool LogIt=true);
    void SetError(USHORT Error, LPCSTR Fmt, ...);
    void SetError(USHORT Error, HRESULT hr, LPCSTR Fmt, ...);


    bool        InFilter(long DevSelect, CSelection &Select);

    CFullValue & FullValue() { return *((CFullValue*)this); };
    CSlotValue & SlotValue() { return *((CSlotValue*)this); };

  DEFINE_SPARES(CSlotCfg);
  };

// -----------------------------------------------------------------------

class CSlot : public CSlotCfg
  {
  public:

    CSlot();
    ~CSlot();


    CSlotSpan   m_Span;
    CSlotRange  m_Range;
    CSlotClamp  m_Clamp;
  
    // Connection

    //HRESULT     m_hrResult;

    DWORD       m_ChgFlags;
    bool        m_bVisible;
    bool        m_bTemp;
    bool        m_bSltDbgOn;

    // To catch Recursion
    DWORD       m_dwFlushChangeCalls;
    DWORD       m_dwChangesInCall;


    // Connects
    CSlotConnArray  m_SetConnects;   // actual Set connects
    CSlotConnArray  m_ReflectedGets;
    CSlotConnect   *m_pGetConnect;   // the Get Connect

    // ...........
    bool        Parse(CSlotCfgStrings * pAdd);
    bool        ParseType(LPCSTR S);
    bool        ParseDriver(LPCSTR S);
    bool        ParseRange(LPCSTR S);
    bool        ParseModifier(LPCSTR S);
    bool        ParseConnects(LPCSTR S);
    bool        ParseConnOperator(CSlotConnect *pC, LPSTR pMod);
    CSlotConnect * ParseConnFunction(LPSTR pConn);
    CSlotConnect * AddGetConnect(LPSTR pTag, bool Inv);
    CSlotConnect * AddSetConnect(LPSTR pTag, bool Inv, DWORD DelayTime, DWORD DelayTime2, bool UseDelay2);

    bool        MustReConnect(CSlot * Other);
    bool        TransferConfiguration(CSlot * Other);

    void        ApplySpanInComing(VARTYPE SlotType, COleVariant &V);
    void        ApplySpanOutGoing(VARTYPE SlotType, COleVariant &V);
    
    bool        SetValue(CChangeItem * pRqst);
    void        RemoveConnects();
    void        InitialiseConnects();
    void        ConnectConnects();
    void        CorrectConnects(CLongArray & NewNos);
    long        ProcessConnects();
    void        SendDisplayValue();
    void        SendStateValue();
    bool        WriteCurrentValue2Device();
    //void        SetRqdValue(LPCTSTR ValueID);
    void        Save(CProfINIFile & SavePF);
    void        Load(CProfINIFile & SavePF);

    bool        SetHold(bool On)
      {
      if (m_State.m_bHold!=On)
        {
        m_State.m_bHold=On;
        m_ChgFlags|=IOMChgFlag_Hold;
        SendStateValue();
        return true;
        }
      return false;
      }
 
    CFullValue & FullValue() { return *((CFullValue*)this); };
    CSlotValue & SlotValue() { return *((CSlotValue*)this); };
    CSlotCfg   & Cfg()       { return *((CSlotCfg*)this); };

    DEFINE_SPARES(CSlot);
  };

// =======================================================================
//
//
//
// =======================================================================

class CSlot;
class CSlotValueArray : public CArray <CSlotValue*, CSlotValue*> {};
class CSlotCfgArray : public CArray <CSlotCfg*, CSlotCfg*> {};
class CSlotArray : public CArray <CSlot*, CSlot*> {};
class CSlotMap : public CMap <LPCSTR, LPCSTR, long, long> {};
class CSlotCfgStringsArray : public CArray <CSlotCfgStrings*, CSlotCfgStrings*> {};

// =======================================================================
//
//
//
// =======================================================================

#endif // !defined(AFX_SLOT_H__45CCC708_5BB4_4E0D_A45C_646FE33D9374__INCLUDED_)
