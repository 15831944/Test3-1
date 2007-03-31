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

CTagIO::CTagIO(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
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
  m_iGet1 = -1;
  m_iGet2 = -1;
  m_iSet1 = -1;

  TagIO.Open(100);
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

const int idDX_GetTagStr  = 1;
const int idDX_GetTagButton = 2;
const int idDX_GetTagInfoButton = 3;
const int idDX_PutTagStr = 4;
const int idDX_SetTagButton = 5;
const int idDX_TagSubscriptionOn = 10;
const int idDX_GetTagSubsStr1 = 11;
const int idDX_GetTagSubsStr2 = 12;
const int idDX_SetTagSubsStr1 = 13;

const int maxElements = 80;

//---------------------------------------------------------------------------

void CTagIO::BuildDataFields()
  {
  DD.CheckBox("On",       "", &m_bOn,         MF_PARAMETER);
  DD.Text(" ");
  DD.Text("----------------------------------------");
  DD.Text("Example of GetTag and SetTag...");
  DD.String("GetTag", "", idDX_GetTagStr, MF_PARAMETER);
  DD.Button("GetTagInfo", "", idDX_GetTagInfoButton);
  DD.Button("GetValueNow", "", idDX_GetTagButton);
  DD.Double("GetValue", "", &m_dGetValue, MF_RESULT|MF_NanOK, m_GetCnv);
  DD.Text((LPCSTR)m_sGetStatus);
  DD.Text(" ");
  DD.String("PutTag", "", idDX_PutTagStr, MF_PARAMETER);
  DD.Double("ValueToSet", "", &m_dSetValue, MF_PARAMETER|MF_NanOK, m_SetCnv);
  DD.Button("SetValueNow", "", idDX_SetTagButton);
  DD.Text((LPCSTR)m_sSetStatus);
  

  DD.Text(" ");
  DD.Text(" ");
  DD.Text("----------------------------------------");
  DD.Text("Example of tag subscription...");
  DD.CheckBox("TagSubscriptionOn", "", idDX_TagSubscriptionOn, MF_PARAMETER);
  DD.String("GetTagSubs1", "", idDX_GetTagSubsStr1, MF_PARAMETER);
  DD.String("GetTagSubs2", "", idDX_GetTagSubsStr2, MF_PARAMETER);
  DD.Show(m_iGet1>=0);
  MCnv Cnv;
  if (m_bSubsActive && m_iGet1>=0)
    Cnv.m_Index = TagIO.CnvIndex[m_iGet1]; //unfortunatly this is not valid until a solve
  DD.Double("GetValue1", "", &m_dGetValueSubs1, MF_RESULT|MF_NanOK, Cnv);
  DD.Show(m_iGet2>=0);
  if (m_bSubsActive && m_iGet2>=0)
    Cnv.m_Index = TagIO.CnvIndex[m_iGet2];
  DD.Double("GetValue2", "", &m_dGetValueSubs2, MF_RESULT|MF_NanOK, Cnv);
  DD.String("SetTagSubs1", "", idDX_SetTagSubsStr1, MF_PARAMETER);
  DD.Show(m_iSet1>=0);
  if (m_bSubsActive && m_iSet1>=0)
    Cnv.m_Index = TagIO.CnvIndex[m_iSet1];
  DD.Double("SetValue1", "", &m_dSetValueSubs1, MF_PARAMETER|MF_NanOK, Cnv);
  }

//---------------------------------------------------------------------------

bool CTagIO::AddSubsTag(CString & Tag, LPCSTR NewTag, bool TestParam/* = false*/)
  {
  CString OldTag(Tag);
  Tag = NewTag;
  TagIO.FormatAsTagOnly(Tag);
  if (Tag!=OldTag)
    {//changed...
    if (Tag.GetLength()>0)
      {
        MTagIOInfo TagInfo;
        const int RetCode = TagIO.GetTagInfo(Tag, TagInfo);
        switch (RetCode)
        {
        case MTagIO_OK:
          break;
        case MTagIO_NotFound:
          Log.Message(MMsg_Warning, "Tag '%s' not found.", Tag);
          break;
        default:
          Log.Message(MMsg_Warning, "Tag '%s' not allowed.", Tag);
          break;
          //TagInfo.CnvIndex;
        }
      }
    m_bBuildListRqd = true;
    //MyTagsHaveChanged();
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

bool CTagIO::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_GetTagStr:
      if (DX.HasReqdValue)
        {
        m_sGetTag = DX.String;
        TagIO.FormatAsTagAndCnv(m_sGetTag);
        MTagIOInfo TagInfo;
        const int RetCode = TagIO.GetTagInfo(m_sGetTag, TagInfo);
        if (RetCode==MTagIO_OK)
          m_GetCnv.m_Index = TagInfo.CnvIndex;
        }
      DX.String = m_sGetTag;
      return true;
    case idDX_GetTagInfoButton:
		  if (DX.HasReqdValue && DX.Bool)
        {
        MTagIOInfo TagInfo;
        const int RetCode = TagIO.GetTagInfo(m_sGetTag, TagInfo);
        switch (RetCode)
          {
          case MTagIO_OK: m_sGetStatus.Format("Tag OK: DataType:%d(%s)  CnvIndex:%d(%s)  Flags:%d", TagInfo.DataType, TagObjTypeNames[TagInfo.DataType].Str, TagInfo.CnvIndex, "todo"/*gs_Cnvs[TagInfo.CnvIndex].Name*/, TagInfo.Flags); break;
          case MTagIO_NotSpecified: m_sGetStatus = "Tag not specified."; break;
          case MTagIO_NotFound: m_sGetStatus = "Tag not found."; break;
          case MTagIO_NotAllowed: m_sGetStatus = "Tag not allowed."; break;
          }
        Log.Message(MMsg_Error, m_sGetStatus);
        }
  		DX.Bool = false;
      return true;
    case idDX_PutTagStr:
      if (DX.HasReqdValue)
        {
        m_sSetTag = DX.String;
        TagIO.FormatAsTagAndCnv(m_sSetTag);
        MTagIOInfo TagInfo;
        const int RetCode = TagIO.GetTagInfo(m_sSetTag, TagInfo);
        if (RetCode==MTagIO_OK)
          m_SetCnv.m_Index = TagInfo.CnvIndex;
        }
      DX.String = m_sSetTag;
      return true;
    case idDX_GetTagButton:
		  if (DX.HasReqdValue && DX.Bool)
        {
        int RetCode = TagIO.GetTag(m_sGetTag, m_dGetValue);
        if (RetCode==MTagIO_OK)
          {
          m_sGetStatus.Format("Retrieved %s", m_sGetTag);
          }
        else
          {
          m_sGetStatus.Format("Error %d", RetCode);
          }
        }
  		DX.Bool = false;
      return true;
    case idDX_SetTagButton:
		  if (DX.HasReqdValue && DX.Bool)
        {
        int RetCode = TagIO.SetTag(m_sSetTag, m_dSetValue);
        if (RetCode==MTagIO_OK)
          {
          m_sSetStatus.Format("Set %s", m_sSetTag);
          }
        else
          {
          m_sSetStatus.Format("Error %d", RetCode);
          }
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
          {
          TagIO.Close();
          m_iGet1 = -1;
          m_iGet2 = -1;
          m_iSet1 = -1;
          }
        }
      DX.Bool=m_bSubsActive;
      return true;
    case idDX_GetTagSubsStr1:
      if (DX.HasReqdValue)
        {
        AddSubsTag(m_sGetTagSubs1, DX.String);
        }
      DX.String = m_sGetTagSubs1;
      return true;
    case idDX_GetTagSubsStr2:
      if (DX.HasReqdValue)
        {
        AddSubsTag(m_sGetTagSubs2, DX.String);
        }
      DX.String = m_sGetTagSubs2;
      return true;
    case idDX_SetTagSubsStr1:
      if (DX.HasReqdValue)
        {
        AddSubsTag(m_sSetTagSubs1, DX.String);
        }
      DX.String = m_sSetTagSubs1;
      return true;
    }

  return false;
  }

//---------------------------------------------------------------------------

bool CTagIO::ValidateDataFields()
  {
  if (m_bSubsActive && m_bBuildListRqd)
    {
    //TagIO.Close();
    TagIO.RemoveAll();
    m_iGet1 = TagIO.Add(m_sGetTagSubs1, "GetValue1", MTIO_Get);
    m_iGet2 = TagIO.Add(m_sGetTagSubs2, "GetValue2", MTIO_Get);
    m_iSet1 = TagIO.Add(m_sSetTagSubs1, "SetValue1", MTIO_Set);
    if (m_iGet1<0 && m_iGet1!=-3)
      Log.Message(MMsg_Error, "Error with GetTagSubs1 (%s)", m_sGetTagSubs1);
    if (m_iGet2<0 && m_iGet2!=-3)
      Log.Message(MMsg_Error, "Error with GetTagSubs1 (%s)", m_sGetTagSubs2);
    if (m_iSet1<0 && m_iSet1!=-3)
      Log.Message(MMsg_Error, "Error with SetTagSubs1 (%s)", m_sSetTagSubs1);
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
        if (m_iGet1>=0)
          m_dGetValueSubs1 = TagIO.DValue[m_iGet1]; //always as SI units
        if (m_iGet2>=0)
          m_dGetValueSubs2 = TagIO.DValue[m_iGet2]; //always as SI units
        if (m_iSet1>=0)
          TagIO.DValue[m_iSet1] = m_dSetValueSubs1; //always as SI units
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
