#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//---------------------------------------------------------------------------

class HatchHeater : public MBaseMethod
  {
	friend class CST_CondensingSolverFn;
	friend class CST_CondensateFinder;

	public:


    HatchHeater(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual ~HatchHeater(void);

    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();
    virtual bool EvalJoinPressures();
    virtual void EvalProducts();
    virtual void ClosureInfo(MClosureInfo & CI);


    // Macro Model (Flash Train)...
    virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval)  {};
    virtual bool    MacroMdlValidNd(int iIONo)                { return true; };
    virtual void    MacroMdlAddNd(int iIONo)                  {};
    virtual void    MacroMdlRemoveNd(int iIONo)               {};
    virtual CMacroMdlBase* MacroMdlActivate()                 { return m_FTC; };
    virtual void    MacroMdlDeactivate()                      {};

  protected:

	void	  DoCondensingHeater(MStream& SteamIn, MStream& SlurIn, MStream& CondOut, MStream& SlurOut);
	double	CalcTco(MStream& ShellI, MStream& TubeI, MStream& ShellO, MStream& TubeO);
	double	CalcAvgCp(int opt,MStream& FlowI, MStream& FlowO);
	double	CalcExitCp(int opt,MStream& FlowI, MStream& FlowO);
	
  CString			m_sDesc;
  long			  CalcMode;
  bool			  bOnLine;
  double			dActualDuty;
  double			dU;
  double			dA;
  double      dLMTD;
  double      m_dDuty;
  double			dDamping;

	double			HeatLoss;
	double			HeatLost;
	CString			HeaterSize;

	// Slurry Side
	double			SlurInT;
	double			SlurOutT;
	double			SlurHeatGain;
	double			ReactHeat;
	double			EndoHeat;

	// Steam / Liquor Side
	double			SteamInT;
	double			CondOutT;
	double			StmHeatDrop;

	MReactionBlk	m_RB;				// Reaction block
    MVLEBlk         m_VLE;				// VLE helper class
    MFT_Condenser   m_FTC;				// Flash Train Steam Demand (sink)

	long			DoPrint;
  long      DoPrint2;
  double    CpTestTol;
  long      EqualCp;
  };

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

class CST_CondensateFinder : public MRootFinder
{
	public:
    CST_CondensateFinder(HatchHeater * pUnit, MStream &TubeI, MStream &ShellI, MStream &TubeO, MStream &ShellO);
    ~CST_CondensateFinder();
    double Function(double x);
    double Function2(double x);
    double Function3(double x);

	protected:
	
	MStream&	m_TubeI;
    MStream&	m_ShellI;
    MStream&	m_TubeO;
    MStream&	m_ShellO;

    HatchHeater&	m_Unit;
    MVLEBlk&		m_VLE;

    static MToleranceBlock s_Tol;
};
