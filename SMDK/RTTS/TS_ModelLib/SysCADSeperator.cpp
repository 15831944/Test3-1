//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SysCADSeperator_CPP
#include "SysCADSeperator.h"
//#pragma optimize("", off)

//====================================================================================

const int idFeed      = 0;
const int idProduct1  = 1;
const int idProduct2  = 2;
const int idProduct3  = 3;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "Product1",       "Product1",  idProduct1,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { "Product2",       "Product2",  idProduct2,  0,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { "Product3",       "Product3",  idProduct3,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { NULL },
  };

//====================================================================================

static double SeperatorDraw[] = { MDrw_Poly,  -4.,3.,  5.,-1.,  1.,-3.,  -4.,3.,
                           MDrw_End};

 DEFINE_TRANSFER_UNIT(SysCADSeperator, "SeperatorUnit", DLL_GroupName)
void SysCADSeperator_UnitDef::GetOptions()
  {
  SetDefaultTag("SC",TRUE);
  SetDrawing("SizeSeparation", SeperatorDraw);
  //SetTreeDescription("Size Separation:RioTinto Seperator Unit");
  SetTreeDescription("RTTS:RioTinto Seperator Unit");
  SetDescription("RioTinto Seperator");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD|MLicense_RTTS);
  };

//====================================================================================

//SysCADSeperator::SysCADSeperator(TaggedObject * pNd) : MBaseMethod(pNd)
SysCADSeperator::SysCADSeperator(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  // Contruct the type of Seperator
  m_Method = CSeperator::eMethod_EfficiencyCurve; 
  m_pCSeperator = &m_CSeperator_EfficiencyCurve;

  // Initialisation Flag
  bInit = true;
  m_bTwoDecks = true;
  m_dOtherLiqToUS = 1.0;
  m_dOtherSolToUS = 1.0;
  }

//---------------------------------------------------------------------------

void SysCADSeperator::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void SysCADSeperator::BuildDataFields()
{

  static MDDValueLst DDMethod[]={
    {CSeperator::eMethod_LoadBased,         "LoadBasedScreen"},
    {CSeperator::eMethod_EfficiencyCurve,   "EfficiencyCurve_BySize"},
    {CSeperator::eMethod_GlobalPartition,   "FixedPartition_BySize"},
    {CSeperator::eMethod_ComponentPartition,"FixedPartition_ByComponent"},
    {0}};

    // SysCAD Tag Name convention ==> no spaces between words
    // Use MF_VISIBLE to make parameters invisible

    // Seperator Parameters
    DD.Page("Configuration");

    //DD.Bool("N", "", &m_bTwoDecks , MF_RESULT);//MF_PARAMETER | MF_SET_ON_CHANGE | MF_PARAM_STOPPED);

    DD.ObjectBegin("TS_Seperation", "Seperation" );


    DD.Long("Method", "", (long*)&m_Method, MF_PARAMETER|MF_SET_ON_CHANGE|MF_PARAM_STOPPED, DDMethod);
  	DD.Double("OtherLiquidSplitToUS", "", &m_dOtherLiqToUS, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  	DD.Double("OtherSolidSplitToUS", "", &m_dOtherSolToUS, MF_PARAMETER, MC_Frac("%"));

    int l = 0;

    switch(m_Method)
      {
      case CSeperator::eMethod_LoadBased:
		  m_pCSeperator =  &m_CSeperator_LoadBased;
          break;

      case CSeperator::eMethod_EfficiencyCurve:
		  m_pCSeperator =  &m_CSeperator_EfficiencyCurve;
          break;

	  case CSeperator::eMethod_GlobalPartition:
		  m_pCSeperator =  &m_CSeperator_GlobalPartition;
		  break;

	  case CSeperator::eMethod_ComponentPartition:
		  m_pCSeperator =  &m_CSeperator_ComponentPartition;
		  break;

      default:
	    m_pCSeperator = NULL;
        break;

      }

	  // We Build all the data field irrespective of the
	  // selected method so all data is persistent.
	  // We just specify the visibility based on the selected method.
	  if (m_pCSeperator==&m_CSeperator_LoadBased)
  		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CSeperator_LoadBased.BuildDataFields(DD);

	  if (m_pCSeperator==&m_CSeperator_EfficiencyCurve)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CSeperator_EfficiencyCurve.BuildDataFields(DD);

	  if (m_pCSeperator==&m_CSeperator_GlobalPartition)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CSeperator_GlobalPartition.BuildDataFields(DD);

	  if (m_pCSeperator==&m_CSeperator_ComponentPartition)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CSeperator_ComponentPartition.BuildDataFields(DD);

	  DD.ObjectEnd();

}

//---------------------------------------------------------------------------

void SysCADSeperator::EvalProducts()
  {
  try
    {
    MStream Feed; // SysCAD Feed Stream
    FlwIOs.AddMixtureIn_Id(Feed, idFeed); //sum all input streams

    //
    // Get References to our Output Streams using the FlwIOs helper class and IO ids
    //
    MStream & P1 = FlwIOs[FlwIOs.First[idProduct1]].Stream; // SysCAD Output Stream
    MStream & P2 = FlwIOs[FlwIOs.First[idProduct2]].Stream; // SysCAD Output Stream (may be invalid if not connected!!!)
    MStream & P3 = FlwIOs[FlwIOs.First[idProduct3]].Stream; // SysCAD Output Stream

    m_bTwoDecks = (FlwIOs.First[idProduct2]>=0);

    //Set both outlet streams to equal inlet stream
    P1=Feed;
    P3=Feed;
    if (m_bTwoDecks)
      {
      P2 = Feed;
      }

    if (Feed.MassFlow()>1.0e-5)
      {
      MIPSD & PSD=*Feed.FindIF<MIPSD>();

      if (!IsNothing(PSD)) // Only do something if our input stream has a SizeData
        {  
        if (m_pCSeperator)
          {
          m_pCSeperator->EvalProducts(Feed,P1,P2,P3,m_bTwoDecks,bInit);
          bInit = false;
          }
        }
      else
        {
        if (Feed.MassFlow()>1.0e-5)
          Log.Message(MMsg_Warning, "No PSD Properties present, report to underflow");
        if (m_bTwoDecks)
          {
          P2.ZeroMass();
          }
        }

      }
    else
      {
      if (m_bTwoDecks)
        {
        P2.ZeroMass();
        }
      }

    //
    // Handle solids and gas species without size data (send all to P3)...
    // Individual evals should handle liquid split
    //
    const int WaterIndex = gs_MVDefn.Lookup("H2O(l)");
    for (int l_SpeciesIndex=0;  l_SpeciesIndex < Feed.Count(); l_SpeciesIndex ++)
      {
      if (Feed.IsGas(l_SpeciesIndex))
        {//put all vapours to US
        P1.putM(l_SpeciesIndex, 0.0);
        if (m_bTwoDecks)
          P2.putM(l_SpeciesIndex, 0.0);
        P3.putM(l_SpeciesIndex, Feed.getM(l_SpeciesIndex));
        }
      else if (Feed.IsLiquid(l_SpeciesIndex) && l_SpeciesIndex!=WaterIndex)
        {//split liquids excluding water
        double m = Feed.getM(l_SpeciesIndex);
        double mus = m * m_dOtherLiqToUS;
        P1.putM(l_SpeciesIndex, m-mus);
        if (m_bTwoDecks)
          {
          m = mus;
          mus = m * m_dOtherLiqToUS;
          P2.putM(l_SpeciesIndex, m-mus);
          }
        P3.putM(l_SpeciesIndex, mus);
        }
      else if (Feed.IsSolid(l_SpeciesIndex) && Feed.HasSizeData(l_SpeciesIndex)==false)
        {//split solids without PSD data
        double m = Feed.getM(l_SpeciesIndex);
        double mus = m * m_dOtherSolToUS;
        P1.putM(l_SpeciesIndex, m-mus);
        if (m_bTwoDecks)
          {
          m = mus;
          mus = m * m_dOtherSolToUS;
          P2.putM(l_SpeciesIndex, m-mus);
          }
        P3.putM(l_SpeciesIndex, mus);
        }
      }
    }

  //
  // Catch SysCAD Model Specific Exceptions
  //
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }

  //
  // Catch Floating Point Exceptions
  //
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }

  //
  // Catch System Exceptions
  //
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }

  //
  // Catch Unkown Exceptions
  //
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//====================================================================================

