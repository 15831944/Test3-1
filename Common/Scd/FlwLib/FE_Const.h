//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __FE_CONST_H
#define __FE_CONST_H

// =========================================================================
// FlwNode Status Bits
const dword FNS_OK        = 0x0000000;
const dword FNS_Error     = 0x0000001;
const dword FNS_Warning   = 0x0000002;
const dword FNS_On        = 0x0000004;
const dword FNS_Off       = 0x0000008;
const dword FNS_LFlwP     = 0x0000010;
const dword FNS_LNoFlw    = 0x0000020;
const dword FNS_Tear      = 0x0000040;
const dword FNS_MacMdl    = 0x0000080;
const dword FNS_NettQmP   = 0x0000100;
const dword FNS_NettQmN   = 0x0000200;
const dword FNS_UFlw      = 0x0000400;
const dword FNS_UNoFlw    = 0x0000800;
const dword FNS_IsSrc     = 0x0001000;
const dword FNS_IsSnk     = 0x0002000;
const dword FNS_InActive  = 0x0004000;
const dword FNS_LFlwN     = 0x0008000;
const dword FNS_Hold      = 0x0010000;
const dword FNS_Marked    = 0x0020000;
const dword FNS_LnkXfer   = 0x0040000;
const dword FNS_LnkSimple = 0x0100000;
const dword FNS_LnkLinear = 0x0200000;
const dword FNS_LnkFull   = 0x0400000;
//const dword FNS_IsELnk    = 0x0100000;
//const dword FNS_IsALnk    = 0x0200000;
//const dword FNS_IsElec    = 0x0400000;

// =========================================================================
typedef enum ODE_Methods    
  {
  ODE_Euler,
  ODE_RK2, 
  ODE_RK4, 
  ODE_LastMethod
  };
// =========================================================================
#define BadListMax 10

// =========================================================================
#endif
