//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "examplereactor.h"
#pragma optimize("", off)

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

static double Drw_SimpleReactor[] = { MDrw_Poly, -3,4, -3,-4, 3,-4, 3,4, -3,4,
                             MDrw_End };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(SimpleReactor, "Reactor", DLL_GroupName)

void SimpleReactor_UnitDef::GetOptions()
  {
  SetDefaultTag("SR", true);
  SetDrawing("Tank", Drw_SimpleReactor);
  SetTreeDescription("Demo:Simple Reactor");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

//---------------------------------------------------------------------------

SimpleReactor::SimpleReactor(MUnitDefBase * pUnitDef, TaggedObject * pNd) :
  MBaseMethod(pUnitDef, pNd),
  m_RB(this, false, "RB")
  {
  //default values...
  m_dFeedT = StdT;
  m_dProdT = StdT;
  m_RB.Disable();
  }

//---------------------------------------------------------------------------

void SimpleReactor::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

const int idDX_Description = 1;
const int idDX_ProdT = 2;
const int idDX_TempDiff = 3;
const int idDX_DoSomething = 4;

void SimpleReactor::BuildDataFields()
  {
  DD.Text("");
  DD.Text("Requirements...");
  m_RB.OnOffCheckBox();
  DD.String("Desc", "Description", idDX_Description, MF_PARAMETER);
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedT", "", &m_dFeedT, MF_RESULT, MC_T("C"));
  DD.Double("ProdT", "", idDX_ProdT, MF_RESULT, MC_T("C"));
  DD.Double("TempDiff", "", idDX_TempDiff, MF_RESULT|MF_NO_FILING, MC_dT("C"));
  if (m_RB.Enabled())
    {
    DD.Page("RB", false); //when reaction is on, always force this on to a new "RB" tab page
    }

  DD.Text("");
  DD.Button("DoSomething", "", idDX_DoSomething); 

  m_RB.BuildDataFields();
  }

//---------------------------------------------------------------------------

bool SimpleReactor::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_Description:
      if (DX.HasReqdValue)
        m_sDesc=DX.String;
      DX.String=(LPCSTR)m_sDesc;
      return true;
    case idDX_ProdT:
      if (DX.HasReqdValue)
        m_dProdT=DX.Double;//although this is a result, need to set this to recover state at project load time
      DX.Double=m_dProdT;
      return true;
    case idDX_TempDiff:
      //do not need to set anything this is simply a calculated result
      DX.Double=m_dProdT-m_dFeedT;
      return true;
    case idDX_DoSomething:
      if (DX.HasReqdValue && DX.Bool)
        {
        // Do something here, because the button has been pressed
        }
      DX.Bool = false;
      return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void SimpleReactor::EvalProducts()
  {
  try
    {
    MStreamI QI; //initialise local empty copy of a stream
    FlwIOs.AddMixtureIn_Id(QI, idFeed); //sum of all feed streams
    const long index = FlwIOs.First[idProd];
    MStream & QO = FlwIOs[index].Stream; //get reference to the actual output stream

    QO = QI; //set output = input (copies all qualities, etc)

    //const double h0 = QO.totHf(); //get enthalpy
    m_RB.EvalProducts(QO); //call the reaction code on the stream provided
    //double dHf = m_RB.HfSumRct();

    //get display values...
    m_dFeedT = QI.T;
    m_dProdT = QO.T;
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
