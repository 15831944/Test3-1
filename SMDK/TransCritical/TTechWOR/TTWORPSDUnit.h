//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-05-16 05:31:55 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"

#endif
#include "brahmapsd.h"


#ifdef TTDEBUG
#include "..\TTCommon\ttdbg.h"
#else
#include "..\TTCommon\ttnodbg.h"
#endif




#include <sstream>
#include <iomanip>
//---------------------------------------------------------------------------





// Worlsely Model




class CPSDUnit: public MBaseMethod
{
 public:

  BrahmaPSD b;
  CPSDUnit(MUnitDefBase *pUnitDef,TaggedObject * pNd);
  virtual ~CPSDUnit(void);

  void Init();
  void BuildDataFields();
  bool ValidateDataFields();
  void EvalProducts();

  void DoPSDUnit(MStream &);
  void displayPSD(BrahmaPSD&);
  

 protected:
  bool bDisplayOnly;
  long iPSDType;
  long iDisplayType;
  double m_dCnv;
  double m_dMassSolids;
  double m_dVolSlurry;
  double dd[32];
  double dd1[32];

#ifdef TTDEBUG
    bool bTTDebug;
    TTDbg dbg;
#endif


  
  
};

