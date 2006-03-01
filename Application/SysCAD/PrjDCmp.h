//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __PRJDCMP_H
#define __PRJDCMP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __TAGDB_H
  #include "tagdb.h"
#endif

//------------------------------------------------------------------------

class CRootObj
  {
  public:
    Strng Tag;
    Strng Class;
  };

typedef CMap <char*,char*,CRootObj*,CRootObj*> CNdMap;
typedef CList <CRootObj, CRootObj&> CNdList;

//------------------------------------------------------------------------

class CObjTag
  {
  public:
    flag Found;
    Strng Str;
    Strng ClsTag;
    CPkDataItem* pPItem;

    CObjTag() 
      { 
      Found=false;
      pPItem=NULL;
      };
    CPkDataItem& Item()
      {
      return *pPItem;
      };
  };

typedef CSCDList <CObjTag, CObjTag&> CObjTagList;
typedef CSCDListIter <CObjTagList, CObjTag*> CObjTagListIter;
typedef CMap <char*,char*,CObjTag*,CObjTag*> CObjTagMap;

//------------------------------------------------------------------------

typedef CList <Strng, Strng&> CClsTagList;
typedef CMap <char*,char*,Strng*,Strng*> CClsTagMap;

//------------------------------------------------------------------------
// CPrjDataCompare dialog

class CPrjDataCompare : public CDialog
{
// Construction
public:
	CPrjDataCompare(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrjDataCompare)
	enum { IDD = IDD_PRJCOMPARE };
	CString	m_Prj1;
	CString	m_Prj2;
	double	m_Tolerance;
	CString	m_Report1;
	BOOL	m_FirstInClass;
	BOOL	m_MissingTags;
	BOOL	m_MissingValues;
	BOOL	m_ValuesDifferent;
	//}}AFX_DATA

  CNdMap  NdMaps[2];
  CNdList Nds[2];
  
  CClsTagList ClsTagLst;
  CClsTagMap  ClsTagMap;

  FILE * hRpt;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjDataCompare)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrjDataCompare)
	virtual void OnOK();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  void DoBrowse(CString & Prj);
  flag DoMessage(char * ClsTag0);
  flag CompareObject(CObjTagList& List0, CObjTagMap & Map0, 
                     CObjTagList& List1, CObjTagMap & Map1, 
                     FILE* hRpt, double Tol);
  flag CompareObjects(CXM_ObjectData *pObjData0, CXM_ObjectData *pObjData1);


public:
  flag DoCompare();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
