//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EXAMPLETANKDYN_CPP
#include "exampletankdyn.h"

//====================================================================================
//define connection ids for convenience
const int idIn = 0;
const int idOut = 1;

//define Input/Output connections
static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;     Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Input",          "Input",   idIn,   0,  10,    0,    1.0f,  MIO_InOut|MIO_Material|MIO_PipeEntry|MIO_ApertureHoriz },
    { "Output",         "Output",  idOut,  0,  10,    0,    0.5f,  MIO_InOut|MIO_Material|MIO_PipeEntry                   },
    { NULL },
  };

//define default drawing symbol
static double TankDraw[] = { MDrw_Poly,  -9,8,  -7,-8,  7,-8,  9,8,
                           MDrw_End};

//---------------------------------------------------------------------------
//macro to declare and add the model to the system
DEFINE_SURGE_UNIT(Tank, "Tank", DLL_GroupName)

void Tank_UnitDef::GetOptions()
  {
  SetDefaultTag("TK", true);
  SetDrawing("Tank", TankDraw);
  SetTreeDescription("Demo:Tank");
  SetDescription("Example Dynamic Tank");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

//---------------------------------------------------------------------------

Tank::Tank(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  dFeedQm = 0.0;
  dProdQm0 = 0.0;
  dProdQm1 = 0.0;
  }

//---------------------------------------------------------------------------

void Tank::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void Tank::BuildDataFields()
  {
  DD.Text("Requirements...");
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedQm", "", &dFeedQm,     MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm0", "", &dProdQm0,    MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm1", "", &dProdQm1,    MF_RESULT|MF_NO_FILING, MC_Qm);
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
  
  if (IsSolveDirect)
    {
    // Get Inputs
    MStream QI;
    FlwIOs.AddMixtureIn_Id(QI, idIn); //sum all inputs

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
    FlwIOs.AddMixtureIn_Id(QI, idIn); //sum all inputs

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
