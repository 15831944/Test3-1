//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "examplereaction.h"
//#pragma optimize("", off)

//====================================================================================

static MInitialiseTest InitTest("ExampleReaction");

static MSpeciePtr   spAlumina          (InitTest, "Al2O3(l)", false, __FILE__, DLL_GroupName);
static MSpeciePtr   spWater            (InitTest, "H2O(l)", false, __FILE__, DLL_GroupName);
static MSpeciePtr   spTHA              (InitTest, "Al2O3.3H2O(s)", false, __FILE__, DLL_GroupName);
//static MSpeciePtr   spCausticSoda      (InitTest, "NaOH(l)", false);
//static MSpeciePtr   spOccSoda          (InitTest, "Na2O(s)", false);
//static MSpeciePtr   spSodiumCarbonate  (InitTest, "Na2CO3(l)", false);
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

static double Drw_SimpleReaction[] = { MDrw_Poly, -3,4, -3,-4, 3,-4, 3,4, -3,4,
                             MDrw_End };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(SimpleReaction, "Reaction", DLL_GroupName)

void SimpleReaction_UnitDef::GetOptions()
  {
  SetDefaultTag("SR", true);
  SetDrawing("Tank", Drw_SimpleReaction);
  SetTreeDescription("Demo:Simple Reaction");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

//---------------------------------------------------------------------------

SimpleReaction::SimpleReaction(MUnitDefBase * pUnitDef, TaggedObject * pNd) : 
  MBaseMethod(pUnitDef, pNd),
  m_QProd(this, "QProd")//,
  //m_RB(this, "RB")
  {
  //default values...
  m_dSolPrecip = 0.0;
  dFeedT = StdT;
  dProdT = StdT;
  }

//---------------------------------------------------------------------------

void SimpleReaction::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void SimpleReaction::BuildDataFields()
  {
  DD.Text("");
  DD.Text("Requirements...");
  //m_RB.OnOffCheckBox();
  
  DD.Double("SolPrecip", "", &m_dSolPrecip, MF_PARAMETER, MC_Qm("kg/s"));
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedT", "", &dFeedT, MF_RESULT|MF_NO_FILING, MC_T("C"));
  DD.Double("ProdT", "", &dProdT, MF_RESULT|MF_NO_FILING, MC_T("C"));

  DD.Object(m_QProd, MDD_RqdPage);
  //m_RB.BuildDataFields();
  }

//---------------------------------------------------------------------------

bool SimpleReaction::ExchangeDataFields()
  {
  return false;
  }

//---------------------------------------------------------------------------

void SimpleReaction::EvalProducts()
  {
  try
    {
    MStreamI Work; //initialise local empty copy of a stream
    FlwIOs.AddMixtureIn_Id(Work, idFeed); //sum of all feed streams
    const long index = FlwIOs.First[idProd];
    MStream & QO = FlwIOs[index].Stream; //get reference to the actual output stream

    QO = Work; //set output = input (copies all qualities, etc)

    //const double h0 = QO.totHf(); //get enthalpy
    //m_RB.EvalProducts(QO); //call the reaction code on the stream provided
    //double dHf = m_RB.HfSumRct();

    double FC = -1.0;
    MIBayer & FeedB=*Work.FindIF<MIBayer>();
    if (!IsNothing(FeedB))
      {
      //double bleeble = FeedB.TOOC(Work.T);
      FC = FeedB.FreeCaustic(Work.T);
      }
    
    //msCp * MassFlow = totCp

    const double MW_Al2O3     = spAlumina.MW;          //101.961278
    const double MW_Al2O3_3H2O= spTHA.MW;              //156.007118
    const double fact = MW_Al2O3/MW_Al2O3_3H2O;
    const double HoR = 711.28;
    double w = m_dSolPrecip;
    if (w*fact>QO.M[spAlumina])
      w = QO.M[spAlumina]/fact;
    const double TIn = Work.T; // temperature (K)
    const double P = Work.P;
    const double THAIn = Work.M[spTHA];
    const double CpIn = Work.totCp(MP_All, TIn, P);
    const double HfIn = Work.totHf(MP_All, TIn, P);

    double T = TIn;
    QO.M[spAlumina] = Work.M[spAlumina]-fact*w; 		// Alumina transfer: solution to solid
    QO.M[spTHA]     = Work.M[spTHA]+w;
    QO.M[spWater]   = Work.M[spWater]-(1-fact)*w; 	// Water transferred to Hydrate

    const double THAOut = QO.M[spTHA];
    const double SolPrecip = THAOut - THAIn; //kg/s

    //method 1 : No HOR, simply adjust masses...
    QO.MarkStateChanged(); //this forces recalculation of enthalpy based on new stream makeup
    const double T1 = QO.T;
    const double Hf1 = QO.totHf();
    const double Cp1 = QO.totCp();
    const double dHf1 = Hf1-HfIn;

    //method 2 : Using Cp, set T...
    const double Cp = QO.totCp(MP_All, T, P);
    const double enthIn = CpIn * K2C(TIn);
    const double NewT = (SolPrecip*HoR*fact + enthIn) / GTZ(Cp);
    QO.SetTP(C2K(NewT), P);
    const double T2 = QO.T;
    const double Hf2 = QO.totHf();
    const double Cp2 = QO.totCp();
    const double dHf2 = Hf2-HfIn;

    //method 3 : Use HOR, set totHf
    const double HeatOfCr = HfIn + SolPrecip*fact*HoR;// Heat of crystallisation
    QO.Set_totHf(HeatOfCr);
    const double T3 = QO.T;
    const double Hf3 = QO.totHf();
    const double Cp3 = QO.totCp();
    const double dHf3 = Hf3-HfIn;

    //get display values...
    m_QProd = QO;
    dFeedT = TIn;
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

//====================================================================================
