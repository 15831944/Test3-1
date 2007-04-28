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

DEFINE_CONTROL_UNIT(CTagGetAndSetExample, "TagIO", DLL_GroupName)

void CTagGetAndSetExample_UnitDef::GetOptions()
  {
  SetDefaultTag("TIO");
  SetDrawing("TagIO", Drw_TagIO);
  SetTreeDescription("Demo:TagIO");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

CTagGetAndSetExample::CTagGetAndSetExample(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd),
  m_GetItem(TagIO),
  m_SetItem(TagIO),
  m_GetSubs1(TagIO),
  m_GetSubs2(TagIO),
  m_SetSubs1(TagIO)
  {
  //default values...
  m_bOn       = true;

  m_dGetValue = 0.0;
  m_dSetValue = 0.0;

  m_dGetValueSubs1 = 0.0;
  m_dGetValueSubs2 = 0.0;
  m_dSetValueSubs1 = 0.0;
  }

//---------------------------------------------------------------------------

void CTagGetAndSetExample::Init()
  {
  }

//---------------------------------------------------------------------------

bool CTagGetAndSetExample::PreStartCheck()
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

const int maxCols=3;
const int maxRows=4;

const int idDX_XY = idDX_SetTagSubsStr1+10;
const int idDX_AB = idDX_XY+(maxCols*maxRows);

//---------------------------------------------------------------------------

void CTagGetAndSetExample::BuildDataFields()
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

  DD.Show(m_GetSubs1.IsActive);
  DD.Double("GetValue1", "", &m_dGetValueSubs1, MF_RESULT|MF_NanOK, m_GetSubs1.Cnv);

  DD.Show(m_GetSubs2.IsActive);
  DD.Double("GetValue2", "", &m_dGetValueSubs2, MF_RESULT|MF_NanOK, m_GetSubs2.Cnv);

  DD.Show();
  DD.String("SetTagSubs1", "", idDX_SetTagSubsStr1, MF_PARAMETER);
  DD.Show(m_SetSubs1.IsActive);//m_iSet1>=0);
  DD.Double("SetValue1", "", &m_dSetValueSubs1, MF_PARAMETER|MF_NanOK, m_SetSubs1.Cnv);

#if 01
  //Example for matrix / grid
  DD.Show();
  DD.Page("TableExample");
  DD.Text("------------------------------------------------------------");
  DD.Text("Matrix");
  DD.MatrixBegin("ScdXY", "XY", maxCols, maxRows, 8, 0, "X", "Y", 0);

  for (int y=0; y<maxRows; y++)
    {
    DD.GridRowStart();
    for (int x=0; x<maxCols; x++)
      {
  		DD.MatrixElementStart(x, y);
      DD.Double("X", "", idDX_XY+x+(y*maxCols), MF_RESULT|MF_NO_FILING, MC_None);
      }
    }

  DD.MatrixEnd();
                           
  DD.Text("------------------------------------------------------------");
  DD.Text("Grid");
  
  DD.GridBegin("AB", maxCols, maxRows);

  CString Txt;
  DD.GridColumnHeader("A\\B", 5, 1, 0);
  for (int x=0; x<maxCols; x++)
    {
    Txt.Format("B%i", x);
    DD.GridColumnHeader(Txt, 9, 1);
    }

  for (int y=0; y<maxRows; y++)
    {
    Txt.Format("A%i", y);
    DD.GridRowHeader(Txt);
    DD.GridRowStart();
    for (int x=0; x<maxCols; x++)
      {
      Txt.Format("AB[%i][%i]", x,y);
      DD.Double(Txt, "", idDX_AB+x+(y*maxCols), MF_PARAMETER|MF_NO_FILING, MC_None);
      }
    }

  DD.GridEnd();

  DD.Text("------------------------------------------------------------");
#endif
  }

//---------------------------------------------------------------------------

bool CTagGetAndSetExample::ExchangeDataFields()
  {
  if (DX.Handle >= idDX_XY && DX.Handle < idDX_XY + maxCols*maxRows)
    {
    DX.Double  = DX.Handle - idDX_XY; 
    return true;
    }
  if (DX.Handle >= idDX_AB && DX.Handle < idDX_AB + maxCols*maxRows)
    {
    DX.Double  = 100+DX.Handle - idDX_AB; 
    return true;
    }

  switch (DX.Handle)
    {
    case idDX_GetTagStr:
      if (DX.HasReqdValue)
        m_GetItem.Tag = TagIO.FormatAsTagAndCnv(DX.String); //force correct formating for user entry of a tag
      DX.String = m_GetItem.Tag;
      return true;
    case idDX_GetTagInfoButton:
		  if (DX.HasReqdValue && DX.Bool)
        {//example for retrieving and displaying information about a tag
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
        m_SetItem.Tag = TagIO.FormatAsTagAndCnv(DX.String); //force correct formating for user entry of a tag
      DX.String = m_SetItem.Tag;
      return true;
    case idDX_GetTagButton:
		  if (DX.HasReqdValue && DX.Bool)
        {//example of direct (once off) retrieval of tag value
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
        {//example of direct (once off) setting of tag value
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
        TagIO.Active = DX.Bool;
      DX.Bool=TagIO.Active;
      return true;
    case idDX_GetTagSubsStr1:
      if (DX.HasReqdValue)
        m_GetSubs1.Tag=DX.String;
      DX.String = m_GetSubs1.Tag;
      return true;
    case idDX_GetTagSubsStr2:
      if (DX.HasReqdValue)
        m_GetSubs2.Tag=DX.String;
      DX.String = m_GetSubs2.Tag;
      return true;
    case idDX_SetTagSubsStr1:
      if (DX.HasReqdValue)
        m_SetSubs1.Tag=DX.String;
      DX.String = m_SetSubs1.Tag;
      return true;
    }

  return false;
  }

//---------------------------------------------------------------------------

bool CTagGetAndSetExample::ValidateDataFields()
  {
  if (TagIO.ValidateReqd())
    {                                    //only check the IO subscription tags if required and if allowed
    if (TagIO.StartValidateDataFields())
      {
      if (m_GetSubs1.Configure(10, NULL, "GetValue1", MTagIO_Get)>0)  //try subscribe (read) to this tag
        m_dGetValueSubs1 = m_GetSubs1.DoubleSI;
      if (m_GetSubs2.Configure(33, NULL, "GetValue2", MTagIO_Get)>0)  //try subscribe (read) to this tag
        m_dGetValueSubs2 = m_GetSubs2.DoubleSI;
      if (m_SetSubs1.Configure(44, NULL, "SetValue1", MTagIO_Set|MTagIO_Parm)>0) //try subscribe (write) to this tag
        m_dSetValueSubs1 = m_SetSubs1.DoubleSI;                         //get the initial value from the tag
      }
    TagIO.EndValidateDataFields();
   
    }
  return true;
  }

//---------------------------------------------------------------------------

void CTagGetAndSetExample::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  { 
  }

//---------------------------------------------------------------------------

void CTagGetAndSetExample::EvalCtrlActions(eScdCtrlTasks Tasks)    
  { 
  }

//---------------------------------------------------------------------------

void CTagGetAndSetExample::EvalCtrlStrategy(eScdCtrlTasks Tasks)   
  { 
  if (m_bOn) 
    {
    try
      {
      if (TagIO.Active)//m_bSubsActive)
        {//tag subscriptions for this model are active; must always get/set values
        
        //get the values of the active (valid) individual get (read) subscription tags
        if (m_GetSubs1.IsActive)
          m_dGetValueSubs1 = m_GetSubs1.DoubleSI; //always as SI units
        if (m_GetSubs2.IsActive)
          m_dGetValueSubs2 = m_GetSubs2.DoubleSI; //always as SI units
        if (m_SetSubs1.IsActive)
          m_SetSubs1.DoubleSI = m_dSetValueSubs1; //always as SI units
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

void CTagGetAndSetExample::EvalCtrlTerminate(eScdCtrlTasks Tasks)
  {
  }

//---------------------------------------------------------------------------
