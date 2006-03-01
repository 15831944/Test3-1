//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#include "stdafx.h"
#define __Matrix_TBEDT_CPP
#include "sc_defs.h"
#include "limits.h"
#include "gpfuncs.h"
#include "vectors.h"
#include "Matrix_TB.h"
#include "fixedit.h"
#include "Matrix_TBedt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

CMatrixTBEdt::CMatrixTBEdt(pFxdEdtView pView_, pCMatrixTB pFn_) :
  FxdEdtBookRef(pView_)
  {
  pFn = pFn_;
  }

//---------------------------------------------------------------------------

CMatrixTBEdt::~CMatrixTBEdt()
  {
  }

//---------------------------------------------------------------------------

void CMatrixTBEdt::StartBuild()
  {
  pFn->SetView(pView);
  pFn->StartBuild();
  }

//---------------------------------------------------------------------------

void CMatrixTBEdt::Build()
  {
  pFn->SetView(pView);
  pFn->Build();
  }

//---------------------------------------------------------------------------

void CMatrixTBEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  pFn->Load(EI, Str);
  }

//---------------------------------------------------------------------------

long CMatrixTBEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  pFn->Parse(EI, Str);
  
  return 0;
  }

//---------------------------------------------------------------------------

long CMatrixTBEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  pFn->SetView(pView);
  pFn->ButtonPushed(EI, Str);
  return 0;
  }

//---------------------------------------------------------------------------
