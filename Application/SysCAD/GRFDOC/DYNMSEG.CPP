//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include "grf3drw.h"

DynamicSegment::DynamicSegment(CEntInView* p, pchar Tag_, pchar Var_, CCnvIndex CnvInx, pchar CnvTxt_, pchar Type_, byte DynTyp, COLORREF Col_, double minv_, double maxv_, double CurVal, int segment)
  {
  ptr             = p;
  nxt             = NULL; 
  prv             = NULL; 
  Tag             = Tag_;
  Var             = Var_;
  CnvTxt          = CnvTxt_;
  Type            = Type_; 
  iDynTyp         = DynTyp;
  display_col     = Col_;
  display_min     = minv_;
  display_max     = Max(minv_, maxv_);
  display_sivalue = CurVal;
  display_last    = 0.0;
  display_freq    = 0;
  seg             = segment;
  iCnv            = CnvInx;
  }

//---------------------------------------------------------------------------

DynamicSegment::~DynamicSegment()
  {
  }

//---------------------------------------------------------------------------
