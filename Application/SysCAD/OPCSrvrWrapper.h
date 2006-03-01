#pragma once

#include "scdver.h"
#include "gpwfuncs.h"
#include "executiv.h"
#include "sfe_clnt.h"
#include "CommsSrvrBase.h"

class COPCSrvrWrapper
  {
  public:

  protected:
    HINSTANCE       hOPCLib;    //handle for OPC DLL
    byte            iOPCUseCnt; //counter for OPC DLL use
    CCommsServerManagerBase* pMngr; //pointer to the one and only global OPC Server Manager

  public:
    COPCSrvrWrapper();
    ~COPCSrvrWrapper();
    static void     RegisterOPCServer();
    bool            OpenSrvr(LPCTSTR EXEName, bool bResetReg, int iServer);
    void            CloseSrvr();
    void            Options();
    void            CloseOptions();
    CCommsServerManagerBase* OPCSrvrMngr() { return pMngr; };

    bool            EnableMainOPCSrvr();
    void            DisableMainOPCSrvr();
  };

extern COPCSrvrWrapper* gs_pMainOPCSrvr;
extern bool         OpenMainOPCSrvr(LPCTSTR EXEName, bool bResetReg, int iServer);
extern void         CloseMainOPCSrvr();
extern bool         GetMainOPCSrvrEnable();
extern bool         EnableMainOPCSrvr(bool On);
extern void         DisableMainOPCSrvr();
extern void         LoadMainOPCOptions(CProfINIFile & PF);
extern void         SaveMainOPCOptions(CProfINIFile & PF);

//===========================================================================
