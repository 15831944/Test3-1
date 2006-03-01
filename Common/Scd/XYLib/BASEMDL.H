//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __BASEMDL_H
#define  __BASEMDL_H

#ifndef __FIXEDIT_H
#include "fixedit.h"
#endif  
#ifndef __TAGOBJ_H
#include "tagobj.h"
#endif  
#ifndef __VECTORS_H
#include "vectors.h"
#endif

#ifdef __BASEMDL_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

_FWDDEF(CBaseMdl)
class DllImportExport CBaseMdl : public FxdEdtBookRef, public TaggedObject
  {
  protected:
    long       iNParms;   //number of parmaters
	  CDVector   Parms;	    //array of parameters
  	CDVector   MinParms;  //minimum values of parameters
	  CDVector   MaxParms;  //maximum values of parameters
	  CDVector   ParmsTol;  //tolerance of parameters
    //used for parameter estimation (regression)...
    long       iMaxIter;  //maximum number of iterations allowed
    long       iIter;     //iterations used
    double     dSa;       //sum squares err ???  
  public:
    CBaseMdl(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual void Clear();
    virtual void CopyModel(pCBaseMdl pMd);
    virtual void BuildDataDefn(DataDefnBlk & DDB);
    virtual flag DataXchg(DataChangeBlk &DCB) { return False; };
    virtual void BuildParmsBlk(flag bShowEqn);
    virtual flag InitParms(flag DoIt) { return False; };
    void LoadParmsBlk(FxdEdtInfo &EI, Strng & Str);
    long ParseParmsBlk(FxdEdtInfo &EI, Strng & Str, flag &bObjModified);
    long ButtonPushedParmsBlk(FxdEdtInfo &EI, Strng & Str);
    void SetNParms(long NParms);
  	virtual pchar GetParmDesc(long i) { ASSERT(FALSE); return ""; };
    inline void SetParms(CDVector &NewParms) { Parms = NewParms; };
    inline void GetParms(CDVector &NewParms) { NewParms = Parms; };
    inline void GetParmRanges(CDVector& NewMinParms, CDVector& NewMaxParms, CDVector& NewParmsTol)
      { NewMinParms = MinParms; NewMaxParms = MaxParms; NewParmsTol = ParmsTol; };
    inline long NParms() { return iNParms; };
  };

// ===========================================================================

#undef DllImportExport

#endif
