// SysCADMarshalDoc.h : interface of the CSysCADMarshalDoc class
//
///////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SYSCADMARSHALDOC_H__F381BC7E_4BB2_46C3_88CC_BE732398B02C__INCLUDED_)
#define AFX_SYSCADMARSHALDOC_H__F381BC7E_4BB2_46C3_88CC_BE732398B02C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "slot.h"
#include "link.h"
#include "device.h"

enum HINTS
  {
  HINT_UPDATE_CFG,
  HINT_UPDATE_TAGS,
  HINT_UPDATE_TAG_VALUES,
  HINT_UPDATE_ONESLOTVALUE,
  HINT_UPDATE_ONELINKVALUE,
  //HINT_UPDATE_WIDTHS,
  HINT_CLEAR_ALL,
  HINT_UPDATE_CHANGES,
  HINT_CHANGE_VIEW,
  HINT_UPDATE_TRACKING,
  HINT_UPDATE_CHGLOG,
  HINT_UPDATE_EDITS,
  HINT_ERROR_ADDED,
  HINT_SETSELECTION,
  };

inline LPARAM MakeHint(long Hint, long Index=0, byte Flags=0) { return ((0xff & Hint)<<24) | ((Flags&0xf)<<20 )| (0x000fffff & Index) ; };
inline long TheHint(long lHint)    { return (long)((0xff000000 & lHint)>>24); }; 
inline byte HintFlags(long lHint)  { return (byte)((0x00f00000 & lHint)>>20); };
inline long HintIndex(long lHint)  { return (long)((0x000fffff & lHint)); };


const long SelectMask = 0x00ff0000;
const long ItemMask = 0x0000ffff;
const long WhatMask = 0xf0000000;

inline long ItemData(long What, long Select, long Index)
  {
  return(WhatMask & (What << 28)) | (SelectMask & (Select << 16)) | (ItemMask & Index);
  };
inline long ExtractWhat(long Data)
  {
  return(Data & WhatMask) >> 28;
  };
inline long ExtractSelect(long Data)
  {
  long  l = (Data & SelectMask);
  if(l == SelectMask) return -1;
  return l >> 16;
  };
inline long ExtractItem(long Data)
  {
  long  l = (Data & ItemMask);
  if(l == ItemMask) return -1;
  return l;
  };

class CTagView;
class CSysCADMarshalDoc :
  public CDocument
  {
  protected:  // create from serialization only
    CSysCADMarshalDoc(void);
    DECLARE_DYNCREATE(CSysCADMarshalDoc)
  // Attributes
  public:
  // Operations
  public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSysCADMarshalDoc)
  public:
    virtual BOOL  OnNewDocument(void);
    virtual void  Serialize(CArchive &ar);
    virtual BOOL  OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL  OnSaveDocument(LPCTSTR lpszPathName);
    virtual void  OnCloseDocument(void);
    virtual void  DeleteContents(void);
    //}}AFX_VIRTUAL
  // Implementation
  public:
    virtual       ~CSysCADMarshalDoc(void);
  #ifdef _DEBUG
    virtual void  AssertValid(void) const;
    virtual void  Dump(CDumpContext &dc) const;
  #endif
    void          SetTagView(CTagView *pView) { m_pTagView = pView; }
    void          LoadCodeBlocks(LPCSTR CfgCdBlkDB);
    void          LoadSlots(LPCSTR CfgSlotDB);
    void          SaveSlots(LPCSTR CfgSlotDB);
    eViews        m_lView;
    long          m_lDeviceSelect;
    long          m_lMarkedItem;
    CSelection    m_Selection;

    CFullValue    m_EdtValue;
    eConnSrcDst   m_EdtDst;
    long          m_EdtItem;

    //long        m_lSelDevice;
    bool          m_bChgLogAll;
    bool          m_bChgLogTrack;
    bool          m_bMonitor;
    bool          m_bViewCfg;
    bool          m_bViewStats;
    bool          m_bCfgBeginEnd;
    bool          m_bSimulateMode;

    CString       m_sCfgSlotDB;
    CString       m_sCfgCdBlkDB;
    CString       m_SlotTableName;

    CKwikList<CErrorItem *> m_ErrorList;

    long              m_lCfgSequence;
    CSlotCfgArray     m_SlotCfgs;
    CLinkCfgArray     m_LinkCfgs;
    CCdBlkCfgArray    m_CdBlkCfgs;
    CDeviceCfgArray   m_DeviceCfgs;
    CSlotCfgStringsArray m_SlotEdits;
    
    CLongArray        m_IndexOfSlotInList;
    CLongArray        m_IndexOfLinkInList;
    CLongArray        m_CdBlkListIndex;

    CKwikList<CChangeItem*> m_ChangeList;

    void          SetSlotCfg(long CfgSequence, CSlotCfgArray * pSlots);
    void          SetLinkCfg(long CfgSequence, CLinkCfgArray * pLinks);
    void          SetCdBlkCfg(long CfgSequence, CCdBlkCfgArray * pCdBlks);
    void          SetDeviceCfg(long CfgSequence, CDeviceCfgArray * pDevices);

    void          UpdateOneSlot(long SlotNo, CSlotValue * pSlotValue);
    void          UpdateOneLink(long LinkNo, CLinkValue * pLinkValue);
    void          UpdateOneSlotState(long SlotNo, long SlotState);
    void          UpdateOneLinkState(long LinkNo, long LinkState);
    void          XferChangeBlock(long wParam, CChangeBlock*pChangeBlock);
    bool          SetSelection(CSelection & Selection);

    void          AddEdit(CSlotCfgStrings * pCfg);
    void          ClearEdits();
    void          ApplyEdits();  

  protected:
    CTagView                *m_pTagView;

    // Temp
    void                    DoTestLoadpreset(long Start, long End, long Modulo);

  // Generated message map functions
  protected:
    //{{AFX_MSG(CSysCADMarshalDoc)
    afx_msg void OnViewConfiguraton(void);
    afx_msg void OnUpdateViewConfiguraton(CCmdUI *pCmdUI);
    afx_msg void OnViewStatistics(void);
    afx_msg void OnUpdateViewStatistics(CCmdUI *pCmdUI);
	  afx_msg void OnViewHexadecimal();
	  afx_msg void OnUpdateViewHexadecimal(CCmdUI* pCmdUI);
  	afx_msg void OnConfigurationLoaddb();
	  afx_msg void OnViewRefresh();
	  afx_msg void OnUpdateViewRefresh(CCmdUI* pCmdUI);
	  afx_msg void OnChglogAll();
	  afx_msg void OnUpdateChglogAll(CCmdUI* pCmdUI);
	  afx_msg void OnChglogTrk();
	  afx_msg void OnUpdateChglogTrk(CCmdUI* pCmdUI);
	  afx_msg void OnMonitor();
	  afx_msg void OnUpdateMonitor(CCmdUI* pCmdUI);
	afx_msg void OnConfigurationSavedb();
	afx_msg void OnConfigurationCommit();
	afx_msg void OnUpdateConfigurationCommit(CCmdUI* pCmdUI);
	afx_msg void OnConfigurationRefreshdevices();
	afx_msg void OnUpdateConfigurationRefreshdevices(CCmdUI* pCmdUI);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	//}}AFX_MSG
//    afx_msg LRESULT         OnUpdateAll(void);
//    afx_msg LRESULT         OnUpdateTagValues(void);
//    afx_msg LRESULT         OnUpdateTags(void);
//    afx_msg LRESULT         OnUpdateDevices(void);
    DECLARE_MESSAGE_MAP()
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CSysCADMarshalDoc)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
  public:
    afx_msg void OnFileSavestate();
    afx_msg void OnUpdateFileSavestate(CCmdUI *pCmdUI);
    afx_msg void OnFileLoadstate();
    afx_msg void OnUpdateFileLoadstate(CCmdUI *pCmdUI);
    afx_msg void OnModeSimulate();
    afx_msg void OnUpdateModeSimulate(CCmdUI *pCmdUI);
    afx_msg void OnSetValues();
    afx_msg void OnUpdateSetValues(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_SYSCADMARSHALDOC_H__F381BC7E_4BB2_46C3_88CC_BE732398B02C__INCLUDED_)
