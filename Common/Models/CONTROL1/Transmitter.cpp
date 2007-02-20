//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __TRANSMITTER_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "transmitter.h"

#include "scd_wm.h"

#if !SKIPIT 

//==========================================================================

enum {  Fail_None     = 0x0000, 
        Fail_Hold     = 0x0001, 
        Fail_MinVal   = 0x0002, 
        Fail_MaxVal   = 0x0003, 
        Fail_LoVal    = 0x0004, 
        Fail_HiVal    = 0x0005, 
        Fail_Type     = 0x000f, 
        Fail_Ramp     = 0x0010, 
        Fail_Noise    = 0x0020
  };  
enum {  XF_None, 
        XF_Linear, 
        XF_Sqr, 
        XF_Sqrt 
  };  
enum {  Flt_None, 
        Flt_FirstOrder 
  };  
enum {  XL_None, 
        XL_Linear, 
        XL_AtoD, 
        XL_PulseEncode
  };  

const long FunctSubsStartIndex = 500;

//==========================================================================

CTransmitterBlk::CTransmitterBlk():
  m_MeasVar(BXOptions(BXO_Funct|BXO_Blank)),
  m_TxVar(BXOptions(BXO_Blank)), 
  m_RawVar(BXOptions(BXO_Blank))
  {
  //m_MeasVar.Init(TRUE, TRUE);
  m_bValid=0;
  m_iPriority=0;

  m_dMeasValue=0;

  m_Xform.m_iType=0;  
  m_Xform.m_dFactor=1;  
  m_Xform.m_dBias=0;  
  
  m_Flt.m_iType=0;
  m_Flt.m_dTau=1;
  m_Flt.m_dPrevValue=0;
  
  m_Fail.m_iType=0;
  m_Fail.m_dSlewTime=300;
  m_Fail.m_dLoValue=0;
  m_Fail.m_dHiValue=100;
  m_Fail.m_dNoiseStdDevFrac=0.1f;
  m_Fail.m_dPrevValue=0;

  m_dMinValue=0; 
  m_dMaxValue=100; 
  m_dLoLimit=0;  
  m_dHiLimit=100;  

  m_bLoInvert=0; 
  m_bHiInvert=0; 
  m_bLo=0;       
  m_bHi=0;       

  m_dTXValue=0;

  m_RawXform.m_iType=0;
  m_RawXform.m_dMinValue=0;
  m_RawXform.m_dMaxValue=4095;

  m_dRawValue=0;       
  }

//--------------------------------------------------------------------------

CTransmitterBlk::~CTransmitterBlk()
  {
  }

//--------------------------------------------------------------------------

void CTransmitterBlk::Init(CNodeXRefMngr * ERH, int iNo)
  {
//  sTagSuffix.Set("FS_%d", iNo);
  m_iPriority = iNo;
  //pXRM = ERH;
  }

//--------------------------------------------------------------------------




inline double tx_Linear_Interpolate (double x, double x1, double y1, double x2, double y2)
//; The function will return the result of a 2D linear interpolation for x between points
//; (x1,y1) and (x2,y2).  If abs(x1 - x2) < epsilon (with epsilon = 1e-10) then y1 will be
//; returned.
  {
  double epsilon;
  double y, delta_x, gradient;

  epsilon = 1e-10;
  y = y1;
  delta_x = fabs(x1 - x2);
  if (delta_x >= epsilon)
    {
    gradient = (y1 - y2) / (x1 - x2);
	  y = y1 + gradient * (x - x1);	
    }
  return y;
  }

inline double tx_Bounded_Linear_Interpolate (double x,  double x1, double y1, 
						   double x2, double y2,
						   double y_min, double y_max)
//; The function will perform a 2D linear interpolation for x between points (x1,y1) and 
//; (x2,y2).  If abs(x1 - x2) < epsilon (with epsilon = 1e-10) the y1 will be used as result. 
//; The result will be limited to the range [y_min,y_max] before being returned.
  {
  double y;

  y = tx_Linear_Interpolate(x, x1, y1, x2, y2);
  y = Range(y_min, y, y_max);
  return y;
  }

inline double tx_First_Order_Filter (double & current_value, double target_value, double time_constant, double dT)
//; The parameter current_value will follow the parameter target_value with first-order 
//; behaviour as defined by the time constant time_constant in seconds.  If 
//; time_constant <= 0, current_value will follow target_value without delay.
  {
  double new_value;

  if (time_constant <= 0)
    new_value = target_value;
  else
	  new_value = current_value + (target_value - current_value) * (1-exp(-dT / GTZ(time_constant)));
  current_value=new_value;
  return new_value;
  }

void CTransmitterBlk::ExecIns(double dT)
  {
//	double temp_rvalue, target_value, min_p, max_f, period, dt, max_time
//	double frequency_tolerance


  double Value;
  switch (m_Xform.m_iType)
    {
    case XF_Linear:
      Value = m_Xform.m_dFactor * m_dMeasValue + m_Xform.m_dBias;
      break;
    case XF_Sqr:
      Value = m_Xform.m_dFactor * m_dMeasValue + m_Xform.m_dBias;
      Value=Value*Value;
      break;
    case XF_Sqrt:
      Value = m_Xform.m_dFactor * m_dMeasValue + m_Xform.m_dBias;
      Value=Sqrt(GEZ(Value));
      break;
    default:
      Value = m_dMeasValue;
    }

  switch (m_Flt.m_iType)
    {
    case Flt_FirstOrder:
      Value = tx_First_Order_Filter (m_Flt.m_dPrevValue, Range((double)m_dMinValue, Value, (double)m_dMaxValue), m_Flt.m_dTau, dT);
      break;
    default:;
    }

  if ((m_Fail.m_iType & Fail_Type)!=0)
    m_Fail.m_dPrevValue=Value;

  switch (m_Fail.m_iType & Fail_Type)
    {
    case Fail_MinVal:
      Value = m_dMinValue;
      break;
    case Fail_MaxVal:
      Value = m_dMaxValue;
      break;
    case Fail_LoVal:
      Value = m_Fail.m_dLoValue;
      break;
    case Fail_HiVal:
      Value = m_Fail.m_dHiValue;
      break;
    case Fail_Hold:
      Value = m_Fail.m_dPrevValue;
      break;
    }

  if (m_Fail.m_iType & Fail_Ramp)
    {
    double MaxChg=(m_dMaxValue-m_dMinValue)*dT/GTZ(m_Fail.m_dSlewTime);
    Value = m_dTXValue + Range(-MaxChg, Value-m_dTXValue, MaxChg);
    Value = Range((double)m_dMinValue, Value, (double)m_dMaxValue);
    }

  m_dTXValue = Value; 

  // calculate switches
  m_bLo=Valid(m_dTXValue) && (m_dTXValue <= m_dLoLimit);
  m_bHi=Valid(m_dTXValue) && (m_dTXValue >= m_dHiLimit);
  if (m_bLoInvert)
     m_bLo=!m_bLo;
  if (m_bHiInvert)
     m_bHi=!m_bHi;

  // calculate raw value
  switch (m_RawXform.m_iType)
    {
    case XL_Linear:
      m_dRawValue = tx_Bounded_Linear_Interpolate (m_dTXValue, m_dMinValue, m_RawXform.m_dMinValue,
						    	                            m_dMaxValue, m_RawXform.m_dMaxValue,
						    	                            m_RawXform.m_dMinValue, m_RawXform.m_dMaxValue);
      break;
    case XL_AtoD:
      m_dRawValue = tx_Bounded_Linear_Interpolate (m_dTXValue, m_dMinValue, m_RawXform.m_dMinValue,
						    	                            m_dMaxValue, m_RawXform.m_dMaxValue,
						    	                            m_RawXform.m_dMinValue, m_RawXform.m_dMaxValue);
      m_dRawValue = Range((long)m_RawXform.m_dMinValue, (long)m_dRawValue, (long)m_RawXform.m_dMaxValue);
      break;
    case XL_PulseEncode:
      /*
      if (maximum_frequency > 0)
		    dt = DeltaTime()
		    frequency_tolerance = 1e-10
		    min_p = 2*dt
		    max_f = 1 / min_p
		    minimum_frequency = max(minimum_frequency,0)
		    maximum_frequency = min(maximum_frequency, max_f)
		    frequency = tx_Bounded_Linear_Interpolate (value, minimum_value, minimum_frequency,
						    	        maximum_value, maximum_frequency,
						    	        minimum_frequency, maximum_frequency)
		    if (frequency < frequency_tolerance)
			    frequency = 0
		    endif
		    if (frequency > 0)
			    period = 1 / frequency
			    max_time = period / 2
			    timer = timer + dt
			    if (timer >= max_time)
				    timer = 0
				    pulse = not(pulse)	
			    endif
		      else
			    timer = 0
			    pulse = 0
		    endif
	      else
		    pulse = 0
	    endif
	    pulse_inverse = not(pulse)
      */
      m_dRawValue = m_dTXValue;
      break;
    default:
      m_dRawValue = m_dTXValue;
    };
  }

//==========================================================================
//
//  
//
//==========================================================================
/*#D:#T:Flow Controller(1)
#X:#h<General Description>#nThis unit is used to ...
#n#n
#G:Units
*/

flag CTransmitter::bWithCnvComment = true;

const long MaxXMTs=20;
const long MaxIOListLen=2*MaxXMTs+1;

static IOAreaRec BaseTransmitterIOArea = {NULL, NULL, CtrlIOId(0), LIO_Out, nc_CLnk, 0, 5};
static CArray <IOAreaRec, IOAreaRec&> TransmitterIOAreaList;
static CSArray IONames;

static double Drw_Transmitter[] =
       {
       DD_Arc, 0,0,3,
       DD_End
       };

IMPLEMENT_MODELUNIT(CTransmitter, "Transmitter", "", Drw_Transmitter, "Transmitter", "T", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                    "Control:Transmitter",
                   "Control - Transmitter.")

CTransmitter::CTransmitter(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
    //, m_MXRH(this)
  {
  if (TransmitterIOAreaList.GetSize()==0)
    {
    TransmitterIOAreaList.SetSize(MaxIOListLen);
    IONames.SetSize(MaxIOListLen);
    int j=0;
    for (int i=0; i<MaxXMTs; i++)
      {
      IONames[j].Set("T%i.Tx", i);
      TransmitterIOAreaList[j]=BaseTransmitterIOArea;
      TransmitterIOAreaList[j].m_Id=CtrlIOId(j);
      TransmitterIOAreaList[j].SetIOName(IONames[j]());
      TransmitterIOAreaList[j].SetIODesc("");
      j++;
      IONames[j].Set("T%i.Raw", i);
      TransmitterIOAreaList[j]=BaseTransmitterIOArea;
      TransmitterIOAreaList[j].m_Id=CtrlIOId(j);
      TransmitterIOAreaList[j].SetIOName(IONames[j]());
      TransmitterIOAreaList[j].SetIODesc("");
      j++;
      }
    TransmitterIOAreaList[j++]=BaseTransmitterIOArea;
    }

  AttachClassInfo(nc_Control, &TransmitterIOAreaList[0], &NullFlwGroup);

  fActiveHoldOK_Dyn=true;
  bOn = 1;

  bAboutToStart = 0;
  //m_DataBlk.GetSize() = 0;
  //m_DataBlk = NULL;
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  //SetCount(MaxXMTs);
  }

//--------------------------------------------------------------------------

CTransmitter::~CTransmitter()
  {
  SetCount(0);
  }

//--------------------------------------------------------------------------

void CTransmitter::SetCount(long NewSize)
  {
  NewSize = Range(0L, NewSize, MaxXMTs);
  if (NewSize!=m_DataBlk.GetSize())
    {
    m_DataBlk.SetSize(NewSize); 
    if (SortRqd())
      Sort();
    FixIOTags();
    StructureChanged(this);
    }
  }

//--------------------------------------------------------------------------

flag CTransmitter::SortRqd()
  {
  return false; // This will prevent IO problems. - maybe improve

  //for (int i=1; i<m_DataBlk.GetSize(); i++)
  //  {
  //  if (m_DataBlk[i-1]->m_iPriority>m_DataBlk[i]->m_iPriority)
  //    return true;
  //  }
  //return false;
  }

//--------------------------------------------------------------------------

void CTransmitter::Sort()
  {
  //for (int i=0; i<m_DataBlk.GetSize(); i++)
  //  {
  //  int MinPos = -1;
  //  long MinVal = m_DataBlk[i]->m_iPriority;
  //  for (int j=i+1; j<m_DataBlk.GetSize(); j++)
  //    {
  //    if (m_DataBlk[j]->m_iPriority<MinVal)
  //      {
  //      MinPos = j;
  //      MinVal = m_DataBlk[j]->m_iPriority;
  //      }
  //    }
  //  if (MinPos>=0)
  //    {
  //    CTransmitterBlk * p = &m_DataBlk[i];
  //    m_DataBlk[i] = m_DataBlk[MinPos];
  //    m_DataBlk[MinPos] = p;
  //    }
  //  }
  //StructureChanged(this);
  }

//--------------------------------------------------------------------------

void CTransmitter::FixIOTags()
  {
  int N=m_DataBlk.GetSize()*2+1;
  m_IOAreas.SetSize(N);
  int j=0;
  for (int i=0; i<m_DataBlk.GetSize(); i++)
    {
    CTransmitterBlk * pT = m_DataBlk[i];
    if (!pT->m_sID())
      pT->m_sID.Set("T%i",i);
    pT->m_sIDMeas.Set("%s.%s.Meas", Tag(), pT->m_sID());
    pT->m_sIDVal.Set("%s.%s.Tx", Tag(), pT->m_sID());
    pT->m_sIDRaw.Set("%s.%s.Raw", Tag(), pT->m_sID());

    pT->m_sTagMeas.Set("%s.Cfg.[%i].%s", Tag(), i, "MeasVal");
    pT->m_sTagVal.Set("%s.Cfg.[%i].%s", Tag(), i, "TxVal");
    pT->m_sTagRaw.Set("%s.Cfg.[%i].%s", Tag(), i, "RawVal");

    IOAreaRec & AV= m_IOAreas[j];
    AV=BaseTransmitterIOArea;
    AV.SetIODesc(pT->m_sIDVal());
    AV.SetIOName(IONames[j]());
    AV.m_Id=CtrlIOId(j);
    j++;   

    IOAreaRec & AR= m_IOAreas[j];
    AR=BaseTransmitterIOArea;
    AR.SetIODesc(pT->m_sIDRaw());
    AR.SetIOName(IONames[j]());
    AR.m_Id=CtrlIOId(j);
    j++;
    }
  m_IOAreas[j]=BaseTransmitterIOArea;
  
  AttachIOAreas(&m_IOAreas[0], NULL, NULL, true);
  StructureChanged(this);
  }

//--------------------------------------------------------------------------

void CTransmitter::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

pchar CTransmitter::Tag(pchar ReqdTag) 
  { 
  FlwNode::Tag(ReqdTag);
  if (ReqdTag) 
    FixIOTags(); 
  return FlwNode::Tag(); 
  };

//--------------------------------------------------------------------------

void CTransmitter::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  int id=IOId_Self(i);
  int ix=(id-CtrlIOId(0))/2;
  if (ix<m_DataBlk.GetSize())
    {
    CTransmitterBlk * pT = m_DataBlk[ix];
    switch (id%2)
      {
      case 0:
        ID=pT->m_sIDVal();
        Tg=pT->m_sTagVal();
        break;
      case 1:
        ID=pT->m_sIDRaw();
        Tg=pT->m_sTagRaw();
        break;
      }  
    }
  else
    ID=Tg=FullObjTag();
  };

//--------------------------------------------------------------------------

const word idmCount     = 1000;
const word idmCheckBtn  = 1001;
const word idmCfgTags   = 1100;

const word NoOfCfgTags = 10;

//--------------------------------------------------------------------------

void CTransmitter::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "CTransmitter", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",           "",         DC_, "", &bOn,           this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("ShowCnv",      "",         DC_, "", &bWithCnvComment,this, isParmStopped, DDBYesNo);
  DDB.Visibility();
  DDB.Long("", "Tx_Cnt",  DC_, "", idmCount,       this, isParmStopped/*|AffectsStruct*/);
  DDB.Text("");
  if (m_DataBlk.GetSize()>0)
    {
    DDB.Button("Check_tags",   "", DC_,    "",    idmCheckBtn,      this, isParmStopped);
    DDB.Text("");
    DDB.String("State",        "", DC_,    "",    &m_StateLine[0],  this, noSnap|noFile);
    DDB.Text("Error:");
    DDB.String("Msg_1",        "", DC_,    "",    &m_StateLine[1],  this, noSnap|noFile);
    DDB.String("Msg_2",        "", DC_,    "",    &m_StateLine[2],  this, noSnap|noFile);
    DDB.Text("");
    }
  DDB.Text("----------------------------------------");

  static DDBValueLst DDBFail[]={
      {Fail_None,                         "None"             },
      {Fail_Hold,                         "Hold"             },
      {Fail_Hold|Fail_Noise,              "Hold,Noise"       },
      {Fail_MinVal,                       "MinVal"           },
      {Fail_MaxVal,                       "MaxVal"           },
      {Fail_LoVal,                        "LoVal"            },
      {Fail_HiVal,                        "HiVal"            },
      {Fail_MinVal|Fail_Ramp,             "MinVal,Ramp"      },
      {Fail_MaxVal|Fail_Ramp,             "MaxVal,Ramp"      },
      {Fail_LoVal|Fail_Ramp,              "LoVal,Ramp"       },
      {Fail_HiVal|Fail_Ramp,              "HiVal,Ramp"       },
      {Fail_MinVal|Fail_Noise,            "MinVal,Noise"     },
      {Fail_MaxVal|Fail_Noise,            "MaxVal,Noise"     },
      {Fail_LoVal|Fail_Noise,             "LoVal,Noise"      },
      {Fail_HiVal|Fail_Noise,             "HiVal,Noise"      },
      {Fail_MinVal|Fail_Ramp|Fail_Noise,  "MinVal,Ramp,Noise"},
      {Fail_MaxVal|Fail_Ramp|Fail_Noise,  "MaxVal,Ramp,Noise"},
      {Fail_LoVal|Fail_Ramp|Fail_Noise,   "LoVal,Ramp,Noise" },
      {Fail_HiVal|Fail_Ramp|Fail_Noise,   "HiVal,Ramp,Noise" },
      {0}};
  static DDBValueLst DDBXForms[]={
      {XF_None,   "None"   },
      {XF_Linear, "Linear" },
      //{XF_Sqr,    "Sqr"    },
      //{XF_Sqrt,   "Sqrt"   },
      {0}};
  static DDBValueLst DDBFilters[]={
      {Flt_None,        "None"   },
      {Flt_FirstOrder,  "FirstOrder" },
      {0}};
  static DDBValueLst DDBRawXLates[]={
      {XL_None,     "None"   },
      {XL_AtoD,     "AtoD"  },
      {XL_Linear,   "Linear" },
      //{XL_PulseEncode  "PulseWidth" },
      {0}};
  char Buff[128];
  Strng Tag;
  if (DDB.BeginArray(this, "Cfg", "Tx_Cfg", m_DataBlk.GetSize()))
    {
    for (int i=0; i<m_DataBlk.GetSize(); i++)
      {
      if (1)// i>0 && (i % 2)==1)
        {
        sprintf(Buff, "%d", i);
        DDB.Page(Buff, DDB_OptPage);
        }
      DDB.BeginElement(this, i);
      CTransmitterBlk * T = m_DataBlk[i];
      Strng CnvTxt;
      if (bWithCnvComment && XRefsValid() && T->m_bValid)
        GetValidCnvTxt(T->m_MeasVar, CnvTxt);
      DDB.String("ID",                 "",            DC_,          "", idmCfgTags+(i*NoOfCfgTags)+1, this, isParmStopped|isTag);
      DDB.String("MeasTag",            "",            DC_,          "", idmCfgTags+(i*NoOfCfgTags)+2, this, isParmStopped|isTag);
      //DDB.Double("Measurem_dValue",      "MeasVal",     DC_,          "", &T->m_dMeasValue,             this, (T->m_MeasVar.IsBlank()?isParm:isResult|noFileAtAll));
      DDB.Double("Measurem_dValue",      "MeasVal",     DC_,          "", &T->m_dMeasValue,             this, isResult);
      if (CnvTxt.Len())
        DDB.TagComment(CnvTxt());

      DDB.Byte  ("Transform",          "",            DC_,          "",   &T->m_Xform.m_iType,  this, isParm|SetOnChange, DDBXForms);
      DDB.Visibility(NSHM_All, T->m_Xform.m_iType);
      DDB.Float ("Xf.Factor",          "",            DC_,          "",   &T->m_Xform.m_dFactor,      this, isParm);
      DDB.Float ("Xf.Bias",            "",            DC_,          "",   &T->m_Xform.m_dBias,        this, isParm);

      DDB.Visibility();

      DDB.Byte  ("Filter",             "",            DC_,          "",   &T->m_Flt.m_iType,   this, isParm|SetOnChange, DDBFilters);
      DDB.Visibility(NSHM_All, T->m_Flt.m_iType == Flt_FirstOrder);
      DDB.Float ("Flt.TimeConst",      "Flt.Tau",     DC_Time,      "s",  &T->m_Flt.m_dTau,    this, isParm);
      DDB.Double("Flt.PrevValue",      "",            DC_,          "",   &T->m_Flt.m_dPrevValue, this, InitHidden);
      DDB.Visibility();

      DDB.Byte  ("Fail.Mode",          "",            DC_,          "",   &T->m_Fail.m_iType,   this, isParm|SetOnChange, DDBFail);
      DDB.Float ("Fail.LoValue",       "",            DC_,          "",   &T->m_Fail.m_dLoValue,   this, isParm);
      DDB.Float ("Fail.HiValue",       "",            DC_,          "",   &T->m_Fail.m_dHiValue,   this, isParm);
      DDB.Float ("Fail.NoiseStdDev",   "",            DC_,          "%",  &T->m_Fail.m_dNoiseStdDevFrac,   this, isParm);
      DDB.Float ("Fail.SlewTime",      "",            DC_Time,      "s",  &T->m_Fail.m_dSlewTime,   this, isParm);


      DDB.Float ("MinValue",           "MinV",        DC_,          "",   &T->m_dMinValue,   this, isParm);
      DDB.Float ("MaxValue",           "MaxV",        DC_,          "",   &T->m_dMaxValue,   this, isParm);
      DDB.Double("TxValue",            "TxVal",            DC_,          "",   &T->m_dTXValue,      this, isResult);

      DDB.Float ("LoLimit",            "LL",          DC_,          "",   &T->m_dLoLimit,    this, isParm);
      DDB.Float ("HiLimit",            "HL",          DC_,          "",   &T->m_dHiLimit,    this, isParm);

      flag ff=0;
      DDB.CheckBox  ("LoInvert",       "",            DC_,          "",   &T->m_bLoInvert,   this, isParm|SetOnChange);
      DDB.CheckBox  ("HiInvert",       "",            DC_,          "",   &T->m_bHiInvert,   this, isParm|SetOnChange);
      DDB.Bool      ("",               "Lo",          DC_,          "",   &T->m_bLo,         this, isResult);
      DDB.Bool      ("",               "Hi",          DC_,          "",   &T->m_bHi,         this, isResult);

      DDB.Byte      ("RawTransform",   "",            DC_,          "",   &T->m_RawXform.m_iType,  this, isParm|SetOnChange, DDBRawXLates);
      DDB.Visibility(NSHM_All, T->m_RawXform.m_iType>=XL_AtoD);
      DDB.Float     ("Raw.MinValue",   "",            DC_,          "",   &T->m_RawXform.m_dMinValue,    this, isParm);
      DDB.Float     ("Raw.MaxValue",   "",            DC_,          "",   &T->m_RawXform.m_dMaxValue,    this, isParm);
      DDB.Visibility();

      DDB.Double    ("RawValue",       "RawVal",      DC_,          "",   &T->m_dRawValue,           this, isResult);

      DDB.String    ("PLCAddess",      "",            DC_,          "",   &T->m_sPLCAddress,    this, isParmStopped|InitHidden);
      
      DDB.Long      ("Priority",       "",            DC_,          "",   idmCfgTags+(i*NoOfCfgTags)+4, this, isParmStopped|InitHidden);
      DDB.Text("----------------------------------------");
      }
    }
  DDB.EndArray();

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CTransmitter::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmCount:
      if (DCB.rL)
        {
        if (*DCB.rL!=m_DataBlk.GetSize())
          StructureChanged(this);
        SetCount(Range(0L, *DCB.rL, 50L));
        }
      DCB.L = m_DataBlk.GetSize();
      return True;
    case idmCheckBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        //AccNodeWnd::RefreshData(TRUE); ???
        //TODO: need to accept current data before processing button push!
        if (PreStartCheck())
          LogNote(Tag(), 0, "No bad external tag references or function errors");
        bAboutToStart = 0;
        }
      DCB.B=0;
      return True;
    default:
      if (DCB.lHandle>=idmCfgTags)
        {
        int Index = (DCB.lHandle - idmCfgTags) / NoOfCfgTags;
        if (Index<m_DataBlk.GetSize())
          {
          CTransmitterBlk * T = m_DataBlk[Index];
          switch ((DCB.lHandle - idmCfgTags) - (Index * NoOfCfgTags))
            {
            case 1:
              {
              if (DCB.rpC)
                {
                bool OK=true;
                for (int i=0; OK && i<m_DataBlk.GetSize(); i++)
                  if (i!=Index && m_DataBlk[i]->m_sID.XStrICmp(DCB.rpC)==0)
                    OK=false;
                if (OK)
                  {
                  T->m_sID=DCB.rpC;
                  for (int j=0; j<T->m_sID.Length(); j++)
                    if (T->m_sID[j]==' ')
                      T->m_sID[j]=' ';
                  MyTagsHaveChanged();
                  }
                }
              DCB.pC = T->m_sID();
              break;
              }
            case 2:
              {
              if (DCB.rpC)
                {
                if (T->m_MeasVar.SetVar(DCB.rpC, false, NULL, &T->m_sID)==1)
                  {
                  MyTagsHaveChanged();
                  }
                }
              DCB.pC = T->m_MeasVar.sVar();
              break;
              }
            case 4:
              if (DCB.rL)
                {
                T->m_iPriority = *DCB.rL;
                if (SortRqd())
                  {
                  Sort();
                  }
                }
              DCB.L = T->m_iPriority;
              break;
//            case 5:
//              //if (DCB.rD)
//              //  xxx = *DCB.rD;
//              DCB.D = T->dTotalMass/GTZ(T->dTtlTime);
//              break;
//            case 6:
//              //if (DCB.rD)
//              //  xxx = *DCB.rD;
//              DCB.D = T->dTotalMass/GTZ(T->dTtlTime - T->dZeroTime);
//              break;
//            case 7:
//              //if (DCB.rD)
//              //  xxx = *DCB.rD;
//              DCB.D = T->dTotalVol/GTZ(T->dTtlTime);
//              break;
//            case 8:
//              //if (DCB.rD)
//              //  xxx = *DCB.rD;
//              DCB.D = T->dTotalMassS/GTZ(T->dTtlTime);
//              break;
//            case 9:
//              //if (DCB.rD)
//              //  xxx = *DCB.rD;
//              DCB.D = T->dTotalMassL/GTZ(T->dTtlTime);
//              break;
            }
          }
        return True;
        }
    }

  return False;
  }

//--------------------------------------------------------------------------

flag CTransmitter::ValidateData(ValidateDataBlk & VDB)
  {
  if (m_DataBlk.GetSize()<1)
    SetCount(1);
  if (SortRqd())
    {
    Sort();
    FixIOTags();
    }
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag CTransmitter::PreStartCheck()
  {
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  if (bOn)
    {
    bAboutToStart = 1;
    }
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

bool CTransmitter::TestXRefListActive() { return SetXRefListActive(!GetActiveHold() && bOn!=0); }

//---------------------------------------------------------------------------

int CTransmitter::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)//bOn)
    {
    FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<m_DataBlk.GetSize(); i++)
      {
      CTransmitterBlk * T = &m_DataBlk[i];
      int RetCode = T->m_MeasVar.UpdateXRef(T, 0, 1, FunctNo, this, -1, T->m_sTagMeas(), T->m_sIDMeas(), "Tranmitter:Meas", Results);

      T->m_bValid = RetCode==BXR_OK;

      int RetCode1 = T->m_TxVar.UpdateXRef(T, 1, 0, FunctNo, this, CtrlIOId(i*2), T->m_sTagVal(), T->m_sIDVal(), "Tranmitter:Tx", Results);
      int RetCode2 = T->m_RawVar.UpdateXRef(T, 1, 0, FunctNo, this, CtrlIOId(i*2+1), T->m_sTagRaw(), T->m_sIDRaw(), "Tranmitter:Raw", Results);
      }
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void CTransmitter::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int i=0; i<m_DataBlk.GetSize(); i++)
    {
    CTransmitterBlk * p = &m_DataBlk[i];
    p->m_MeasVar.UnlinkXRefs();
    p->m_TxVar.UnlinkXRefs();
    p->m_RawVar.UnlinkXRefs();
    }

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void CTransmitter::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  if (XRefListActive() && ICGetTimeInc() > 0.0)
    {
    //CStopWatch SW;
    //SW.Start();
    //dbgpln("---------- %10s %s","", Tag()); 

    GetNearXRefValues();
    //for (int i=0; i<m_NearXRefs.GetSize(); i++)
    //  if (m_NearXRefs[i]->bMustGet)
    //    m_NearXRefs[i]->GetNearXRefValue();

    //dbgpln("           %10.2f %s",SW.Lap()*1e6, Tag()); 
    //solve pgm functions...
    if (FnMngrPresent())
      {
      CGExecContext ECtx(this);
      ECtx.dIC_Time = ICGetTime();
      ECtx.dIC_dTime = ICGetTimeInc();
      ECtx.OnStart = bAboutToStart;
      ECtx.m_HoldNearXRefGet=true;
      ECtx.m_HoldNearXRefSet=true;
      FnMngr().Execute(ECtx);
      bAboutToStart = 0;
      if (ECtx.DoXStop)
        {
        LogError(Tag(), 0, "SysCAD stopped by function");
        ExecObj()->XStop();
        }
      if (ECtx.DoXIdle)
        {
        LogError(Tag(), 0, "SysCAD paused by function");
        ExecObj()->XIdle();
        }
      }

    //dbgpln("           %10.2f %s",SW.Lap()*1e6, Tag()); 
    //solve FlowStats...
    for (int i=0; i<m_DataBlk.GetSize(); i++)
      {
      CTransmitterBlk * T = m_DataBlk[i];
      if (T->m_bValid)
        T->m_MeasVar.GetValue(T->m_dMeasValue);
      // allow typed in measured value to be acted upon
      T->ExecIns(ICGetTimeInc());
      T->m_TxVar.PutValue(m_DataBlk[i]->m_dTXValue);
      T->m_RawVar.PutValue(m_DataBlk[i]->m_dRawValue);
      }
    //dbgpln("           %10.2f %s",SW.Lap()*1e6, Tag()); 
    //for (i=0; i<NoCIOs(); i++)
    //  {
    //  int Id=IOId_Self(i);
    //  int tx=Id/2;
    //  switch (Id%2)
    //    {
    //    case 0:
    //      SetCIO_Value(i, m_DataBlk[tx]->m_dTXValue);  
    //      //dbgpln("SetXmtValue %10.6f %s", CIO_Value(i), sTag.Buff);
    //      break;
    //    case 1:
    //      SetCIO_Value(i, m_DataBlk[tx]->m_dRawValue);
    //      //dbgpln("SetXmtRaw   %10.6f %s", CIO_Value(i), sTag.Buff);
    //      break;
    //    }
    //  }
    }
  }

//--------------------------------------------------------------------------

void   CTransmitter::SetState(eScdMdlStateActs RqdState)
  {
  int i;
  FlwNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit   : 
    case MSA_ZeroFlows: 
    case MSA_Empty    : 
      for (i=0; i<m_DataBlk.GetSize(); i++)
        {
        CTransmitterBlk * T = m_DataBlk[i];
        T->m_Flt.m_dPrevValue=T->m_dTXValue;
        }
      break;
    case MSA_PreSet   : 
      break;
    case MSA_DynStatsRunInit: 
      break;
    default: 
      break;
    }
  }

//--------------------------------------------------------------------------

void CTransmitter::EvalDiscrete()
  {
  }

//--------------------------------------------------------------------------

int CTransmitter::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int i=0; i<m_DataBlk.GetSize(); i++)
    {
    if (m_DataBlk[i]->m_MeasVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    }
  if (DidChange)
    {
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int CTransmitter::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int i=0; i<m_DataBlk.GetSize(); i++)
    {
    if (m_DataBlk[i]->m_MeasVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects Transmitter model '%s'.", pDelTag, Tag());
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------

dword CTransmitter::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag CTransmitter::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\t?????????"; return 1;
    //case  2: pS="E\tErrors in evaluating functions"; return 1;
    //case  3: pS="E\tEngineering units invalid"; return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================

#endif