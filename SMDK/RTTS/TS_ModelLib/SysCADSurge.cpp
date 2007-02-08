//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SysCADSurge_CPP
#include "SysCADSurge.h"
//#pragma optimize("", off)

//====================================================================================

const int idFeed      = 0;
const int idProduct   = 1;


static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "Product",        "Product",   idProduct ,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
     { NULL },
  };

//====================================================================================

static double SurgeDraw[] = { MDrw_Poly,  -4.,4.,  4.,4.,  4.,-4.,  -4.,-4.,  -4.,4.,
                           MDrw_End};

 DEFINE_TRANSFER_UNIT(SysCADSurge, "SurgeUnit", DLL_GroupName)
void SysCADSurge_UnitDef::GetOptions()
  {
  SetDefaultTag("BIN",TRUE);
  SetDrawing("Surge", SurgeDraw);
  //SetTreeDescription("Size Separation:RioTinto Surge Unit");
  SetTreeDescription("RTTS:RioTinto Surge Unit");
  SetDescription("RioTinto Surge");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD|MLicense_RTTS);
  };

//====================================================================================

//SysCADSurge::SysCADSurge(TaggedObject * pNd) : MBaseMethod(pNd)
SysCADSurge::SysCADSurge(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {

	// Contruct the type of Surge
	m_Method = CSurge::eMethod_PlugFlow; 
	m_pCSurge = NULL;
	//m_pCSurge = &m_CSurge_PlugFlow;

    // Initialisation Flag
    bInit = true;
    
  }

//---------------------------------------------------------------------------

void SysCADSurge::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void SysCADSurge::BuildDataFields()
{

  static MDDValueLst DDMethod[]={
    {CSurge::eMethod_PlugFlow,         "Plug Flow"},
    {CSurge::eMethod_FullyMixed,       "Fully Mixed"},
    {0}};

    // SysCAD Tag Name convention ==> no spaces between words
    // Use MF_VISIBLE to make parameters invisible

    // Surge Parameters
    DD.Page("Configuration");

    //DD.Bool("N", "", &m_bTwoDecks , MF_RESULT);//MF_PARAMETER | MF_SET_ON_CHANGE | MF_PARAM_STOPPED);

   DD.ObjectBegin("TS_Surge", "Surge" );


    DD.Long("Method", "", (long*)&m_Method, MF_PARAMETER|MF_SET_ON_CHANGE|MF_PARAM_STOPPED, DDMethod);
    int l = 0;

    switch(m_Method)
      {
      case CSurge::eMethod_PlugFlow:
		  //m_pCSurge =  &m_CSurge_PlugFlow;
          break;

    //case CSurge::eMethod_FullyMixed:
	//	  m_pCSurge =  &m_CSurge_FullyMixed;
    //      break;

      default:
	    m_pCSurge = NULL;
        break;

      }

	  // We Build all the data field irrespective of the
	  // selected method so all data is persistent.
	  // We just specify the visibility based on the selected method.
	  /*
	  if (m_pCSurge==&m_CSurge_PlugFlow)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CSurge_LoadBased.BuildDataFields(DD);
*/
	  /*
	  if (m_pCSurge==&m_CSurge_FullyMixed)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CSurge_EfficiencyCurve.BuildDataFields(DD);
      */

	  DD.ObjectEnd();

}

//---------------------------------------------------------------------------

void SysCADSurge::EvalProducts()
{

  try
  {
      MStream Feed; // SysCAD Feed Stream
      FlwIOs.AddMixtureIn_Id(Feed, idFeed); //sum all input streams

      //
      // Get References to our Output Streams using the FlwIOs helper class and IO ids
      //

      MStream & Product = FlwIOs[FlwIOs.First[idProduct]].Stream;
 
      MIPSD & PSD=*Feed.FindIF<MIPSD>();

 	  if (m_pCSurge)
	  {
	    //m_pCSurge->EvalProducts(Feed,Product,bInit);
		bInit = false;
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

