//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SysCADComminution_CPP
#include "SysCADSystemHelper.h"
#include "SysCADComminution.h"
//#pragma optimize("", off)

//====================================================================================

const int idFeed      = 0;
const int idDischarge = 1;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,      1,  10,    0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "Discharge",      "Discharge", idDischarge, 1,   1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { NULL },
  };

//====================================================================================

static double ComminutionDraw[] = { MDrw_Poly,  -5.,-5.,  -1.,-5.,  -4.,5.,  -5.,5., -5.,-5.,
                                    MDrw_Poly,   1.,-5.,   5.,-5.,   5.,5.,  4.,5.,  1.,-5.,
                                    MDrw_Poly,  -4.,5.,   4.,5.,     1.,-5.,-1.,-5.,  -4.,5.,
                                    MDrw_TagPos, 0,-8.5,
                                    MDrw_End};

DEFINE_TRANSFER_UNIT(SysCADComminution, "ComminutionUnit", DLL_GroupName)
void SysCADComminution_UnitDef::GetOptions()
  {
  SetDefaultTag("CR",TRUE);
  SetDrawing("SizeAlteration", ComminutionDraw);
  //SetTreeDescription("Comminution:RioTinto Comminution Unit");
  SetTreeDescription("RTTS:RioTinto Comminution Unit");
  SetDescription("RioTinto Comminution");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD|MLicense_RTTS);
  };

//====================================================================================

//SysCADComminution::SysCADComminution(TaggedObject * pNd) : MBaseMethod(pNd)
SysCADComminution::SysCADComminution(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
    // Construct the Persistent System Feed Data
    //FeedStream  = TS_MinProc::PFlowStream1( new TS_MinProc::FlowStream1 );
    //SizInfo = TS_MinProc::PSizingInfo1( new TS_MinProc::SizingInfo1 );
    //MatInfo =  TS_MinProc::PStreamInfo1( new TS_MinProc::StreamInfo1 );

    // Initialisation Flag
    bInit = true;
    m_bOrePropsExist = false;

  }

//---------------------------------------------------------------------------

void SysCADComminution::Init()
{
  SetIODefinition(s_IODefs);

}

//---------------------------------------------------------------------------

void SysCADComminution::BuildDataFields()
{

  static MDDValueLst DDMethod[]={
	{CComminution::eMethod_Individual_Fixed,         "FixedDischarge"},
    {CComminution::eMethod_Whiten,                   "WhitenCrusher"},
    {CComminution::eMethod_HPRC,                     "HPRC"},
    {CComminution::eMethod_Model400,                 "Model400"},
    {0}};

    // SysCAD Tag Name convention ==> no spaces between words
    // Use MF_VISIBLE to make parameters invisible

    // Comminution Parameters
    DD.Page("Configuration");

    DD.Text("Comminution Fields...");

    DD.ObjectBegin("TS_Comminution", "Comminution" );

    DD.Long("Method", "", (long*)&m_Method, MF_PARAMETER|MF_SET_ON_CHANGE|MF_PARAM_STOPPED, DDMethod);
    DD.Text("");
    DD.Text("");
    DD.CheckBox ("OrePropsExist", "",   &m_bOrePropsExist,  MF_RESULT);

    switch(m_Method)
      {
      case CComminution::eMethod_Individual_Fixed:
		  m_pCComminution =  &m_CComminution_Individual_Fixed;
          break;

      case CComminution::eMethod_Whiten:
		  m_pCComminution =  &m_CComminution_Whiten;
          break;

     case CComminution::eMethod_HPRC:
		  m_pCComminution =  &m_CComminution_HPRC;
          break;

     case CComminution::eMethod_Model400:
		  m_pCComminution =  &m_CComminution_Model400;
          break;

      default:
	    m_pCComminution = NULL;
        break;

      }
	  //if (m_pCComminution)
	  //  m_pCComminution->BuildDataFields(DD);

	  // We Build all the data field irrespective of the
	  // selected method so all data is persistent.
	  // We just specify the visibility based on the selected method.
	  if (m_pCComminution==&m_CComminution_Individual_Fixed)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CComminution_Individual_Fixed.BuildDataFields(DD);

	  if (m_pCComminution==&m_CComminution_Whiten)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CComminution_Whiten.BuildDataFields(DD);

	  if (m_pCComminution==&m_CComminution_HPRC)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CComminution_HPRC.BuildDataFields(DD);

	  if (m_pCComminution==&m_CComminution_Model400)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CComminution_Model400.BuildDataFields(DD);
	  DD.ObjectEnd();

}

//---------------------------------------------------------------------------

void SysCADComminution::EvalProducts()
{
  m_bOrePropsExist = (GetGlobalOther("RioTintoOreProps")!=NULL);
  try
  {
      MStreamI Feed; // SysCAD Feed Stream
      FlwIOs.AddMixtureIn_Id(Feed, idFeed); //sum all input streams

      //
      // Get References to our Output Streams using the FlwIOs helper class and IO ids
      //

      MStream & P1 = FlwIOs[FlwIOs.First[idDischarge]].Stream; // SysCAD Output Stream
 
      P1=Feed;
  
      if (Feed.MassFlow()>1.0e-5)
      {

      MIPSD & PSD=*Feed.FindIF<MIPSD>();
 
      if (!IsNothing(PSD)) // Only do something if our input stream has a SizeData
      {  
 	      if (m_pCComminution)
		  {
			    m_pCComminution->EvalProducts((*this),Feed,P1,bInit);
				bInit = false;
		  }
      }
      else
      {
      if (Feed.MassFlow()>1.0e-5)
        Log.Message(MMsg_Warning, "No PSD Properties present, report directly to discharge");
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
    SetIdleRequired("Help");
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
