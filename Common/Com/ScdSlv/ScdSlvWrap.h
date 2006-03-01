// ScdMdlWrap.h: interface for the CScdSlvWrap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ScdMdlWrap_H__FD7B2382_524A_4008_85B9_28423DCEC844__INCLUDED_)
#define AFX_ScdMdlWrap_H__FD7B2382_524A_4008_85B9_28423DCEC844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "ScdSlv.h"
//#include "ScdSpDB.h"
#include "GpFuncs.h"
#include "comdef.h"

#include "resource.h"       // main symbols

#include "mdlcfg.h"
#include "sp_db.h"
#include "Sp_Model.h" 
#include "Sp_Model.h" 

//_COM_SMARTPTR_TYPEDEF(IScdSpDB, __uuidof(IScdSpDB));
//_COM_SMARTPTR_TYPEDEF(IScdSpModel, __uuidof(IScdSpModel));

class CScdSlvWrap  
  {
  public:
    CComBSTR        m_bsCfgFileName; 
    SFEMdlLib       m_FlwLib;//, *pFlwLib;
    SFEMdlLibArray  MdlLibs;
    
//    CComponentDataBase m_CDB; // The DataBase
  
  public:
	  CScdSlvWrap();
	  virtual ~CScdSlvWrap();
	  HRESULT FinalConstruct();
	  void FinalRelease();

    bool InitialiseCfg(BSTR CfgFileName);


  };

#endif // !defined(AFX_ScdMdlWrap_H__FD7B2382_524A_4008_85B9_28423DCEC844__INCLUDED_)
