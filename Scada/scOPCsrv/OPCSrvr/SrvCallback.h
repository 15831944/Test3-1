//**************************************************************************
//**************************************************************************
//
//            Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Callback.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :  Callback object for OPC Server DLL
//
//  Description:  This class interfaces to the application's data
//
//**************************************************************************

#if !defined(_SHELLCALLBACK_H_INCLUDED_)
#define _SHELLCALLBACK_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef  __SCDVER_H
#include "scdver.h"
#endif
#if WITHOPC
#include "OPCDa.h"
#include "scdopcsrv.h"
#include "afxpriv.h"
#ifndef __COMMSSRVRBASE_H
#include "CommsSrvrBase.h"
#endif
#ifndef __EXECUTIV_H
#include "executiv.h"
#endif
#include "tagbrowse.h"

//--------------------------------------------------------------------------

//#define FlatBranchStructure 1
#define FlatBranchStructure 0

class CScdOPCCallBack;
class Branch;

//*******************************************************************
// Override CTag to add a name 
class CScdOPCTag : public CTag
  {
  public:
    CScdOPCTag(CScdOPCCallBack * pCB, LPCTSTR name, Branch* branch);
    virtual ~CScdOPCTag();

    CScdOPCCallBack * m_pCallBack;

    bool       m_bChecked;
    bool       m_bIsValid;
    CString    m_name;
    CString    m_fullname;

    Strng      m_sTagOnly;

    byte       m_cType;
    CCnvIndex  m_CnvInx;
    Strng      m_sCnvTxt;
    Strng_List m_StrLst;

    COleVariant m_LclValue;

    bool       CheckTag();
    void       LoadLocal(CPkDataItem* pItem , FILETIME & FT);
  };

typedef CTypedPtrList<CPtrList, CScdOPCTag*> TagList;
typedef CTypedPtrList<CPtrList, Branch*> BranchList;

//*******************************************************************
// Branch class to hold branches and leaves
class Branch
  {
  public:
    Branch(LPCTSTR name);
    virtual ~Branch();
    void AddTag(CScdOPCTag* pTag);
    void AddBranch( Branch* pBranch)
      { m_branches.AddTail(pBranch); pBranch->m_parent = this; };
    CScdOPCTag* FindTag(const CString& target);
    CString     FindTagName(const CString& target);
    CString     GetPath();

    CString     m_name;
    TagList     m_tags;
    BranchList  m_branches;
    Branch*     m_parent;
  };

//*******************************************************************

class CScdOPCTagArray : public CArray <CScdOPCTag*, CScdOPCTag*> {};  

class CScdOPCCallBack : public COPCCallback, public CExecObj
  {
  friend class ShellBrowser;
  friend class CScdOPCTag;
  friend class CScdOPCManager;
  friend class COPCSrvrStatsDlg;

  public:
    CScdOPCCallBack(/*CScd * Plc*/);
    virtual ~CScdOPCCallBack();

    // New 2.0 Item Properties
    virtual HRESULT QueryNumProperties(
      LPCTSTR     name,
      DWORD     * pdwNumItems,
      LPVOID    * ppVoid);
    virtual HRESULT QueryAvailableProperties(
      LPCTSTR     name,
      DWORD       dwNumItems,
      LPVOID      pVoid,
      DWORD     * pPropertyIDs,
      LPWSTR    * pDescriptions,
      VARTYPE   * pDataTypes);
    virtual HRESULT GetItemProperties(
      LPCTSTR     name,
      DWORD       dwNumItems,
      DWORD     * pPropertyIDs,
      VARIANT   * pData,
      HRESULT   * pErrors);
    virtual HRESULT LookupItemIDs(
      LPCTSTR     name,
      DWORD       dwNumItems,
      DWORD     * pPropertyIDs,
      LPWSTR    * pszNewItemIDs,
      HRESULT   * pErrors);

    virtual OPCSERVERSTATE GetServerState();
    virtual DWORD SetUpdateRate( DWORD newUpdateRate );
    virtual COPCBrowser* CreateBrowser();
    virtual CTag* AddTag(   LPCTSTR name,
      LPCTSTR accessPath,
      VARTYPE requestedType);
    virtual HRESULT ValidateTag(
      CTag* pTag,
      LPCTSTR name,
      LPCTSTR accessPath,
      VARTYPE requestedType);
    virtual HRESULT Remove(
      DWORD       dwNumItems,
      CTag     ** ppTags);
    virtual LPCTSTR GetTagName( CTag * pTag );
    virtual BOOL    GetTagLimits(  CTag * pTag, double *pHigh, double *pLow );
    virtual HRESULT Scan(
      DWORD       dwNumItems,
      CTag     ** ppTags,
      HRESULT   * pErrors);
    /*
    virtual HRESULT Read(
    DWORD       dwNumItems,
    CTag     ** ppTags,
    HRESULT   * pErrors);
    */
    virtual HRESULT ReadTag(CTag      * pTag);
    /*
    virtual HRESULT Write(
    DWORD       dwNumItems,
    CTag     ** ppTags,
    VARIANT   * pItemValues,
    HRESULT   * pErrors);
    */
    virtual HRESULT WriteTag(
      CTag      * pTag,
      VARIANT   & value);
    virtual LPCTSTR GetErrorString(
      HRESULT     dwError,
      LCID        dwLocale);
    virtual LPCTSTR GetVendorString();

    virtual void ConnectNotify(bool Connecting);

    void BuildBrowserBranchRecursive( char* pBranchName , class Branch* pParentBranch , CTagTreeItem *pRoot);
    void PopulateBrowserBranch( Branch* pBranch , CTagTreeItem *pRoot );



  protected:
    CString error;
    //Branch  m_root;
    Branch m_SubscribedTagsRoot; // Top of subscribed tags branch structure (will be FLAT)
    Branch m_BrowseTagsRoot;     // Top of browse tags structure (will be hierachical)
    bool bForceReadRqd;

    long iTagCnt;
    long iScanCnt;
    long iScdWriteOKCnt;
    long iClientWriteOKCnt;
    long iWriteFailCnt;
    long iScdReadOKCnt;
    long iClientReadOKCnt;
    long iReadFailCnt;
    long iExecuteCnt;
    DWORD dwLastScan;
    DWORD dwMeasScanRate;

    void StatsCntReset() { iScanCnt=0; iScdWriteOKCnt=0; iClientWriteOKCnt=0; iWriteFailCnt=0; iScdReadOKCnt=0; iClientReadOKCnt=0; iReadFailCnt=0; };
    void BuildSubsList(CListBox* pList);
    flag ForceWriteSubsDataAll();
    CScdOPCTag * TryMakeTag(LPCTSTR Name, Branch * pBrnch); 
    CScdOPCTag * MakeTag(LPCTSTR Name, Branch * pBrnch);
    void DumpBranchs();
    void SetEnable(bool On);

    CScdOPCTagArray m_TagA;  // Not the owner of tags

  protected:
    // CExecObj Overrides
    virtual flag   EO_QueryTime(const CXM_TimeControl &CB, double &TimeRqd, double &dTimeRqd);
    virtual flag   EO_Start(const CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(const CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(const CXM_TimeControl &CB);
    virtual int    EO_CanClose(Strng_List & Problems);

  };

//*******************************************************************

class ShellBrowser : public COPCBrowser
  {
  public:
    ShellBrowser(CScdOPCCallBack* parent);
    //virtual ~ShellBrowser();
    virtual OPCNAMESPACETYPE QueryOrganization();
    virtual BOOL MoveUp();
    virtual BOOL MoveDown(LPCTSTR branch);
    virtual HRESULT GetNames( OPCBROWSETYPE type,
      LPCTSTR stringFilter,
      VARTYPE dataTypeFilter,
      DWORD accessFilter );
    virtual LPCTSTR GetItemID( LPCTSTR name );
    virtual LPCTSTR Next();
    virtual void Reset();
    virtual HRESULT GetAccessPaths( LPCTSTR name );
    virtual LPCTSTR NextAccessPath();

    Branch*        m_pBranch;
 
  private:
    CScdOPCCallBack* m_parent;
    POSITION       m_pos;

    // For browsing flat
    CStringList    m_paths;
    void           AddTags( Branch* pBranch );

    OPCBROWSETYPE  m_type;
    CString        m_stringFilter;
    VARTYPE        m_dataTypeFilter;
    DWORD          m_accessFilter;
    TCHAR          m_name[256];
    BOOL           m_bDoStringFilter;
    int            m_iFilterLen;
  };


#endif
#endif