//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __SCDVERLIC_H
#define  __SCDVERLIC_H

//===========================================================================
//  License Options
//===========================================================================

#if _MSC_VER>=1400

#ifdef _DEBUG
#define BYPASSLICENSING       01
#elif  _RELEASE
#define BYPASSLICENSING       0
#else
#define BYPASSLICENSING       01
#endif
#define ALLOWCNMVERSION       01

#else

#ifdef _DEBUG
#define BYPASSLICENSING       01
#elif  _RELEASE
#define BYPASSLICENSING       0
#else
#define BYPASSLICENSING       01
#endif
#define ALLOWCNMVERSION       01

#endif
//===========================================================================
//
//===========================================================================

#endif



