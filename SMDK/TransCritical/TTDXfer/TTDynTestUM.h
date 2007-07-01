//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    QAL Classifer Model. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2007-06-29 16:30:25 Rod Stephenson Transcritical Pty Ltd>
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

#include "..\ttcommon\psd.h"







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
  void SetState(MStatesToSet);
 
  //void ClosureInfo(MClosureInfo & CI);
 protected:
  // Streams 
  double dd[30];

  MStream Feed;
  MStream Tank;     // Effective Stream for tank, calculations done on this stream
  MStream SavedTank;  // Save the tank stream contents for efficient recalculation

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

  double m_dSSat;    // Supersaturation
  double m_dSSA;     // Specific surface area
   

  long lPBType;   // Particle Balance Type

  double Nucleation();       // Given numbers, determine new numbers
  double Agglomeration();
  double Growth();

  double AdjustMass();
  void doPrecip();
  void displayPSD(MStream &);
  double M[21];
  long iPSD;
  AgglomKernel ak;
};



double AgglomRate(double);
double NucleationRate(double ssat, double ssa); 
double GrowthRate(double);



#endif
