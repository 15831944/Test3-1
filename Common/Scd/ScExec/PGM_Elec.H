//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PGM_ELEC_H
#define  __PGM_ELEC_H

#ifndef __VECTORS_H
#include "vectors.h"
#endif
#ifndef __PGM_E_H
#include "pgm_e.h"
#endif

#ifdef __PGM_ELEC_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

_FWDDEF(GCETerminalBase)
class DllImportExport GCETerminalBase
  {
  friend class GCETerminal;
  public:
    GCETerminalBase(pGCClassVar pClassVar);
  protected:

  public:
    CString       m_sName;
    CDComplex     m_V;
    //CDComplex     m_I;
    long         &m_lId;

  public:
    GCETerminalBase();
    ~GCETerminalBase();

    void SetTerminal(CETerminal * p) { m_lId=(long)p;}
    void ClrTerminal()               { m_lId=0;}
    CETerminal * Terminal()          { return (CETerminal*)m_lId/*m_pTerm*/;}
  private:
  };

//===========================================================================

class GCETerminal : public GCClassVar
  {
  public:
    GCETerminal(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
//    virtual void OnRestore(pvoid pOldSubClass, pvoid pNewSubClass);
  };

//===========================================================================

_FWDDEF(GCTermStripBase)
class DllImportExport GCTermStripBase
  {
  friend class GCTermStrip;
  public:
    GCTermStripBase(pGCClassVar pClassVar);
  protected:

  public:
    CString       m_sName;
    long          &m_lId;

  public:
    GCTermStripBase();
    ~GCTermStripBase();

    void SetTermStrip(CETermStrip * p) { m_lId=(long)p;}
    void ClrTermStrip()               { m_lId=0;}
    CETermStrip * TermStrip()          { return (CETermStrip*)m_lId/*m_pTerm*/;}

  //    void SetTSIndex(long i)   { m_lId = i; }
  //    long TSIndex()            { return m_lIndex; };
  
  private:
  };

//===========================================================================

class GCTermStrip : public GCClassVar
  {
  public:
    GCTermStrip(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB, pvoid pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB, pvoid pSubClass);
//    virtual void OnRestore(pvoid pOldSubClass, pvoid pNewSubClass);
  };

//===========================================================================
#undef DllImportExport

#endif
