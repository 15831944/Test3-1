//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-06-09 12:26:55 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#include "stdafx.h"
#include "TTWORFilter.h"

#ifdef TTDEBUG
#include <sstream>
#include <iomanip>
#endif
//=========================================================================== 



//   Define the species in the particular precipiation model...



enum FilterMethod {CM_Molerus, CM_Lynch, CM_Plitt };



const long idFeed  = 0;
const long idOFlow  = 1;
const long idUFlow  = 2;
const long idCond = 3;


static MInOutDefStruct s_IODefs[]=
{
	//  Desc;             Name;         Id; Rqd; Max; CnId, FracHgt;  Options;
	{ "Feed",          "Feed1",      idFeed ,   1,  5,    0,    1.0f,  MIO_In |MIO_Material },
	{ "Overflow",      "Overflow",   idOFlow,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Underflow",     "Underflow",  idUFlow,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Condensate",    "Cond",       idCond,    0,   1,    0,    1.0f,  MIO_In |MIO_Material },
	{ NULL },
};



double Drw_CFilter[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_Poly, -5,-10, 5,-8, -5, -8, 5, -10, 
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CFilter, "Filter", DLL_GroupName)

void CFilter_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CFilter);
  SetTreeDescription("Worsley:Filter");
  SetDescription("TODO: Worsley Precip Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------




void CFilter::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {CM_Molerus,   "Molerus" },
    {CM_Lynch,   "Lynch"},
    {CM_Plitt, "Plitt"},
    {0}};


  static MDDValueLst  iPSDDisp[] = {
    {0, "Mass", 0},
    {1, "Fraction", 0},
    {2, "Number", 0},
    NULL
};




  DD.Text  ("");

  DD.CheckBox("On", "",  &bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Verbose", "",  &bVerbose, MF_PARAMETER|MF_SET_ON_CHANGE|MF_INIT_HIDDEN);



  DD.Text  ("Requirements"); 
  DD.Double("CakeSolids", "",   &m_dCakeSolids         ,MF_PARAMETER, MC_Frac("%"));


  DD.Double("ExcessBypassUFlow", "",   &m_dByPass         ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("MeasureSharpness",  "",   &m_dSharp_Index     ,MF_PARAMETER, MC_);
  DD.Double("CutSize",           "",   &m_dCut_Size        ,MF_PARAMETER, MC_);
  DD.Double("L_0",  "",                &m_dL0              ,MF_PARAMETER, MC_);


  DD.Text  ("");
  DD.Text  ("Results Tank");

  
  DD.Double("SolidsSplit",  "",                &m_dxS        ,MF_RESULT, MC_Frac("%"));

    

  
#ifdef TTDEBUG
  //std::string s;
  DD.Show(bTTDebug);
  //    DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER, iPSDDisp);
    DD.Page("Inlet");
    const int NumSpecies = gs_MVDefn.Count();
    DD.Page("Size In");
    for (int i=0; i<=nClasses; i++) { 
      std::stringstream os;
      os << "Size " << std::setw(3) << i << std::setw(12) << L[i];
      // s=os.str();
      DD.Double(os.str().c_str(), "", dd[3]+i, MF_RESULT|MF_NO_FILING, MC_None);
    }
    DD.Page("Size OF");
    for (int i=0; i<=nClasses; i++) {
      std::stringstream os;
      os << "Size " << std::setw(3) << i;
      DD.Double(os.str().c_str(), "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
    }
    DD.Page("Size UF");
    for (int i=0; i<=nClasses; i++) {
      std::stringstream os;
      os << "Size " << std::setw(3) << i;
      DD.Double(os.str().c_str(), "", dd[5]+i, MF_RESULT|MF_NO_FILING, MC_None);
    }
    DD.Page("Split.uf.of");
    for (int i=0; i<=nClasses; i++) {
      std::stringstream os;
      os << "Size " << std::setw(3) << i;
      DD.Double(os.str().c_str(), "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
    }
    
#endif


}

//---------------------------------------------------------------------------

bool CFilter::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }




CFilter::CFilter(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd)
{

  bOnline = 1;
  m_dByPass  = 0.05;
  m_dSharp_Index = 5.0  ;
  m_dCut_Size     = 100.0;
  m_dL0 = 0.0;
  bVerbose = false;
  
  
  

  L[0] = 1.58740105;
  for (int i=1; i<=nClasses; i++) L[i] = L[i-1]*1.25992105;
  for (int i=0; i<=nClasses; i++) Lav[i] = L[i]*1.12992105;



  

}

//---------------------------------------------------------------------------

CFilter::~CFilter()
{
//   delete[] x;
//   delete[] xo;
}

//---------------------------------------------------------------------------

void CFilter::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

const long OM_Simple      = 0;
const long OM_Condensing  = 1;



//---------------------------------------------------------------------------
// Modified for NaAl[OH]4 as primary species.


//---------------------------------------------------------------------------
// All the precipitation rate calcs are done in this routine 
// Takes the product composition and calculates rates from this
// Rates (in kg/s) are put into global x
//---------------------------------------------------------------------------




//------------------------------------------------------------------------
//  Check for convergence of the iteration
//
//
//------------------------------------------------------------------------



void CFilter::DoFilter(MStream &OFlow, MStream &UFlow) 
{
  double pVol_U = 0.0, pVol_O=0.0, mO=0.0, mU=0.0;
  double eff, ab, nl[nClasses+1][2];
  double xS;
  //CNM double xL;  // Solids and Liquids split
  
  MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
  MIPSD & OFPSD = OFlow.IF<MIPSD>(false);
  MIPSD & UFPSD = UFlow.IF<MIPSD>(false);



  for (int i=0; i<=nClasses; i++) {
    if (m_dL0==0) ab=m_dByPass; else ab=m_dByPass*(1-Lav[i]/m_dL0);
    eff = ab + (1-ab)/( 1 + pow(Lav[i]/m_dCut_Size, -m_dSharp_Index) );



    nl[i][0]=FeedPSD.getFrac(i)*eff;
    nl[i][1]=FeedPSD.getFrac(i)*(1-eff);
    mU += nl[i][0];
    mO += nl[i][1];
    dd[6][i] = eff;
    dd[5][i] = m_dByPass;
    dd[4][i] = m_dL0;
    
    
    
  }
  if (mO+mU !=0.0) 
    xS = mU/(mU+mO);
  else
    xS = 1.0;
  m_dxS = xS;
  //  m_dEff = eff;
  double condFlow=0.0;
  if (bCondConnected)
    condFlow = Condensate.Mass();
  UFlow.SetF(Feed, MP_Sol, xS);
  double UFlowSolids = UFlow.Mass();
  double UFlowTot = UFlowSolids/m_dCakeSolids;
  double UFlowLiq = UFlowTot-UFlowSolids;
  // Do we have enough condensate to achieve pure condensate cake liquor?

  if (UFlowLiq < condFlow) {  // Yes
    UFlow.AddF(Condensate, MP_All, UFlowLiq/condFlow);  // Take needed cond;
    OFlow.SetF(Feed, MP_Liq, 1.0);
    OFlow.AddF(Feed, MP_Sol, 1.-xS);
    OFlow.AddF(Condensate, MP_All, 1.-UFlowLiq/condFlow);
  } else {  
    UFlow.AddF(Condensate, MP_All, 1.0);   // Take *all* the condensate 
    double topup  = UFlowLiq - condFlow;   // Need to topup with liquor
    double feedL = Feed.Mass(MP_Liq);
    if (topup<=feedL) { // Is there enough feed to make the density?
      UFlow.AddM(Feed, MP_Liq, topup);
      OFlow.SetM(Feed, MP_Liq, feedL-topup);
      OFlow.AddF(Feed, MP_Sol, 1.-xS);
      if (bVerbose) 
	Log.Message(MMsg_Warning, "Insufficient condensate to achieve pure water cake liquor");
    } else { // Put it all into cake
      UFlow.SetF(Feed, MP_All, 1.0);
      if (bVerbose)
	Log.Message(MMsg_Error, "Insufficient liquor to achieve cake density");
      OFlow.SetF(Feed, MP_All, 0.0);
    }
  }


  
  for (int i=0; i<=nClasses; i++) {
    if (xS<1.0) 
      OFPSD.putFrac(i, nl[i][1]/(1-xS));
    UFPSD.putFrac(i, nl[i][0]/xS);
  }
  OFPSD.Normalise();
  UFPSD.Normalise(); 
  
}
  


void CFilter::EvalProducts()
  {
  try {

    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & OFlow = FlwIOs[FlwIOs.First[idOFlow]].Stream; //Reference to the overflow out stream
    MStream & UFlow = FlwIOs[FlwIOs.First[idUFlow]].Stream; //Reference to the undrflow out stream
    

    OFlow = Feed;
    UFlow = Feed;
    

    OFlow.SetF(Feed, MP_All, .5);
    UFlow.SetF(Feed, MP_All, .5);

    // Check for condensate input stream
    const int iCondensate = FlwIOs.First[idCond];
    bCondConnected = (iCondensate>0);
    if (bCondConnected)
      FlwIOs.AddMixtureIn_Id(Condensate, idCond);


    bool streamOK=((Feed.Mass()>1.0e-3) && (Feed.Mass(MP_Sol)>1.0e-6));
    



    

    MIBayer & ProdB=OFlow.IF<MIBayer>(false);   // Do the checks up front
    Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Feed Stream - Not BATC Bayer Model");

    MIPSD & ProdPSD = OFlow.IF<MIPSD>(false);
    Log.SetCondition(IsNothing(ProdPSD), 1, MMsg_Warning, "Bad Feed Stream - No PSD Available");
    if (IsNothing(ProdB) || IsNothing(ProdPSD)) streamOK = false;




  

    if (bOnline) {


      if (streamOK) {  // Online and have Bayer and SSA properties...
	displayPSD(Feed, 3);
	DoFilter(OFlow, UFlow);
	displayPSD(OFlow, 4);
	displayPSD(UFlow, 5);
      
      } else  {   // Just tidy up and put some sensible stuff in the results...
	if (bVerbose) 
	  Log.Message(MMsg_Note, "Bad Feed.... Filter");
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




void CFilter::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
      //    CI.AddPowerIn(0, -dThermalLoss);
    }
  }




void CFilter::displayPSD(MStream &ms, int scrn)
{
  MIPSD & mpsd = ms.IF<MIPSD>(false);   /// Does the mud stream have PSD...
  int ic = mpsd.getSizeCount();
  if (ic>40) ic=40;
  double x;
  for (int i=0; i<ic; i++) {
    switch (iPSD) {
    case 0:
      x = mpsd.getMass(i);
      break;
    case 1: 
      x = mpsd.getFrac(i);
      break;
    case 2:
      x = mpsd.getMass(i);
      
    }
    dd[scrn][i]=x;
  }
}
