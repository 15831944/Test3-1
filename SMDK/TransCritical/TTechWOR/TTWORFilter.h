//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-06-09 12:18:08 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"

#endif
//#include "batc_bayer\BATCBayerSM.h"
#include <string>
//---------------------------------------------------------------------------









// Worlsely Model

const static int nClasses=25;
const static int nKernels=5;




class CFilter: public MBaseMethod
{
 public:


  CFilter(MUnitDefBase *pUnitDef,TaggedObject * pNd);
  virtual ~CFilter(void);

  void Init();
  void BuildDataFields();
  bool ValidateDataFields();
  void EvalProducts();
  void ClosureInfo(MClosureInfo & CI);


  void DoFilter(MStream &, MStream &);


 protected:
  bool bOnline;
  bool bVerbose;
  long iClassMethod;
  double dTankVol;

  double m_dCakeSolids;
  double m_dByPass;  
  double m_dSharp_Index;  
  double m_dCut_Size;     
  double m_dL0;
  double m_dxS;

  

  double L[nClasses+1];
  double Lav[nClasses+1];

  

  MStreamI Feed;
  MStreamI Condensate;
  bool bCondConnected;   // Do we have a condensate stream connected
  

  
  long iPSD;
  void displayStream(MStream &ms, int scrn);
  void displayPSD(MStream &ms, int scrn);  double dd[7][26];

#ifdef TTDEBUG
    bool bTTDebug;
    //    TTDbg dbg;
#endif //TTDEBUG

     
  
};

