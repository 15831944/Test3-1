//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "examplequalchange.h"
#include "DemoQual.h"
#pragma optimize("", off)

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

static double Drw_QualChange[] = { MDrw_Arc, 0, 0, 5,
                            MDrw_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
                            MDrw_Poly, -5,1, -6,1, -6,0, -5,0, -5,1,
                            MDrw_Poly, 5,-1, 6,-1, 6,0, 5,0, 5,-1,
                            MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(QualChange, "QualChange", DLL_GroupName)

void QualChange_UnitDef::GetOptions()
  {
  SetDefaultTag("QC");
  SetDrawing("HeatExchange", Drw_QualChange);
  SetTreeDescription("Demo:Simple Heater");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Energy);
  SetModelLicense(MLicense_HeatExchange);
  };

//---------------------------------------------------------------------------

QualChange::QualChange(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  bOnLine = true;
  }

//---------------------------------------------------------------------------

void QualChange::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void QualChange::BuildDataFields()
  {
  DD.Text("");
  DD.Text("Requirements...");
  DD.CheckBox("On", "",  &bOnLine, MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  //DD.Double("DutyRqd", "", &dRqdDuty, MF_PARAMETER, MC_Pwr("kW"));
  DD.Text("");
  DD.Text("Results...");
  //DD.Double("Duty", "", &dActualDuty, MF_RESULT, MC_Pwr("kW"));
  //DD.Double("FeedT", "", &dFeedT, MF_RESULT|MF_NO_FILING, MC_T("C"));
  DD.Long("PSD_Count", "", &PSDCount, MF_RESULT|MF_NO_FILING);
  }

//---------------------------------------------------------------------------

void QualChange::EvalProducts()
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
      //MIPSD & PSD = *QI.FindIF<MIPSD>();
      //if ( IsNothing(PSD)==false )

      CDemoQual & xQual =  *QO.FindIF<CDemoQual>();

      if ( IsNothing(xQual)==false )
        {
        PSDCount = xQual.m_dSetProp;
        Log.Message(MMsg_Note, "Qual Data:%f", PSDCount);
        //CDemoQual & QualO =  *QO.FindIF<CDemoQual>();
        xQual.m_dSetProp = PSDCount+1.0;
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

void QualChange::ClosureInfo(MClosureInfo & CI)
  {
  if (CI.DoFlows())
    {
    //CI.AddPowerIn(0, dActualDuty); //ensure heat balance
    }
  }

//====================================================================================
