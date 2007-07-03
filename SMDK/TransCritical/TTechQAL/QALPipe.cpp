//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-01 11:54:19 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $ QAL Extensions by Transcritical Technologies Pty Ltd
//===========================================================================

#include "stdafx.h"
#include "QALPipe.h"

#include <stdio.h>

#define dbgModels 1




//====================================================================================

const long idSuction  = 0;
const long idDischarge  = 1;


enum {iPipe, iReducer, iExpander, iBend, iOrifice, iValve, iThruTee, iBranchTee, nItems};



std::string sPipeData[] = {"Length", "Diameter", "dH", "Roughness"};



  




static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Inlet",     "In",   idSuction,     1,   1,    0,    1.0f,  MIO_In |MIO_Material },
    { "Outlet",    "Out",  idDischarge,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

#define H 3

static double Drw_CQALPipe[] = 
  { 
    MDrw_Poly, -10, -H, -10, H,

    MDrw_Poly, -10, 0, 10, 0,
    MDrw_Poly, 10, -H, 10, H,
    MDrw_End 
  };

#undef H

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CQALPipe, "QALPipe", DLL_GroupName)

void CQALPipe_UnitDef::GetOptions()
  {
  SetDefaultTag("TR");
  SetDrawing("Pipe", Drw_CQALPipe);
  SetTreeDescription("QAL:Flash Pipe");
  SetDescription("TODO: Flash Train Liquor Pipe Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CQALPipe::CQALPipe(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd)
{
  //default values...
  m_lItems =3;
}

//---------------------------------------------------------------------------

CQALPipe::~CQALPipe()
  {
  }

//---------------------------------------------------------------------------

void CQALPipe::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------



static MDDValueLst  DDB1[] = {  // Number of data sets
	{iPipe,    "Pipe", 0},
	{iReducer, "Reducer", 0},
	{iExpander, "Expander", 0},
	{iBend, "Bend", 0},
	{iOrifice, "Orifice", 0},
	{iValve, "Valve", 0},
	{iThruTee, "Thru Tee", 0},
	{iBranchTee, "Branch Tee", 0},
	NULL
};


bool CQALPipe::ValidateDataFields()
  {//ensure parameters are within expected ranges
  m_lItems = Range((long) 0, m_lItems, (long) maxItems);
  return true;
  }



std::string fmt(char * s, int i) 
{
  std::stringstream os;
  os << s << i;
  return os.str();
}




void CQALPipe::BuildDataFields()
{
  DD.Long("Items", "", &m_lItems, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Double("MassFlow", "", &m_dMassFlow, MF_PARAMETER, MC_Qm);
  DD.Page("Entry");
  DD.Double("EntryHead", "", &m_dEntryHead, MF_PARAMETER, MC_L("m")); 

  DD.Text("Stuff");
  for (int i=1; i<m_lItems; i++) {
    DD.Page(fmt("Item", i).c_str());
    DD.Long(fmt("Itype", i).c_str(), "", m_plItemArray+i, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1); 
    switch ((int) m_plItemArray[i]) {
    case iPipe:
      DD.Text("Pipe...");
      DD.Double(fmt("Diameter", i).c_str(), "",  mItemData[i], MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Length", i).c_str(), "",    mItemData[i]+1,   MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("dH", i).c_str(), "",        mItemData[i]+2, MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Roughness", i).c_str(), "", mItemData[i]+3, MF_PARAMETER|MF_NO_FILING, MC_L("mm"));
      break;
      
    case iReducer:

      DD.Text("Reducer...");
      DD.Double(fmt("Diameter", i).c_str(), "",  mItemData[i], MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Length", i).c_str(), "",    mItemData[i],   MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("dH", i).c_str(), "",        mItemData[i]+2, MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Roughness", i).c_str(), "", mItemData[i]+3, MF_PARAMETER|MF_NO_FILING, MC_L("mm"));
      break;

    case iExpander:

      DD.Text("Expander...");
      DD.Double(fmt("Length", i).c_str(), "",    mItemData[i],   MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Diameter", i).c_str(), "",  mItemData[i]+1, MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("dH", i).c_str(), "",        mItemData[i]+2, MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Roughness", i).c_str(), "", mItemData[i]+3, MF_PARAMETER|MF_NO_FILING, MC_L("mm"));
      break;
    case iValve:
      DD.Text("Valve...");
      DD.Double(fmt("K", i).c_str(), "",    mItemData[i],   MF_PARAMETER|MF_NO_FILING, MC_);
      break;
    case iOrifice:
      DD.Text("Orifice...");
      DD.Double(fmt("Length", i).c_str(), "",    mItemData[i],   MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      DD.Double(fmt("Diameter", i).c_str(), "",  mItemData[i]+1, MF_PARAMETER|MF_NO_FILING, MC_L("m"));
      break;
      


    }
    
    

  }
  
  DD.Page("Exit");
  DD.Text("More Stuff");
  

//   DD.Long("Operation",   "", &m_lPumpMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDPumpMode); 
//   DD.Double("PumpRPM", "", &m_dRPM, MF_PARAMETER, MC_pS); 
//   DD.Double("DeltaP", "", &m_dP, MF_RESULT, MC_P); 
//   DD.Double("Qv", "", &m_dQv, MF_RESULT, MC_Qv); 
//   DD.Double("Power", "", &m_dPwr, MF_RESULT, MC_Pwr); 


//   DD.Page("PumpData");

//   DD.CheckBox("RawData", "",  &m_bRawData, MF_PARAMETER|MF_SET_ON_CHANGE);
//   DD.Long("DataSets", "", &iPD, MF_PARAMETER|MF_SET_ON_CHANGE, iPDSets);
//   DD.Text("DataSet 1");
//   DD.Double("RPM1", "",  m_dRPMs, MF_PARAMETER, MC_pS);
//   DD.Double("dPmax1", "", m_dPmax, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
//   DD.Text("Data");
//   DD.Show(!m_bRawData);
//   DD.Double("a11", "", &m_dPumpData[0][0], MF_PARAMETER, MC_None); 
//   DD.Double("a12", "", &m_dPumpData[0][1], MF_PARAMETER, MC_None); 
//   DD.Double("a13", "", &m_dPumpData[0][2], MF_PARAMETER, MC_None);
//   DD.Show(m_bRawData);
//   DD.Double("dP11", "", &m_dRawPumpData[0][0][0], MF_PARAMETER, MC_P);
//   DD.Double("Q11", "",  &m_dRawPumpData[0][0][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP12", "", &m_dRawPumpData[0][1][0], MF_PARAMETER, MC_P);
//   DD.Double("Q12", "",  &m_dRawPumpData[0][1][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP13", "", &m_dRawPumpData[0][2][0], MF_PARAMETER, MC_P);
//   DD.Double("Q13", "",  &m_dRawPumpData[0][2][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP14", "", &m_dRawPumpData[0][3][0], MF_PARAMETER, MC_P);
//   DD.Double("Q14", "",  &m_dRawPumpData[0][3][1], MF_PARAMETER, MC_Qv);
//   DD.Show(iPD>0);
//   DD.Text("DataSet 2");
//   DD.Double("RPM2", "",  m_dRPMs+1, MF_PARAMETER, MC_pS);
//   DD.Double("dPmax2", "", m_dPmax+1, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
//   DD.Text("Data");
//   DD.Show(!m_bRawData && iPD>0);
//   DD.Double("a21", "", &m_dPumpData[1][0], MF_PARAMETER, MC_None); 
//   DD.Double("a22", "", &m_dPumpData[1][1], MF_PARAMETER, MC_None); 
//   DD.Double("a23", "", &m_dPumpData[1][2], MF_PARAMETER, MC_None); 
//   DD.Show(m_bRawData && iPD>0);
//   DD.Double("dP21", "", &m_dRawPumpData[1][0][0], MF_PARAMETER, MC_P);
//   DD.Double("Q21", "",  &m_dRawPumpData[1][0][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP22", "", &m_dRawPumpData[1][1][0], MF_PARAMETER, MC_P);
//   DD.Double("Q22", "",  &m_dRawPumpData[1][1][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP23", "", &m_dRawPumpData[1][2][0], MF_PARAMETER, MC_P);
//   DD.Double("Q23", "",  &m_dRawPumpData[1][2][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP24", "", &m_dRawPumpData[1][3][0], MF_PARAMETER, MC_P);
//   DD.Double("Q24", "",  &m_dRawPumpData[1][3][1], MF_PARAMETER, MC_Qv);
//   DD.Show(iPD>1);
//   DD.Text("DataSet 3");
//   DD.Double("RPM3", "", m_dRPMs+2, MF_PARAMETER, MC_pS);
//   DD.Double("dPmax3", "", m_dPmax+2, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
//   DD.Text("Data");
//   DD.Show(!m_bRawData && iPD>1);
//   DD.Double("a31", "", &m_dPumpData[2][0], MF_PARAMETER, MC_None); 
//   DD.Double("a32", "", &m_dPumpData[2][1], MF_PARAMETER, MC_None); 
//   DD.Double("a33", "", &m_dPumpData[2][2], MF_PARAMETER, MC_None); 
//   DD.Show(m_bRawData && iPD>1);
//   DD.Double("dP31", "", &m_dRawPumpData[2][0][0], MF_PARAMETER, MC_P);
//   DD.Double("Q31", "",  &m_dRawPumpData[2][0][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP32", "", &m_dRawPumpData[2][1][0], MF_PARAMETER, MC_P);
//   DD.Double("Q32", "",  &m_dRawPumpData[2][1][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP33", "", &m_dRawPumpData[2][2][0], MF_PARAMETER, MC_P);
//   DD.Double("Q33", "",  &m_dRawPumpData[2][2][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP34", "", &m_dRawPumpData[2][3][0], MF_PARAMETER, MC_P);
//   DD.Double("Q34", "",  &m_dRawPumpData[2][3][1], MF_PARAMETER, MC_Qv);
//   DD.Show(iPD>2);
//   DD.Text("DataSet 4");
//   DD.Double("RPM4", "", m_dRPMs+3, MF_PARAMETER, MC_pS);
//   DD.Double("dPmax4", "", m_dPmax+3, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
//   DD.Text("Data");
//   DD.Show(!m_bRawData  && iPD>2);
//   DD.Double("a41", "", &m_dPumpData[3][0], MF_PARAMETER, MC_None); 
//   DD.Double("a42", "", &m_dPumpData[3][1], MF_PARAMETER, MC_None); 
//   DD.Double("a43", "", &m_dPumpData[3][2], MF_PARAMETER, MC_None); 
//   DD.Show(m_bRawData && iPD>2);
//   DD.Double("dP41", "", &m_dRawPumpData[3][0][0], MF_PARAMETER, MC_P);
//   DD.Double("Q41", "",  &m_dRawPumpData[3][0][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP42", "", &m_dRawPumpData[3][1][0], MF_PARAMETER, MC_P);
//   DD.Double("Q42", "",  &m_dRawPumpData[3][1][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP43", "", &m_dRawPumpData[3][2][0], MF_PARAMETER, MC_P);
//   DD.Double("Q43", "",  &m_dRawPumpData[3][2][1], MF_PARAMETER, MC_Qv);
//   DD.Double("dP44", "", &m_dRawPumpData[3][3][0], MF_PARAMETER, MC_P);
//   DD.Double("Q44", "",  &m_dRawPumpData[3][3][1], MF_PARAMETER, MC_Qv);
  
//   DD.Show();
//   DD.Page("Efficiency");
//   DD.Text("Peak Efficiency");
//   DD.Double("eMax", "", &m_dEffData[0][0], MF_PARAMETER, MC_None);
//   DD.Double("QMax", "",  &m_dEffData[0][1], MF_PARAMETER, MC_Qv);
//   DD.Text("Curve Points");
//   DD.Double("e1", "", &m_dEffData[1][0], MF_PARAMETER, MC_None);
//   DD.Double("Q1", "",  &m_dEffData[1][1], MF_PARAMETER, MC_Qv);
//   DD.Double("e2", "", &m_dEffData[2][0], MF_PARAMETER, MC_None);
//   DD.Double("Q2", "",  &m_dEffData[2][1], MF_PARAMETER, MC_Qv);
//   DD.Double("e3", "", &m_dEffData[3][0], MF_PARAMETER, MC_None);
//   DD.Double("Q3", "",  &m_dEffData[3][1], MF_PARAMETER, MC_Qv);
//   DD.Double("e4", "", &m_dEffData[4][0], MF_PARAMETER, MC_None);
//   DD.Double("Q4", "",  &m_dEffData[4][1], MF_PARAMETER, MC_Qv);
//   DD.Text("Coeffs");
//   DD.Double("a2", "",  m_dEff,  MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("a3", "",  m_dEff+1,  MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("a4", "",  m_dEff+2,  MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Text("Test");
//   DD.Double("QQ", "",  &m_dQQ, MF_PARAMETER, MC_Qv);
//   DD.Double("Eff", "",  &m_dEf, MF_RESULT|MF_NO_FILING, MC_None);
  
  /*    
  DD.Page("Debug");
  DD.Double("tmp1", "",  &m_tmp1,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp2", "",  &m_tmp2,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp3", "",  &m_tmp3,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp4", "",  &m_tmp4,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp5", "",  &m_tmp5,  MF_RESULT|MF_NO_FILING, MC_None);
  */
}





double CQALPipe::findDP() 
{
  double dP = -5.0;
  

  return dP;
}

      


//---------------------------------------------------------------------------

bool CQALPipe::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------
bool CQALPipe::EvalJoinPressures()
  {
  if (1)
    {//set pressures at each join (pipes connected to unit)
      for (int j=0; j<Joins.Count; j++) {
	double Pj=Joins[j].GetProbalPIn();


	Joins[j].SetProbalP(Pj+m_dP, false, true);
      }
    return true;
    }
  else
    {
    //INCOMPLETECODE() 
    }
  return false;
  }





//---------------------------------------------------------------------------

void CQALPipe::EvalProducts()
{
  try    {
    MStreamI PipeI;
    MStream & PipeO = FlwIOs[FlwIOs.First[idDischarge]].Stream;
    FlwIOs.AddMixtureIn_Id(PipeI, idSuction);

    PipeO = PipeI;
    m_dP = findDP();
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

void CQALPipe::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
}

//====================================================================================
