//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2006-06-05 16:43:54 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#include "stdafx.h"
#include "TTWORPSDUnit.h"
//=========================================================================== 



//   Define the species in the particular precipiation model...






const long idFeed  = 0;
const long idProd  = 1;




static MInOutDefStruct s_IODefs[]=
{
	//  Desc;             Name;         Id; Rqd; Max; CnId, FracHgt;  Options;
	{ "Feed",          "Feed1",      idFeed ,   1,  5,    0,     1.0f,  MIO_In |MIO_Material },
	{ "Prod",          "Prod",   idProd,   1,  1,    0,    1.0f,  MIO_Out|MIO_Material },
	{ NULL },
};



double Drw_CPSDUnit[] = 
  { 
    MDrw_Poly, -10, -10, 10,-10, 10,10, -10,10, -10, -10, 
    -10, -9, -8, -9,
    -8,  -7, -6, -7, 
    -6,  -6, -4, -6,
    -4,  -2, -2, -2,
    -2,   3,  0,  3,
     0,   5,  2,  5,
     2,   6,  4,  6,
     4,   4,  6, 4,
     6,  -3 ,  8, -3,
     8,  -8,  10, -8,
     
    MDrw_End 
  };



//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(CPSDUnit, "PSDUnit", DLL_GroupName)

void CPSDUnit_UnitDef::GetOptions()
{
  SetDefaultTag("PC");
  SetDrawing("Tank", Drw_CPSDUnit);
  SetTreeDescription("Worsley:PSDUnit");
  SetDescription("Worsley Precip Model: PSD Generation and Display");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_Alumina);
  SetModelLicense(MLicense_HeatExchange|MLicense_Alumina);
};

//---------------------------------------------------------------------------




void CPSDUnit::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {0,   "Number/kg" },
    {1,   "Number/m^3@25"},
    {2,   "Mass Fraction"},
    {0}};



  DD.Text  ("");
  DD.Double("Conversion", "", &m_dCnv, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass.Solids", "", &m_dMassSolids, MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("Vol.Slurry", "", &m_dVolSlurry, MF_RESULT|MF_NO_FILING, MC_Qv);
  DD.Long("PSD.Type", "", &iPSDType, MF_PARAMETER, DDB1);
  DD.Long("Display.Type", "", &iDisplayType, MF_PARAMETER, DDB1);
  DD.CheckBox("DisplayOnly", "", &bDisplayOnly, MF_PARAMETER|MF_SET_ON_CHANGE);
  





  

  
  const int NumSpecies = gs_MVDefn.Count();
  DD.Show(!bDisplayOnly);
  DD.Page("Size Distribution");
  if (iPSDType==0)
    DD.Text("Number per kg solids");
  else
    DD.Text("Number per m^3 slurry @25");
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i << std::setw(12) << L[i] << " " << Lav[i] ;
    DD.Double(os.str().c_str(), "", dd+i, MF_PARAMETER, MC_None);
  }

  DD.Double("Number", "", dd+26, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Area", "", dd+27, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Vol", "", dd+28, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass", "", dd+29, MF_RESULT|MF_NO_FILING, MC_None);

  DD.Show();


  DD.Page("SizeOut");
  switch (iDisplayType) {
  case 0:
    DD.Text("Number per kg solids");
    break;
  case 1:
    DD.Text("Number per m^3 slurry @25");
    break;
  case 2:
    DD.Text("Mass Fraction");
    break;
  }
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size_Out " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd1+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Text("");
  DD.Double("Number", "", dd1+26, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Area", "", dd1+27, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Vol", "", dd1+28, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass", "", dd1+29, MF_RESULT|MF_NO_FILING, MC_None);



}

//---------------------------------------------------------------------------

bool CPSDUnit::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }




CPSDUnit::CPSDUnit(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd)
{   //default values...
  iPSDType = 0;
  iDisplayType = 0;
  bDisplayOnly = false;
  


}

//---------------------------------------------------------------------------

CPSDUnit::~CPSDUnit() 
{
}

//---------------------------------------------------------------------------

void CPSDUnit::Init()
{
  SetIODefinition(s_IODefs);
#ifdef TTDEBUG
  dbg.tcltk_init();
#endif
}

//---------------------------------------------------------------------------







void CPSDUnit::DoPSDUnit(MStream &Prod) 
{
  try {
    if (bDisplayOnly)
      b = BrahmaPSD(Prod);
    else {
      if (iPSDType == 0) 
	b = BrahmaPSD(dd, m_dCnv);
      else 
	b = BrahmaPSD(dd, 1.0);
    }
  }
  catch(...) {
    Log.Message(MMsg_Error, "Error generating PSD");
  }
  

  dd[27]=b.getArea();
  dd[28]=b.getVol();
  dd[26]=b.getN();
  
  double rhoS = b.put(Prod);
  dd[29] = rhoS;



}
  
  
void CPSDUnit::displayPSD(BrahmaPSD &psd) {

  for (int i=0; i<=nClasses; i++) {
    dd1[i]=psd.getN(i);
    if (iDisplayType==0) dd1[i] /= m_dCnv;
  }

  dd1[26] = psd.getN();
  dd1[27] = psd.getArea();
  dd1[28] = psd.getVol();
  dd1[29] = m_dCnv;
  
}
  



    


void CPSDUnit::EvalProducts()
  {

  try {

    MStreamI Feed;
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & Prod = FlwIOs[FlwIOs.First[idProd]].Stream; //Reference to the overflow out stream
    //dbg.log("EvalProds");
    Prod = Feed;

    bool streamOK=((Feed.Mass()>1.0e-9) && (Feed.Mass(MP_Sol)>1.0e-9));
    
    m_dMassSolids = Prod.Mass(MP_Sol);
    m_dVolSlurry = Prod.Volume(MP_All, C2K(25));


    MIBayer & ProdB=Prod.IF<MIBayer>(false);   // Do the checks up front
    Log.SetCondition(IsNothing(ProdB), 1, MMsg_Warning, "Bad Feed Stream - Not Bayer Model");

    MIPSD & ProdPSD = Prod.IF<MIPSD>(false);
    Log.SetCondition(IsNothing(ProdPSD), 1, MMsg_Warning, "Bad Feed Stream - No PSD Available");
    if (IsNothing(ProdB) || 
	IsNothing(ProdPSD) ||
	(m_dVolSlurry < 1.0e-4)) streamOK = false;
    

    //if (streamOK) dbg.log("Stream OK");
    //else dbg.log("Bad Stream");



     if (streamOK) {  // Online and have Bayer and SSA properties...
       m_dCnv  = m_dMassSolids/m_dVolSlurry;
       
       DoPSDUnit(Prod);
       displayPSD(b);
       
       
      
     } else  {   // Just tidy up and put some sensible stuff in the results...
       // Log.Message(MMsg_Warning, "Bad Stream: PSD Unit");
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









