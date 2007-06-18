#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif
//#include "..\..\Transcritical\TTCommon\STHx.h"


//---------------------------------------------------------------------------

enum OpMode { OM_Simple, OM_Condensing, OM_Evaporating };

class CCARTubeDigester : public MBaseMethod//, CSTHx
{
	friend class HXSolver;
	friend class CondensingHXSolver;
	friend class EvaporatingHXSolver;
	friend class SimpleHXSolver;
public:
	CCARTubeDigester(MUnitDefBase * pUnitDef, TaggedObject * pNd);
	virtual ~CCARTubeDigester(void);

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
	MReactionBlk    m_RB;     // Reactions
	MVLEBlk         m_VLE;    // Vapor-Liquid equilibrium
	MFT_Condenser   m_FTC;    // Flash Train
    MEnvironHXBlk   m_EHX;    // Environmental

	//Input Variables
	bool			m_bOn;
	OpMode			m_eOperationMode;
	double			m_dHTC;
	double			m_dActualArea;
	double			m_dLMTDFact;
	double			m_dReqQmVent;

	//Output Variables
	double			m_dUA;
	double			m_dLMTD;
	double			m_dUALMTD;
	double			m_dDuty;
	double			m_dTheorArea;

	//Meh:
	double			m_dP;

	//Functions
	void			DoSimpleHeater(MStream HotI);
	void			DoCondensingHeater(MStream HotI);
	void			DoEvaporatingHeater(MStream HotI);
};



