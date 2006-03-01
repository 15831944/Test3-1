//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DDE_MNGR_H
#define  __DDE_MNGR_H

#ifndef __COMMSSRVRBASE_H
#include "CommsSrvrBase.h"
#endif

//--------------------------------------------------------------------------
class CDdeExec;
class CDDEStatsDlg;

class CScdDDEManager : public CCommsServerManagerBase
  {
  friend class CDDEStatsDlg;
  public:
    static CScdDDEManager* pTheMngr;
    static CCommsServerManagerBase* Init();
    static bool Stop();

  public:
    CScdDDEManager();
    virtual ~CScdDDEManager();
    virtual bool ServerOK() { return pXDde!=NULL; };
    virtual char* Name() { return "DDE"; };
    virtual char* StatusStr(Strng & Status);
    virtual bool OpenStatusWnd();
    virtual bool IsStatusWndOpen() { return (pTheDDEStatsDlg!=NULL); };
    virtual bool CloseStatusWnd();
    virtual void UpdateStatusWnd(WPARAM wParam=0, LPARAM lParam=0);

    CDdeExec* DDEExec() { return pXDde; };

  protected:
    CDdeExec*     pXDde;
    CDDEStatsDlg* pTheDDEStatsDlg;
  };

//--------------------------------------------------------------------------
#endif


