//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "hsc_helper.h"
#include "gibbsreactor.h"
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

static double Drw_GibbsReactor[] = { MDrw_Poly, -3,4, -3,-4, 3,-4, 3,4,
                                     MDrw_End };


//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(GibbsReactor, "GibbsReactor1", DLL_GroupName)

void GibbsReactor_UnitDef::GetOptions()
  {
  SetDefaultTag("GR");
  SetDrawing("Tank", Drw_GibbsReactor);
  //SetTreeDescription("Process:Unit:CSIRO Gibbs Reactor");
  SetTreeDescription("CSIRO:Gibbs Reactor1");
  SetModelSolveMode(MSolveMode_Probal);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

//---------------------------------------------------------------------------

GibbsReactor::GibbsReactor(TaggedObject * pNd) : 
  MBaseMethod(pNd),
  m_RB(this, "RB")
  {
  //default values...
  m_iReactionMethod = GRM_Gibbs;
  m_iIterations = 0;
  m_dGMin = 0;
  m_dFeedT = StdT;
  m_dProdT = StdT;
  }

//---------------------------------------------------------------------------

void GibbsReactor::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void GibbsReactor::BuildDataFields()
  {
  static MDDValueLst DDB1[]={
    {GRM_None, "None"},
    {GRM_RB, "Reaction Block"},
    {GRM_Gibbs, "Gibbs Reaction"},
    {0}};
  DD.Text("");
  DD.Text("Requirements...");
  DD.Long("Model", "", (long*)&m_iReactionMethod, MF_PARAM_STOPPED|MF_SET_ON_CHANGE, DDB1);
  if (m_iReactionMethod==GRM_RB)
    {
    m_RB.OnOffCheckBox();
    }
  DD.Text("");
  DD.Text("Results...");
  //DD.String("Status", "", &m_sStatus1);
  DD.Text((LPCSTR)m_sStatus1);
  DD.Show(m_iReactionMethod==GRM_Gibbs);
  DD.Long("Iterations", "", &m_iIterations, MF_RESULT);
  DD.Double("GMin", "", &m_dGMin, MF_RESULT, MC_Qe("kJ/s"));
  DD.Show();
  DD.Double("FeedT", "", &m_dFeedT, MF_RESULT|MF_NO_FILING, MC_T("C"));
  DD.Double("ProdT", "", &m_dProdT, MF_RESULT|MF_NO_FILING, MC_T("C"));
  if (m_iReactionMethod==GRM_RB)
    {
    m_RB.BuildDataFields();
    }
  }

//---------------------------------------------------------------------------

/*
  const long SpecieCount = gs_MVDefn.Count();
  for (int i=0; i<SpecieCount; i++)
    {
    MSpecieDefn & SD = gs_MVDefn[i];
    tag = SD.Tag();
    symbol = SD.Symbol();
    MW = SD.MolecularWt();
    TLo = SD.TempLimitLo();
    THi = SD.TempLimitHi();
    }
*/
void GibbsReactor::GibbsReaction(MStream & Stream)
  {
  m_GH.sUnitTag = this->getTag();
  const long SpecieCount = gs_MVDefn.Count();
  for (int i=0; i<GibbsSpecieCount; i++)
    {
    int index = gs_MVDefn.LookupNX(RqdGibbsSpecies[i]);
    if (index>=0)
      {
      //MSpecieDefn & SD = gs_MVDefn[index];
      m_GH.Ml[i] = Stream.getMl(index); //kmol/s
      }
    else
      {
      m_GH.Ml[i] = 1.0e-15;
      }
    }
  m_GH.T = Stream.T; //K

  int RetCode = gs_HSC.ExecuteGibbs(m_GH);
  switch (RetCode)
    {
    case 0:
      {
      m_sStatus1 = "OK";
      for (int i=0; i<GibbsSpecieCount; i++)
        {
        int index = gs_MVDefn.LookupNX(RqdGibbsSpecies[i]);
        if (index>=0)
          {
          //Stream.setMl(index, m_GH.Ml[i]); //kmol/s
          MSpecieDefn & SD = gs_MVDefn[index];
          Stream.M[index] = m_GH.Ml[i]*SD.MolecularWt(); //kmol/s
          }
        }
      m_iIterations = m_GH.Iterations;
      m_dGMin = m_GH.gmin;
      break;
      }
    case -1: m_sStatus1 = "Error: Unable to initialise HSC gibbs5.dll"; break;
    case -2: m_sStatus1.Format("Error: Unable to open input file in folder %s", HSC_file_path); break;
    case -3: m_sStatus1 = "Error: HSC Gibbs Calculate function failed"; break;
    case -4: m_sStatus1.Format("Error: Unable to open output file in folder %s", HSC_file_path); break;
    case -5: m_sStatus1 = "Error: Empty output file"; break;
    }
  Log.SetCondition(RetCode!=0, 1, MMsg_Error, m_sStatus1);
  }

//---------------------------------------------------------------------------

void GibbsReactor::EvalProducts()
  {
  try
    {
    MStream QI; //initialise local empty copy of a stream
    FlwIOs.AddMixtureIn_Id(QI, idFeed); //sum of all feed streams
    const long index = FlwIOs.First[idProd]; //get index of the output stream from list of all connected streams
    MStream & QO = FlwIOs[index].Stream; //get reference to the actual output stream

    QO = QI; //set output = input (copies all qualities, etc)

    m_dFeedT = QI.T;
    m_iIterations = -1;
    m_dGMin = 0.0;
    switch (m_iReactionMethod)
      {
      case GRM_None:
        break;
      case GRM_RB:
        m_RB.EvalProducts(QO); //call the reaction code on the stream provided
        break;
      case GRM_Gibbs:
        {
        GibbsReaction(QO);
        break;
        }
      }

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
