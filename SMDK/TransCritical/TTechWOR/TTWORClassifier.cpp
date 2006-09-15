//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-07-04 09:54:09 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#include "stdafx.h"
#include "TTWORClassifier.h"

//=========================================================================== 



//   Define the species in the particular precipiation model...



enum ClassifierMethod {CM_Molerus, CM_Lynch, CM_Plitt };



const long idFeed  = 0;
const long idOFlow  = 1;
const long idUFlow  = 2;


static MInOutDefStruct s_IODefs[]=
{
	//  Desc;             Name;         Id; Rqd; Max; CnId, FracHgt;  Options;
	{ "Feed",          "Feed1",      idFeed ,   1,  5,    0,    1.0f,  MIO_In |MIO_Material },
	{ "Overflow",      "Overflow",   idOFlow,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ "Underflow",     "Underflow",  idUFlow,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ NULL },
};



double Drw_CClassifier[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CClassifier, "Classifier", DLL_GroupName)

void CClassifier_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CClassifier);
  SetTreeDescription("Worsley:Classifier");
  SetDescription("TODO: Worsley Precip Model");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------




void CClassifier::BuildDataFields()
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

// #ifdef TTDEBUG
//   DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
// #endif
  DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER, iPSDDisp);


  DD.CheckBox("On", "",  &bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);


  DD.Long  ("ClassMethod", "", &iClassMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);

  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));


  DD.Text  ("");
  DD.Text  ("Results Tank");



  DD.Double("ExcessBypassUFlow", "",   &m_dByPass         ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("MeasureSharpness",  "",   &m_dSharp_Index     ,MF_PARAMETER, MC_);
  DD.CheckBox("CalcCutSize", "",       &bCalcCut_Size     ,MF_PARAMETER | MF_SET_ON_CHANGE);
  DD.Show(bCalcCut_Size);
  DD.Double("Viscosity", "", &dViscosity, MF_RESULT, MC_Visc);
  DD.Double("ViscCorrection", "", &tTerm, MF_RESULT, MC_);
  DD.Show();



  DD.Double("CutSize",           "",   &m_dCut_Size        , (bCalcCut_Size ? MF_RESULT : MF_PARAMETER), MC_);
  DD.Double("LiquorToUFlow",  "",      &m_dSlurry_split    ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("OverPassOFlow",  "",      &m_dOver_pass       ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("L_0",  "",                &m_dL0              ,MF_PARAMETER, MC_);
	  
  DD.Double("SolidsSplit",  "",                &m_dxS        ,MF_RESULT, MC_Frac("%"));
  DD.Double("Efficiency",  "",                &m_dEff        ,MF_RESULT, MC_);

    

  
  //std::string s;
  DD.Show();
  //    DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER, iPSDDisp);
    DD.Page("Inlet");
    const int NumSpecies = gs_MVDefn.Count();
    DD.Page("Size In");
    for (int i=0; i<=nClasses; i++) { 
      std::stringstream os;
      os << "Size " << std::setw(3) << i << std::setw(12) << L[i] << " " << Lav[i] ;
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
    DD.Page("Efficiency");
    for (int i=0; i<=nClasses; i++) {
      std::stringstream os;
      os << "Size " << std::setw(3) << i;
      DD.Double(os.str().c_str(), "", dd[7]+i, MF_RESULT|MF_NO_FILING, MC_None);
    }
    


}

//---------------------------------------------------------------------------

bool CClassifier::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }




CClassifier::CClassifier(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd)
{   //default values...
//   x=new double[4];
//   xo = new double[4];




  bOnline = 1;
  m_dByPass  = 0.05;
  m_dSharp_Index = 5.0  ;
  m_dCut_Size     = 100.0;
  m_dSlurry_split = .05;
  m_dOver_pass = 0.0;
  m_dL0 = 0.0;
  
  

  L[0] = 1.58740105;
  for (int i=1; i<=nClasses; i++) L[i] = L[i-1]*1.25992105;
  for (int i=0; i<=nClasses; i++) Lav[i] = L[i]*1.12992105;



  

}

//---------------------------------------------------------------------------

CClassifier::~CClassifier()
{
//   delete[] x;
//   delete[] xo;
}

//---------------------------------------------------------------------------

void CClassifier::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

const long OM_Simple      = 0;
const long OM_Condensing  = 1;




double Viscosity_cp(MStream & s) {
  MIBayer & sB=s.IF<MIBayer>(false);
  double T = 25.0;
  double C = sB.CausticConc(25+273.15);
  double A = sB.AluminaConc(25+273.15);
  double LVis = -0.11801 + 0.00413*C - 7.8966*1e-5*C*T + 3.712*1e-7*C*T*T
    + 0.0015685*A + 8.679199e-06*A*C - 2.436e-05*A*T;
  //          + 0.0018944*(S-C) + 0.0070503*TOC + 0.002471*Cl+ 0.018308*SO4;

  return pow(10., LVis); 
    

}



void CClassifier::DoClassifier(MStream  &OFlow, MStream &UFlow) 
{
  double pVol_U = 0.0, pVol_O=0.0, mO=0.0, mU=0.0;
  double eff, ab, nl[nClasses+1][2];
  double xS;
  
  MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
  MIPSD & OFPSD = OFlow.IF<MIPSD>(false);
  MIPSD & UFPSD = UFlow.IF<MIPSD>(false);

  if (bCalcCut_Size) {
    double visc = Viscosity_cp(Feed);
    dViscosity = visc/1000.;
    double grho = 9.81*(2420-Feed.Density(MP_All));
    double t = sqrt(visc/grho);
    tTerm = t;
  }  
			



  for (int i=0; i<=nClasses; i++) {
    if (m_dL0==0) ab=m_dByPass; else ab=m_dByPass*(1-Lav[i]/m_dL0);
    switch (iClassMethod) {
    case CM_Molerus:
      eff = ab + (1-ab) * (1-m_dOver_pass) /
	( 1 + pow(Lav[i]/m_dCut_Size, -m_dSharp_Index) );
      break;
    case CM_Lynch:
      eff = ab + (1-ab) * (1-m_dOver_pass) * 
	( exp(m_dSharp_Index*Lav[i]/m_dCut_Size) - 1 ) /
	( exp(m_dSharp_Index) +  exp(m_dSharp_Index*Lav[i]/m_dCut_Size) - 2 );
      break;
    case CM_Plitt:
      eff = ab + (1-ab) * (1-m_dOver_pass) *
	( 1 - exp(-0.693147*pow(Lav[i]/m_dCut_Size, m_dSharp_Index)) );
      break;
    }
    nl[i][0]=FeedPSD.getFrac(i)*eff;
    nl[i][1]=FeedPSD.getFrac(i)*(1-eff);
    mU += nl[i][0];
    mO += nl[i][1];

    dd[7][i] = eff;
    
  }
  if (mO+mU !=0.0) 
    xS = mU/(mU+mO);
  else
    xS = 1.0;
  m_dxS = xS;
  m_dEff = eff;
  
  
  UFlow.SetF(Feed, MP_Liq, m_dSlurry_split);
  UFlow.AddF(Feed, MP_Sol, xS);
  OFlow.SetF(Feed, MP_Liq, 1.-m_dSlurry_split);
  OFlow.AddF(Feed, MP_Sol, 1.-xS);
  for (int i=0; i<=nClasses; i++) {
    if (xS<1.0) 
      OFPSD.putFrac(i, nl[i][1]/(1-xS));
    UFPSD.putFrac(i, nl[i][0]/xS);
  }
  OFPSD.Normalise();
  UFPSD.Normalise(); 
  
}
  
  
  



    


void CClassifier::EvalProducts()
  {
  try {

    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & OFlow = FlwIOs[FlwIOs.First[idOFlow]].Stream; //Reference to the overflow out stream
    MStream & UFlow = FlwIOs[FlwIOs.First[idUFlow]].Stream; //Reference to the undrflow out stream
    
    OFlow = Feed;
    UFlow = Feed;
    

    OFlow.SetF(Feed, MP_All, .5);
    UFlow.SetF(Feed, MP_All, .5);
    

    bool streamOK=((Feed.Mass()>1.0e-3) && (Feed.Mass(MP_Sol)>1.0e-6));
    
    
    MIBayer & ProdB=OFlow.IF<MIBayer>(false);   // Do the checks up front
    Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Feed Stream - Not BATC Bayer Model");

    MIPSD & ProdPSD = OFlow.IF<MIPSD>(false);
    Log.SetCondition(IsNothing(ProdPSD), 1, MMsg_Warning, "Bad Feed Stream - No PSD Available");
    if (IsNothing(ProdB) || IsNothing(ProdPSD)) streamOK = false;


    if (bOnline && streamOK) {  // Online and have Bayer and SSA properties...
      

      DoClassifier(OFlow, UFlow);
      displayPSD(Feed, 3);
      displayPSD(OFlow, 4);
      displayPSD(UFlow, 5);
      

    } else  {   // Just tidy up and put some sensible stuff in the results...
      Log.Message(MMsg_Warning, "Bad Feed.... Classifier");
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




void CClassifier::ClosureInfo(MClosureInfo & CI)
  {//ensure heat balance
  if (CI.DoFlows())
    {
      //    CI.AddPowerIn(0, -dThermalLoss);
    }
  }





void CClassifier::displayPSD(MStream &ms, int scrn)
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
