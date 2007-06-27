//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __MD_METHOD_EX_H
#define  __MD_METHOD_EX_H

#if _MSC_VER < 1300
#error "Incorrect verion of compiler being used"
#endif

#pragma warning (disable : 4355) // this used in base member initializer list 

//#include "..\..\..\smdk\include\md_headers.h"
//
//#include "md_method_ex.h"

#ifdef __MD_METHOD_EX_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDK1)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


class DllImportExport MUnitDefBaseEx : public MUnitDefBase 
  {
  friend class CBBBase;
  friend class CBBSurge;
  friend class CBBTransfer;
  public:
    MUnitDefBaseEx(bool WithDevelopementChecks);
    virtual           ~MUnitDefBaseEx();
    virtual           MSubConstructBase * Construct(TaggedObject * pNd) = 0;

  public:
    // Set License Options... (should use ONE of the following)
    void              SetModelLicense(DWORD License);

  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
//Class MTransferUnitDefBase: Unit Definition base class for Transfer unit models.
class DllImportExport MTransferUnitDefBaseEx : public MUnitDefBaseEx
  {
  public:
    MTransferUnitDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks);
    virtual ~MTransferUnitDefBaseEx() {};
  };

//---------------------------------------------------------------------------

#ifdef SMDK_Debug
#define DEVCHECKSON   1
#else
#define DEVCHECKSON   0
#endif

//---------------------------------------------------------------------------
/*Macro for adding a SMDK transfer unit to the system. Chose the 'Class' and 'DLL'
names carefully as once these are released and used in projects they cannot easily
be changed, if they are changed old projects will not load (unless the scd_models.mdb
is carefully edited to make the equivalent changes!)
The parameters are:
  Obj   : The C++ class object name (ie your class as declared in the source code)
          (eg SimpleReactor)
  Class : A short string name for your class. Must follow valid tag naming rules
          (ie no spaces, etc) (eg "Reactor")
  DLL   : A short string name for the group this belongs to. This is usually the DLL group and is
          as a #define DLL_GroupName in a shared header file such as stdafx.h
          (eg #define DLL_GroupName "DemoUsr")
  */
#define DEFINE_TRANSFER_UNIT_EX(Obj, Class, DLL) \
class Obj##_UnitDef :  public MTransferUnitDefBaseEx \
  { \
  public: \
    Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL); \
    ~Obj##_UnitDef(); \
    virtual MSubConstructBase* Construct(TaggedObject * pNd); \
    virtual void GetOptions(); /* to be supplied by the user*/ \
  }; \
extern Obj##_UnitDef Obj##_UnitD; \
Obj##_UnitDef::Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL) : \
  MTransferUnitDefBaseEx(pClass, pSubClass, pShortDesc, pDLL, DEVCHECKSON) { GetOptions(); }; \
Obj##_UnitDef::~Obj##_UnitDef() { }; \
MSubConstructBase* Obj##_UnitDef::Construct(TaggedObject * pNd) { return new Obj(this, pNd); }; \
Obj##_UnitDef Obj##_UnitD(Class, NULL, "User:"##Class, DLL);

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//Class MSurgeUnitDefBase: Unit Definition base class for unit models with surge (contents).
class DllImportExport MSurgeUnitDefBaseEx : public MUnitDefBaseEx
  {
  public:
    MSurgeUnitDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks);
    virtual ~MSurgeUnitDefBaseEx() {};
  };

//---------------------------------------------------------------------------

/*Macro for adding a SMDK surge unit to the system. See description for DEFINE_TRANSFER_UNIT.*/
#define DEFINE_SURGE_UNIT_EX(Obj, Class, DLL) \
class Obj##_UnitDef :  public MSurgeUnitDefBase \
  { \
  public: \
    Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL); \
    ~Obj##_UnitDef(); \
    virtual MSubConstructBase* Construct(TaggedObject * pNd); \
    virtual void GetOptions(); /* to be supplied by the user*/ \
  }; \
extern Obj##_UnitDef Obj##_UnitD; \
Obj##_UnitDef::Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL) : \
  MSurgeUnitDefBaseEx(pClass, pSubClass, pShortDesc, pDLL, DEVCHECKSON) { GetOptions(); }; \
Obj##_UnitDef::~Obj##_UnitDef() { }; \
MSubConstructBase* Obj##_UnitDef::Construct(TaggedObject * pNd) { return new Obj(this, pNd); }; \
Obj##_UnitDef Obj##_UnitD(DLL##"*"##Class, NULL, "User:"##Class, DLL);

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//Class MControlUnitDefBaseEx: Unit Definition base class for unit models with surge (contents).
class DllImportExport MControlUnitDefBaseEx : public MUnitDefBaseEx
  {
  public:
    MControlUnitDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks);
    virtual ~MControlUnitDefBaseEx() {};
  };

//---------------------------------------------------------------------------

/*Macro for adding a Kenwalt SMDK surge unit to the system. See description for DEFINE_TRANSFER_UNIT.*/
#define DEFINE_CONTROL_UNIT_EX(Obj, Class, DLL) \
class Obj##_UnitDef :  public MControlUnitDefBaseEx \
  { \
  public: \
    Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL); \
    ~Obj##_UnitDef(); \
    virtual MSubConstructBase* Construct(TaggedObject * pNd); \
    virtual void GetOptions(); /* to be supplied by the user*/ \
  }; \
extern Obj##_UnitDef Obj##_UnitD; \
Obj##_UnitDef::Obj##_UnitDef(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR pShortDesc, LPCTSTR pDLL) : \
  MControlUnitDefBaseEx(pClass, pSubClass, pShortDesc, pDLL, DEVCHECKSON) { GetOptions(); }; \
Obj##_UnitDef::~Obj##_UnitDef() { }; \
MSubConstructBase* Obj##_UnitDef::Construct(TaggedObject * pNd) { return new Obj(this, pNd); }; \
Obj##_UnitDef Obj##_UnitD(Class, NULL, "User:"##Class, DLL);

//---------------------------------------------------------------------------

#undef DllImportExport

#endif
