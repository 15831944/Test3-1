// PLC5.h : main header file for the PLC5 application
//

#if !defined(AFX_PLC5_H__4140FE62_5C31_4430_9CD7_EB0CDB54ED1B__INCLUDED_)
#define AFX_PLC5_H__4140FE62_5C31_4430_9CD7_EB0CDB54ED1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include <afxtempl.h> 
//#include "Callback.h"   // FSServer callback class
#include "PLC.h"   // FSServer callback class

//*********************************************************
//
//
//
//*********************************************************

//#include "ScdCOMWrapper.h"
#include "ScdCmdIF.h"

//*********************************************************
//
//
//
//*********************************************************

#include ".\OpcSrvr\Callback.h"   // FSServer callback class

#define WM_COMCMD   (WM_USER+1)
#define WM_COMEVT   (WM_USER+2)
#define WM_INITLOAD (WM_USER+3)

//*********************************************************

class CPLC5App;

class CPLC5OPCCallBack : public CMyOPCCallBack
  {
  public:
    CPLC5App * m_TheApp;
  
  CPLC5OPCCallBack(CPLC5App * TheApp, CPLC5Array & Plc) : 
    CMyOPCCallBack(Plc)
    {
    m_TheApp=TheApp;
    };
  };

//*********************************************************

class CTransferBlk 
  {
  public:
    bool      m_bOK;
              
    CString   m_sDstP;
    CString   m_sDstA;
    CString   m_sSrcP;
    CString   m_sSrcA;
    long      m_nLen;

    CAddress *m_pSrc;
    CAddress *m_pDst;

    CTransferBlk ()
      {
      m_bOK=false;
      m_nLen=0;
      m_pSrc=NULL;
      m_pDst=NULL;
      }
    void DoTransfer();
  };

class CTransferBlkArray : public CArray <CTransferBlk*, CTransferBlk*>
  {
  public:
    ~CTransferBlkArray()
      {
      for (int i=0; i<GetSize(); i++)
        delete GetAt(i);
      }
  };

//*********************************************************

class CForceBlk 
  {
  public:
    bool      m_bOK;
              
    CString   m_sDstP;
    CString   m_sDstA;
    CArray<float, float> m_Values;
    long      m_nLen;

    CAddress *m_pDst;

    CForceBlk ()
      {
      m_bOK=false;
      m_nLen=0;
      m_pDst=NULL;
      }
    void DoTransfer();
  };

class CForceBlkArray : public CArray <CForceBlk*, CForceBlk*>
  {
  public:
    ~CForceBlkArray()
      {
      for (int i=0; i<GetSize(); i++)
        delete GetAt(i);
      }
  };

/////////////////////////////////////////////////////////////////////////////
// CPLC5App:
// See PLC5.cpp for the implementation of this class
//

class CPLC5App : public CWinApp
{
public:
	CPLC5App();

	CScdCmdIF	      	 m_ScdCmdIF;
  bool             m_UpNAbout;

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPLC5App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

  // Implementation

	//{{AFX_MSG(CPLC5App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  //afx_msg LRESULT OnOpcCmd(WPARAM wParam, LPARAM lParam);
  //

};


extern CPLC5App theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLC5_H__4140FE62_5C31_4430_9CD7_EB0CDB54ED1B__INCLUDED_)
