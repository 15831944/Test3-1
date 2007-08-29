//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2007-08-29 10:58:03 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $ QAL Extensions by Transcritical Technologies Pty Ltd
//===========================================================================

#include "stdafx.h"
#include "TTMSCompress.h"
#include <stdio.h>



//====================================================================================

const long idSuction  = 0;
const long idDischarge  = 1;

enum {PM_NONE, PM_FIXED, PM_BOOST, PM_RATIO};
static MDDValueLst DDPumpMode[]=
  {
    {0, "None"},    // Detailed compressor calc
    {1, "Fixed"},
    {2, "Boost"},
    {3, "Ratio"},
    {NULL}
  };

static MDDValueLst DDCalcMode[]=
  {
    {0, "Isentropic"},    
    {1, "Polytropic"},
    {2, "Specify.K"},
    {NULL}
  };


static MDDValueLst DDStages[]=
  {
    {1, "1"},    
    {2, "2"},
    {3, "3"},
    {4, "4"},
    {5, "5"},
    {6, "6"},
    {NULL}
  };
static MDDValueLst DDThrows[]=
  {
    {1, "1"},    
    {2, "2"},
    {3, "3"},
    {4, "4"},
    {NULL}
  };

static char* thHd[] = {
  "Throw 1",
  "Throw 2",
  "Throw 3",
  "Throw 4"
};


static char* thHd1[] = {
  "Throw1",
  "Throw2",
  "Throw3",
  "Throw4"
};

static char* stHd1[] = {
  "Stage1",
  "Stage2",
  "Stage3",
  "Stage4",
  "Stage5",
  "Stage6"
};








static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Suction",     "PumpIn",   idSuction,     1,   1,    0,    1.0f,  MIO_In |MIO_Material },
    { "Discharge",   "PumpOut",  idDischarge,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

#define H  3

static double Drw_CTTMSCompress[] = 
  { 
    MDrw_Poly, -10, -10, -10, 10, 5, 5, 5, -5, -10, -10,
    MDrw_End 
  };

#undef H

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CTTMSCompress, "TTMSCompress", DLL_GroupName)

void CTTMSCompress_UnitDef::GetOptions()
  {
  SetDefaultTag("TR");
  SetDrawing("Pump", Drw_CTTMSCompress);
  SetTreeDescription("TTech:MultistageCompressor");
  SetDescription("TODO: Basic Single Compression Stage");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CTTMSCompress::CTTMSCompress(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd)
{
  m_lPumpMode = 1;
  m_lCalcMode = 0;
  m_dPRatio = 3;
  m_dPolyEff=1;  //default values...
  m_dPolyK = 0.32;
  m_lStages = 4;
  m_lThrows = 4;
  m_dStroke = 4.5*25.4/1000.;
  m_dRodDiam = 1.125*25.4/1000.;
  m_dRPM = 1200./60;
  m_dTIntercool = C2K(40.0);

  
  for (int i=0; i<4; i++) {
    td[i].heStage=td[i].ceStage=i+1;
    td[i].heDiam = td[i].ceDiam = 12.0/(i+1)*25.4/1000.; //12", 6", 4", 3"
    td[i].heCV = td[i].ceCV = 0.1;
  }
  
  

}

//---------------------------------------------------------------------------

CTTMSCompress::~CTTMSCompress()
  {
  }

//---------------------------------------------------------------------------

void CTTMSCompress::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------





void CTTMSCompress::BuildDataFields()
{
    
  DD.Long("Operation",   "", &m_lPumpMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDPumpMode); 
  DD.Long("Calculation",   "", &m_lCalcMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDCalcMode); 
  DD.Double("Discharge.Pressure", "", &m_dP, m_lPumpMode==PM_FIXED ? MF_PARAMETER : MF_RESULT, MC_P); 
  DD.Double("Pressure.Boost", "", &m_dDeltaP, m_lPumpMode==PM_BOOST ? MF_PARAMETER : MF_RESULT, MC_P); 
  DD.Double("Pressure.Ratio", "", &m_dPRatio, m_lPumpMode==PM_RATIO ? MF_PARAMETER : MF_RESULT, MC_);
  DD.Show(m_lCalcMode==0);
  DD.Double("Adiabatic.Efficiency", "", &m_dPolyEff, MF_PARAMETER, MC_);
  DD.Show(m_lCalcMode==1);
  DD.Double("Polytropic.Efficiency", "", &m_dPolyEff, MF_PARAMETER, MC_);
  DD.Show(m_lCalcMode==2);
  DD.Double("Polytropic.K", "", &m_dPolyK, MF_PARAMETER, MC_);
  DD.Show();

  DD.Text("Compressor Data");
  DD.Long("Stages", "", &m_lStages, MF_PARAMETER|MF_SET_ON_CHANGE, DDStages);
  DD.Double("RPM", "", &m_dRPM, MF_PARAMETER, MC_pS);
  DD.Double("Stroke", "", &m_dStroke, MF_PARAMETER, MC_L("in"));
  DD.Double("Rod.Diameter", "", &m_dRodDiam, MF_PARAMETER, MC_L("in"));
  
  DD.Double("Intercooler.T", "", &m_dTIntercool, MF_PARAMETER, MC_T("C"));

  DD.Double("TIn", "", &m_dTIn, MF_RESULT, MC_T("C")); 
  DD.Double("TOut", "", &m_dTOut, MF_RESULT, MC_T("C")); 
  DD.Double("Power", "", &m_dPwr, MF_RESULT, MC_Pwr); 


  DD.Double("Compressor.Efficiency", "",  &m_dEff, MF_PARAMETER, MC_None);
  DD.Double("MWT", "",  &m_dMWT, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Gas.Cp", "",  &m_dGasCp, MF_RESULT|MF_NO_FILING, MC_CpMs);
  DD.Double("Gas.Cv", "",  &m_dGasCv, MF_RESULT|MF_NO_FILING, MC_CpMs);
  DD.Double("Gas.Gamma", "",  &m_dGasGamma, MF_RESULT|MF_NO_FILING, MC_None);



  DD.Page("Throw Data");
  DD.Text("HE... Head End,   CE... Crank End, CV... Clearance Volume");

  DD.Long("Throws", "", &m_lThrows, MF_PARAMETER|MF_SET_ON_CHANGE, DDThrows);
  for (int i=0; i<m_lThrows; i++) {
    DD.Text(thHd[i]);
    DD.StructBegin(thHd1[i]);
    DD.CheckBox("Tandem", "", &td[i].isTandem, MF_PARAMETER|MF_SET_ON_CHANGE);
    DD.CheckBox("Disable.HE", "", &td[i].heDisabled, MF_PARAMETER|MF_SET_ON_CHANGE|MF_INIT_HIDDEN);
    DD.CheckBox("Disable.CE", "", &td[i].ceDisabled, MF_PARAMETER|MF_SET_ON_CHANGE|MF_INIT_HIDDEN);
    DD.Long("HE.Stage", "", &td[i].heStage, MF_PARAMETER);
    DD.Show(td[i].isTandem);
    DD.Long("CE.Stage", "", &td[i].ceStage, MF_PARAMETER);
    DD.Show();
    DD.Double("HE.Diam", "", &td[i].heDiam, MF_PARAMETER, MC_L("in"));
    DD.Show(td[i].isTandem);
    DD.Double("CE.Diam", "", &td[i].ceDiam, MF_PARAMETER, MC_L("in"));
    DD.Show();
    DD.Double("HE.CV", "", &td[i].heCV, MF_PARAMETER, MC_Frac("%"));
    DD.Double("CE.CV", "", &td[i].ceCV, MF_PARAMETER, MC_Frac("%"));

    td[i].heSV = m_dStroke*CircleArea(td[i].heDiam);
    td[i].ceSV = m_dStroke*(CircleArea(td[i].ceDiam)-CircleArea(m_dRodDiam));
    td[i].hePD = td[i].heSV*m_dRPM;
    td[i].cePD = td[i].ceSV*m_dRPM;
    
    
    DD.Double("HE.SweptVol", "", &td[i].heSV, MF_RESULT, MC_Vol);
    DD.Double("CE.SweptVol", "", &td[i].ceSV, MF_RESULT, MC_Vol);   
    DD.Double("HE.PD", "", &td[i].hePD, MF_RESULT, MC_Qv);
    DD.Double("CE.PD", "", &td[i].cePD, MF_RESULT, MC_Qv);
    DD.StructEnd();  
  }


  for (int i=0; i<m_lStages; i++) {
    DD.Page(stHd1[i]);
    DD.StructBegin(stHd1[i]);
    DD.Double("P.Suction", "", &sd[i].pSuction, MF_RESULT, MC_P("kPa"));
    DD.Double("P.Discharge", "", &sd[i].pDischarge, MF_RESULT, MC_P("kPa"));
    DD.Double("T.Discharge", "", &sd[i].TDischarge, MF_RESULT, MC_T("C"));
    DD.Double("P.Ratio", "", &sd[i].pRatio, MF_RESULT, MC_T("C"));
    DD.Double("Z.Suction", "", &sd[i].zS, MF_RESULT, MC_);
    DD.Double("Z.Discharge", "", &sd[i].zD, MF_RESULT, MC_);
    DD.Double("Q.Cooler", "", &sd[i].qCooler, MF_RESULT, MC_Pwr);
    DD.Double("v.Valve", "", &sd[i].vValve, MF_RESULT, MC_Ldt);  
    DD.StructEnd();  
  }  
}







      


//---------------------------------------------------------------------------

bool CTTMSCompress::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------
bool CTTMSCompress::EvalJoinPressures()
  {
  if (1)
    {//set pressures at each join (pipes connected to unit)
      for (int j=0; j<Joins.Count; j++)
      {
      double Pj=Joins[j].GetProbalPIn();


      Joins[j].SetProbalP(m_dP, false, true);
      
      }
    return true;
    }
  else
    {
    //INCOMPLETECODE() 
    }
  return false;
  }



void CTTMSCompress::doSimpleCompressor(MStream & sIn, MStream & sOut)
{
  double pIn = sIn.P;
  double cP = m_dGasCp = sIn.msCp(MP_Gas);
  double mwt = m_dMWT = sIn.MoleWt(MP_Gas);
  double R = 8.3143/mwt;
  double cV = m_dGasCv = cP-R;
  double gamma = m_dGasGamma = cP/cV;
  double k = (gamma-1)/gamma;
  double tOut;
  if (m_lCalcMode==0) {
    tOut = m_dTOut = pow(m_dPRatio, k)*sIn.T;
    double dT = tOut-sIn.T;
    dT /= m_dPolyEff;
    tOut = sIn.T+dT;
  }
  if (m_lCalcMode==1) {
    tOut = m_dTOut = pow(m_dPRatio, k/m_dPolyEff)*sIn.T;
  }
  
  if (m_lCalcMode==2) {
    k=m_dPolyK;
    tOut = m_dTOut = pow(m_dPRatio, k)*sIn.T;
  }
  
  sOut.T = tOut;
  
  
}




//---------------------------------------------------------------------------

void CTTMSCompress::EvalProducts()
{
  try           
    {
    MStreamI PumpI;
    MStream & PumpO = FlwIOs[FlwIOs.First[idDischarge]].Stream;
    FlwIOs.AddMixtureIn_Id(PumpI, idSuction);
    PumpO = PumpI;
    m_dTIn = PumpI.T;
    double pIn = PumpI.P;

    switch (m_lPumpMode) {
    case PM_NONE:
      Log.Message(MMsg_Error, "Not Implemented");
      break;
    case PM_FIXED:
      m_dDeltaP = m_dP-pIn;
      m_dPRatio = m_dP/pIn;
      break;
    case PM_BOOST:
      m_dP = pIn + m_dDeltaP;
      m_dPRatio = m_dP/pIn;
      break;
    case PM_RATIO:
      m_dP = pIn*m_dPRatio;
      m_dDeltaP = m_dP-pIn;
      break;
    }
    
    if (m_lPumpMode) {
      doSimpleCompressor(PumpI, PumpO);
      double dGasWork = PumpO.totHz() - PumpI.totHz();
      m_dPwr = dGasWork/m_dEff;
    }
    

    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
}

//--------------------------------------------------------------------------

void CTTMSCompress::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
}

//====================================================================================
