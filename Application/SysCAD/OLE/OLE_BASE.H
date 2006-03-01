//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __OLE_BASE_H
#define  __OLE_BASE_H

#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

//===========================================================================

extern UINT wm_SyscadOleCommand;

const UINT OLE_QUIT          = 1;
const UINT OLE_DOAUTOITEM    = 2;

extern CWnd* pOleInfoWnd;
extern BOOL UsingExcel8or9;

enum OleAutoServers { OleAutoIsExcel };

//===========================================================================

COleVariant GetVariant(PkDataUnion* pItem, CCnvIndex iCnv=0, char* pCnvTxt=NULL);

//===========================================================================

class OVariant : public COleVariant
  {
  public:
    OVariant() : COleVariant() {};

    OVariant(const VARIANT& varSrc) : COleVariant(varSrc) {};
    OVariant(LPCVARIANT pSrc) : COleVariant(pSrc) {};
    OVariant(const COleVariant& varSrc) : COleVariant(varSrc) {};

    OVariant(LPCTSTR lpszSrc) : COleVariant(lpszSrc) {};
    OVariant(CString& strSrc) : COleVariant(strSrc) {};

    OVariant(BYTE nSrc) : COleVariant(nSrc) {};
    OVariant(short nSrc, VARTYPE vtSrc = VT_I2) : COleVariant(nSrc, vtSrc) {};
    OVariant(long lSrc, VARTYPE vtSrc = VT_I4) : COleVariant(lSrc, vtSrc) {};
    OVariant(const COleCurrency& curSrc) : COleVariant(curSrc) {};

    OVariant(float fltSrc) : COleVariant(fltSrc) {};
    OVariant(double dblSrc) : COleVariant(dblSrc) {};
    OVariant(const COleDateTime& timeSrc) : COleVariant(timeSrc) {};

    OVariant(const CByteArray& arrSrc) : COleVariant(arrSrc) {};
    OVariant(const CLongBinary& lbSrc) : COleVariant(lbSrc) {};

    OVariant(BOOL b);
    OVariant(LPDISPATCH v);
    BOOL ForceType(VARTYPE vartype);
    void Set(BOOL v);
    void Set(short v);
    void Set(long v);
    void Set(LPDISPATCH v);
  };

//===========================================================================

const byte OleInfoTxtFlag = 0x01;
const byte OleInfoCountFlag = 0x02;
const byte OleInfoNameFlag = 0x04;

class COleInfoMsg
  {
  public:
    int iCount;
    Strng sTxt;
    Strng sName;
    byte Flags;

    COleInfoMsg(int Count, char* pTxt, char* pName)
      { iCount = Count; sTxt = pTxt; sName = pName; Flags = (OleInfoTxtFlag | OleInfoCountFlag | OleInfoNameFlag); };
    COleInfoMsg(int Count, char* pTxt)
      { iCount = Count; sTxt = pTxt; Flags = (OleInfoTxtFlag | OleInfoCountFlag); };
    COleInfoMsg(int Count)
      { iCount = Count; Flags = OleInfoCountFlag; };
    COleInfoMsg(char* pTxt)
      { sTxt = pTxt; Flags = OleInfoTxtFlag; };
    COleInfoMsg(char* pTxt, char* pName)
      { sTxt = pTxt; sName = pName; Flags = (OleInfoTxtFlag | OleInfoNameFlag); };
  };

//===========================================================================
/*#C:Pure virtual base class for OLE Automation sessions.*/
class COleBaseAuto
  {
  public:
    Strng sFileName;        //filename for automation server
    CExecObj* pExecObj;     //executive object to use for EO messages etc
    CXM_Route xRoute;        //executive object to notify about errors
    flag bPrompt:1,         //show message box before each automation
         bLock:1,           //lock automation server
         bActivateSysCAD:1; //make SysCAD active and bring to front after report
    CScdCOCmdBlk * m_pComCmd; // CNM Optionally Holds info relating to the source of a ComCmd for Event Processing

    COleBaseAuto(CExecObj* ExecObj, CXM_Route* pRoute, char* FileName, flag Lock);
    virtual ~COleBaseAuto();
    virtual int DoAutomation() = 0;
    virtual int AutomationServer() = 0; // return one of the OleAutoServers constants
    virtual char* OLEServerDesc() = 0; // return name/description of the OleAutoServer
    virtual char* ShortDesc() = 0;
    void ActivateSysCAD();
    void ReportError(long ErrNo, char * fmt, ...);
    void ReportWarning(long ErrNo, char * fmt, ...);
    void Prompt(char * fmt, ...);
    void SendInfo(char * fmt, ...);
  protected:
    void ReportLogMsg(long ErrNo, char* pMsg, BOOL Err);
  };

//===========================================================================

class COleAutoQueueItem
  {
  public:
    COleBaseAuto* pAutoItem;
    COleAutoQueueItem* pNxt;

    COleAutoQueueItem(COleBaseAuto* pItem)
      { pNxt = NULL; pAutoItem = pItem; };
  };

//===========================================================================

#endif


