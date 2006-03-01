//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __OP_DB_H
#define  __OP_DB_H

#ifndef __SP_DBEXTRA_H
  #include "sp_dbextra.h"
#endif

#if defined(__OP_DB_CPP)
  #define DllImportExport DllExport
#elif !defined(RTTS)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

enum ePropNames { enMaterial, enDescription/*, enLump, enGroup, enDensity, enDI, 
//                  enTheorFe, enTheorSi, enTheorAl, enTheorP, enTheorMn, enTheorLOI, 
                  enK1C, enA, enb, enAb, enRockA, enUTS, enCritUTS, enPPV, enb80, enUCS*/ };

class DllImportExport OreProp : public SpecieProp
  {
  friend class OrePropDataBase;
  protected:
  public:
    //OreProp();
    //virtual ~OreProp();

    char*         Material    () { return sProp[enMaterial    ](); };
    char*         Description () { return sProp[enDescription ](); };
/*    char*         Group       () { return sProp[enGroup       ](); };
    double        Lump        () { return dProp[enLump        ]; };
    double        Density     () { return dProp[enDensity     ]; };
    double        DI          () { return dProp[enDI          ]; };
//    double        TheorFe     () { return dProp[enTheorFe     ]; };
//    double        TheorSi     () { return dProp[enTheorSi     ]; };
//    double        TheorAl     () { return dProp[enTheorAl     ]; };
//    double        TheorP      () { return dProp[enTheorP      ]; };
//    double        TheorMn     () { return dProp[enTheorMn     ]; };
//    double        TheorLOI    () { return dProp[enTheorLOI    ]; };
    double        K1C         () { return dProp[enK1C         ]; };
    double        A           () { return dProp[enA           ]; };
    double        b           () { return dProp[enb           ]; };
    double        Ab          () { return dProp[enAb          ]; };
    double        RockA       () { return dProp[enRockA       ]; };
    double        UTS         () { return dProp[enUTS         ]; };
    double        CritUTS     () { return dProp[enCritUTS     ]; };
    double        PPV         () { return dProp[enPPV         ]; };
    double        b80         () { return dProp[enb80         ]; };
    double        UCS         () { return dProp[enUCS         ]; };*/

    //calculated...
//    double        T10(double Ecs) { return dProp[enA] * (1.0 - Exps(-1.0*dProp[enb]*Ecs)); };
    double        CompPercentage(int EnIndex) { return dProp[EnIndex]; };
  };

//---------------------------------------------------------------------------

class DllImportExport OrePropDataBase : public CSpeciePropDataBase
  {
  public:
    OrePropDataBase();
    virtual ~OrePropDataBase();

    int           InitAndLoad();
    OreProp *     FindSolid(char* Tag) { return (OreProp*)CSpeciePropDataBase::FindSolid(Tag); };
    OreProp &     operator [](int s) 
      { 
      SpecieProp* px = ((*((CSpeciePropDataBase*)this))[s]);
      OreProp *   p1 = (OreProp *)px;
      return (*p1);
      }

    //assays...
    CArray<CString, CString&> AssayCompNames;
    CArray<int, int&> AssayCompEnIndex;

    int AssayCompCount() { return AssayCompNames.GetSize(); };
    const char* AssayName(int Index) { return (const char*)AssayCompNames[Index]; };
    int AssayEnIndex(int Index) { return AssayCompEnIndex[Index]; };
  
  protected:
    void CopyToSMDK();
    SpeciePropCfgItem* pCfgItems;
  };

extern DllImportExport OrePropDataBase ODB; //global Ore Properties Database

// ===========================================================================

#undef DllImportExport

#endif

