//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

//{{AFX_INCLUDES()
#include "ssdbgrid.h"
//}}AFX_INCLUDES
#ifndef __MDLCFG_H
#define __MDLCFG_H

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include "resource.h"
#include "gpfuncs.h"
#include "scdctrls.h"
#include "sc_defs.h"
#include "sfe_base.h"

#ifdef __MDLCFG_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

//===========================================================================

//typedef CSCDList <Strng, Strng&>  SpecieList;

class MdlInfo
  {
  public:
    RequestModelInfoRec Rec;
    int                 iBranch;
    int                 iGroup;
    int                 iGrp;
    WORD                fLicMdlBlocked:1,
                        fLicModeBlocked:1;
    HTREEITEM           hItem;
    
    MdlInfo()
      {
      iBranch = 0;
      iGroup = 0;
      iGrp = 0;
      fLicMdlBlocked = 0;
      fLicModeBlocked = 0;
      hItem = NULL;
      };
    MdlInfo (const MdlInfo & X)
      {
      iBranch =           X.iBranch;
      iGroup =            X.iGroup;
      iGrp =              X.iGrp;
      fLicMdlBlocked =    X.fLicMdlBlocked;
      fLicModeBlocked =   X.fLicModeBlocked;
      hItem =             X.hItem;
      };
  };
typedef CSCDList <MdlInfo, MdlInfo&>  DllMdlList;
typedef CArray <MdlInfo, MdlInfo&>  DllMdlArray;

class SpMdlInfo
  {
  public:
    Strng               ShortDesc;
    Strng               Name;
    Strng               DLLName;
    SpMdlInfo()
      {
      };
    SpMdlInfo(char * SDesc, char * Nm, char * DLLNm)
      {
      ShortDesc = SDesc;
      Name = Nm;
      DLLName   = DLLNm;
      };
    SpMdlInfo(const SpMdlInfo & I)
      {
      ShortDesc = I.ShortDesc;
      Name      = I.Name;
      DLLName   = I.DLLName;
      };
    SpMdlInfo & operator=(const SpMdlInfo & I)
      {
      ShortDesc = I.ShortDesc;
      Name      = I.Name;
      DLLName   = I.DLLName;
      return *this;
      };
  };

class CMdlCfgBase;
typedef CSCDList <SpMdlInfo, SpMdlInfo&>  SpMdlList;
typedef CArray <SpMdlInfo, SpMdlInfo&>  SpMdlArray;
typedef CArray <CMdlCfgBase * , CMdlCfgBase *> PropPageArray;
  
enum DllType { DT_Unloadable, DT_FlwLib, DT_SMDK, DT_Other };
class CDllInfo 
  { 
  public : 
    Strng        Name;
    Strng        FullName; 
    HINSTANCE    hInst;
    DllType      Type;
    DllMdlList   Mdls;
    HTREEITEM    hItem;
    WORD         bLoad:1,
                 bLoadOK:1,
                 bSMDK:1;
    int          iPriority;
    CArray <Strng, Strng&>    m_RqdSpcs;
    CArray <CString, LPCTSTR> m_DependentDLLList;

    CDllInfo()
      {
      Type  = DT_Unloadable;
      hItem = NULL;
      hInst = NULL;
      bLoad = 0;
      bLoadOK = 0;
      bSMDK = 0;
      iPriority = 88;
      };
    CDllInfo(CDllInfo &I)             // copy constructor
      {
      Name       = I.Name;
      FullName   = I.FullName;
      Type       = I.Type;
      hItem      = I.hItem;
      hInst      = I.hInst;
      bLoad      = I.bLoad;
      bLoadOK    = I.bLoadOK;
      bSMDK      = I.bSMDK;
      iPriority  = I.iPriority;
      Mdls.RemoveAll();
      POSITION pos = I.Mdls.GetHeadPosition();
      while (pos)
        Mdls.AddTail(I.Mdls.GetNext(pos));
      m_RqdSpcs.RemoveAll();
      for (int i=0; i<I.m_RqdSpcs.GetSize(); i++)
        m_RqdSpcs.Add(I.m_RqdSpcs[i]);
      m_DependentDLLList.RemoveAll();
      for (i=0; i<I.m_DependentDLLList.GetSize(); i++)
        m_DependentDLLList.Add(I.m_DependentDLLList[i]);
      };
    CDllInfo & operator=(CDllInfo & I)
      {
      Name       = I.Name;
      FullName   = I.FullName;
      Type       = I.Type;
      hItem      = I.hItem;
      hInst      = I.hInst;
      bLoad      = I.bLoad;
      bLoadOK    = I.bLoadOK;
      bSMDK      = I.bSMDK;
      iPriority  = I.iPriority;
      Mdls.RemoveAll();
      POSITION pos = I.Mdls.GetHeadPosition();
      while (pos)
        Mdls.AddTail(I.Mdls.GetNext(pos));
      m_RqdSpcs.RemoveAll();
      for (int i=0; i<I.m_RqdSpcs.GetSize(); i++)
        m_RqdSpcs.Add(I.m_RqdSpcs[i]);
      m_DependentDLLList.RemoveAll();
      for (i=0; i<I.m_DependentDLLList.GetSize(); i++)
        m_DependentDLLList.Add(I.m_DependentDLLList[i]);
      return *this;
      };
    int FindRqdSpecie(char* pSp)
      {
      if (pSp)
        for (int i=0; i<m_RqdSpcs.GetSize(); i++)
          if (strcmp(pSp, m_RqdSpcs[i]())==0)
            return i;
      return -1;
      };

  };

typedef CArray <CDllInfo, CDllInfo&> CBaseDllInfoArray;

class CDllInfoArray : public CBaseDllInfoArray
  {
  public:
    int FindByName(char* pName)
      {
      for (int i=0; i<GetSize(); i++)
        if (_stricmp(ElementAt(i).Name(), pName)==0)
          return i;
      return -1;
      };
    void SortByPriority();
  };

//===========================================================================

class SpCfgItem
  {
  public:
    Strng Name;
    Strng Comp;
    int Src;
    int HasVP;
    int Occ;
    int Index;
  };
typedef CArray <SpCfgItem, SpCfgItem&> BaseSpCfgSpecies;

class SpCfgSpecies : public BaseSpCfgSpecies
  {
  public:
    int FindBySpecieName(char* pName)
      {
      if (pName==NULL || pName[0]==0)
        return -1;
      const int len = strlen(pName);
      for (int i=0; i<GetSize(); i++)
        if (ElementAt(i).Comp.Len()==len && _stricmp(ElementAt(i).Comp(), pName)==0)
          return i;
      return -1;
      };
  };

//---------------------------------------------------------------------------

_FWDDEF(CMdlCfgSheet)
class DllImportExport CMdlCfgSheet : public CPropertySheet
  {
  DECLARE_DYNAMIC(CMdlCfgSheet)
  public:
    CMdlCfgSheet(DWORD LicCategory, char *CfgName, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CMdlCfgSheet(DWORD LicCategory, char *CfgName, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CMdlCfgSheet();

    CPropertyPage* pCfgPage;         
    CPropertyPage* pOptPage;         
    CPropertyPage* pSpcPage;   
    CPropertyPage* pMdlPage;   

    PropPageArray PropPages; 
    DWORD dwLicCategory; //Categories allowed by licensing
    
    flag IsStatsPage();
    
    //{{AFX_VIRTUAL(CMdlCfgSheet)
    public:
    virtual BOOL OnInitDialog();
    //}}AFX_VIRTUAL

  protected:
    //CSaveRestResHandle SRH;

    //void StoreCurPageNo();
    //{{AFX_MSG(CMdlCfgSheet)
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnNcDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  public:
    //Strng         m_CfgFiles;
    Strng         m_CfgFile;
    Strng         m_CfgFileTmp;
    Strng         m_DefaultDesc;
    
    CProfINIFile  Cfg;
    CDllInfoArray DLLs;
    CArray <Strng, Strng&> SMDKDLLs;
    CSfeSrvrBase *pSfeBase;
    SpCfgSpecies  m_Species;
    //Strng_List    Species;
    CSArray       m_FlashSpecies;
    CSArray       m_DllRqdSpecies;
    Strng         SDBDef;
    Strng         SDBCfg;
    SpMdlArray    SpModels;

    void    SetCfgFile(char *CfgFile) { m_CfgFile=CfgFile; };
    char *  GetCfgFile() { return m_CfgFile(); };
    //char *  GetCfgFiles() { return m_CfgFiles(); };

    int     GetLoadableDLLs(char * Path);
    BOOL    DoLoad(int i);
    int     LoadDLLs();
    void    LoadDLLPropPages();
    int     UnLoadDLLs();
    int     FindDLL(char* pDLLName) { return DLLs.FindByName(pDLLName); };
    BOOL    GetSpecieNames(char * Filename, int Src);

  };

//===========================================================================

class CMdlCfgBase : public CPropertyPage 
  {
  public:
    CMdlCfgBase(UINT IDD, CMdlCfgSheet * Sheet);

    flag            bInited;
    CMdlCfgSheet *  pSheet;
  
    CProfINIFile  & Cfg;
    SpCfgSpecies  & m_Species;
    CSArray       & m_FlashSpecies;
    CSArray       & m_DllRqdSpecies;
    SpMdlArray    & SpModels;
    CDllInfoArray & DLLs;
    Strng         & SDBDef;
    Strng         & SDBCfg;
  };

inline CMdlCfgBase::CMdlCfgBase(UINT IDD, CMdlCfgSheet * Sheet):
  CPropertyPage(IDD),
  Cfg(Sheet->Cfg),
  m_Species(Sheet->m_Species),
  SDBDef(Sheet->SDBDef),
  SDBCfg(Sheet->SDBCfg),
  m_FlashSpecies(Sheet->m_FlashSpecies),
  m_DllRqdSpecies(Sheet->m_DllRqdSpecies),
  SpModels(Sheet->SpModels),
  DLLs(Sheet->DLLs)
  {
  bInited=false;
  pSheet=Sheet;
  };

//===========================================================================

class CMdlCfgCfg : public CMdlCfgBase 
  {
  public:
    CMdlCfgCfg(CMdlCfgSheet * Sheet);
    //{{AFX_DATA(CMdlCfgCfg)
    enum { IDD = IDD_PP_MDLCFG_CFG };
    CComboBox	m_DefNetMode;
    CComboBox	m_PBNodeMode;
    CComboBox	m_PBFlowMode;
    CComboBox	m_PBHeatMode;
    CComboBox	m_DynNodeMode;
    CComboBox	m_DynFlowMode;
    CComboBox	m_DynHeatMode;
    CComboBox	m_MaxNodeMode;
    CComboBox	m_MaxFlowMode;
    CComboBox	m_MaxHeatMode;
    CComboBox	m_FlashComp;
    CComboBox	m_DefSpMdl;
    BOOL m_bPBAllowed;
    BOOL m_bDynAllowed;
	  //CString	m_AtmosPress;
	  CString	m_CfgFiles;
	  CString	m_Desc;
	  double	m_MaxPress;
	  double	m_MaxTemp;
	  double	m_MinPress;
	  double	m_MinTemp;
	  double	m_NormPress;
	  double	m_NormTemp;
	  double	m_StdPress;
	  double	m_StdTemp;
	  //}}AFX_DATA
	  //{{AFX_VIRTUAL(CMdlCfgCfg)
	  public:

	  virtual void OnOK();
	  virtual BOOL OnSetActive();
	  virtual BOOL OnKillActive();
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
    CArray <int, int> m_DefSpMdlIndex;
	  //{{AFX_MSG(CMdlCfgCfg)
	  virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeNetMode();
    afx_msg void OnSelchangeMaxNodeMode();
    afx_msg void OnSelchangeMaxFlowMode();
    afx_msg void OnSelchangeMaxHeatMode();
    afx_msg void OnProbalAllowed();
    afx_msg void OnDynamicAllowed();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

    void CheckModes();
  };

//===========================================================================

class CMdlCfgOpt : public CMdlCfgBase 
  {
  public:
	  CMdlCfgOpt(CMdlCfgSheet * Sheet);
	  //{{AFX_DATA(CMdlCfgOpt)
	  enum { IDD = IDD_PP_MDLCFG_OPT };
	  //}}AFX_DATA
	  //{{AFX_VIRTUAL(CMdlCfgOpt)
	  public:
	  virtual void OnOK();
	  virtual BOOL OnSetActive();
	  virtual BOOL OnKillActive();
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
	  //{{AFX_MSG(CMdlCfgOpt)
	  virtual BOOL OnInitDialog();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };

//---------------------------------------------------------------------------
/*
class CMdlCfgMdls : public CMdlCfgBase 
  {
  public:
    CMdlCfgMdls(CMdlCfgSheet * Sheet);
    //{{AFX_DATA(CMdlCfgMdls)
    enum { IDD = IDD_PP_MDLCFG_DLLS };
    CSelectTreeCtrl	m_DllTree;
    int		m_Sort;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CMdlCfgMdls)
	  public:
	  virtual void OnOK();
	  virtual BOOL OnSetActive();
	  virtual BOOL OnKillActive();
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
    void BuildTree();
    void SaveTree();
    BOOL UseDLLTree;
	  //{{AFX_MSG(CMdlCfgMdls)
	  virtual BOOL OnInitDialog();
    afx_msg void OnSort();
    //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };
*/
//---------------------------------------------------------------------------

typedef BOOL (*fn_GetDLLInfo)(MDLLInfo* pDLLInfo);
typedef CMdlCfgBase* (*fn_GetCfgPropPg)(int iPage, CMdlCfgSheet * pSheet);

class CMdlCfgSpcs : public CMdlCfgBase 
  {
  public:
	  CMdlCfgSpcs(CMdlCfgSheet * Sheet);
    //{{AFX_DATA(CMdlCfgSpcs)
    enum { IDD = IDD_PP_MDLCFG_SPCS };
    CComboBox	m_Filter;
    CStatic	m_What;
    CButton	m_Defaults;
    CStatic	m_SpDefDB;
    CStatic	m_SpCfgDB;
    CCustomListCtrl	m_SpDBList;
    CCustomListCtrl	m_SpCfgList;
    double	m_LoTol;
    double	m_HiTol;
    BOOL	m_bUseIdeal;
    //}}AFX_DATA

    CImageList ImgList;
    int iSpDefaults;
    int iPrevFilter;
#if WITHDEFAULTSPDB
    BOOL bExcludeDefSpDB;
    BOOL bPrevExcludeDefSpDB;
#endif
    BOOL bUseStdFns4H2O;
    BOOL bNumericStartTagsBad;
    BOOL bPrevNumericStartTagsBad;
    long m_lCfgFilePrjFileVerNo;

    //{{AFX_VIRTUAL(CMdlCfgSpcs)
	  public:
	  virtual void OnOK();
	  virtual BOOL OnSetActive();
	  virtual BOOL OnKillActive();
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL
  protected:
    //{{AFX_MSG(CMdlCfgSpcs)
    virtual BOOL OnInitDialog();
    afx_msg void OnSpadd();
    afx_msg void OnSpremove();
    afx_msg void OnSpmoveup();
    afx_msg void OnSpmovedn();
    afx_msg void OnSpsepar();
    afx_msg void OnDblclkSpdblist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEndlabeleditSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClickSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSetfocusSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeleteitemSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnInsertitemSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeydownSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSpdefaults();
    afx_msg void OnKeydownSpdblist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClickSpdblist(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelchangeFilter();
    afx_msg void OnChangeHitol();
    afx_msg void OnChangeLotol();
    afx_msg void OnUseideal();
    afx_msg void OnItemchangedSpcfglist(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

    void BuildSpDBList();
    void SaveSpList();
    void ChangeSpDefDB();
    char * GetCfgFile() { return pSheet->m_CfgFile(); };
    //char * GetCfgFiles() { return pSheet->m_CfgFiles(); };
    void EnableEditButtons();
    void UpdateWhat(UINT Type, LPCTSTR Comp, int Index);
    void UpdateTTols(LPCTSTR Comp, LPCTSTR LoTol, LPCTSTR HiTol, LPCTSTR Ideal, int Index);
    int FndFirstItem(int BOT_Phase);
    int FndLastItem(int BOT_Phase=-1);
  };

//---------------------------------------------------------------------------

#undef DllImportExport 

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

