 // SysCAD Copyright Kenwalt (Pty) Ltd 1995

#ifndef  __2D_CBSPL_H
#define  __2D_CBSPL_H

#include "cbspline.h"

#undef DllImportExport /* already defined */
#ifdef __2D_CBSPL_CPP
  #define DllImportExport DllExport
#else
  #define DllImportExport DllImport
#endif


//============================================================================

DEFINE_TAGOBJ(C2DCubicSpline);
class DllImportExport C2DCubicSpline : public C2DModel
  {
  public:
	CDVector Yderiv, Xderiv;
  CubicSpline  CSpline;
  public:
    C2DCubicSpline(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
  virtual ~C2DCubicSpline();
    
  virtual void   CopyModel(pC2DCubicSpline pMd); 
  virtual void   Clear();
  virtual void   BuildDataDefn(DataDefnBlk & DDB);

  virtual double Xy(double Yi);
  virtual double Yx(double Xi);
	virtual long GetNParms() { return 0; };
  virtual flag ReFit();

    // FxdEdtBookRef Overrides
  virtual void Build();
  virtual void Load(FxdEdtInfo &EI, Strng & Str);
  virtual long Parse(FxdEdtInfo &EI, Strng & Str);
  virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
  };

// ===========================================================================

#undef DllImportExport

#endif
