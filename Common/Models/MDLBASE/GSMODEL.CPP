//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "scdver.h"

#if WITHDOUBLEGSMODEL
  #include "GSMODEL.double.cpp"
#else
  #include "GSMODEL.float.cpp"
#endif
