//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EXAMPLESPLITTER_CPP
#include "examplesplitter.h"

//====================================================================================

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      0,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product1",       "Product1",  1,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Product2",       "Product2",  2,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(Splitter, "Splitter", DLL_GroupName)
void Splitter_UnitDef::GetOptions()
  {
  SetDefaultTag("SP", true);
  //SetDrawing("Tank", Drw_Splitter);
  SetTreeDescription("Demo:Splitter");
  };

//---------------------------------------------------------------------------

Splitter::Splitter(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  bDoPhaseSplit = false;
  dRqdFracSplit = 0.5;
  dRqdSolSplit = 0.0;
  dRqdLiqSplit = 0.0;
  dRqdVapSplit = 1.0;

  dFeedQm = 0.0;
  dProdQm0 = 0.0;
  dProdQm1 = 0.0;
  }

//---------------------------------------------------------------------------

void Splitter::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void Splitter::BuildDataFields()
  {
  DD.Text("Requirements...");
  DD.CheckBox("SplitByPhase", "",  &bDoPhaseSplit, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Double("Split", "",       &dRqdFracSplit, MF_PARAMETER|(bDoPhaseSplit ? MF_NO_VIEW : 0), MC_Frac);
  DD.Double("SolidSplit", "",  &dRqdSolSplit,  MF_PARAMETER|(bDoPhaseSplit ? 0 : MF_NO_VIEW), MC_Frac);
  DD.Double("LiquidSplit", "", &dRqdLiqSplit,  MF_PARAMETER|(bDoPhaseSplit ? 0 : MF_NO_VIEW), MC_Frac);
  DD.Double("VapourSplit", "", &dRqdVapSplit,  MF_PARAMETER|(bDoPhaseSplit ? 0 : MF_NO_VIEW), MC_Frac);
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedQm", "",  &dFeedQm,     MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm0", "", &dProdQm0,    MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm1", "", &dProdQm1,    MF_RESULT|MF_NO_FILING, MC_Qm);

  /* The 4 lines for "Split" to "VapourSplit" could be coded as follows:
  unsigned long FieldFlags;
  if (bDoPhaseSplit)
    FieldFlags = MF_PARAMETER|MF_NO_VIEW;
  else
    FieldFlags = MF_PARAMETER;
  DD.Double("Split", "",   &dRqdFracSplit, MC_Frac, FieldFlags);
  if (bDoPhaseSplit)
    FieldFlags = MF_PARAMETER;
  else
    FieldFlags = MF_PARAMETER|MF_NO_VIEW;
  DD.Double("SolidSplit", "",  &dRqdSolSplit, MC_Frac, FieldFlags);
  DD.Double("LiquidSplit", "", &dRqdLiqSplit, MC_Frac, FieldFlags);
  DD.Double("VapourSplit", "", &dRqdVapSplit, MC_Frac, FieldFlags);
  */
  }

//---------------------------------------------------------------------------

void Splitter::EvalProducts()
  {
  try
    {
    if (1)
      {
      // Examples of Environment Variables
      double X1=StdP;
      double X2=StdT;
      double X3=AtmosPress();
      double X4=AmbientTemp();
      double X5=BaseElevation();
      double X6=WindSpeed();
      double X7=WindDirection();
      }

    //get handles to input and output streams...
    MStream QI;
    FlwIOs.AddMixtureIn_Id(QI, 0);
    MStream & QO0 = FlwIOs[FlwIOs.First[1]].Stream;
    MStream & QO1 = FlwIOs[FlwIOs.First[2]].Stream;

    //make outlet temperature and pressure same as input
    QO0.SetTP(QI.T, QI.P);
    QO1.SetTP(QI.T, QI.P);

    //force input values to be in meaningful ranges...
    dRqdFracSplit = Range(0.0, dRqdFracSplit, 1.0);
    dRqdSolSplit = Range(0.0, dRqdSolSplit, 1.0);
    dRqdLiqSplit = Range(0.0, dRqdLiqSplit, 1.0);
    dRqdVapSplit = Range(0.0, dRqdVapSplit, 1.0);

    //do the work...
    const int NumSpecies = gs_MVDefn.Count();
    for (int i=0; i<NumSpecies; i++)
      {
      if (bDoPhaseSplit)
        {
        if (gs_MVDefn[i].Phase() & MP_Sol)
          QO0.M[i] = QI.M[i] * dRqdSolSplit;
        else if (gs_MVDefn[i].Phase() & MP_Liq)
          QO0.M[i] = QI.M[i] * dRqdLiqSplit;
        else
          QO0.M[i] = QI.M[i] * dRqdVapSplit;
        }
      else
        {
        QO0.M[i] = QI.M[i] * dRqdFracSplit;
        }
      QO1.M[i] = QI.M[i] - QO0.M[i]; //remainder goes to second outlet
      }

    //get display values...
    dFeedQm = QI.MassFlow();
    dProdQm0 = QO0.MassFlow();
    dProdQm1 = QO1.MassFlow();
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

//====================================================================================
