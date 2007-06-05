//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    QAL Classifer Model. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2007-06-04 01:30:46 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#ifndef  __TTDYNTEST_H
#define  __TTDYNTEST_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __TTDYNTEST_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif









//---------------------------------------------------------------------------


class CDynTestTank : public MBaseMethod
{
 public:


  CDynTestTank(MUnitDefBase *pUnitDef,TaggedObject * pNd);
  virtual ~CDynTestTank(void);

  void Init();
  void BuildDataFields();
  bool ValidateDataFields();
  void EvalProducts();
  //void ClosureInfo(MClosureInfo & CI);
  double NucleationRate();
 protected:
  // Streams 

  MStream Feed;
  MStream Tank;     // Effective Stream for tank, calculations done on this stream\

    // Booleans
  bool bOnline;
  bool bGrowthOn;
  bool bAgglomOn;
  bool bNuclOn;
  bool bAttritionOn;
  bool started;
  // Doubles


  double dTankVol;   // Actual volume of tank
  double dTankVolume; // Volume of fluid in tank
  double dTankLevel;
  double dTankDiameter;
  double dTankHeight;
  double dTankMass;    // Total Mass of fluid in tank
  double dQvin;
  double dQvout;
  double dQmin;
  double dQmout;  
  double dTin;   
  double dTout;
  double dDeltaT;    // Timestep for iteration
  double dMassInflow;
  double dMassInflow1;
  

  long lPBType;   // Particle Balance Type

  double M[21], F[21];
};

#endif
