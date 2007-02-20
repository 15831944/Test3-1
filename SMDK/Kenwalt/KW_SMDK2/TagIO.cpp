//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __TAGIO_CPP
#include "tagio.h"
#include <math.h>
#pragma optimize("", off)

//====================================================================================

//TODO: A Proper icon
static double Drw_TagIO[] = 
  { 
  MDrw_Poly,  -8.,8.,  8.,8.,  8.,-8., -8.,-8., -8.,8., 
  MDrw_Poly,  -5.,8.,  -5.,-8., 
  MDrw_Poly,  5.,8.,  5.,-8., 
  MDrw_End 
  };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(CTagIO, "TagIO", DLL_GroupName)
void CTagIO_UnitDef::GetOptions()
  {
  SetDefaultTag("TIO");
  SetDrawing("TagIO", Drw_TagIO);
  SetTreeDescription("Demo:TagIO");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

CTagIO::CTagIO(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  srand((unsigned)time(NULL));

  //default values...
  m_bOn       = true;
  m_OnFly     = true;

  m_GetTemp   = StdT;
  m_PutTemp   = StdT;
  m_DeltaTemp = 0.0;

  }

//---------------------------------------------------------------------------

void CTagIO::Init()
  {
  }

//---------------------------------------------------------------------------

bool CTagIO::PreStartCheck()
  {
  return true;
  }

//---------------------------------------------------------------------------

const int idDX_OnFly  = 1;
const int idDX_GetTag = 2;
const int idDX_PutTag = 3;

const int maxElements = 80;

//---------------------------------------------------------------------------

void CTagIO::BuildDataFields()
  {

  DD.CheckBox("On",       "", &m_bOn,         MF_PARAMETER);
  DD.CheckBox("OnFly",    "", idDX_OnFly,     MF_PARAMETER);

  DD.Text(" ");
  DD.String("GetTempTag", "", idDX_GetTag,    MF_PARAMETER);
  DD.String("PutTempTag", "", idDX_PutTag,    MF_PARAMETER);

  DD.Text(" ");
  DD.Double("GetTemp",    "", &m_GetTemp,     0,            MC_T("C"));
  DD.Double("DeltaTemp",  "", &m_DeltaTemp,   MF_PARAMETER, MC_dT("C"));
  DD.Double("PutTemp",    "", &m_PutTemp,     0,            MC_T("C"));
  }

//---------------------------------------------------------------------------

bool CTagIO::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_OnFly:
      if (DX.HasReqdValue)
        {
        m_OnFly = DX.Bool;
        if (m_OnFly)
          {
          TagIO.Close();
          m_iGet = -1;
          m_iPut = -1;
          }
        else
          {
          TagIO.Open(100);
          m_iGet = TagIO.Add(m_GetTempTag, "Tag2Get", TIO_Get);
          m_iPut = TagIO.Add(m_PutTempTag, "Tag2Put", TIO_Set);
          if (m_iGet<0 || m_iPut<0)
            {
            /*Error*/;
            }
          }
        }
      DX.Bool=m_OnFly;
      return true;
    case idDX_GetTag:
      if (DX.HasReqdValue)
        m_GetTempTag = DX.String;
      DX.String = m_GetTempTag;
      return true;
    case idDX_PutTag:
      if (DX.HasReqdValue)
        m_PutTempTag = DX.String;
      DX.String = m_PutTempTag;
      return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

bool CTagIO::ValidateDataFields()
  {
  return true;
  }

//---------------------------------------------------------------------------

void    CTagIO::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  { 
  }; 
void    CTagIO::EvalCtrlActions(eScdCtrlTasks Tasks)    
  { 
  ; 
  };
void    CTagIO::EvalCtrlStrategy(eScdCtrlTasks Tasks)   
  { 
  if (m_bOn) 
    {
    try
      {
      if (m_OnFly)
        {
        if (TagIO.Peek(m_GetTempTag, m_GetTemp)!=TagIO_OK)
          { /* Error*/ }

        m_PutTemp=m_GetTemp+m_DeltaTemp;

        if (TagIO.Poke(m_PutTempTag, m_PutTemp)!=TagIO_OK)
          { /* Error*/ }
        }
      else
        {
        if (1) // The efficient option
          {
          m_GetTemp = TagIO.DValue[m_iGet];
          m_PutTemp = m_GetTemp + m_DeltaTemp;
          TagIO.DValue[m_iPut] = m_PutTemp;
          }
        else
          {
          m_GetTemp = TagIO.DValue[m_GetTempTag];
          m_PutTemp = m_GetTemp + m_DeltaTemp;
          TagIO.DValue[m_PutTempTag] = m_PutTemp;
          }
        }
      }
    catch (MMdlException &ex)
      {
      Log.Message(MMsg_Error, ex.Description);
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
  };
void    CTagIO::EvalCtrlTerminate(eScdCtrlTasks Tasks)
  {
  ; 
  }; 

//---------------------------------------------------------------------------
