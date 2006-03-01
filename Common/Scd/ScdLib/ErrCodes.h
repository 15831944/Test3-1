//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __ERRCODES_H
#define __ERRCODES_H

#ifdef __ERRCODES_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

const long E_None                       =     0;

//Automation Errors...
const long AE_None                      =     0;
const long AE_NullProjectName           =    -1;
const long AE_CannotLoadProject         =    -2;
const long AE_NotStopped                =    -3;
const long AE_NullTag                   =    -4;
const long AE_ProjectNotOpen            =    -5;
const long AE_TagNotFound               =    -6;
const long AE_TagNotNumeric             =    -7;
const long AE_TimeoutWaitingForStart    =    -8;

//Executive Errors...
const long EE_None                      =     0;
const long EE_StartError                = -1000;

//General Errors...
const long GE_None                      =     0;
const long GE_LicNotAllowed             = -3000;

//Automation Notes...
const long AN_ProjectAllreadyOpen       =   -10001;

//===========================================================================
#undef DllImportExport

#endif

