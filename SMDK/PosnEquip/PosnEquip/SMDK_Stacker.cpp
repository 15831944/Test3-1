//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "stacker.h"
#include "StackerManager.h"
#include "smdk_stacker.h"
//#pragma optimize("", off)

//====================================================================================

//const long idFeed = 0;
//const long idProd = 1;

/*static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",        "Product",   idProd,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };*/

static double Drw_Stacker[] = { 
                            MDrw_Poly, 0,0, -8,0, -8,3, 0,3, 0,0,
                            MDrw_Poly, 0,0, 20,0, 20,-3, 0,-3, 0,0,
                            MDrw_Poly, 0,3, 0,6,
                            MDrw_Poly, -5.5,3, 0,6, 14,0,
                            MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(Stacker, "Stacker", DLL_GroupName)
void Stacker_UnitDef::GetOptions()
  {
  SetDefaultTag("STK", true);
  SetDrawing("Control", Drw_Stacker);
  SetTreeDescription("Position Equipment:Stacker");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

Stacker::~Stacker()
{
	CStacker::StackerManager.Remove(&S);

}
//---------------------------------------------------------------------------

Stacker::Stacker(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  m_dSlewRate = 0.0;
  m_dLuftRate = 0.0;
  m_dTravelRate = 0.0;
  m_dSlewDir = 0.0;
  m_dLuftDir = 1.0;
  m_dTravelDir = 1.0;

  m_dInitSlew = 0.0;
  m_dInitLuft = 0.0;
  m_dInitTravel = 0.0;
  S.m_nArrayIndex = CStacker::StackerManager.Add(&S);
  bOnLine = true;
  bLoaded = false;
  }

//---------------------------------------------------------------------------

void Stacker::Init()
  {
	  S.m_szName = getTag();
	  m_dSlewPos = m_dInitSlew;
	  m_dLuftPos = m_dInitLuft;
	  m_dTravelPos = m_dInitTravel;
	  UpdateStacker();
  }

//---------------------------------------------------------------------------

const int idDX_SlewPos = 1;
const int idDX_SlewStop= 2;
const int idDX_SlewRev = 3;
const int idDX_TravelPos = 4;
const int idDX_TravelStop = 5;
const int idDX_TravelRev = 6;
const int idDX_LuffPos = 7;
const int idDX_LuffStop = 8;
const int idDX_LuffRev = 9;
const int idDX_InitSlew = 10;
const int idDX_InitLuff = 11;
const int idDX_InitTravel = 12;

void Stacker::BuildDataFields()
  {
  DD.Text("");
  DD.CheckBox("On", "",  &bOnLine, MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  DD.Text("Parameters");
  DD.Double("Rail_Height", "", &S.m_dRailHeight, MF_PARAMETER, MC_L("m"));
  DD.Double("Long_Travel_Slope", "", &S.m_dLongTravelSlope, MF_PARAMETER, MC_None);
  DD.Text("");
  DD.Double("Boom_Length", "", &S.m_dBoomLength, MF_PARAMETER, MC_L("m"));
  DD.Double("Boom_Height", "", &S.m_dBoomHeight, MF_PARAMETER, MC_L("m"));
  DD.Double("Boom_Width", "", &S.m_dBoomWidth, MF_PARAMETER, MC_L("m"));
  DD.Double("Boom_Luft_Pivot_Height", "", &S.m_dBoomLuftPivotHeight, MF_PARAMETER, MC_L("m"));
  DD.Double("Boom_Luft_Pivot_XOffset", "", &S.m_dBoomLuftPivotXOffset, MF_PARAMETER, MC_L("m"));
  DD.Double("Boom_Luft_Pivot_Centre_Offset", "", &S.m_dBoomLuftPivotCentreOffset, MF_PARAMETER, MC_L("m"));
  DD.Text("");
  DD.Double("Counter_Weight_Length", "", &S.m_dCWLength, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Height", "", &S.m_dCWHeight, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Width", "", &S.m_dCWWidth, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Luft_Pivot_Height", "", &S.m_dCWLuftPivotHeight, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Luft_Pivot_XOffset", "", &S.m_dCWLuftPivotXOffset, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Luft_Pivot_Centre_Offset", "", &S.m_dCWLuftPivotCentreOffset, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Offset_Angle", "", &S.m_dCWAngle, MF_PARAMETER, MC_Ang("deg"));
  DD.Text("");
  DD.CheckBox("Independant_Counter_Weight","",&S.m_bStaticTower,MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  DD.Double("Tower_Height", "", &S.m_dTowerHeight, MF_PARAMETER, MC_L("m"));
  DD.Double("Tower_Offset", "", &S.m_dTowerOffset, MF_PARAMETER, MC_L("m"));
  DD.Double("Boom_Cable_Anchor_Point", "", &S.m_dBoomAnchor, MF_PARAMETER, MC_L("m"));
  DD.Double("Counter_Weight_Cable_Anchor_Point", "", &S.m_dCWAnchor, MF_PARAMETER, MC_L("m"));
  DD.Double("EW_Offset", "", &S.m_dxOffset, MF_PARAMETER, MC_L("m"));
  DD.Text("");
  DD.Double("Slew_Rate", "", &m_dSlewRate, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Luft_Rate", "", &m_dLuftRate, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Travel_Rate", "", &m_dTravelRate, MF_PARAMETER, MC_Ldt("m/s"));

  DD.Text("");
  DD.CheckBox("BucketWheel","",&S.m_bBucketWheel,MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  if(S.m_bBucketWheel)
  {
	DD.Double("Bucket_Wheel_Diamter", "", &S.m_dBucketWheelDiameter, MF_PARAMETER, MC_L("m"));
	DD.Double("Bucket_Wheel_YOffset", "", &S.m_dBucketWheelYOffset, MF_PARAMETER, MC_L("m"));
  }
  DD.Text("");
  DD.CheckBox("Cabin","",&S.m_bCabin,MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  if(S.m_bCabin)
  {
	DD.Double("Cabin_Length", "", &S.m_dCabinLength, MF_PARAMETER, MC_L("m"));
	DD.Double("Cabin_Height", "", &S.m_dCabinHeight, MF_PARAMETER, MC_L("m"));
	DD.Double("Cabin_XOffset", "", &S.m_dCabinXOffset, MF_PARAMETER, MC_L("m"));
  }

  DD.Text("");
  DD.CheckBox("Base","",&S.m_bBase,MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  if(S.m_bBase)
  {
	DD.Double("Base_Length", "", &S.m_dBaseLength, MF_PARAMETER, MC_L("m"));
	DD.Double("Base_Height", "", &S.m_dBaseHeight, MF_PARAMETER, MC_L("m"));
	DD.Double("Base_Width", "", &S.m_dBaseWidth, MF_PARAMETER, MC_L("m"));
  }

  DD.Text("");
  DD.Text("Limits");
  DD.Double("Slew_High_Limit", "", &S.m_dSlewHighLim, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Slew_Low_Limit", "", &S.m_dSlewLowLim, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Luff_High_Limit", "", &S.m_dLuffHighLim, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Luff_Low_Limit", "", &S.m_dLuffLowLim, MF_PARAMETER, MC_Ang("deg"));

  DD.Text("");
  DD.Text("Minimum Distance Reports");
  for(int i = 0; i < CStacker::StackerManager.Count(); i ++)
  {
	  CStacker * pStacker = CStacker::StackerManager.Get(i);
		if(pStacker != &S) 
		DD.CheckBox(pStacker->m_szName, "",  &S.m_bTestMap[pStacker].m_bCheck, MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  }

  DD.Page("Control ");
  DD.Text("Initialisation");
  DD.Double("Initial_Slew_Position", "", idDX_InitSlew, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Initial_Luft_Position", "", idDX_InitLuff, MF_PARAMETER, MC_Ang("deg"));
  DD.Double("Initial_Long_Travel_Position", "", idDX_InitTravel, MF_PARAMETER, MC_L("m"));

  DD.Text("");
  DD.Button("Forward Slew", "", idDX_SlewPos); 
  DD.Button("Stop Slew", "", idDX_SlewStop); 
  DD.Button("Reverse Slew", "", idDX_SlewRev); 
  DD.Text("");
  DD.Button("Positive Luff", "", idDX_LuffPos); 
  DD.Button("Stop Luff", "", idDX_LuffStop); 
  DD.Button("Negative Luff", "", idDX_LuffRev); 
  DD.Text("");
  DD.Button("Forward Travel", "", idDX_TravelPos); 
  DD.Button("Stop Travel", "", idDX_TravelStop); 
  DD.Button("Negative Travel", "", idDX_TravelRev); 


  DD.Page("Results");
  //DD.Text("Results...");
  DD.Text("");
  DD.Double("Slew_Position", "", &m_dSlewPos, MF_RESULT, MC_Ang("deg"));
  DD.Double("Luft_Position", "", &m_dLuftPos, MF_RESULT, MC_Ang("deg"));
  DD.Double("Travel_Position", "", &m_dTravelPos, MF_RESULT, MC_L("m"));
  DD.Long("Array_Position", "", &S.m_nArrayIndex, MF_RESULT);

  DD.Text("");
  DD.Text("Minimum Distance");
	// Iterate through map
	map<CStacker *, StackerRec>::iterator i;
	CString szTag;

	for(i = S.m_bTestMap.begin(); i!= S.m_bTestMap.end(); i++)
	{
		CStacker * pStacker = i->first;
		if(i->second.m_bCheck) 
		{
			szTag = pStacker->m_szName + ".MinDistance";
			DD.Double(szTag, "", &S.m_bTestMap[pStacker].m_dMinDistance, MF_PARAMETER, MC_L("m"));
		}

	}
  }

//---------------------------------------------------------------------------

void Stacker::EvalProducts()
  {
  try
    {
    if (bOnLine)
	{
		double dMinDist;
		CEdge edge;
		double dt = getDeltaTime();
		m_dTravelPos += m_dTravelRate*m_dTravelDir*dt;
		m_dSlewPos += m_dSlewRate*m_dSlewDir*dt;
		m_dLuftPos += m_dLuftRate*m_dLuftDir*dt;

		if(m_dSlewPos < S.m_dSlewLowLim) m_dSlewPos = S.m_dSlewLowLim;
		if(m_dSlewPos > S.m_dSlewHighLim) m_dSlewPos = S.m_dSlewHighLim;
		if(m_dLuftPos < S.m_dLuffLowLim) m_dLuftPos = S.m_dLuffLowLim;
		if(m_dLuftPos > S.m_dLuffHighLim) m_dLuftPos = S.m_dLuffHighLim;
		UpdateStacker();

		// Iterate through map
		map<CStacker *, StackerRec>::iterator i;
		for(i = S.m_bTestMap.begin(); i!= S.m_bTestMap.end(); i++)
		{
			CStacker * pStacker = i->first;
			if(i->second.m_bCheck)  
			{
				StackerMinDistance(S,*pStacker,edge,dMinDist);
				S.m_bTestMap[pStacker].m_dMinDistance = dMinDist;
			}
		}
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

//====================================================================================

void Stacker::UpdateStacker(void)
{
	S.SetPos(m_dSlewPos,m_dLuftPos,m_dTravelPos);
	S.Update();
}

bool Stacker::ValidateDataFields()
{
	//if(m_dInitSlew < S.m_dSlewLowLim) return false;
	//if(m_dInitSlew > S.m_dSlewHighLim) return false;
	//if(m_dInitLuft < S.m_dLuffLowLim) return false;
	//if(m_dInitLuft > S.m_dLuffHighLim) return false;

	S.m_szName = getTag();
	S.Initialise();
	if(!bLoaded)
	{
		m_dSlewPos = m_dInitSlew;
		m_dLuftPos = m_dInitLuft;
		m_dTravelPos = m_dInitTravel;
		bLoaded = true;
	}

	UpdateStacker();

	// This forces an update when data is changed
	if(CStacker::StackerManager.m_bViewerStarted)
		CStacker::StackerManager.UpdateViewerThread();
	return true;
};  

void Stacker::StartSolution()
{
	  m_dSlewPos = m_dInitSlew;
	  m_dLuftPos = m_dInitLuft;
	  m_dTravelPos = m_dInitTravel;
	  UpdateStacker();
};  

bool Stacker::PreStartCheck()
{
	return true;
}

bool Stacker::ExchangeDataFields()
  {
  switch (DX.Handle)
    {

    case idDX_InitSlew:
      if (DX.HasReqdValue)
	  {
        m_dInitSlew=DX.Double;//although this is a result, need to set this to recover state at project load time
	    m_dSlewPos = m_dInitSlew;
	  }
        DX.Double=m_dInitSlew;
      return true;

    case idDX_InitLuff:
      if (DX.HasReqdValue)
	  {
        m_dInitLuft=DX.Double;//although this is a result, need to set this to recover state at project load time
	    m_dLuftPos = m_dInitLuft;
	  }
        DX.Double=m_dInitLuft;
      return true;

    case idDX_InitTravel:
      if (DX.HasReqdValue)
	  {
        m_dInitTravel=DX.Double;//although this is a result, need to set this to recover state at project load time
	    m_dTravelPos = m_dInitTravel;
	  }
        DX.Double=m_dInitTravel;
      return true;

	case idDX_SlewPos:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dSlewDir = 1.0;
        }
      DX.Bool = false;
      return true;

     case idDX_SlewStop:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dSlewDir = 0.0;
        }
      DX.Bool = false;
      return true;

    case idDX_SlewRev:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dSlewDir = -1.0;
        }
      DX.Bool = false;
      return true;

	case idDX_LuffPos:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dLuftDir = 1.0;
        }
      DX.Bool = false;
      return true;

    case idDX_LuffStop:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dLuftDir = 0.0;
        }
      DX.Bool = false;
      return true;

    case idDX_LuffRev:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dLuftDir = -1.0;
        }
      DX.Bool = false;
      return true;

	case idDX_TravelPos:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dTravelDir = 1.0;
        }
      DX.Bool = false;
      return true;

    case idDX_TravelStop:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dTravelDir = 0.0;
        }
      DX.Bool = false;
      return true;

    case idDX_TravelRev:
      if (DX.HasReqdValue && DX.Bool)
        {
			m_dTravelDir = -1.0;
        }
      DX.Bool = false;
      return true;

  }
  return false;
  }


