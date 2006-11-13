//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   Time-stamp: <2006-11-01 11:54:49 Rod Stephenson Transcritical Pty Ltd>
// $Nokeywords: $ QAL Extensions by Transcritical Technologies Pty Ltd
//===========================================================================

#include "stdafx.h"
#include "QALPump.h"

#include <stdio.h>





//====================================================================================

const long idSuction  = 0;
const long idDischarge  = 1;


static MDDValueLst DDPumpMode[]=
  {
    {0, "None"},
    {1, "Boost"},
    {2, "Fixed"},
    {3, "Curve"},
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

static double Drw_CQALPump[] = 
  { 
    MDrw_Poly, -10, -H, -10, H, -H, 10, H, 10, 10, H, 10, -H,
    H, -10, -H, -10, -10, -H,

    MDrw_Poly, 0, 10, 15, 10, 15, 7, 7, 7,
    MDrw_Poly, -7, -7, -10, -10, 10, -10, 7, -7,
    MDrw_End 
  };

#undef H

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CQALPump, "QALPump", DLL_GroupName)

void CQALPump_UnitDef::GetOptions()
  {
  SetDefaultTag("TR");
  SetDrawing("Pump", Drw_CQALPump);
  SetTreeDescription("QAL:Liquor Pump");
  SetDescription("TODO: Flash Train Liquor Pump Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

CQALPump::CQALPump(MUnitDefBase *pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd)
{
  //default values...
}

//---------------------------------------------------------------------------

CQALPump::~CQALPump()
  {
  }

//---------------------------------------------------------------------------

void CQALPump::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------


static MDDValueLst  iPDSets[] = {  // Number of data sets
	{0, "1", 0},
	{1, "2", 0},
	{2, "3", 0},
	{3, "4", 0},
	NULL
};



void CQALPump::BuildDataFields()
{
    
  DD.Long("Operation",   "", &m_lPumpMode , MF_PARAMETER|MF_SET_ON_CHANGE, DDPumpMode); 
  DD.Double("PumpRPM", "", &m_dRPM, MF_PARAMETER, MC_pS); 
  DD.Double("DeltaP", "", &m_dP, MF_RESULT, MC_P); 
  DD.Double("Qv", "", &m_dQv, MF_RESULT, MC_Qv); 
  DD.Double("Power", "", &m_dPwr, MF_RESULT, MC_Pwr); 


  DD.Page("PumpData");

  DD.CheckBox("RawData", "",  &m_bRawData, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Long("DataSets", "", &iPD, MF_PARAMETER|MF_SET_ON_CHANGE, iPDSets);
  DD.Text("DataSet 1");
  DD.Double("RPM1", "",  m_dRPMs, MF_PARAMETER, MC_pS);
  DD.Double("dPmax1", "", m_dPmax, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
  DD.Text("Data");
  DD.Show(!m_bRawData);
  DD.Double("a11", "", &m_dPumpData[0][0], MF_PARAMETER, MC_None); 
  DD.Double("a12", "", &m_dPumpData[0][1], MF_PARAMETER, MC_None); 
  DD.Double("a13", "", &m_dPumpData[0][2], MF_PARAMETER, MC_None);
  DD.Show(m_bRawData);
  DD.Double("dP11", "", &m_dRawPumpData[0][0][0], MF_PARAMETER, MC_P);
  DD.Double("Q11", "",  &m_dRawPumpData[0][0][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP12", "", &m_dRawPumpData[0][1][0], MF_PARAMETER, MC_P);
  DD.Double("Q12", "",  &m_dRawPumpData[0][1][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP13", "", &m_dRawPumpData[0][2][0], MF_PARAMETER, MC_P);
  DD.Double("Q13", "",  &m_dRawPumpData[0][2][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP14", "", &m_dRawPumpData[0][3][0], MF_PARAMETER, MC_P);
  DD.Double("Q14", "",  &m_dRawPumpData[0][3][1], MF_PARAMETER, MC_Qv);
  DD.Show(iPD>0);
  DD.Text("DataSet 2");
  DD.Double("RPM2", "",  m_dRPMs+1, MF_PARAMETER, MC_pS);
  DD.Double("dPmax2", "", m_dPmax+1, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
  DD.Text("Data");
  DD.Show(!m_bRawData && iPD>0);
  DD.Double("a21", "", &m_dPumpData[1][0], MF_PARAMETER, MC_None); 
  DD.Double("a22", "", &m_dPumpData[1][1], MF_PARAMETER, MC_None); 
  DD.Double("a23", "", &m_dPumpData[1][2], MF_PARAMETER, MC_None); 
  DD.Show(m_bRawData && iPD>0);
  DD.Double("dP21", "", &m_dRawPumpData[1][0][0], MF_PARAMETER, MC_P);
  DD.Double("Q21", "",  &m_dRawPumpData[1][0][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP22", "", &m_dRawPumpData[1][1][0], MF_PARAMETER, MC_P);
  DD.Double("Q22", "",  &m_dRawPumpData[1][1][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP23", "", &m_dRawPumpData[1][2][0], MF_PARAMETER, MC_P);
  DD.Double("Q23", "",  &m_dRawPumpData[1][2][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP24", "", &m_dRawPumpData[1][3][0], MF_PARAMETER, MC_P);
  DD.Double("Q24", "",  &m_dRawPumpData[1][3][1], MF_PARAMETER, MC_Qv);
  DD.Show(iPD>1);
  DD.Text("DataSet 3");
  DD.Double("RPM3", "", m_dRPMs+2, MF_PARAMETER, MC_pS);
  DD.Double("dPmax3", "", m_dPmax+2, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
  DD.Text("Data");
  DD.Show(!m_bRawData && iPD>1);
  DD.Double("a31", "", &m_dPumpData[2][0], MF_PARAMETER, MC_None); 
  DD.Double("a32", "", &m_dPumpData[2][1], MF_PARAMETER, MC_None); 
  DD.Double("a33", "", &m_dPumpData[2][2], MF_PARAMETER, MC_None); 
  DD.Show(m_bRawData && iPD>1);
  DD.Double("dP31", "", &m_dRawPumpData[2][0][0], MF_PARAMETER, MC_P);
  DD.Double("Q31", "",  &m_dRawPumpData[2][0][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP32", "", &m_dRawPumpData[2][1][0], MF_PARAMETER, MC_P);
  DD.Double("Q32", "",  &m_dRawPumpData[2][1][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP33", "", &m_dRawPumpData[2][2][0], MF_PARAMETER, MC_P);
  DD.Double("Q33", "",  &m_dRawPumpData[2][2][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP34", "", &m_dRawPumpData[2][3][0], MF_PARAMETER, MC_P);
  DD.Double("Q34", "",  &m_dRawPumpData[2][3][1], MF_PARAMETER, MC_Qv);
  DD.Show(iPD>2);
  DD.Text("DataSet 4");
  DD.Double("RPM4", "", m_dRPMs+3, MF_PARAMETER, MC_pS);
  DD.Double("dPmax4", "", m_dPmax+3, MF_PARAMETER, MC_P);  // Pressure boost for pump stalled
  DD.Text("Data");
  DD.Show(!m_bRawData  && iPD>2);
  DD.Double("a41", "", &m_dPumpData[3][0], MF_PARAMETER, MC_None); 
  DD.Double("a42", "", &m_dPumpData[3][1], MF_PARAMETER, MC_None); 
  DD.Double("a43", "", &m_dPumpData[3][2], MF_PARAMETER, MC_None); 
  DD.Show(m_bRawData && iPD>2);
  DD.Double("dP41", "", &m_dRawPumpData[3][0][0], MF_PARAMETER, MC_P);
  DD.Double("Q41", "",  &m_dRawPumpData[3][0][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP42", "", &m_dRawPumpData[3][1][0], MF_PARAMETER, MC_P);
  DD.Double("Q42", "",  &m_dRawPumpData[3][1][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP43", "", &m_dRawPumpData[3][2][0], MF_PARAMETER, MC_P);
  DD.Double("Q43", "",  &m_dRawPumpData[3][2][1], MF_PARAMETER, MC_Qv);
  DD.Double("dP44", "", &m_dRawPumpData[3][3][0], MF_PARAMETER, MC_P);
  DD.Double("Q44", "",  &m_dRawPumpData[3][3][1], MF_PARAMETER, MC_Qv);
  
  DD.Show();
  DD.Page("Efficiency");
  DD.Text("Peak Efficiency");
  DD.Double("eMax", "", &m_dEffData[0][0], MF_PARAMETER, MC_None);
  DD.Double("QMax", "",  &m_dEffData[0][1], MF_PARAMETER, MC_Qv);
  DD.Text("Curve Points");
  DD.Double("e1", "", &m_dEffData[1][0], MF_PARAMETER, MC_None);
  DD.Double("Q1", "",  &m_dEffData[1][1], MF_PARAMETER, MC_Qv);
  DD.Double("e2", "", &m_dEffData[2][0], MF_PARAMETER, MC_None);
  DD.Double("Q2", "",  &m_dEffData[2][1], MF_PARAMETER, MC_Qv);
  DD.Double("e3", "", &m_dEffData[3][0], MF_PARAMETER, MC_None);
  DD.Double("Q3", "",  &m_dEffData[3][1], MF_PARAMETER, MC_Qv);
  DD.Double("e4", "", &m_dEffData[4][0], MF_PARAMETER, MC_None);
  DD.Double("Q4", "",  &m_dEffData[4][1], MF_PARAMETER, MC_Qv);
  DD.Text("Coeffs");
  DD.Double("a2", "",  m_dEff,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("a3", "",  m_dEff+1,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("a4", "",  m_dEff+2,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Text("Test");
  DD.Double("QQ", "",  &m_dQQ, MF_PARAMETER, MC_Qv);
  DD.Double("Eff", "",  &m_dEf, MF_RESULT|MF_NO_FILING, MC_None);
  
  /*    
  DD.Page("Debug");
  DD.Double("tmp1", "",  &m_tmp1,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp2", "",  &m_tmp2,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp3", "",  &m_tmp3,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp4", "",  &m_tmp4,  MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("tmp5", "",  &m_tmp5,  MF_RESULT|MF_NO_FILING, MC_None);
  */
}



void inv3x3(double a[3][3], double b[3]) {
    double tmp = a[0][0];
    a[0][1] /= tmp;
    a[0][2] /= tmp;
    a[0][0] = 1;
    b[0] /= tmp;
    a[1][1] -= a[1][0]*a[0][1];
    a[1][2] -= a[1][0]*a[0][2];
    b[1] -= a[1][0]*b[0];
    a[1][2] /= a[1][1];
    b[1] /= a[1][1];
    a[2][1] -= a[2][0]*a[0][1];
    a[2][2] -= a[2][0]*a[0][2];
    b[2] -= a[2][0]*b[0];
    a[2][2] -= a[2][1]*a[1][2];
    b[2] -= a[2][1]*b[1];
    b[2] /= a[2][2];
    b[1] -= a[1][2]*b[2];
    b[0] -= a[0][1]*b[1] + a[0][2]*b[2];
}

void CQALPump::fitCurveData() 
{
  double a[3][3], b[3];
  double s2, s3, s4, s5, s6;
  double sy1, sy2, sy3;
  for (int i=0; i<=iPD; i++) {
    s2=s3=s4=s5=s6=0.0;
    sy1=sy2=sy3=0.0;
    for (int j=0; j<4; j++) {
      double z = m_dRawPumpData[i][j][1];
      double z2 = z*z;
      double z3 = z*z2;
      s2+=z2;
      s3+=z3;
      s4+=z2*z2;
      s5+=z2*z3;
      s6+=z3*z3;
      double zy = m_dRawPumpData[i][j][0]-m_dPmax[i];
      sy1+=zy*z;
      sy2+=zy*z2;
      sy3+=zy*z3;
    }
    a[0][0] = s2;
    a[1][0] = a[0][1] = s3;
    a[2][0] = a[1][1] = a[0][2] = s4;
    a[2][1] = a[1][2] = s5;
    a[2][2] = s6;
    b[0] = sy1;
    b[1] = sy2;
    b[2] = sy3;
    // Invert 3x3
    inv3x3(a, b);
    

    m_dPumpData[i][0] = b[0];
    m_dPumpData[i][1] = b[1];
    m_dPumpData[i][2] = b[2];
  }
  
}

double CQALPump::Efficiency(double q) {
  double z = (q-m_dEffData[0][1]);
  return m_dEffData[0][0]+z*z*(m_dEff[0]+z*(m_dEff[1]+z*m_dEff[2]));
}



void CQALPump::fitEffData() 
{
  double a[3][3], b[3];
  double s4, s5, s6, s7, s8;
  double sy2, sy3, sy4;
  for (int i=0; i<=iPD; i++) {
    s4=s5=s6=s7=s8=0.0;
    sy2=sy3=sy4=0.0;
    for (int j=1; j<=4; j++) {
      double z = (m_dEffData[j][1]-m_dEffData[0][1]);
      double z2 = z*z;
      double z3 = z*z2;
      s4+=z2*z2;
      s5+=z3*z2;
      s6+=z3*z3;
      s7+=z *z3*z3;
      s8+=z3*z3*z2;
      double zy = m_dEffData[j][0]-m_dEffData[0][0];
      sy2+=zy*z2;
      sy3+=zy*z3;
      sy4+=zy*z2*z2;
    }
    m_tmp1=s4;
    m_tmp2=s5;
    m_tmp3=s6;
    m_tmp4=s7;
    m_tmp5=s8;
    

    a[0][0] = s4;
    a[1][0] = a[0][1] = s5;
    a[2][0] = a[1][1] = a[0][2] = s6;
    a[2][1] = a[1][2] = s7;
    a[2][2] = s8;
    b[0] = sy2;
    b[1] = sy3;
    b[2] = sy4;
    // Invert 3x3
    inv3x3(a, b);
    

    m_dEff[0] = b[0];
    m_dEff[1] = b[1];
    m_dEff[2] = b[2];
  }
  
}



double CQALPump::calcDP(int i, double qv) 
{
  return m_dPmax[i] + qv*(m_dPumpData[i][0] + qv*(m_dPumpData[i][1] + qv*m_dPumpData[i][2]));
}



double CQALPump::findDP() 
{
  double dP1, dP2, dP3, dP4, dP;
  switch (iPD) {
  case 0:   // Single set of pump data, just scale linearly
    dP1 = calcDP(0, m_dQv);
    dP = dP1*m_dRPM/m_dRPMs[0];
    break;
  case 1:   // Two pump curves
    dP1 = calcDP(0, m_dQv);
    dP2 = calcDP(1, m_dQv);
    dP = linMap(m_dRPM, m_dRPMs[0], dP1, m_dRPMs[1], dP2);
    break;
  case 2:   // Three pump curves
    dP1 = calcDP(0, m_dQv);
    dP2 = calcDP(2, m_dQv);
    dP = linMap(m_dRPM, m_dRPMs[0], dP1, m_dRPMs[2], dP2);
    break;
  case 3:   // Four pump curves
    dP1 = calcDP(0, m_dQv);
    dP2 = calcDP(3, m_dQv);
    dP = linMap(m_dRPM, m_dRPMs[0], dP1, m_dRPMs[3], dP2);
    break;
  }
  return dP;
}

      


//---------------------------------------------------------------------------

bool CQALPump::ConfigureJoins()
  { 
  MBaseMethod::ConfigureJoins();
  //Joins.Count=2;
  return true; 
  }

//---------------------------------------------------------------------------
bool CQALPump::EvalJoinPressures()
  {
  if (1)
    {//set pressures at each join (pipes connected to unit)
      for (int j=0; j<Joins.Count; j++)
      {
      double Pj=Joins[j].GetProbalPIn();


      Joins[j].SetProbalP(Pj+m_dP, false, true);
      
      #if dbgFlwNode
      Dbg.PrintLn("EJP: %12.2f  %s[%i]", Pm, Tag, j );
      #endif
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

void CQALPump::EvalProducts()
{
  try           
    {
    MStream PumpI;
    MStream & PumpO = FlwIOs[FlwIOs.First[idDischarge]].Stream;
    if (m_bRawData) 
      fitCurveData();
    fitEffData();
    FlwIOs.AddMixtureIn_Id(PumpI, idSuction);

    PumpO = PumpI;
    m_dQv = PumpO.VolumeFlow();
    m_dP = findDP();
    m_dEf = Efficiency(m_dQv);
    m_dPwr = m_dP*m_dQv/m_dEf;
    
    
      

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

void CQALPump::ClosureInfo(MClosureInfo & CI)
{
  if (CI.DoFlows())
    {
    //  HtIn += dActualDuty; //ensure heat balance
    }
}

//====================================================================================
