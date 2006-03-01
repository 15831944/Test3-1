//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __MATHLIBTO_H
#define  __MATHLIBTO_H

#if defined(__MATHLIBTO_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#ifndef __AFXTEMPL_H__
#include <afxtempl.h> // "xafxtempl.h"  
#endif

//===========================================================================
//
//  
//
//===========================================================================

class DllImportExport DualDbl
  {
  public:
  //protected:
    double         dVal;
    double         dOvr;
  public:
    DualDbl()      { dVal=0.0; dOvr=dNAN; };

    double         operator() ()        { return Valid(dOvr) ? dOvr : HasNANFlag(dOvr, NF_Star) ? dNAN : dVal; };
    DualDbl      & operator=(double v)  { dVal=v; return *this; };
    double         Val()                { return dVal; };
    double         Ovr()                { return dOvr; };
    operator       double()             { return Valid(dOvr) ? dOvr : HasNANFlag(dOvr, NF_Star) ? dNAN : dVal; };

    void           SetVal(double Val, pTaggedObject pThis);
    void           SetOvr(double Ovr, pTaggedObject pThis);
    void           ClrOvr(pTaggedObject pThis)  { SetOvr(dNAN, pThis);  };
    void           Clear(pTaggedObject pThis)   { SetVal(0.0, pThis); SetOvr(dNAN, pThis);  };

    void           BuildDataDefn(DataDefnBlk & DDB, pchar Nm, pchar Sym, CCnvIndex Cnv, pchar pCnv_, long lHandle, pTaggedObject pThis, char * ValStr, char * OvrStr, DDEF_Flags ValFlags=0, DDEF_Flags OvrFlags=0, bool ForceOldMethod=false);
    flag           DataXchg(DataChangeBlk & DCB, long lHandle, pTaggedObject pThis);
  };

//---------------------------------------------------------------------------

inline void DualDbl::SetVal(double Val, pTaggedObject pThis)
  {
  //if (Valid(dOvr)!=Valid(Ovr))
  //  pThis->StructureChanged(pThis);
  dVal=Val;
  };

//---------------------------------------------------------------------------

inline void DualDbl::SetOvr(double Ovr, pTaggedObject pThis)
  {
  if (pThis && Valid(dOvr)!=Valid(Ovr))
    pThis->StructureChanged(pThis);
  dOvr=Ovr;
  };

//===========================================================================
//
//  
//
//===========================================================================

class CToleranceBlock;

class DllImportExport CToleranceBlockArray : public CArray <CToleranceBlock*, CToleranceBlock*>  {};

class DllImportExport CToleranceBlock
  {
  protected:
    CToleranceBlock * m_pParent;

    CString   m_sName;
    DWORD     m_dwFlags;
    DualDbl   m_dMaxIters;
    DualDbl   m_dAbs;
    DualDbl   m_dRel;
    DualDbl   m_dAbsMult;
    DualDbl   m_dRelMult;

    CnvAttribute m_AbsCnv;

    double    m_dErr;

  public:

    static CToleranceBlockArray s_Array;

  public:

    CToleranceBlock(DWORD Use, LPCTSTR Name, double Abs, double Rel, long MaxIters=100, 
                    DWORD Flags=(TBF_UseMax|TBF_UseAbs|TBF_UseRel),
                    CCnvIndex AbsIndex=DC_Frac, LPCTSTR AbsTxt="%");
    CToleranceBlock(DWORD Use, CToleranceBlock * pParent, LPCTSTR Name, double AbsMult, double RelMult, long MaxIters=100, 
                    DWORD Flags=(TBF_UseMaxParent|TBF_UseAbsParent|TBF_UseRelParent));
    virtual ~CToleranceBlock();
    CToleranceBlock *Parent()         { return m_pParent; };

    void      SetMaxIters(double MaxIters);
    void      SetAbs(double Abs);
    void      SetRel(double Rel);
    void      SetAbsMult(double Abs);
    void      SetRelMult(double Rel);
    void      SetFlags(DWORD Flags);

    LPCTSTR   GetName()               { return m_sName; };
    long      GetMaxIters()           { return (long)((m_dwFlags&TBF_UseMaxParent) ? m_pParent->GetMaxIters() : m_dMaxIters()); };
    double    GetAbs()                { return (m_dwFlags&TBF_UseAbsParent) ? m_dAbsMult()*m_pParent->GetAbs() : m_dAbs(); };
    double    GetRel()                { return (m_dwFlags&TBF_UseRelParent) ? m_dRelMult()*m_pParent->GetRel() : m_dRel(); };
    double    GetAbsMult()            { return m_dAbsMult(); };
    double    GetRelMult()            { return m_dRelMult(); };
                                      
    double    GetNormalError()        { return m_dErr; };
    double    GetActualError()        { return m_dErr*GetRel(); };
    DWORD     GetFlags()              { return m_dwFlags; };
                                      
    bool      UseMax()                { return (m_dwFlags & TBF_UseMax      )!=0; };
    bool      UseAbs()                { return (m_dwFlags & TBF_UseAbs      )!=0; };
    bool      UseRel()                { return (m_dwFlags & TBF_UseRel      )!=0; };
    bool      Hide()                  { return (m_dwFlags & TBF_Hide        )!=0; };
    bool      UseMaxParent()          { return (m_dwFlags & TBF_UseMaxParent)!=0; };
    bool      UseAbsParent()          { return (m_dwFlags & TBF_UseAbsParent)!=0; };
    bool      UseRelParent()          { return (m_dwFlags & TBF_UseRelParent)!=0; };
    bool      HasParent()             { return (m_pParent!=NULL); };

    DualDbl & GetAbsDD()              { return m_dAbs; };
    DualDbl & GetRelDD()              { return m_dRel; };
    DualDbl & GetMaxItersDD()         { return m_dMaxIters; };
    DualDbl & GetAbsMultDD()          { return m_dAbsMult; };
    DualDbl & GetRelMultDD()          { return m_dRelMult; };
                                      
    CnvAttribute & GetAbsCnv()        { return (m_dwFlags&TBF_UseAbsParent) ? m_pParent->GetAbsCnv() : m_AbsCnv; };

    bool ConvergedDV(double D, double V)
      {
      const double Vt = Max(fabs(V), fabs(V + D));
      m_dErr = fabs(D) / (GetAbs() + Vt * GetRel());
      return (m_dErr < 1.0);
      }

    bool ConvergedVV(double V1, double V2)
      {
      const double Vt = Max(fabs(V1), fabs(V2));
      m_dErr = fabs(V2-V1) / (GetAbs()+ Vt * GetRel());
      return (m_dErr < 1.0);
      }
    
  };

//===========================================================================
//
//  
//
//===========================================================================

#undef DllImportExport 

#endif  /* __MATHLIBTO_H */
