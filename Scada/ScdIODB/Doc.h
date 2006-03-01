//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Doc.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  The Document class contains all stored data and
//                creation/deletion of devices/tags, etc.
//
//**************************************************************************

#ifndef __DOC_H__             // Only Include Once
#define __DOC_H__

class CTagView;

void InsertAlphabetically(OModbusTag* pTag, OTagList& pList );
void InsertByAddress(OModbusTag* pTag, OTagList& list );
void InsertInOrder(OModbusTag* pTag, OTagList& list );

const UINT MONITOR_ID = 45000;   // TagView in monitor mode
const UINT DDE_ID = 45001;       // app timer to invoke OnIdle->DDE toolkit
// hints to view::Update functions. (UPDATE_ALL is the default sent by MFC)
enum HINTS {UPDATE_ALL, UPDATE_DEVICES, UPDATE_TAGS,
            UPDATE_TAG_VALUES, ADD_DEVICE, CLEAR_ALL};

//*******************************************************************
class CDoc : public CDocument
{
protected: // create from serialization only
   CDoc();
   virtual ~CDoc();
   DECLARE_SERIAL(CDoc)

// Attributes
public:
   OTagGroup   devices;       // collection of devices
   OTagGroup*  pCurrent;
   //OPortList   ports;
   UINT        nIDClipFormat;
   CTagView*   pTagView;

// Operations
public:
   OTag*       FindTag( const CString& target, WORD RequestedType, ODevice** = NULL );
   void        OnProperties();
   void        DeleteCurrent();
   void        SetTagView(CTagView* pView) { pTagView = pView; }
   BOOL        IsRunning() {return run;}

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CDoc)
   public:
   virtual BOOL OnNewDocument();
   virtual BOOL OnOpenDocument( LPCTSTR lpszPathName );
   virtual void Serialize(CArchive& ar);
   //}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:
   WORD  nextIndex;  // for naming devices

   static DWORD ThreadStub(DWORD arg);
   DWORD  DataScanThread();
   HANDLE hScanThread;
   BOOL   run;    // flags the thread

   // Generated message map functions
protected:
   //{{AFX_MSG(CDoc)
   afx_msg void OnAddNewdevice();
   afx_msg void OnAddNewgroup();
   afx_msg void OnUpdateAddNewgroup(CCmdUI* pCmdUI);
   afx_msg void OnAddNewtag();
   afx_msg void OnUpdateAddNewtag(CCmdUI* pCmdUI);
   afx_msg void OnEditPaste();
   afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
   afx_msg void OnEditPorts();
   afx_msg void OnMonitor();
   afx_msg void OnUpdateMonitor(CCmdUI* pCmdUI);
  afx_msg LRESULT OnUpdateAddDevice(WPARAM, LPARAM);
  afx_msg LRESULT OnUpdateTags(WPARAM, LPARAM);
	afx_msg void OnUpdateall();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void DoAddNewDevice(ODevice *pNewCurrent, BOOL UpdateByMessage);
  
  };

/////////////////////////////////////////////////////////////////////////////
extern CDoc* theDoc;

#endif
