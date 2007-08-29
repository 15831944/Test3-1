//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   General PSD Numbers Helper Class. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2007-07-01 21:05:08 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#ifndef  __PSD_H
#define  __PSD_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif



// Helper Classes for basic PSD numbers information


class AgglomKernel
{
  int nClasses;
 public:
  AgglomKernel() {};
  AgglomKernel(int , double, double *D);
  double beta[30][30];     // Need to dynamically allocate these...
  double bsplit[30][30];
};







class PSD 
{
  double *n;    // Local numbers with corrections
  double *ndot; // Rate of change of numbers
  double *D;    // Local copy of diameters in microns
  double solidsDensity;  //Solids Density of PSD species
  double m_dVol;
  double m_dArea;
  double m_dNumber;
  double m_dVolumeCorrection;
  double m_dAreaCorrection;
  int nClasses;
 public:
  //PSD() {}
  //PSD(MStream);
  //PSD(MStream &, int);
  PSD(MStream &, MSpeciePtr&);
  ~PSD();

  double getArea() {return m_dArea;}
  double getVol() {return m_dVol;}         // Get total volume of particles in PSD
  double getVol(int);    // Get volume in bin i
  double getMass();        
  double getMass(int);
  double getFrac(int);
  double getD(int i){return D[i];}
  double getN();          // Fetch all the numbers to local N, return total
  double getN(int i) {return n[i];}     // Fetch numbers in bin i
  double getNTot() {return m_dNumber;}
  void putN(MStream &);  // Adjust the numbers in a stream
  

  /// These are base models for growth and nucleation, returning additional mass PSDs
  double Nucleation(double N0, int binN=1);   // Nucleate N0 new particles to bin N
  double Growth(double deltaR, int from=1);   // Growth of deltaR
  void Agglomeration(double, AgglomKernel&);
};

#endif    // __PSD_H
