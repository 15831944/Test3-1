//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    QAL Classifer Model. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2006-11-01 11:53:16 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $
//===========================================================================

#ifndef  __QALCLASSIFIER_H
#define  __QALCLASSIFIER_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __QALCLASSIFIER_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


//**/ #define TTDEBUG



//---------------------------------------------------------------------------
double linMap(double x, double x1, double y1, double x2, double y2);



const double t0 = 323.15;  // Reference temperature for kinetic rate
const double bayerRefTemp = 273.15+25.0;   // Bayer liquor reference temperature

const double clAExMin = .05;
const double clAExMax =.4;
const double clAEyMin = .1;
const double clAEyMax = 1.;
const double d45micron = 45.e-6;

double clAE(double);
double wtSub45(MStream &);

class QALClassifier : public MBaseMethod
{
    friend class CdPSolverFn;
    friend class PSDSolverFn2;
    MStream QI;
    MStream QOverflow;		// Temporary for calculation of viscosity etc;
    MStream QUnderflow;     // Temporary for underflow calculations

    bool bStdFn;   // Standard functionality
    long iStdMode;  // Mode of operation in std model 
     
       
  
    double dTankVolume;         // Classifier tank volume (m3)
    double dTankDiameter;		// Diameter, meters
    double dTankArea;			// Tank upflow area (m2)
    double dMaxAreaEfficiency;	//
    double dAFactor;
    double d_50c;               // Classifier cutpoint (micron)
    double dS_x;                // Solids conc in underflow from classification
    double dX_c;                // Critical classificatio zone volume 
    // as fraction of classifier vol
    double dS_cp;               // Compression point solids (ultimate settled) 
    double dCPPorosity;			// Compression Point Porosity
    double dk_c;				// Compression rate (1/hr)
    double dS_u;                // Desired underflow solids
    double dS_o;
    double dS_i;				// Solids in incoming stream
    double alpha;               // Classifier Cut Sharpness
    double drhoSol;				// Solids density
    double dSolidsFlow;
    double dPSDSolidsFlow;		// PSD solids content in solids stream
    double dSolidsUnderflow;	// Solids in underflow
    double dFu;					// Preliminary estimate for underflow
    double dFuR;				// Reqd flow in mode 1
    double dSR;					// Solids recovery in mode 2
    double dClassEff;			// Classifier Area Efficiency
    double dRe;                 // Reynolds no. for particle stokes flow
    double dOverflowViscosity;  // Slurry Viscosity
    double dOverflowLiqVisc;    // Liquid Viscosity
    double dOverflowDensity;
    double dOverflowSolidsDensity; 
    double dWtFr45;				// Wt fraction passing 45 screen
    double dOverflowDDiff;
    double dOverflowNu;         // Overflow kinematic visc, for reynolds No.
    double dUpflowVelocity;
    double dRf;
    double dVcom;
    double solidsToUnderflow;
    double dUnderflowSolids;	// Actual underflow solids
    double dMaxLSplit;			// Maximum split of incoming liquid to achieve
    // desired solids underflow
    double dOxUFlow;			// Oxalate level in underflow, if specified.
    long iPSD;					// What to display in PSD windows
    long iMode;					// How to control balance 
    // 0: uflow solids, 
    // 1, fraction, 
    // 2 flow rate

    long iOSplit;				// How to partition oxalate: 
    // 0 -with liquid, 
    // 1 with hydrate, 
    // 2 specify in uflow
    long iThermalLossMethod;
    double dTempDropRqd;
    double dThermalLossRqd;
    double dThermalLossO;
    double dThermalLossU;
  
    double dThermalLoss;
    double dThermTempDrop;



    int GIBBSITE;
    int OXALATE;


    void doFirstEstimate();
    double fF(double x);
    double fG(double x);
    double fRf();
    double fVcom();
    double upflowVelocity();
    double dP();         // Particle size at which settling==upflow
    void calcOverflowProperties();
    void doSolidsUnderflow();

#ifdef TTDEBUG
    void displayStream(MStream &ms, int scrn);
    void displayPSD(MStream &ms, int scrn);
    // Display at most 40 components - should allocate dynamically, but this
    // is just for debugging purposes.
    double dd[8][40];     
#endif


    friend class TTSolverFn;
 public:
    QALClassifier(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual void  ClosureInfo(MClosureInfo & CI);
    double EvalBalance(double x);
    double EvalBalance(double x, double y);

};

#endif
