//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef  __QGSPMDL_H
#define  __QGSPMDL_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif  
#ifndef __SP_MODEL_H
  #include <sp_model.h>
#endif  

//#define MDLBASE
//#define BASIC1
//#include "models.h"

//#ifndef __ALSPMDLB_H
//  #include <alspmdlb.h>
//#endif  
#ifndef __QBSPMDL_H
  #include <qbspmdl.h>
#endif  
 
#ifdef __QGSPMDL_CPP
  #define DllImportExport DllExport
#elif !defined(QALSPMDL)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//                            General QAL Specie Models
//
// ===========================================================================

DEFINE_SPMODEL(QSMBauxite)
DEFINE_SPMODEL(QSMLime)

// ==========================================================================
//
//                           Bauxite Specie Model
//
// ==========================================================================

class DllImportExport QSMBauxite : public SpModel
  {
  DEFINE_SPARES(QSMBauxite)
  protected:
    flag		       fDoCalc;
    double         dA_Moisture;
    double         dA_TCA, dA_TAA, dA_THA, dA_TotalSiO2;
    double         dA_Quartz, dA_Fe2O3, dA_TiO2, dA_P2O5, dA_TOOC, dA_LOI;
    static         CArray <int, int> LclParm;
  
  public:
    QSMBauxite(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QSMBauxite();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<QSMBauxite*>(pOther)) != NULL; };
    virtual SpModel * PreferredModel(pSpModel pOther)
      {
      if (pOther->Class()==&QSMBayerClass)
        return pOther;
      return this;
      };

    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual DDEF_Flags FixSpEditFlags(DDEF_Flags Flags, int iSpid, flag AsParms);
    //virtual double RefTemp() { return C_2_K(0.0); };
    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB);
  
  public:
    // ConditionBlk Override
    DEFINE_CI(QSMBauxite, SpModel, 3);
  };

// ==========================================================================
//
//                          Limestone Specie Model
//
// ==========================================================================

class DllImportExport QSMLime : public SpModel
  {
    DEFINE_SPARES(QSMLime)
  public:

    QSMLime(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~QSMLime();

    virtual bool   IsBaseClassOf(pSpModel pOther) 
      { return (dynamic_cast<QSMLime*>(pOther)) != NULL; };
    virtual SpModel * PreferredModel(pSpModel pOther)
      {
      if (pOther->Class()==&QSMBayerClass)
        return pOther;
      return this;
      };

    virtual double RefTemp() { return C_2_K(0.0); };
  };

// ==========================================================================

#undef DllImportExport

#endif
