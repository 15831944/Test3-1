//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ALSPMDLB_H
#define  __ALSPMDLB_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif 
#ifndef __SP_MODEL_H
  #include "sp_model.h"
#endif 
#include "..\..\..\SMDK\Include\md_alumina.h"
#include "..\..\..\SMDK\Include\md_psd.h"

#ifdef __ALSPMDLB_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
// Virtual Base class for Bayer type Streams
//
// ===========================================================================

class DllImportExport SMBayerRqdComp
  {
  public:
    double  m_Temp;     // Temperature
    double  m_rh;       // mass hydrate/mass slurry ratio
    double  m_rl;       // mass liquor /mass slurry ratio
    double  m_ra;       // mass Alumina/mass slurry ratio
    double  m_rc;       // mass Na2O/mass slurry ratio
    double  m_rCl;      // mass NaCl/mass slurry ratio
    double  m_rF;       // mass NaF/mass slurry ratio
    double  m_rCO3;     // mass Na2CO3/mass slurry ratio
    double  m_rSO4;     // mass Na2SO4/mass slurry ratio
    double  m_rSi;      // mass SiO2(impurity)/mass slurry ratio
    double  m_rFe;      // mass Fe2O3(impurity)/mass slurry ratio
    double  m_rOX;      // mass Na2C2O4/mass slurry ratio
    double  m_rOC;      // mass other carbon/mass slurry ratio
    double  m_rSoda;    // mass soda/mass slurry ratio

    SMBayerRqdComp();
  };

#define TestBayerForWater 1

class DllImportExport SMBayerBase : public SpModelEx, public MIBayer
  {
  public:
    SMBayerBase(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SMBayerBase();

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);

    QUALITY_TYPECAST();
  };

// ==========================================================================
//
//                                  End
//
// ==========================================================================

#undef DllImportExport

#endif
