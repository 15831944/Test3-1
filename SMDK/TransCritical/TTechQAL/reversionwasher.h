//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __REVERSIONWASHER_H
#define  __REVERSIONWASHER_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __REVERSIONWASHER_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


//---------------------------------------------------------------------------


const double t0 = 323.15;  // Reference temperature for kinetic rate
const double bayerRefTemp = 273.15+25.0;   // Bayer liquor reference temperature

class ReversionWasher : public MBaseMethod
{
    friend class SESolverFn;
 public:
    ReversionWasher(MUnitDefBase *pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual void EvalProducts();
    virtual void  ClosureInfo(MClosureInfo & CI);

    void displayStream(MStream & ms, int);
    double scandrettBalance(double x);

 protected:
    MStream QI1;
    MStream QI0;
    long           iThermalLossMethod;
    double         dTempDropRqd;
    double         dThermalLossRqd;
    double         dThermalLossO;
    double         dThermalLossU;

    double         dThermalLoss;
    double         dThermTempDrop;

    bool    bResTbyMud;       // Calculate residence time by mud volume or tank volume
    bool    bQALModel;        // Is stream model standard Bayer or QAL Bayer
    bool bRB;                 // Include reaction block
    double  dTankVolume;      // Total tank volume
    double  dMudLevel;        // Mud level as fraction of tank volume
    double  dMudReactionSolids; // Mud solids at interface, eg 300gpl
    double  dUnderflowSolids;   // Mud solids after squeezing out revert liquor, eg 500gpl 
    double  dK0;              // Kinetic rate constant, 490 in lab, 30 in plant!!!!
    double  dReversionTime;         // Time for mud stream reversion.
    double dDelta;            // Difference for numeric differentiation
    double  dMudTemp;               // Temperature in reverting mud bed
    double  dVolume1;
    double  dVolume2;
    double  dMass1;
    double dSolidsRho;
    double dInitReversionRate;
    double dInitAtoC;
    double dInitAConc;
    double dInitAluminaSat;
    double dScandrettEff;
    double dAScandrettEff;
    double dReactionHeat;
    double dWashBypass;   // Direct to overflow - this will eventually determine scandrett effcy
    double dCMud;         // Caustic level in incoming mud stream


    long iSteps;          // Number of steps for reversion integration
    long iODE;            // ODDE solver
    long iScand;          // Scandrett Species
    double  dt[12];      // temporaries for display
    double dd[3][40];    // Display at most 40 components - should allocate dynamically, but this 
    // is just for debugging purposes.

    MReactionBlk  m_RB;


};


class ReversionStream 
{
	double matmp;   /// Temporaries for saving stream data
	double mctmp;
	double mgtmp;
	void setX(double x);
	void restoreX() {MudStream.M[ALUMINA] = matmp; MudStream.M[CAUSTIC]=mctmp; MudStream.M[GIBBSITE]=mgtmp;}

protected:
	MStream & MudStream;
	MIBayer & MudB;
	double dK0;
	double mwtRatio;
	double dDelta;
	bool bQALModel;

	int GIBBSITE;
	int ALUMINA;
	int CAUSTIC;
	int ALUMINAS;

public:
	ReversionStream(MStream & ms_, MIBayer & mb_,  
		double k0_, double dD_, bool bQALModel = false);  
	double  dAdt() const;        // Basic reversion rate
	double  dAdMAL() const;      // change in Alumina concentration given change in XAL
	double dXALdt() {return dAdt()/dAdMAL();}
	double dXALdt(double x);
	void revert(double t_hr, int nSteps = 50);
	void revert1(double m);
	double revert2(double deltaT);
	MStream & getStream() {return MudStream;}

};

#endif
