//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __COMMSSRVRBASE_H
#define  __COMMSSRVRBASE_H

#if defined(__COMMSSRVRBASE_CPP)
  #define DllImportExport DllExport
#else
  #define DllImportExport DllImport
#endif

//===========================================================================
//Pure virtual base class from which the "SysCAD communication server" managers
//are derived. Example OPC Server. This allows indirect access via a call to
//virtual methods.
class DllImportExport CCommsServerManagerBase
  {
  public:
    CCommsServerManagerBase();
    virtual char*     Name() = 0;
    virtual void      LoadOrSaveOptions(CProfINIFile & PF, bool Loading) { };
    virtual bool      ServerOK()          { return false; };
    virtual bool      OpenStatusWnd()     { return false; };
    virtual bool      IsStatusWndOpen()   { return false; };
    virtual bool      CloseStatusWnd()    { return false; };
    virtual void      UpdateStatusWnd(WPARAM wParam=0, LPARAM lParam=0) { };
    
    virtual bool      AllowInvalidTags()  { return false; };
    virtual bool      Enabled()           { return false; };
    virtual bool      SetEnable(bool On)  { return false; };
  };

DllImportExport flag OPCServerAllowed();
DllImportExport void SetOPCServerAllowed(flag Allowed);

//------------------------------------------------------------------------

#undef DllImportExport 

#endif
