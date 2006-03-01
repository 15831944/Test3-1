//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "examplesimpleheater.h"

//====================================================================================

const long idFeed = 0;
const long idProd = 1;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",        "Product",   idProd,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_SimpleHeater[] = { MDrw_Arc, 0, 0, 5,
                            MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
                            MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
                            MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
                            MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(SimpleHeater, "SimpHeater", DLL_GroupName)
void SimpleHeater_UnitDef::GetOptions()
  {
  SetDefaultTag("SH", true);
  SetDrawing("HeatExchange", Drw_SimpleHeater);
  SetTreeDescription("Demo:Simple Heater");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

SimpleHeater::SimpleHeater(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  bOnLine = true;
  dRqdDuty = 10.0;
  dActualDuty = 0.0;
  dFeedT = StdT;
  dProdT = StdT;
  }

//---------------------------------------------------------------------------

void SimpleHeater::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void SimpleHeater::BuildDataFields()
  {
  DD.Text("");
  DD.Text("Requirements...");
  DD.CheckBox("On", "",  &bOnLine, MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  DD.Double("DutyRqd", "", &dRqdDuty, MF_PARAMETER, MC_Pwr("kW"));
  DD.Text("");
  DD.Text("Results...");
  DD.Double("Duty", "", &dActualDuty, MF_RESULT, MC_Pwr("kW"));
  DD.Double("FeedT", "", &dFeedT, MF_RESULT|MF_NO_FILING, MC_T("C"));
  DD.Double("ProdT", "", &dProdT, MF_RESULT|MF_NO_FILING, MC_T("C"));
  }

//---------------------------------------------------------------------------

void SimpleHeater::EvalProducts()
  {
  try
    {
    MStream QI; //initialise local empty copy of a stream
    FlwIOs.AddMixtureIn_Id(QI, idFeed); //sum of all feed streams
    const long index = FlwIOs.First[idProd];
    MStream & QO = FlwIOs[index].Stream; //get reference to the actual output stream

    QO = QI; //set output = input

    if (bOnLine)
      {
      const double h0 = QO.totHf(); //get enthalpy
      QO.Set_totHf(h0+dRqdDuty); //set new enthalpy
      dActualDuty = dRqdDuty;
      }
    else
      dActualDuty = 0.0;

    //get display values...
    dFeedT = QI.T;
    dProdT = QO.T;
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

void SimpleHeater::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    CI.AddPowerIn(0, dActualDuty); //ensure heat balance
    }
  }

//====================================================================================
