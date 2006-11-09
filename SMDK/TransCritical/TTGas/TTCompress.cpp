//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-08 13:16:50 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $ QAL Extensions by Transcritical Technologies Pty Ltd
//===========================================================================

#include "stdafx.h"
#include "TTCompress.h"

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
    {0, "Isentropic"},    // Detailed compressor calc
    {1, "Polytropic"},
    {NULL}
  };






static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Suction",     "PumpIn",   idSuction,     1,   1,    0,    1.0f,  MIO_In |MIO_Material },
    { "Discharge",   "PumpOut",  idDischarge,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

#define H  3

static double Drw_CTTCompress[] = 
  { 
    MDrw_Poly, -10, -10, -10, 10, 5, 5, 5, -5, -10, -10,
    MDrw_End 
  };

#undef H

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CTTCompress, "TTCompress", DLL_GroupName)

void CTTCompress_UnitDef::GetOptions()
  {
  SetDefaultTag("TR");
  SetDrawing("Pump", Drw_CTTCompress);
  SetTreeDescription("TTech:CompressStage");
  SetDescription("TODO: Basic Single Compression Stage");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CTTCompress::CTTCompress(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd)
{
  m_lPumpMode = 1;
  m_lCalcMode = 0;
  m_dPRatio = 3;
  m_dPolyEff=1;  //default values...
  m_dPolyK = 0.32;
}

//---------------------------------------------------------------------------

CTTCompress::~CTTCompress()
  {
  }

//---------------------------------------------------------------------------

void CTTCompress::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------





void CTTCompress::BuildDataFields()
{
    
  DD.Long("Operation",   "", &m_lPumpMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDPumpMode); 
  DD.Long("Calculation",   "", &m_lCalcMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDCalcMode); 
  DD.Double("PumpRPM", "", &m_dRPM, MF_PARAMETER, MC_pS);
  DD.Double("Discharge.Pressure", "", &m_dP, m_lPumpMode==PM_FIXED ? MF_PARAMETER : MF_RESULT, MC_P); 
  DD.Double("Pressure.Boost", "", &m_dDeltaP, m_lPumpMode==PM_BOOST ? MF_PARAMETER : MF_RESULT, MC_P); 
  DD.Double("Pressure.Ratio", "", &m_dPRatio, m_lPumpMode==PM_RATIO ? MF_PARAMETER : MF_RESULT, MC_);
  DD.Show(m_lCalcMode==0);
  DD.Double("Isentropic.Efficiency", "", &m_dPolyEff, MF_PARAMETER, MC_);
  DD.Show(m_lCalcMode==1);
  DD.Double("Polytropic.K", "", &m_dPolyK, MF_PARAMETER, MC_);
  DD.Show();
  
    
  DD.Double("TIn", "", &m_dTIn, MF_RESULT, MC_T("C")); 

  DD.Double("TOut", "", &m_dTOut, MF_RESULT, MC_T("C")); 
  DD.Double("Power", "", &m_dPwr, MF_RESULT, MC_Pwr); 


  DD.Double("Compressor.Efficiency", "",  &m_dEff, MF_PARAMETER, MC_None);
  DD.Double("MWT", "",  &m_dMWT, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Gas.Cp", "",  &m_dGasCp, MF_RESULT|MF_NO_FILING, MC_CpMs);
  DD.Double("Gas.Cv", "",  &m_dGasCv, MF_RESULT|MF_NO_FILING, MC_CpMs);
  DD.Double("Gas.Gamma", "",  &m_dGasGamma, MF_RESULT|MF_NO_FILING, MC_None);
  
  
}







      


//---------------------------------------------------------------------------

bool CTTCompress::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------
bool CTTCompress::EvalJoinPressures()
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



void CTTCompress::doSimpleCompressor(MStream & sIn, MStream & sOut)
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
    k=m_dPolyK;
    tOut = m_dTOut = pow(m_dPRatio, k)*sIn.T;
  }
  
  sOut.T = tOut;
  
  
}




//---------------------------------------------------------------------------

void CTTCompress::EvalProducts()
{
  try           
    {
    MStream PumpI;
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

void CTTCompress::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
}

//====================================================================================
