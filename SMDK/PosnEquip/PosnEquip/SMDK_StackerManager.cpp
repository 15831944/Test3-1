//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "StackerManager.h"
#include "SMDK_StackerManager.h"
#include "smdk_stacker.h"
#pragma optimize("", off)

//====================================================================================

//const long idFeed = 0;
//const long idProd = 1;

/*static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      idFeed,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",        "Product",   idProd,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };*/

static double Drw_Stacker[] = { 
                            MDrw_Poly, 0,0, -8,0, -8,3, 0,3, 0,0,
                            //MDrw_Poly, 0,0, 20,0, 20,-3, 0,-3, 0,0,
                            //MDrw_Poly, 0,3, 0,6,
                            //MDrw_Poly, -5.5,3, 0,6, 14,0,
                            MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(StackerManager, "Stacker Manager", DLL_GroupName)
void StackerManager_UnitDef::GetOptions()
  {
  SetDefaultTag("MGR", true);
  SetDrawing("Control", Drw_Stacker);
  SetTreeDescription("Position Equipment:Stacker Manager");
  SetDescription("Over sees a set of Stackers");
  SetModelSolveMode(MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

StackerManager::~StackerManager()
{

}
//---------------------------------------------------------------------------

StackerManager::StackerManager(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  }

//---------------------------------------------------------------------------

const int idDX_Description = 1;
const int idDX_ProdT = 2;
const int idDX_TempDiff = 3;
const int idDX_StartStopViewer = 4;


void StackerManager::BuildDataFields()
  {
  DD.Text("");
  DD.CheckBox("On", "",  &bOnLine, MF_PARAM_STOPPED|MF_SET_ON_CHANGE);
  DD.Text("Parameters");
  DD.Text("");
  DD.Text("Viewer...");
  DD.String("FilePath", "Viewer File", idDX_Description, MF_PARAMETER);
  DD.Text("");
  CString szText;
  if(CStacker::StackerManager.m_bViewerStarted)
	  szText = "Stop Viewer  ";
  else
		szText = "Start Viewer  ";

  DD.Button(szText, "", idDX_StartStopViewer); 
 
  }

//---------------------------------------------------------------------------

bool StackerManager::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_Description:
      if (DX.HasReqdValue)
        m_szText=DX.String;
	  CStacker::StackerManager.m_szFileName = m_szText;
      DX.String=(LPCSTR)m_szText;
      return true;
    //case idDX_ProdT:
    //  if (DX.HasReqdValue)
    //    m_dProdT=DX.Double;//although this is a result, need to set this to recover state at project load time
    //  DX.Double=m_dProdT;
    //  return true;
    //case idDX_TempDiff:
    //  //do not need to set anything this is simply a calculated result
    //  DX.Double=m_dProdT-m_dFeedT;
    //  return true;
    case idDX_StartStopViewer:
      if (DX.HasReqdValue && DX.Bool && !m_bLoading)
        {
			if(CStacker::StackerManager.m_bViewerStarted)
				CStacker::StackerManager.StopViewerThread();
			else
			{
			// Create Viewer if not already started
			CStacker::StackerManager.StartViewerThread();

			// This forces an update
			CStacker::StackerManager.UpdateViewerThread();
			}
        }
      DX.Bool = false;
	  m_bLoading = false;
      return true;

   }
  return false;
  }

//---------------------------------------------------------------------------


void StackerManager::EvalProducts()
  {
  try
    {
    if (bOnLine)
	{
		// Update the view
		if(CStacker::StackerManager.m_bViewerStarted)
			CStacker::StackerManager.UpdateViewerThread();

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

//====================================================================================


bool StackerManager::ValidateDataFields()
{
	return true;
};  

void StackerManager::StartSolution()
{
};  

bool StackerManager::PreStartCheck()
{
	return true;
}

void StackerManager::Init()
{
	m_bLoading = true;

};  

