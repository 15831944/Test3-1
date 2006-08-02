//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __SCDVERLIC_H
#define  __SCDVERLIC_H

//===========================================================================
//  License Options
//===========================================================================

#define FORCEMINESERVE          0 



#if (FORCEMINESERVE)
// for debug only
#define ALSOALLOWMINESERVEMDLS  0
#define ALLOWCNMVERSION         01

#elif (_MSC_VER>=1400)

#define ALSOALLOWMINESERVEMDLS  01

#ifdef _DEBUG
#define BYPASSLICENSING         01
#elif  _RELEASE
#define BYPASSLICENSING         01
#else
#define BYPASSLICENSING         0
#endif
#define ALLOWCNMVERSION         01

#else

#define ALSOALLOWMINESERVEMDLS  01

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

//===========================================================================
//
//===========================================================================

#endif



