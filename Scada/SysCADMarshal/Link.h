// Link.h: interface for the CLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Link_H__DD2C690A_FD4F_47FC_BFAD_F811DFB00005__INCLUDED_)
#define AFX_Link_H__DD2C690A_FD4F_47FC_BFAD_F811DFB00005__INCLUDED_

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

#pragma pack (push, 1)
struct CLinkState
  {
  unsigned short  m_nError;
  unsigned short  m_bSimTagValid:1,
                  m_bSimTagIsParm:1,
                  m_bHold:1;
  CLinkState()
    {
    m_nError=0;
    m_bSimTagValid=0;
    m_bSimTagIsParm=0;
    m_bHold=0;
    }
  };
#pragma pack (pop)


class CLinkValue : public CFullValue
  {
  public:
    CLinkValue(WORD VType);
    CLinkValue(const CLinkValue & V);
    CLinkValue & operator =(const CLinkValue & V);

    long        m_lLink;
    DWORD       m_dwTransactionID;
    long        m_lValueChanges;   // Number of times the value has changed
    long        m_lValuesIgnored;  // Number of times the value has 'changed' but been ignored because values are the 'same'
    long        m_lCfgSequence;    // Number of times the Configuration has changed

    CLinkState  m_State;
    long      & StateLong() { return *(long*)&m_State; };

  DEFINE_SPARES(CLinkValue);
  };

// -----------------------------------------------------------------------

class CLinkCondBlk 
  {
  public:
    CLinkCondBlk();

    bool         m_On;
    bool         m_BaseValueValid;
    bool         m_BaseChg;
    long         m_ChgCount;
    double       m_BaseValue;
  };

class CSlot;
class CLinkCfg : public CLinkValue, public CSlotValueSet
  {
  public:
	  //CLinkCfg();
    CLinkCfg(LPCSTR Tag, LPCSTR ConnectTag, LPCSTR CnvTxt, LPCSTR Values, LPCSTR Desc, WORD VType, bool Get, bool Set);
	  ~CLinkCfg();
    CLinkCfg(const CLinkCfg & V);
    CLinkCfg & operator =(const CLinkCfg & V);
    
    bool        InFilter(long DevSelect, CSelection &Select);

    void SetError(USHORT Error, bool LogIt=true);
    void SetError(USHORT Error, LPCSTR Fmt, ...);
    void SetError(USHORT Error, HRESULT hr, LPCSTR Fmt, ...);

    CFullValue & FullValue() { return *((CFullValue*)this); };
    CLinkValue & LinkValue() { return *((CLinkValue*)this); };

    CString     m_sTag;
    CString     m_sSimTag;
    //CString     m_sValues;
    CString     m_sCnvTxt;
    CString     m_sDesc;
    bool        m_bSet;
    bool        m_bGet;
    //bool        m_bSimIsParm;
    //bool        m_bSimTagValid;
 
    bool        m_bInUse;          // true if Used in Current Config
    bool        m_bInFilter;
    CLinkCondBlk m_CondBlk;


    CSlot     * m_pSlot;
    
  DEFINE_SPARES(CLinkCfg);
  };

// -----------------------------------------------------------------------

class CLink : public CLinkCfg
  {
  public:
	  CLink(CSlot * pSlot, LPCSTR Tag, LPCSTR SimTag, LPCSTR CnvTxt, LPCSTR Values, LPCSTR Desc, WORD VType, bool Get, bool Set);
	  virtual ~CLink();
    
    bool        m_bVisible;
    bool        m_bTemp;
    bool        m_bMustSend;
    bool        m_bRangeInited;

    DWORD       m_dwLastWriteTick;
    //double      m_dFixedRange;
    double      m_dMinValue;
    double      m_dMaxValue;
    double      m_dLastAppliedValue;

    DWORD       m_ChgFlags;

    // To catch Recursion
    DWORD       m_dwFlushChangeCalls;
    DWORD       m_dwChangesInCall;

    //CSlot     * m_pSlot;

    CSlotConnArray  m_ReflectedGets;      //linked list of actual connects some reflected from 

    LPCSTR      TypeToString() const { return ::TypeToString(m_vValue.vt);};

    void        RemoveConnects();
    void        InitialiseConnects();
    long        ProcessConnects();
  
    bool        SetValue(CChangeItem * pRqst);
    void        SendDisplayValue();
    void        SendStateValue();
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
   
    DEFINE_SPARES(CLink);
  };

// =======================================================================
//
//
//
// =======================================================================

class CLink;
class CLinkValueArray : public CArray <CLinkValue*, CLinkValue*> {};
class CLinkCfgArray : public CArray <CLinkCfg*, CLinkCfg*> {};
class CLinkArray : public CArray <CLink*, CLink*> {};
class CLinkMap : public CMap <LPCSTR, LPCSTR, long, long> {};

// =======================================================================
//
//
//
// =======================================================================

#endif // !defined(AFX_Link_H__DD2C690A_FD4F_47FC_BFAD_F811DFB00005__INCLUDED_)
