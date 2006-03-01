// CodeBlocks.h: interface for the CCdBlks class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CODEBLOCKS_H__25833459_F055_4295_BFD5_2F6472038D9F__INCLUDED_)
#define AFX_CODEBLOCKS_H__25833459_F055_4295_BFD5_2F6472038D9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// =======================================================================
//
//
//
// =======================================================================

class CSlotConnect;

// =======================================================================
//
//
//
// =======================================================================

class CAddCdBlk 
  {
  public :
    CString    m_sTag;
    CString    m_sSchedule;              
    CString    m_sCdBlk;              
    CString    m_sDescription;              
  };

// =======================================================================
//
//
//
// =======================================================================

#pragma pack (push, 1)
struct CCdBlkState
  {
  unsigned short m_nError;
  unsigned short m_bHold:1;
  CCdBlkState()
    {
    m_nError=0;
    m_bHold=0;
    }
  };
#pragma pack (pop)

class CCdBlkCfg
  {
  public:
    CCdBlkCfg();
    ~CCdBlkCfg();
    CCdBlkCfg(const CCdBlkCfg & V);
    CCdBlkCfg & operator =(const CCdBlkCfg & V);
    
    long        m_lCdBlk;      
    CString     m_sTag;        
    CString     m_sSchedule;              
    CString     m_sCdBlk;              
    CString     m_sDescription; 
    long        m_lCfgSequence;
                              
    CCdBlkState m_State;
    long      & StateLong() { return *(long*)&m_State; };
  

    void SetError(USHORT Error);
    void SetError(USHORT Error, LPCSTR Fmt, ...);
    void SetError(USHORT Error, HRESULT hr, LPCSTR Fmt, ...);
  };

// =======================================================================
//
//
//
// =======================================================================

class CCdBlk : public CCdBlkCfg
  {
  public:
    CCdBlk();
    ~CCdBlk();
    
    bool       Parse(CAddCdBlk * pAdd, LPCSTR CdFile);
    
    bool       m_bValid;
    
    GCInsMngr  m_Code;    
    
    CArray <CSlotConnect*, CSlotConnect*> m_ReflectedGets; // Array of Connects which get from here

    void   RemoveConnects();
    void   InitialiseConnects();
  
  };

// =======================================================================
//
//
//
// =======================================================================

class CCdBlk;
class CCdBlkArray : public CArray <CCdBlk*, CCdBlk*> {};
class CCdBlkCfgArray : public CArray <CCdBlkCfg*, CCdBlkCfg*> {};
class CCdBlkMap : public CMap <LPCSTR, LPCSTR, long, long> {};

// =======================================================================
//
//
//
// =======================================================================

#endif // !defined(AFX_CODEBLOCKS_H__25833459_F055_4295_BFD5_2F6472038D9F__INCLUDED_)
