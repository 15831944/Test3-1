// ScdSlv.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f ScdSlvps.mk in the project directory.

#include "ScdConnectPoints.h"
#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "ScdSlv.h"

#include "ScdSlv_i.c"
#include "..\ScdIF\ScdIF_i.c"
#include "..\ScdMdl\ScdMdl_i.c"

#include "ScdSolver.h"
#include "TagObj.h"
#include "ScdMessage.h"
#include "ScdMessages.h"
#include "ScdProbal.h"
#include "ScdDebug.h"
#include "ScdTest.h"
#include "ScdSlvTag.h"
#include "ScdSlvTags.h"
#include "ScdDynamic.h"
#include "ScdNodes.h"
#include "ScdNode.h"
//#include "ScdUnits.h"
//#include "ScdUnit.h"
//#include "ScdLinks.h"
//#include "ScdLink.h"
#include "ScdConnect.h"
#include "ScdConnects.h"
#include "ScdConnectPoint.h"
#include "ScdConnectPoints.h"
#include "ScdNodeClass.h"
#include "ScdWire.h"
#include "ScdWires.h"
#include "ScdTerminal.h"
#include "ScdTerminals.h"
#include "ScdTermStrip.h"
#include "ScdTermStrips.h"
#include "ScdComponent.h"
#include "ScdComponents.h"
#include "ScdElectrics.h"
#include "ScdComplex.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_ScdMessage, CScdMessage)
  OBJECT_ENTRY(CLSID_ScdMessages, CScdMessages)
  OBJECT_ENTRY(CLSID_ScdProbal, CScdProbal)
  OBJECT_ENTRY(CLSID_ScdDynamic, CScdDynamic)
  OBJECT_ENTRY(CLSID_ScdDebug, CScdDebug)
  OBJECT_ENTRY(CLSID_ScdTest, CScdTest)
  OBJECT_ENTRY(CLSID_ScdSlvTags, CScdSlvTags)
  OBJECT_ENTRY(CLSID_ScdSlvTag, CScdSlvTag)
  OBJECT_ENTRY(CLSID_ScdNodes, CScdNodes)
  OBJECT_ENTRY(CLSID_ScdNode, CScdNode)
  //OBJECT_ENTRY(CLSID_ScdUnits, CScdUnits)
  //OBJECT_ENTRY(CLSID_ScdUnit, CScdUnit)
  //OBJECT_ENTRY(CLSID_ScdLinks, CScdLinks)
  //OBJECT_ENTRY(CLSID_ScdLink, CScdLink)
  OBJECT_ENTRY(CLSID_ScdSolver, CScdSolver)
  OBJECT_ENTRY(CLSID_ScdConnect, CScdConnect)
  OBJECT_ENTRY(CLSID_ScdConnects, CScdConnects)
  OBJECT_ENTRY(CLSID_ScdConnectPoint, CScdConnectPoint)
  OBJECT_ENTRY(CLSID_ScdConnectPoints, CScdConnectPoints)
  OBJECT_ENTRY(CLSID_ScdNodeClass, CScdNodeClass)
  OBJECT_ENTRY(CLSID_ScdWire, CScdWire)
  OBJECT_ENTRY(CLSID_ScdWires, CScdWires)
  OBJECT_ENTRY(CLSID_ScdTerminal, CScdTerminal)
  OBJECT_ENTRY(CLSID_ScdTerminals, CScdTerminals)
  OBJECT_ENTRY(CLSID_ScdTermStrip, CScdTermStrip)
  OBJECT_ENTRY(CLSID_ScdTermStrips, CScdTermStrips)
  OBJECT_ENTRY(CLSID_ScdComponent, CScdComponent)
  OBJECT_ENTRY(CLSID_ScdComponents, CScdComponents)
  OBJECT_ENTRY(CLSID_ScdElectrics, CScdElectrics)
  OBJECT_ENTRY(CLSID_ScdComplex, CScdComplex)
END_OBJECT_MAP()

class CScdSlvApp : public CWinApp
  {
  public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CScdSlvApp)
  public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CScdSlvApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

BEGIN_MESSAGE_MAP(CScdSlvApp, CWinApp)
  //{{AFX_MSG_MAP(CScdSlvApp)
  // NOTE - the ClassWizard will add and remove mapping macros here.
  //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CScdSlvApp theApp;

BOOL CScdSlvApp::InitInstance()
  {
  _Module.Init(ObjectMap, m_hInstance, &LIBID_ScdSlv);
  TRACE0("ScdSlv : InitInstance\n");
  for (int i=0; i<10; i++)
    AtlTrace("----> %08x\n", ObjectMap[i].pCF);
  return CWinApp::InitInstance();
  }

int CScdSlvApp::ExitInstance()
  {
  TRACE0("ScdSlv : ExitInstance\n");
  for (int i=0; i<10; i++)
    AtlTrace("<---- %08x\n", ObjectMap[i].pCF);
  _Module.Term();
  return CWinApp::ExitInstance();
  }

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
  {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  TRACE0("ScdSlv : Calling AfxDllCanUnloadNow\n");
  return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
  }

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
  {
  return _Module.GetClassObject(rclsid, riid, ppv);
  }

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
  {
  // registers object, typelib and all interfaces in typelib
  return _Module.RegisterServer(TRUE);
  }

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
  {
  return _Module.UnregisterServer(TRUE);
  }


