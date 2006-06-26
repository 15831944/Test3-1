//===========================================================================
//=== SysCAD SMDK - Hyprod PSD Definition 2006 : Alcan, Kenwalt           ===
//===========================================================================

#ifndef PSDPERSONALITY_H
#define PSDPERSONALITY_H

#include "md_share1.h"
#include "md_method.h"
#include "md_psd.h"

enum eAlcanPSDTypes { eAlcanPSD_Log, eAlcanPSD_Rosin, eAlcanPSD_Table, eAlcanPSD_File};

const int idDX_Set = 100;
const double HPDSize = 4.0; //should retrieve this from PSD Defn used

class PSDPersonality
{
  double * FracPass;
  double * Intervals;

  struct
      {
      double   m_dMedian;
      double   m_dQS;    
      double   m_dm149;  
      double   m_dm106;  
      double   m_dm74;   
      double   m_dm53;   
      double   m_dm45;   
      double   m_dm33;   
      double   m_dm20;   
      double   m_dq1;    
      double   m_dq3;    
      double   m_dd10;   
      double   m_dd90;
      double   m_dCrit;
      } m_Rqd;//, m_Meas;

  protected:
    MIPSD *m_PSD;

    eAlcanPSDTypes m_eType;
public:
  PSDPersonality();
  ~PSDPersonality();
  void Init(MIPSD *PSD);
  void BuildDataDefn(MDataDefn & DD);
  void ReplaceDistribution(MIPSD *PSDO);
  bool SetDistribution();

  double        NormInv(double x);
  double        NormDist(double x);
  double        NormDens(double x);

  void          RegressXY (const long Count, const double x[], const double y[], double RegRslt[]);
  void          RegressLN (const long Count, const double Diam[], const double WPrct[], double RegRslt[]);
  void          RegressRR (const long Count, const double Diam[], const double WPrct[], double RegRslt[]);
  void          HyprodDataFromLN (double RegRslt[]);
  void          HyprodDataFromRR (double RegRslt[]);
  void          HyprodDataFromTable(long Count, double Diam[], double WPrct[], double RegRslt[]);
  void          HyprodDist(long NIntervals, double SizeClass[], double wCumul[]);
};

#endif