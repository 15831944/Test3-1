#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
//#include "..\..\Transcritical\TTCommon\STHx.h"


//---------------------------------------------------------------------------

enum OpMode { OM_Simple, OM_Condensing, OM_Evaporating, OM_CondEvap };

struct HXSideVars
{
	HXSideVars();
	OpMode	m_eOpMode;
	double	m_dQm;
	double	m_dCp;
	double	m_dTi;
	double	m_dTo;
	double	m_dPi;
	double	m_dPo;
	double	m_ddT;
	double	m_dSatT;
	double	m_dSatP;
	double	m_dSatPP;
	double	m_ddHz;

	double	m_dInHz;

	void	BuildDataFields(MDataDefn DD);
	void	ReadInStream(const MStream& InStream);
	void	ReadOutStream(const MStream& OutStream);
};

class CKWATubeDigester : public MBaseMethod//, CSTHx
{
	friend class HXSolver;
	friend class CondensingHXSolver;
	friend class EvaporatingHXSolver;
	friend class SimpleHXSolver;
	friend class CondEvapHXSolver;
public:
	CKWATubeDigester(MUnitDefBase * pUnitDef, TaggedObject * pNd);
	virtual ~CKWATubeDigester(void);

	virtual void    Init();
	virtual void    BuildDataFields();
	virtual bool	ValidateDataFields();
	virtual bool    ExchangeDataFields();
	virtual bool	PreStartCheck();
	virtual bool    ConfigureJoins();
	virtual bool    EvalJoinPressures();
	virtual void    EvalProducts();
	virtual void    ClosureInfo(MClosureInfo & CI);

	// Macro Model (Flash Train)...
	virtual void    MacroMdlEvaluate(eScdMacroMdlEvals Eval)  {};
	virtual bool    MacroMdlValidNd(int iIONo)                { return true; };
	virtual void    MacroMdlAddNd(int iIONo)                  {};
	virtual void    MacroMdlRemoveNd(int iIONo)               {};
	virtual CMacroMdlBase* MacroMdlActivate()                 { return m_FTC; };
	virtual void    MacroMdlDeactivate()                      {};

protected:
	//SMDK Objects:
	MReactionBlk    m_HotRB, m_ColdRB;     // Reactions
	MVLEBlk         m_HotVLE, m_ColdVLE;    // Vapor-Liquid equilibrium
	MFT_Condenser   m_FTC;    // Flash Train
    MEnvironHXBlk   m_HotEHX, m_ColdEHX;    // Environmental

	//Input Variables
	bool			m_bOn;
	OpMode			m_eOperationMode;
	double			m_dHTC;
	double			m_dActualArea;
	double			m_dLMTDFact;
	double			m_dReqQmVent;
	bool			m_bIgnoreAreaLimit;

	//Output Variables
	double			m_dUA;
	double			m_dLMTD;
	double			m_dUALMTD;
	double			m_dDuty;
	double			m_dTheorArea;

	//Meh:
	double			m_dP;
	HXSideVars		m_HotSide, m_ColdSide;

	//Functions
	void			DoSimpleHeater(MStream HotI, MStream CoolI);
	void			DoCondensingHeater(MStream HotI, MStream CoolI);
	void			DoEvaporatingHeater(MStream HotI, MStream CoolI);
	void			DoCondEvapHeater(MStream HotI, MStream CoolI);

	void			DoVent(MStream& HotI);
};



