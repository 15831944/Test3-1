//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EXAMPLETANKDYN_CPP
#include "exampletankdyn.h"

//====================================================================================
//define Input/Output connections
                                         
static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;     Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Top of Tank",    "Top",     0,   0,  10,    0,    1.0f,  MIO_InOut|MIO_Material|MIO_PipeEntry|MIO_ApertureHoriz },
    { "Side of Tank",   "Side",    1,   0,  10,    0,    0.5f,  MIO_InOut|MIO_Material|MIO_PipeEntry                   },    
    { "Bottom of Tank", "Base",    2,   0,  10,    0,    0.0f,  MIO_InOut|MIO_Material|MIO_PipeEntry|MIO_ApertureHoriz },
    { "OverFlow",       "OverFlw", 3,   0,  10,    0,    1.0f,  MIO_InOut|MIO_Material|MIO_PipeEntry                   },
    { "GasVent",        "GasVent", 4,   0,  10,    0,    1.0f,  MIO_InOut|MIO_Material|MIO_PipeEntry|MIO_GasVent       },
    { NULL },
  };

IMPLEMENT_SURGEMETHOD(Tank, "Tank", "Demo:Tank", "Data", NULL, DLL_GroupName);

Tank::Tank(TaggedObject * pNd) : MBaseMethod(pNd)
  {
  //default values...
  dFeedQm = 0.0;
  dProdQm0 = 0.0;
  dProdQm1 = 0.0;
  }

//---------------------------------------------------------------------------

void Tank::Init()
  {
  SetRunModeOptions(MO_Dynamic);
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void Tank::BuildDataFields()
  {
  DD.Text("Requirements...");
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedQm",  &dFeedQm,     MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm0", &dProdQm0,    MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm1", &dProdQm1,    MF_RESULT|MF_NO_FILING, MC_Qm);
  }

//---------------------------------------------------------------------------

bool Tank::PreStartCheck(CString & ErrMsg) 
  { 
  return true; 
  }

//---------------------------------------------------------------------------

void Tank::EvalProducts()
  {
  //get handles to input and output streams...
  
  if (IsProBal)
    {
    // Get Inputs
    MStream QI;
    FlwIOs.AddMixtureIn(QI);

    // Total Estimated Out 
    double FT=0;
    for (int o=FlwIOs.First_Out; o>=0; o=FlwIOs.Next_Out[o])
      FT+=FlwIOs[o].EstFlwOut;
    FT=GTZ(FT);

    // Divide Inputs between Outputs
    for (o=FlwIOs.First_Out; o>=0; o=FlwIOs.Next_Out[o])
      {
      MStream & QO = FlwIOs[o].Stream;
      QO = QI;
      QO *= FlwIOs[o].EstFlwOut/FT;
      }

    //get display values...
    dFeedQm = QI.MassFlow(); //or dFeedQm = QI.getQm();
    }
  else
    {
    // Get Inputs
    MStream QI;
    FlwIOs.AddMixtureIn(QI);

    // Total Estimated Out 
    double FT=0;
    for (int o=FlwIOs.First_Out; o>=0; o=FlwIOs.Next_Out[o])
      FT+=FlwIOs[o].EstFlwOut;
    FT=GTZ(FT);

    // Divide Inputs between Outputs
    for (o=FlwIOs.First_Out; o>=0; o=FlwIOs.Next_Out[o])
      {
      MStream & QO = FlwIOs[o].Stream;
      QO = QI;
      QO *= FlwIOs[o].EstFlwOut/FT;
      }

    //get display values...
    dFeedQm = QI.MassFlow(); //or dFeedQm = QI.getQm();
    }
  }

//---------------------------------------------------------------------------

void Tank::EvalDerivs(double StepSize) 
  {
  };

//---------------------------------------------------------------------------

void Tank::EvalIntegral(double StepSize) 
  {
  };

//====================================================================================
