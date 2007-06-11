//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SQ_LOCK_H
#define  __SQ_LOCK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __SP_MODEL_H
  #include "sp_model.h"
#endif  

#ifdef __SQ_LOCK_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(SQLockup);
class DllImportExport SQLockup : public SpQuality 
  {
  DEFINE_SPARES(SQLockup)
  public:
    //XYFn           SizeFn;
    CDVector       Lockup;

    SQLockup(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SQLockup();

    DEFINE_QUALITYPTR(SQLockup, "Lockup");

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    
    virtual void   ExchangeSpecies(SpQuality * Other);
    virtual void   ZeroMass();
    virtual void   ZeroDeriv();
    virtual void   ScaleMass(PhMask PhaseM, double Mult);
    virtual void   ScaleMass(CIArray & SpIds, double Mult);
    virtual void   ScaleMass(CSysVector  &Scl, double Mult);
    virtual void   SetMassF(CSysVector  &M1, SpQuality * pQual2);
    virtual void   AddMassF(CSysVector  &M1, SpQuality * pQual2, CSysVector  &M2);
    virtual void   SubMassF(CSysVector  &M1, SpQuality * pQual2, CSysVector  &M2);
    virtual void   Copy(pSpModel pMdl2, SpQuality * pQual2);
    virtual void   AddDeriv(pSpModel pMdl2, SpQuality * pQual2, double Sgn_);
    virtual void   AddDiscrete(pSpModel pMdl2, SpQuality * pQual2, double Sgn_);
    virtual void   ODEOperate(CODEDataBlock & ODB);
  };

// ===========================================================================

#undef DllImportExport 

#endif
