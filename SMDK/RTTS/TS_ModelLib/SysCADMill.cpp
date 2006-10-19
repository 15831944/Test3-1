//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SysCADMill_CPP
#include "SysCADSystemHelper.h"
#include "SysCADMill.h"
#pragma optimize("", off)

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

static double MillDraw[] = { MDrw_Poly,  -5.,-5.,  -1.,-5.,  -4.,5.,  -5.,5., -5.,-5.,
                             MDrw_Poly,   1.,-5.,   5.,-5.,   5.,5.,  4.,5.,  1.,-5.,
                             MDrw_Poly,  -4.,5.,   4.,5.,     1.,-5.,-1.,-5.,  -4.,5.,
                             MDrw_End};

DEFINE_TRANSFER_UNIT(SysCADMill, "MillUnit", DLL_GroupName)
void SysCADMill_UnitDef::GetOptions()
  {
  SetDefaultTag("MILL",TRUE);
  SetDrawing("SizeAlteration", MillDraw);
  //SetTreeDescription("Comminution:RioTinto Milling Unit");
  SetTreeDescription("RTTS:RioTinto Milling Unit");
  SetDescription("RioTinto Mill");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD|MLicense_RTTS);

  };

//====================================================================================

//SysCADMill::SysCADMill(TaggedObject * pNd) : MBaseMethod(pNd)
SysCADMill::SysCADMill(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)

  {
    // Initialisation Flag
    bInit = true;
    m_bOrePropsExist = false;
  }

//---------------------------------------------------------------------------

void SysCADMill::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

void SysCADMill::BuildDataFields()
{

  static MDDValueLst DDMethod[]={
    {CMill::eMethod_Ball,                     "Ball"},
    {CMill::eMethod_Rod,                      "Rod"},
#if WithFortranModel430
    {CMill::eMethod_AGSAG,                    "AG/SAG"},
#endif
    {0}};

    // SysCAD Tag Name convention ==> no spaces between words
    // Use MF_VISIBLE to make parameters invisible

    // Mill Parameters
    DD.Page("Configuration");

    DD.Text("Mill Fields...");

    DD.ObjectBegin("TS_Mill", "Mill" );

    DD.Long("Method", "", (long*)&m_Method, MF_PARAMETER|MF_SET_ON_CHANGE|MF_PARAM_STOPPED, DDMethod);
    DD.Text("");
    DD.Text("");
    DD.CheckBox ("OrePropsExist", "",   &m_bOrePropsExist,  MF_RESULT);

    switch(m_Method)
      {
      case CMill::eMethod_Ball:
		  m_pCMill =  &m_CMill_Ball;
          break;

     case CMill::eMethod_Rod:
		  m_pCMill =  &m_CMill_Rod;
          break;

#if WithFortranModel430
     case CMill::eMethod_AGSAG:
		  m_pCMill =  &m_CMill_AGSAG;
          break;
#endif

      default:
	    m_pCMill = NULL;
        break;

      }

	  // We Build all the data field irrespective of the
	  // selected method so all data is persistent.
	  // We just specify the visibility based on the selected method.
	  if (m_pCMill==&m_CMill_Ball)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CMill_Ball.BuildDataFields(DD);

	  if (m_pCMill==&m_CMill_Rod)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CMill_Rod.BuildDataFields(DD);

	  if (m_pCMill==&m_CMill_AGSAG)
		DD.Show(true,true,true);
	  else
	    DD.Show(false,true,true);
	  m_CMill_AGSAG.BuildDataFields(DD);

	  DD.ObjectEnd();
}

//---------------------------------------------------------------------------

void SysCADMill::EvalProducts()
{
  m_bOrePropsExist = (GetGlobalOther("RioTintoOreProps")!=NULL);
  try
  {
      MStream Feed; // SysCAD Feed Stream
      FlwIOs.AddMixtureIn_Id(Feed, idFeed); //sum all input streams

      //
      // Get References to our Output Streams using the FlwIOs helper class and IO ids
      //

      MStream & P1 = FlwIOs[FlwIOs.First[idDischarge]].Stream; // SysCAD Output Stream
 
      P1=Feed;
  
      if (Feed.MassFlow()>1.0e-5)
      {

      MIPSD & PSD=*Feed.FindIF<MIPSD>();
 
      long    l_SizeCount      = PSD.getSizeCount();
      long    l_PSDVectorCount = PSD.getPSDVectorCount();

      if (!IsNothing(PSD)) // Only do something if our input stream has a SizeData
      {  
 	      if (m_pCMill != NULL )
		  {
			    m_pCMill->EvalProducts((*this),Feed,P1,bInit);
				bInit = false;
		  }
		  else
		  {
			Log.Message(MMsg_Warning, "m_pCMill==NULL?  (Model430 not available!?!");

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
