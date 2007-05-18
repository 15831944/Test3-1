// ScdCOMClassFactory.cpp: implementation of the CScdCOMClassFactory class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#define WITHSCDINTERFACES
#define __COMSTUFF_CPP

#include "COMStuff.h"

#include "comcat.h"
#include "ScdCATIDS.h"
#include <atlbase.h>
#include <atlconv.h>
#include "..\..\Com\ScdIF\ScdIF.h"
#include "..\..\Com\ScdIF\ScdIF_i.c"

#include "sp_model.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//========================================================================
//
//
//
//========================================================================

CGlobalInterfaceTable g_GIT;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

bool CCOMObjManager::AddClass(LPCTSTR Cls1, LPCTSTR ProgID)
  {

  if (strcmp(Cls1, "COMSpModel")==0)
    {
    INCOMPLETECODE(__FILE__, __LINE__); // should no longer come in here
    // Does Tmp2 hold a valid SpModel ProgID
    USES_CONVERSION;
    ICatInformation *pCatInfo = NULL;
    HRESULT hr;
    BOOL ClsFound=false;

    CComBSTR pid(ProgID);
    CLSID rqdclsid;
    hr=CLSIDFromProgID(pid, &rqdclsid);
    if(SUCCEEDED(hr))
      {
      // Get the categories manager.
      hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 0,
                  CLSCTX_SERVER, IID_ICatInformation, (void**)&pCatInfo);

      if(SUCCEEDED(hr))
        {
        IEnumCLSID *pCLSIDS = NULL;
        CATID catids[1];
        catids[0] = CATID_SpecieModel;

        hr = pCatInfo->EnumClassesOfCategories(1, catids, -1, 0, &pCLSIDS);
        CLSID clsid[50];

        do
          {
          DWORD numb = 0;
          hr = pCLSIDS->Next(50, clsid, &numb);
          if(SUCCEEDED(hr))
            {
            for(DWORD i = 0; !ClsFound&& i < numb; i++)
              ClsFound=IsEqualCLSID(clsid[i], rqdclsid);
            }
          }
        while(!ClsFound && hr == S_OK);
        pCLSIDS->Release();
        }
      pCatInfo->Release();
      }
    if (ClsFound)
      {

      TagObjClass *pC=TagObjClass::AddCOMClass(
        (LPTSTR)ProgID,          // pProgID,     
        (LPTSTR)ProgID,          // pClassName,  
        SpModel::GroupName,      // pGroup_,     
        (LPTSTR)ProgID,          // pClassId_,   
        "",                      // pVersion_,   
        "",                      // pDrwGroup_,    
        "",                      // pDefTag_,    
        TOC_SYSTEM,              // dwCat,            // Fetch from ???
        (LPTSTR)ProgID,          // ShortDesc,   
        (LPTSTR)ProgID,          // pDesc,       
        0xffffffff);             // SelectMask); 

      return true;
      }
    else
      LogError("Sfe_Server", LF_Exclamation, "%s Unknown SpModelCOM Class", ProgID);
    }
  return false;
  }

//--------------------------------------------------------------------

TaggedObject * CCOMObjManager::Construct(TagObjClass *pClass, LPCTSTR pRqdTag, TaggedObject *pAttach, TagObjAttachment eAttach) 
  { 
  CComBSTR pid(pClass->ProgID());
  CComPtr<IUnknown> pUnk;
  CComPtr<IScdTaggedObject> pITagObj;

  HRESULT hr;
  hr=pUnk.CoCreateInstance(pid);
  if (FAILED(hr))
    return NULL;
  hr=pUnk.QueryInterface(&pITagObj);
  if (FAILED(hr))
    return NULL;

  long pRP;
  hr=pITagObj->get_RawModelPtr(&pRP);
  if (FAILED(hr))
    return NULL;

  TaggedObject* pTO=(TaggedObject*)pRP;
  pTO->SetClass(pClass);
  pTO->SetTag((LPTSTR)pRqdTag);
  pTO->AttachMeToObj(pAttach, eAttach);

  GIScdTaggedObject*pgpTO=new GIScdTaggedObject;
  
  pgpTO->Globalize(pITagObj);
  pTO->MarkAsCOMOwned(pgpTO); 

  return pTO;
  };

//--------------------------------------------------------------------

void CCOMObjManager::Destroy(TaggedObject * pObj)
  {
  pObj->DestroyCOMOwned();
  };

//--------------------------------------------------------------------
