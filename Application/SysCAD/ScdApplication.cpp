// ScdApplication.cpp : Implementation of CScdApplication
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdApplication.h"
#include "sc_defs.h"
#include "project.h"
#include "cmd_mngr.h"
#include ".\scdapplication.h"
#include "licbase.h"
#include "syscad.h"


//////////////////////////////////////////////////////////////////////


#include <map>
#include <string>
//typedef std::map<std::string,IUnknown*> CScdAppmapObjects;


//////////////////////////////////////////////////////////////////////

CScdAppmapObjects CScdApplicationFactory::sm_mapObjects;

STDMETHODIMP CScdApplicationFactory::GetObject(LPOLESTR pszItem,     //Pointer to name of the object requested
                                               DWORD dwSpeedNeeded,  //Speed requirements on binding
                                               IBindCtx *pbc,        //Pointer to bind context object to be used
                                               REFIID riid,          //Reference to the identifier of the 
                                               // interface pointer desired
                                               void **ppvObject      //Address of output variable that receives 
                                               // the interface pointer requested in riid
                                               )
  {

  char szName[100];
  wcstombs(szName,pszItem,sizeof(szName)/sizeof(szName[0]));
  std::string strName(szName);
  HRESULT hr=E_NOTIMPL;
  IUnknown* pIUnkwn;
  CScdAppmapObjects::const_iterator it_find;
  it_find=sm_mapObjects.find(strName);
  if(it_find!=sm_mapObjects.end())
    {
    //I have found our object
    ATLTRACE("CScdApplicationFactory::GetObject returns an existing object\n");
    pIUnkwn=it_find->second;
    return pIUnkwn->QueryInterface(riid,ppvObject);
    }
  else 
    {
    hr = CreateInstance(NULL, riid,  ppvObject);
    ATLTRACE("CScdApplicationFactory::GetObject creates a new object: %s\n",szName);
    //make it a strong table, so I call AddRef 
    IUnknown* pIUnknown =(IUnknown*)(*ppvObject);
    pIUnknown->AddRef();
    sm_mapObjects.insert(std::pair<std::string,IUnknown*>(strName,pIUnknown));
    }
  return hr;
  }
//STDMETHODIMP CScdApplicationFactory::RemoveObject(BSTR bstrObjectName)
//  {
//
//  TCHAR szName[100];
//  wcstombs(szName,bstrObjectName,sizeof(szName)/sizeof(szName[0]));
//  std::string strName(szName);
//  HRESULT hr;
//  IUnknown* pIUnkwn;
//  CScdAppmapObjects::iterator it_find;
//  it_find=sm_mapObjects.find(strName);
//  if(it_find!=sm_mapObjects.end())
//    {
//    //I have found our object
//    ATLTRACE("CScdApplicationFactory::RemoveObject removes object %s\n",szName);
//    pIUnkwn=it_find->second;
//    sm_mapObjects.erase(it_find);
//    pIUnkwn->Release();
//    hr = S_OK;
//    }else
//      hr= S_FALSE;
//
//    return hr;
//  }


/////////////////////////////////////////////////////////////////////////////
// CScdApplication

CScdApplication::CScdApplication() : CScdCOCmdBase(WMU_COM_APP)
  {
  // m_pUnkMarshaler = NULL;
  }

STDMETHODIMP CScdApplication::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] =
    {
    &IID_IScdApplication
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }


void CScdApplication::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_LoadProject:
      Fire_OnProjectOpened(Data);
      break;
    };
  };

DWORD gs_ServerWait = 0; // Initialised in SysCAD @ Startup

HRESULT CScdApplication::FinalConstruct()
  {
  FC_TESTLICENSE(gs_License.AllowCOMApp());

  DWORD iDelay=20; 
  // Loops for gs_ServerWait Seconds
  for (DWORD iLoop=0; !CProject::sm_SysCADInited; iLoop+=iDelay)
    {
    if (gs_ServerWait>0 && iLoop>=gs_ServerWait)
      return E_FAIL; // Timeout and hence fail
    Sleep(iDelay);
    }

  FC_ROF(m_pSolver.CoCreateInstance(CLSID_ScdSolver));
  FC_ROF(m_pOptions.CoCreateInstance(CLSID_ScdOptions));
  FC_ROF(m_pProject.CoCreateInstance(CLSID_ScdProject));
  FC_ROF(m_pLicense.CoCreateInstance(CLSID_ScdLicenseApp));
  AddChildEvents(m_pSolver.p);
  AddChildEvents(m_pProject.p);
  AddChildEvents(m_pOptions.p);
  AddChildEvents(m_pLicense.p);
  gs_License.PushState();
  return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
  }

void CScdApplication::FinalRelease()
  {
  gs_License.PopState();
  ////m_pUnkMarshaler.Release();
  m_pLicense.Release();
  m_pProject.Release();
  m_pOptions.Release();
  m_pSolver.Release();
  }


STDMETHODIMP CScdApplication::get_Messages(IScdMessages **pMessages)
  {
  appSCD_COMENTRYGET(long, pMessages)
    {
    Scd.ReturnH(m_pSolver->get_Messages(pMessages));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::get_Debug(IScdDebug **pDebug)
  {
  appSCD_COMENTRYGET(long, pDebug)
    {
    Scd.ReturnH(m_pSolver->get_Debug(pDebug));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::get_Test(IScdTest **pTest)
  {
  appSCD_COMENTRYGET(long, pTest)
    {
    Scd.ReturnH(m_pSolver->get_Test(pTest));
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdApplication::OpenProject(BSTR bsPrjFolder, IScdProject **pProject)
  {
  appSCD_COMENTRY(long)
    {
    ScdApp()->m_HoldAddToRecentFileList++;

    SCD_LclStr(PrjFolder, bsPrjFolder);

    LPTSTR S=new char[strlen(PrjFolder)+1];
    strcpy(S, PrjFolder);
    switch (PostComCmd(ComCmd_LoadProject, (long)S))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    ScdApp()->m_HoldAddToRecentFileList--;
    if (Scd.HResultOK())
      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::CreateProject(BSTR bsCfgFolder, BSTR bsPrjFolder, VARIANT_BOOL WithVersion, VARIANT_BOOL RemoveOld, IScdProject ** pProject)
  {
  appSCD_COMENTRY(long)
    {
    ScdApp()->m_HoldAddToRecentFileList++;

    SCD_LclStr(CfgFolder, bsCfgFolder);
    SCD_LclStr(PrjFolder, bsPrjFolder);
    
    CScdCmdParms *pP = new CScdCmdParms;

    pP->m_sParms[0]=CfgFolder;
    pP->m_sParms[1]=PrjFolder;
    pP->m_bParms[0]=WithVersion;
    pP->m_bParms[1]=RemoveOld;

    switch (PostComCmd(ComCmd_CreateProject, (long)pP))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    ScdApp()->m_HoldAddToRecentFileList--;
    if (Scd.HResultOK())
      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdApplication::get_License(IScdLicenseApp **pLicense)
  {
  appSCD_COMENTRYGET(long, pLicense)
    {
    Scd.ReturnH(m_pLicense.CopyTo(pLicense));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::get_Options(IScdOptions **pOptions)
  {
  appSCD_COMENTRYGET(long, pOptions)
    {
    Scd.ReturnH(m_pOptions.CopyTo(pOptions));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::CloseProject(VARIANT_BOOL SaveIt)
  {
  appSCD_COMENTRY(long)
    {
    ScdApp()->m_HoldAddToRecentFileList++;

    gs_pXCmd->bSaveOnExit=SaveIt;
    gs_pXCmd->bNoSaveOnExit=!SaveIt;
    switch (PostComCmd(ComCmd_CloseProject, NULL))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    ScdApp()->m_HoldAddToRecentFileList--;
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::SetWindowState(eScdWndStateCmds ReqdState)
  {
  appSCD_COMENTRY(long)
    {
    switch (PostComCmd(ComCmd_SetAppWndState, (long)ReqdState))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdApplication::get_Project(IScdProject** pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    Scd.ReturnH(m_pProject.CopyTo(pVal));
    }
  SCD_COMEXIT
  }

// =======================================================================
//
//
//
// =======================================================================
