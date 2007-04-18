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
  MDrw_Poly,  -5.,5.,  5.,5.,  5.,-5., -5.,-5., -5.,5., 
  MDrw_Poly,  -3.5,5.0,  -3.5,-5.0, 
  MDrw_Poly,  3.5,5.0,  3.5,-5.0, 
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

CTagIO::CTagIO(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd),
  m_GetItem(TagIO),
  m_SetItem(TagIO)
  {
  //default values...
  m_bOn       = true;

  m_dGetValue = 0.0;//dNAN;
  m_dSetValue = 0.0;


  m_bSubsActive = false;
  m_bBuildListRqd = true;
  m_dGetValueSubs1 = 0.0;//dNAN;
  m_dGetValueSubs2 = 0.0;//dNAN;
  m_dSetValueSubs1 = 0.0;
  //m_iGet1 = -1;
  //m_iGet2 = -1;
  //m_iSet1 = -1;

  TagIO.Open(3);
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

enum 
  {
  idDX_GetTagStr = 1,
  idDX_GetTagButton,
  //idDX_GetTagValue,
  idDX_GetTagInfoButton,
  idDX_PutTagStr,
  idDX_SetTagButton,
  //idDX_SetTagValue, 
  idDX_TagSubscriptionOn,
  idDX_GetTagSubsStr1,
  idDX_GetTagSubsStr2,
  idDX_SetTagSubsStr1
  };

const int maxElements = 80;

//---------------------------------------------------------------------------

void CTagIO::BuildDataFields()
  {
  DD.CheckBox("On",       "", &m_bOn,         MF_PARAMETER);
  DD.Text(" ");
  DD.Text("----------------------------------------");
  DD.Text("Example of GetTag and SetTag...");
#if 01
  DD.String("GetTag", "", idDX_GetTagStr, MF_PARAMETER);
  DD.Button("GetTagInfo",  "", idDX_GetTagInfoButton);
  DD.Button("GetValueNow", "", idDX_GetTagButton);
  DD.Double("GetValue",    "", &m_dGetValue, MF_RESULT|MF_NanOK);
  DD.Text(m_sGetStatus);
  DD.Text(" ");
  DD.String("PutTag", "", idDX_PutTagStr, MF_PARAMETER);
  DD.Double("ValueToSet", "", &m_dSetValue, MF_PARAMETER|MF_NanOK);
  DD.Button("SetValueNow", "", idDX_SetTagButton);
  DD.Text(m_sSetStatus);
#endif

  DD.Text(" ");
  DD.Text(" ");
  DD.Text("----------------------------------------");
  DD.Text("Example of tag subscription...");
  DD.CheckBox("TagSubscriptionOn", "", idDX_TagSubscriptionOn, MF_PARAMETER);
  DD.String("GetTagSubs1", "", idDX_GetTagSubsStr1, MF_PARAMETER);
  DD.String("GetTagSubs2", "", idDX_GetTagSubsStr2, MF_PARAMETER);

  DD.Show(TagIO[0]->IsActive);
  DD.Double("GetValue1", "", &m_dGetValueSubs1, MF_RESULT|MF_NanOK, TagIO[0]->Cnv);

  DD.Show(TagIO[1]->IsActive);
  DD.Double("GetValue2", "", &m_dGetValueSubs2, MF_RESULT|MF_NanOK, TagIO[1]->Cnv);

  DD.Show();
  DD.String("SetTagSubs1", "", idDX_SetTagSubsStr1, MF_PARAMETER);
  DD.Show(TagIO[2]->IsActive);//m_iSet1>=0);
  DD.Double("SetValue1", "", &m_dSetValueSubs1, MF_PARAMETER|MF_NanOK, TagIO[2]->Cnv);
  }

//---------------------------------------------------------------------------

bool CTagIO::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_GetTagStr:
      if (DX.HasReqdValue)
        m_GetItem.Tag = TagIO.FormatAsTagAndCnv(DX.String);
      DX.String = m_GetItem.Tag;
      return true;
    case idDX_GetTagInfoButton:
		  if (DX.HasReqdValue && DX.Bool)
        {
        MTagIOResult RetCode = m_GetItem.CheckTag();
        if (RetCode==MTagIO_OK)
          {
          m_sGetStatus.Format("Tag OK: DataType:%d(%s)  CnvIndex:%d(%s)  Flags:%x", 
                            m_GetItem.DataType,
                            TagObjTypeNames[m_GetItem.DataType].Str, 
                            m_GetItem.Cnv.m_Index, 
                            gs_Cnvs.Name(m_GetItem.Cnv), 
                            m_GetItem.IOFlags); 
          }
        else
          m_sGetStatus = TagIO.ResultString(RetCode);
        Log.Message(MMsg_Error, m_sGetStatus);
        }
  		DX.Bool = false;
      return true;
    case idDX_PutTagStr:
      if (DX.HasReqdValue)
        m_SetItem.Tag = TagIO.FormatAsTagAndCnv(DX.String);
      DX.String = m_SetItem.Tag;
      return true;
    case idDX_GetTagButton:
		  if (DX.HasReqdValue && DX.Bool)
        {
        MTagIOResult RetCode = m_GetItem.ReadValue();
        if (RetCode==MTagIO_OK)
          {
          m_dGetValue = m_GetItem.DoubleCnv;
          m_sGetStatus.Format("Retrieved %s SI:%.2f Cnv:%.2f", m_GetItem.Tag, m_GetItem.DoubleSI, m_GetItem.DoubleCnv);
          }
        else
          m_sGetStatus.Format("Error %s", TagIO.ResultString(RetCode));
        }
  		DX.Bool = false;
      return true;
    case idDX_SetTagButton:
		  if (DX.HasReqdValue && DX.Bool)
        {
        MTagIOResult RetCode = m_SetItem.CheckTag();
        if (RetCode==MTagIO_OK)
          {
          m_SetItem.DoubleCnv = m_dSetValue;
          RetCode = m_SetItem.WriteValue();
          m_sSetStatus.Format("Written %s", TagIO.ResultString(RetCode));
          }
        else
          m_sSetStatus.Format("Error %s", TagIO.ResultString(RetCode));
        }
  		DX.Bool = false;
      return true;

    case idDX_TagSubscriptionOn:
      if (DX.HasReqdValue)
        {
        bool Prev = m_bSubsActive;
        m_bSubsActive = DX.Bool;
        if (m_bSubsActive)
          {
          if (Prev!=m_bSubsActive)
            m_bBuildListRqd = true;
          }
        else
          TagIO.Close();
        }
      DX.Bool=m_bSubsActive;
      return true;
    case idDX_GetTagSubsStr1:
      if (DX.HasReqdValue)
        TagIO[0]->Tag=DX.String;
      DX.String = TagIO[0]->Tag;
      return true;
    case idDX_GetTagSubsStr2:
      if (DX.HasReqdValue)
        TagIO[1]->Tag=DX.String;
      DX.String = TagIO[1]->Tag;
      return true;
    case idDX_SetTagSubsStr1:
      if (DX.HasReqdValue)
        TagIO[2]->Tag=DX.String;
      DX.String = TagIO[2]->Tag;
      return true;
    }

  return false;
  }

//---------------------------------------------------------------------------

bool CTagIO::ValidateDataFields()
  {
  if (TagIO.ValidateReqd() && m_bSubsActive)
    {

    if (TagIO.StartValidateDataFields())
      {
      if (TagIO.Set(0, NULL, "GetValue1", MTagIO_Get)>0)
        m_dGetValueSubs1 = TagIO[0]->DoubleSI;
      if (TagIO.Set(1, NULL, "GetValue2", MTagIO_Get)>0)
        m_dGetValueSubs2 = TagIO[1]->DoubleSI;
      if (TagIO.Set(2, NULL, "SetValue1", MTagIO_Set|MTagIO_Parm)>0)
        {
        m_dSetValueSubs1 = TagIO[2]->DoubleSI;
        }
      }
    TagIO.EndValidateDataFields();
    
    m_bBuildListRqd = false;
    }
  return true;
  }

//---------------------------------------------------------------------------

void CTagIO::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  { 
  }

//---------------------------------------------------------------------------

void CTagIO::EvalCtrlActions(eScdCtrlTasks Tasks)    
  { 
  }

//---------------------------------------------------------------------------

void CTagIO::EvalCtrlStrategy(eScdCtrlTasks Tasks)   
  { 
  if (m_bOn) 
    {
    try
      {
      if (m_bSubsActive)
        {
        //get/set the values
        if (TagIO[0]->IsActive)
          m_dGetValueSubs1 = TagIO[0]->DoubleSI; //always as SI units
        if (TagIO[1]->IsActive)
          m_dGetValueSubs2 = TagIO[1]->DoubleSI; //always as SI units
        if (TagIO[2]->IsActive)
          TagIO[2]->DoubleSI = m_dSetValueSubs1; //always as SI units
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
  }

//---------------------------------------------------------------------------

void CTagIO::EvalCtrlTerminate(eScdCtrlTasks Tasks)
  {
  }

//---------------------------------------------------------------------------
