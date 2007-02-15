//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SysCADMill_H
#define  __SysCADMill_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif


#ifdef __SysCADMill_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(UserMethods)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

#include "FixedComminution.h"
#include "BallMill.h"
#include "RodMill.h"
#include "AGSAGMill.h"

//---------------------------------------------------------------------------

class SysCADMill : public MBaseMethod
  {
  public:

    //SysCADMill(TaggedObject * pNd);
    SysCADMill(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual bool PreStartCheck() { bInit = true; return true; }; 

  protected:
    // Seperator Method
    CMill::eMethod m_Method;
    bool  m_bOrePropsExist; //true if OreProperties database found

    // Persistent Storage for each Seperation Type
    CComminution_Fixed			m_CComminution_Individual_Fixed;
    CMill_Rod	    m_CMill_Rod;
    CMill_Ball		m_CMill_Ball;
    CMill_AGSAG		m_CMill_AGSAG;

    // The Selected Method
    CMill          *m_pCMill;

    // Flag to tell EvalProducts to initialise
    bool bInit;
 
  };

#endif