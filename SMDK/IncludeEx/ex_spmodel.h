//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __EX_SPMODEL_H
#define  __EX_SPMODEL_H

#if _MSC_VER < 1300
#error "Incorrect verion of compiler being used"
#endif

#pragma warning (disable : 4355) // this used in base member initializer list 

#ifdef __EX_SPMODEL_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(DEVLIBEX)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

//---------------------------------------------------------------------------

class DllImportExport MSpModelDefBaseEx : public MSpModelDefBase 
  {
  public:
    MSpModelDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR Desc, unsigned long Flgs, LPCTSTR DLL);
    virtual ~MSpModelDefBaseEx();
    virtual MSubConstructBase* Construct(TaggedObject * pNd) = 0;
  };

//---------------------------------------------------------------------------

#define DEFINE_SPECIEMODEL_EX(Obj) \
class Obj##SpMdlDef :  public MSpModelDefBaseEx \
  { \
  public: \
  Obj##SpMdlDef(LPCTSTR pName, LPCTSTR pClassId, LPCTSTR pShortDesc, LPCTSTR pDesc, unsigned long Flgs, LPCTSTR pDLL); \
  virtual MSpModelBase * Construct(TaggedObject * pNd); \
  }; \
  extern Obj##SpMdlDef Obj##_D; \
  extern Obj * gs_p##Obj;  

#define IMPLEMENT_SPECIEMODEL_EX(Obj, Class, ShortDesc, Desc, DLL) \
  Obj##SpMdlDef::Obj##SpMdlDef(LPCTSTR pName, LPCTSTR pClassId, LPCTSTR pShortDesc, LPCTSTR pDesc, unsigned long Flgs, LPCTSTR pDLL) : \
  MSpModelDefBaseEx(pName, pClassId, pShortDesc, pDesc, Flgs, pDLL) { }; \
  MSpModelBase * Obj##SpMdlDef::Construct(TaggedObject * pNd) { return new Obj(pNd); }; \
  Obj##SpMdlDef Obj##_D(Class, NULL, ShortDesc, Desc, 0, DLL); \
  Obj * gs_p##Obj=NULL;

//---------------------------------------------------------------------------

#undef DllImportExport

#endif
