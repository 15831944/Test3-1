//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __SCDVERLIC_H
#define  __SCDVERLIC_H

//===========================================================================
//  License Options
//===========================================================================
/* KGA: Moved options below to licbase.h because any #define changes do NOT appear to affect dependincies!!!

#define CURTIN_ACADEMIC_LICENSE 0
#define FORCEMINESERVE          0 

#if (FORCEMINESERVE)
// for debug only
#define ALSOALLOWMINESERVEMDLS  0
#define ALLOWCNMVERSION         01

#elif (_MSC_VER>=1400)

#define ALSOALLOWMINESERVEMDLS  0

#ifdef _DEBUG
#define BYPASSLICENSING         01
#elif  _RELEASE
#define BYPASSLICENSING         01
#else
#define BYPASSLICENSING         01
#endif
#define ALLOWCNMVERSION         01

#else

#define ALSOALLOWMINESERVEMDLS  0

#ifdef _DEBUG
#define BYPASSLICENSING         0
#elif  _RELEASE
#define BYPASSLICENSING         0
#else
#define BYPASSLICENSING         0
#endif
#define ALLOWCNMVERSION         01

#endif

//===========================================================================
//
//===========================================================================

#ifdef DISTRIBUTION
#undef  BYPASSLICENSING  
#define BYPASSLICENSING         0
#undef  ALLOWCNMVERSION 
#define ALLOWCNMVERSION         0
#undef  ALSOALLOWMINESERVEMDLS
#define ALSOALLOWMINESERVEMDLS  0
#endif
*/
//===========================================================================
//
//===========================================================================

#endif



