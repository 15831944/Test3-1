//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  __PIDCON_CPP

#include "stdafx.h"
#include "sc_defs.h"
#include "pgm_e.h"
#include "pgm_c.h"
#include "pidcon.h"
//#include "optoff.h"

//==========================================================================

const long FunctSubsStartIndex = 500;

//==========================================================================

const int XRefTags = 4;
inline int MeasIOInx(int Index)    { return (Index*XRefTags)+0; };
inline int SptIOInx(int Index)     { return (Index*XRefTags)+1; };
inline int OutIOInx(int Index)     { return (Index*XRefTags)+2; };
inline int FeedFwdIOInx(int Index) { return (Index*XRefTags)+3; };

const long MaxPIDs=20;
const long MaxIOListLen=XRefTags*MaxPIDs+1;

PIDInfo::PIDInfo(int Index):
  m_MeasVar(BXO_Funct), 
  m_OutputVar(BXO_Blank),
  m_SetPointVar(BXOptions(BXO_Funct|BXO_Blank)),
  m_FeedFwdVar(BXOptions(BXO_Funct|BXO_Blank))
  {
  //m_MeasVar.Init(TRUE, FALSE, MeasIOInx(Index));
  //m_SetPointVar.Init(TRUE, TRUE, SptIOInx(Index));
  //m_OutputVar.Init(FALSE, TRUE, OutIOInx(Index));
  
//  m_sName = "";
  bOn = 1;
  bValid = 0;
  iPriority = 0;
  }

PIDInfo::~PIDInfo()
  {
  }

//==========================================================================

PID2Info::PID2Info(int Index) :
  PIDInfo(Index), CPID2(this)
  {
  }

PID2Info::~PID2Info()
  {
  }

void PID2Info::Init(CNodeXRefMngr * ERH, int iNo)
  {
  Strng X;
  X.Set("PID_%d", iNo);
  SetTagSuffix(X());
  iPriority = iNo;
    //m_sName.Set("PID_%d", iNo);
  pXRM = ERH;
  }

void PID2Info::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj)
  {
  //save internal variables...
  DDB.Double("Period",          "",     DC_, "", &m_Period,       pObj, noView|isParm|NAN_OK);
  DDB.Bool  ("Reset",           "",     DC_, "", &m_Reset,        pObj, noView|isParm);
  DDB.Bool  ("PrvAuto",         "",     DC_, "", &m_PrvAuto,      pObj, noView|isParm);
  DDB.Double("PrvSetPoint",     "",     DC_, "", &m_PrvSetPoint,  pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvOutput",       "",     DC_, "", &m_PrvOutput,    pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvPBand",        "",     DC_, "", &m_PrvPBand,     pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvDc",           "",     DC_, "", &m_PrvDc,        pObj, noView|isParm|NAN_OK);
  DDB.Long  ("HoldCvgCount",    "",     DC_, "", &m_HoldCvgCount, pObj, noView|isParm);
  }

//==========================================================================

PID3Info::PID3Info(int Index) :
  PIDInfo(Index), CPID3(this)
  {
  }

PID3Info::~PID3Info()
  {
  }

void PID3Info::Init(CNodeXRefMngr * ERH, int iNo)
  {
  Strng X;
  X.Set("PID_%d", iNo);
  SetTagSuffix(X());
  iPriority = iNo;
//  m_sName.Set("PID_%d", iNo);
  pXRM = ERH;
  }

void PID3Info::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj)
  {
  //save internal variables...
  DDB.Double("Period",          "",     DC_, "", &m_Period,       pObj, noView|isParm|NAN_OK);
  DDB.Bool  ("Reset",           "",     DC_, "", &m_Reset,        pObj, noView|isParm);
  DDB.Bool  ("PrvAuto",         "",     DC_, "", &m_PrvAuto,      pObj, noView|isParm);
  DDB.Double("PrvSetPoint",     "",     DC_, "", &m_PrvSetPoint,  pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvOutput",       "",     DC_, "", &m_PrvOutput,    pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvGn",           "",     DC_, "", &m_PrvGn,        pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvDc",           "",     DC_, "", &m_PrvDc,        pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvMeas",         "",     DC_, "", &m_PrvMeas,      pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvRange",        "",     DC_, "", &m_PrvRange,     pObj, noView|isParm|NAN_OK);
  DDB.Long  ("HoldCvgCount",    "",     DC_, "", &m_HoldCvgCount, pObj, noView|isParm);
  }

//==========================================================================

PID4Info::PID4Info(int Index) :
  PIDInfo(Index), CPID4(this)
  {
  }

PID4Info::~PID4Info()
  {
  }

void PID4Info::Init(CNodeXRefMngr * ERH, int iNo)
  {
  Strng X;
  X.Set("PID_%d", iNo);
  SetTagSuffix(X());
  iPriority = iNo;
//  m_sName.Set("PID_%d", iNo);
  pXRM = ERH;
  }

void PID4Info::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj)
  {
  //save internal variables...
  DDB.Double("Period",          "",     DC_, "", &m_Period,       pObj, noView|isParm|NAN_OK);
  DDB.Bool  ("Reset",           "",     DC_, "", &m_Reset,        pObj, noView|isParm);
  DDB.Bool  ("PrvAuto",         "",     DC_, "", &m_PrvAuto,      pObj, noView|isParm);
  DDB.Double("PrvSetPoint",     "",     DC_, "", &m_PrvSetPoint,  pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvOutput",       "",     DC_, "", &m_PrvOutput,    pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvGn",           "",     DC_, "", &m_PrvGn,        pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvDc",           "",     DC_, "", &m_PrvDc,        pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvMeas",         "",     DC_, "", &m_PrvMeas,      pObj, noView|isParm|NAN_OK);
  DDB.Double("PrvRange",        "",     DC_, "", &m_PrvRange,     pObj, noView|isParm|NAN_OK);
  DDB.Long  ("HoldCvgCount",    "",     DC_, "", &m_HoldCvgCount, pObj, noView|isParm);
  }


//==========================================================================
//
//  PID Controller
//
//==========================================================================
/*#D:#T:PID Controller(1)
#X:#h<General Description>#nThis unit is used to simulate any number of PID 
controllers on the plant.  The control logic used to manipulate the variables 
is implemented once at the end of every SysCAD iteration.  The PID controller 
may be used in Mass Balancing or in Dynamic mode.  In Dynamic mode it closely 
simulates an actual plant PID loop, while in Mass Balance mode the time dependant 
factors have no relevance.
#n#n
#n#h<Inputs and Outputs>#nThere are NO inputs or outputs that can be connected
to this unit.
#n#n
#h<Variables to be supplied by the user>#n
#i<On> : On or off. If on, the PID controllers are solved each iteration,
otherwise they are ignored.#n
#i<No_of_PIDs> : The required number of independent PID controllers required.#n
#nFor each PID controller the following variables must be configured (note that
some can only be altered when the solver is stopped):#n
#i<Name> : A text name given to the PID controller. It is often refered to using this name.#n
#i<Meas_Tag> : This is used to determine the input (measured variable) for the
PID controller. It can either be a valid tag or a function. If it is a tag, the
required engineering units may also be entered after the tag in brackets with a
space between the tag and the units (eg "Tnk_1.Lvl (%)"). If it is a function,
the first character must be an '=' followed by a valid expression. See the PGM
documentation for the required syntax.#n
#i<Output_Tag> : This can be left blank or a valid tag can be specified. If a
tag is specified the required engineering units may also be entered after the
tag in brackets with a space between the tag and the units (eg "VLV_3.Posn.Y (%)").
Note that only numerical tags that can be set (are white in access window) can be
entered here.#n
#i<On> : On or off. If on, this PID controller is solved each iteration, otherwise
it is ignored.#n
#i<SetPoint> : The required set point (required value of the measured input), it
should be between the minimum and maximum input (measured) values.#n
#i<Reverse> : The direction in which the PID controller will operate (forward or
reverse acting).  Two options are available, either 0 (off) or 1 (checked).  If the
Controlled variable must move in the same direction as the Controlling variable, then 
this value must be checked.  For example, if the level of a tank is being controlled 
by adjusting the speed of a pump which is pumping Out of the tank, then when the tank 
level rises, the pump speed must also increase.  In this case this variable will be 
set to 1 (checked).#nIf the tank level is being controlled by a valve on an incoming pipe, then when the 
tank level rises the valve position must decrease.  In this case this variable is set 
to 0 (off).#n
#i<PropBand> : The proportional band of the controller.  In mass balancing mode, this is 
the only variable which will influence the operation of the controller.  The larger the 
value the more the control will be damped.#n
#i<Integral> : The intergration constant of the controller.  This is only used in Dynamic 
mode. #b<Note>: This should be 0 in ProBal mode.#n
#i<Derivative> : The derivative constant of the controller.  This is only used in Dynamic 
mode. #b<Note>: This should be 0 in ProBal mode.#n
#i<InMin> : The expected minimum input to the controller. #n
#i<InMax> : The expected maximum input to the controller.  Note: If the actual input to the 
controller moves outside of these expected values, the controller will cease to operate in 
the expected direction.#n
#i<OutMin> : The minimum output from the controller. #n
#i<OutMax> : The maximum output from the controller. #n
#n#n
#h<Associated variables>#n
For each PID controller the following associated variables will be shown:#n
#i<Meas> : The current value of the measured variable specified in Meas_Tag.#n
#i<Output> : The current value of the output of the PID controller.#n
#n#n
#h<Buttons>#n
#i<Check tags and functions> : This can only be used while SysCAD is not solving
or running. It is used to check all the tags and engineering units used by the
PID controllers, as well as the syntax of all the functions. This is done
automatically when the solver starts, so you do not need to use this button.
#b<Note>: If you have just changed a tag or function, you need to press OK for
any changes to be saved before using this button.#n
#n
#n#h<Other>#n
Default model prefix:PID#n
Short name:PID_Cont#n
Model type:Unit#n
#G:Units
*/


static IOAreaRec BasePIDIOArea = {NULL, NULL, CtrlIOId(0), LIO_In, nc_CLnk, 0, 1};
static CArray <IOAreaRec, IOAreaRec&> PIDIOAreaList;
static CSArray IONames;
static int s_nPIDControls=0;
#define PIDCOUNT PID.GetSize()

static double Drw_PIDControl[] =
       {
       DD_Poly, -4,-4, -4,4, 4,4, 4,-4, -4,-4,
       DD_Poly, -4,0, 0,4, 4,0, 0,-4, -4,0,
       DD_End
       };

IMPLEMENT_MODELUNIT(PIDControl, "PID_Cont", "1", Drw_PIDControl, "Control", "PID", TOC_ALL|TOC_DYNAMICFLOW|TOC_GRP_GENERAL|TOC_STD_KENWALT,  
                    "Control:PID",
                   "PID Controller model.")

flag PIDControl::bWithCnvComment = true;

//--------------------------------------------------------------------------

PIDControl::PIDControl(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwNode(pClass_, TagIn, pAttach, eAttach)
  {
  s_nPIDControls++;
  m_bFixIOReqd=1;
  if (PIDIOAreaList.GetSize()==0)
    {
    PIDIOAreaList.SetSize(MaxIOListLen);
    IONames.SetSize(MaxIOListLen);
    int j=0;
    for (int i=0; i<MaxPIDs; i++)
      {
      IONames[j].Set("C%i.Meas", i);
      PIDIOAreaList[j]=BasePIDIOArea;
      PIDIOAreaList[j].m_Id=CtrlIOId(j);
      PIDIOAreaList[j].pName=IONames[j]();
      PIDIOAreaList[j].pDesc="";
      j++;
      IONames[j].Set("C%i.Spt", i);
      PIDIOAreaList[j]=BasePIDIOArea;
      PIDIOAreaList[j].m_Id=CtrlIOId(j);
      PIDIOAreaList[j].pName=IONames[j]();
      PIDIOAreaList[j].pDesc="";
      j++;
      IONames[j].Set("C%i.Out", i);
      PIDIOAreaList[j]=BasePIDIOArea;
      PIDIOAreaList[j].m_Id=CtrlIOId(j);
      PIDIOAreaList[j].pName=IONames[j]();
      PIDIOAreaList[j].pDesc="";
      PIDIOAreaList[j].Dirn=LIO_Out;
      PIDIOAreaList[j].m_nIORqd=0;
      PIDIOAreaList[j].m_nIOMax=5;
      j++;
      IONames[j].Set("C%i.FF", i);
      PIDIOAreaList[j]=BasePIDIOArea;
      PIDIOAreaList[j].m_Id=CtrlIOId(j);
      PIDIOAreaList[j].pName=IONames[j]();
      PIDIOAreaList[j].pDesc="";
      j++;
      }
    PIDIOAreaList[j++]=BasePIDIOArea;
    }

  AttachClassInfo(nc_Control, &PIDIOAreaList[0], &NullFlwGroup);

//  AttachIOAreas(&PIDIOAreaList[0], NULL, NULL, true);

  fActiveHoldOK_Dyn=true;
  bOn = 1;
  //bDoneExtRefs = 0;
  bAboutToStart = 0;
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  }

//--------------------------------------------------------------------------

PIDControl::~PIDControl()
  {
  //dbgpln("~PIDControl : %s", Tag());
  SetCount(0);
  s_nPIDControls--;
  if (s_nPIDControls==0)
    {
    PIDIOAreaList.SetSize(0);
    IONames.SetSize(0);
    }
  }

//--------------------------------------------------------------------------

void PIDControl::SetCount(long NewSize)
  {
  NewSize = max(NewSize, 0L);
  if (NewSize!=PID.GetSize())
    {
    long iCount=PID.GetSize();
    for (int i=NewSize; i<iCount; i++)
      delete PID[i];
    PIDType.SetSize(NewSize);
    PID.SetSize(NewSize); 
    for (int i=iCount; i<NewSize; i++)
      {
      if (PrjFileVerNo()>=20)
        {
        PIDType[i] = 3; 
        PID[i] = new PID3Info(i);
        }
      else
        {
        PIDType[i] = 2; 
        PID[i] = new PID2Info(i);
        }
      
      PID[i]->Init(this, i);
      }

    if (SortRqd())
      Sort();
    FixIOTags();
    StructureChanged(this);
    }
  }

//--------------------------------------------------------------------------
#define WithPIDSort 1

flag PIDControl::SortRqd()
  {
#if WithPIDSort
  for (int i=1; i<PID.GetSize(); i++)
    {
    if (PID[i-1]->iPriority>PID[i]->iPriority)
      return true;
    }
#else
#endif
  return false;
  }

//--------------------------------------------------------------------------

void PIDControl::Sort()
  {
#if WithPIDSort
  for (int i=0; i<PID.GetSize(); i++)
    {
    int MinPos = -1;
    long MinVal = PID[i]->iPriority;
    for (int j=i+1; j<PID.GetSize(); j++)
      {
      if (PID[j]->iPriority<MinVal)
        {
        MinPos = j;
        MinVal = PID[j]->iPriority;
        }
      }
    if (MinPos>=0)
      {
      byte t=PIDType[i];
      PIDType[i]=PIDType[MinPos];
      PIDType[MinPos]=t;

      PIDInfo* p = PID[i];
      PID[i] = PID[MinPos];
      PID[MinPos] = p;
      }
    }
  StructureChanged(this);
  m_bFixIOReqd=1;
  #endif
  }

//--------------------------------------------------------------------------

void PIDControl::ResetData(flag Complete)
  {
  }

//--------------------------------------------------------------------------

void PIDControl::Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg)
  {
  int id=IOId_Self(i);
  int ix=(id-CtrlIOId(0))/XRefTags;
  if (ix<PID.GetSize())
    {
    PIDInfo * pT = PID[ix];
    switch (id%XRefTags)
      {
      case 0:
        ID=pT->m_sIDMeas();
        Tg=pT->m_sTgMeas();
        break;
      case 1:
        ID=pT->m_sIDSpt();
        Tg=pT->m_sTgSpt();
        break;
      case 2:
        ID=pT->m_sIDOut();
        Tg=pT->m_sTgOut();
        break;
      case 3:
        ID=pT->m_sIDFeedFwd();
        Tg=pT->m_sTgFeedFwd();
        break;
      }  
    }
  else
    {
    ID=Tg=FullObjTag();
    }
  };

pchar PIDControl::Tag(pchar ReqdTag) 
  { 
  FlwNode::Tag(ReqdTag);
  if (ReqdTag) 
    FixIOTags(); 
  return FlwNode::Tag(); 
  };

void PIDControl::FixIOTags()
  {
  m_bFixIOReqd=false;
  m_IOAreas.SetSize(PID.GetSize()*XRefTags+1);
  m_IOConnd.SetSize(PID.GetSize()*XRefTags+1);
  int j=0;
  for (int i=0; i<PID.GetSize(); i++)
    {
    PIDInfo * pT = PID[i];
    if (!pT->m_sID())
      pT->m_sID.Set("C%i",i);
    pT->m_sIDMeas.Set("%s.%s.Meas", Tag(), pT->m_sID());
    pT->m_sIDSpt.Set("%s.%s.Spt", Tag(), pT->m_sID());
    pT->m_sIDOut.Set("%s.%s.Out", Tag(), pT->m_sID());
    pT->m_sIDFeedFwd.Set("%s.%s.FF", Tag(), pT->m_sID());
    pT->m_sTgMeas.Set("%s.Cfg.[%i].Meas", Tag(), i);
    pT->m_sTgSpt.Set("%s.Cfg.[%i].Spt", Tag(), i);
    pT->m_sTgOut.Set("%s.Cfg.[%i].Out", Tag(), i);
    pT->m_sTgFeedFwd.Set("%s.Cfg.[%i].FF", Tag(), i);

    IOAreaRec & AM= m_IOAreas[j];
    AM=BasePIDIOArea;
    AM.pDesc=pT->m_sIDMeas();
    AM.pName=IONames[j]();
    AM.m_Id=CtrlIOId(j);
    j++;

    IOAreaRec & AS= m_IOAreas[j];
    AS=BasePIDIOArea;
    AS.pDesc=pT->m_sIDSpt();
    AS.pName=IONames[j]();
    AS.m_Id=CtrlIOId(j);
    j++;

    IOAreaRec & AO= m_IOAreas[j];
    AO=BasePIDIOArea;
    AO.pDesc=pT->m_sIDOut();
    AO.pName=IONames[j]();
    AO.m_Id=CtrlIOId(j);
    AO.Dirn=LIO_Out;
    AO.m_nIORqd=0;
    AO.m_nIOMax=10;
    j++;

    IOAreaRec & AF= m_IOAreas[j];
    AF=BasePIDIOArea;
    AF.pDesc=pT->m_sIDFeedFwd();
    AF.pName=IONames[j]();
    AF.m_Id=CtrlIOId(j);
    j++;
    }
  m_IOAreas[j++]=BasePIDIOArea;

  AttachIOAreas(&m_IOAreas[0], NULL, NULL, true);
  StructureChanged(this);
  }

//--------------------------------------------------------------------------

const word idmPIDCount     = 1000;
const word idmPIDCheckBtn  = 1001;
const word idmCfgTags      = 1100;

//const word NoOfCfgTags = 11;
const word NoOfCfgTags = 13;

//---------------------------------------------------------------------------

void PIDControl::PostConnect(int IONo)
  {
  FlwNode::PostConnect(IONo);
  int i=IOId_Self(IONo)-CtrlIOId(0);
  if (i>=0)
    {
    m_IOConnd.SetSize(Max(i+1, m_IOConnd.GetSize()));
    m_IOConnd[i]=true;
    }
  };

//---------------------------------------------------------------------------

void PIDControl::PreDisConnect(int IONo)
  {
  FlwNode::PreDisConnect(IONo);
  int i=IOId_Self(IONo)-CtrlIOId(0);
  if (i>=0)
    {
    m_IOConnd[i]=false;
    }
  };

//--------------------------------------------------------------------------

void PIDControl::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, "PIDControl", NULL, DDB_NoPage);
  DDB.Text("");
  DDB.CheckBoxBtn("On",     "",               DC_, "", &bOn,        this, isParmStopped, DDBYesNo);
  DDB.CheckBoxBtn("ShowCnv",      "",         DC_, "", &bWithCnvComment, this, isParmStopped, DDBYesNo);
  DDB.Long("No_of_PIDs", "Number_of_PIDs", DC_, "", idmPIDCount, this, isParmStopped/*|AffectsStruct*/);
  DDB.Text("");

  if (PIDCOUNT)
    {
    DDB.Button("Check tags and functions", "", DC_, "", idmPIDCheckBtn, this, isParmStopped);
    DDB.Text("");
    DDB.String("State",        "", DC_,    "",    &m_StateLine[0],  this, noSnap|noFile);
    DDB.Text("Error:");
    DDB.String("Msg_1",        "", DC_,    "",    &m_StateLine[1],  this, noSnap|noFile);
    DDB.String("Msg_2",        "", DC_,    "",    &m_StateLine[2],  this, noSnap|noFile);
    DDB.Text("");
    }
  DDB.Text("----------------------------------------");

  char Buff[128];
  Strng Tag;
  static DDBValueLst DDBPIDType[] = {
    {2, "2 - Original" },
    {3, "3 - Clarke"},
	  {4, "4 - Clarke FF"},
    {0}};
  if (DDB.BeginArray(this, "Cfg", "PID_Cfg", PIDCOUNT))
    {
    for (int i=0; i<PIDCOUNT; i++)
      {
      if (i>0 && (i % 2)==1)
        {
        sprintf(Buff, "Cfg_%d", i);
        DDB.Page(Buff, DDB_RqdPage);
        }
      DDB.BeginElement(this, i);
      DDB.String("ID",               "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+10, this, isParmStopped);
      DDB.Byte("Type",               "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+0, this, isParmStopped, DDBPIDType);
      PIDInfo* p = PID[i];
      Strng InCnvTxt,OutCnvTxt;
      if (bWithCnvComment && XRefsValid() && p->bValid)
        {
        GetValidCnvTxt(p->m_MeasVar, InCnvTxt);
        GetValidCnvTxt(p->m_OutputVar, OutCnvTxt);
        }
      DDB.String("Name",             "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+1, this, isParmStopped);
      DDB.String("SetPoint_Tag",     "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+4, this, m_IOConnd[SptIOInx(i)]?isTag:isParmStopped|isTag|isFunct);
      DDB.String("Measured_Tag",     "Meas_Tag",   DC_, "", idmCfgTags+(i*NoOfCfgTags)+2, this, m_IOConnd[MeasIOInx(i)]?isTag:isParmStopped|isTag|isFunct);
      DDEF_Flags PID4ViewFlag = (PIDType[i]!=4) ? DDEF_NOVIEW : 0; 
      DDB.String("Feedforward_Tag", "FeedForward_Tag",   DC_, "", idmCfgTags+(i*NoOfCfgTags)+11, this, m_IOConnd[FeedFwdIOInx(i)]?isTag|PID4ViewFlag:isParmStopped|isTag|isFunct|PID4ViewFlag);
      DDB.Double("FeedFwd",         "",                  DC_, "", &(p->FeedFwd()), this, isParmConstruct|InitHidden|PID4ViewFlag);

      DDB.String("Output_Tag",       "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+3, this, m_IOConnd[OutIOInx(i)]?isTag:isParmStopped|isTag);
      DDB.Double("SetPoint",         "Spt",        DC_, "", idmCfgTags+(i*NoOfCfgTags)+6, this, p->m_SetPointVar.IsBlank()/* ||p->m_SetPointVar.m_bIsIO*/  ? isParm : 0);
      DDB.Range(p->InMn(), p->InMx());
      if (InCnvTxt.Len())
        DDB.TagComment(InCnvTxt());
      DDB.Double("Measured_Value",   "Meas",       DC_, "", &(p->Meas()),    this, 0);
      DDB.Range(p->InMn(), p->InMx());
      if (InCnvTxt.Len())
        DDB.TagComment(InCnvTxt());
      //WHY 2 outputs???  CNM - Probably to rationlise names ??
      //WHY isParmStopped??? (shouldn't it be isResult?)
      DDB.Double("Output_Value",     "Out",        DC_, "", idmCfgTags+(i*NoOfCfgTags)+12,     this, isParm);
      //if (!DDB.ForView() && !DDB.ForFiling())
      if (OrigPrjFileVerNo()<92 && !DDB.ForView() && !DDB.ForFiling())
        DDB.Double("",                 "Output",     DC_, "", idmCfgTags+(i*NoOfCfgTags)+12,     this, isParm|InitHidden);
      DDB.Range(p->OutMn(), p->OutMx());
      if (OutCnvTxt.Len())
        DDB.TagComment(OutCnvTxt());
      //DDB.Double("SetPoint",         "SetPoint",   DC_, "", &(p->Spt), this, isParm);
      //DDB.Double("SetPoint",         "SetPoint",   DC_, "", idmCfgTags+(i*NoOfCfgTags)+6, this, isParm);
      DDB.CheckBoxBtn("On",          "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+5, this, isParmStopped, DDBYesNo);
      DDB.CheckBoxBtn("Auto",        "",           DC_, "", &(p->AutoMan()), this, isParm/*|InitHidden*/, DDBYesNo);
      DDEF_Flags Old=DDB.GetVisibility();
      //DDB.Visibility(NM_Probal|SM_All|HM_All);
      DDB.Visibility(NM_Dynamic|SM_All|HM_All);
      DDB.CheckBoxBtn("SetPointTrack", "SptTrk",   DC_, "", &(p->SptTrk()), this, isParm|InitHidden, DDBYesNo);
      DDB.Visibility(NM_Dynamic|SM_All|HM_All);
      DDB.CheckBoxBtn("TrackMinMaxMeas", "",       DC_, "", &(p->TrackMinMaxMeas()), this, isParm|InitHidden, DDBYesNo);
      DDB.CheckBoxBtn("TrackMinMaxOut", "",        DC_, "", &(p->TrackMinMaxOut()), this, isParm|InitHidden, DDBYesNo);
      DDB.SetVisibility(Old);
  	  if (PIDType[i]==4)
        DDB.CheckBoxBtn("ReverseActing", "RevActing", DC_, "", &(p->RevActing()), this, isParm, DDBYesNo);
      else
        DDB.CheckBoxBtn("Reverse_Acting", "Reverse", DC_, "", &(p->RevActing()), this, isParm, DDBYesNo);
      DDB.Double("Gain",             "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+8, this, isParm);
      DDB.Double("PropBand",         "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+7, this, isParm);
      DDB.Double("Integral",         "",           DC_, "", &(p->Ic()),     this, isParm);
      DDB.Double("Derivative",       "",           DC_, "", &(p->Dc()),     this, isParm);
      DDB.Double("Bias",             "",           DC_, "", &(p->Bias()),   this, isParm|PID4ViewFlag);

      DDB.Double("Minimum_Input",    "InMin",      DC_, "", &(p->InMn()),   this, isParm);
      if (InCnvTxt.Len())
        DDB.TagComment(InCnvTxt());
      DDB.Double("Maximum_Input",    "InMax",      DC_, "", &(p->InMx()),    this, isParm);
      if (InCnvTxt.Len())
        DDB.TagComment(InCnvTxt());
      DDB.Double("Minimum_Output",   "OutMin",     DC_, "", &(p->OutMn()),   this, isParm);
      if (OutCnvTxt.Len())
        DDB.TagComment(OutCnvTxt());
      DDB.Double("Maximum_Output",   "OutMax",     DC_, "", &(p->OutMx()),   this, isParm);
      if (OutCnvTxt.Len())
        DDB.TagComment(OutCnvTxt());

      DDB.Double("ETerm",            "",           DC_, "", &(p->ETerm()),   this, isParmConstruct|InitHidden);
      DDB.Double("PTerm",            "",           DC_, "", &(p->PTerm()),   this, isParmConstruct|InitHidden);
      DDB.Double("ITerm",            "",           DC_, "", &(p->ITerm()),   this, isParmConstruct|InitHidden);
      DDB.Double("DTerm",            "",           DC_, "", &(p->DTerm()),   this, isParmConstruct|InitHidden);
      
      DDB.Text(" ");
      DDB.Long  ("Priority",         "",           DC_, "", idmCfgTags+(i*NoOfCfgTags)+9, this, isParmStopped);
      DDB.Byte  ("TuneRule",         "",           DC_, "", &(p->TuneRule()), this, isParm|InitHidden, DDBTuneRules);
//TODO Restore
      //DDB.Double("TuneOut",          "",     DC_, "", &(p->TuneOut()), this, isParm|InitHidden|NAN_OK);

      Old=DDB.GetVisibility();
      DDB.Visibility(NM_Probal|SM_All|HM_All);
      DDB.Double("Tolerance_Abs",    "AbsTol",     DC_Frac, "%", &(p->EPSAbs()), this, isParm|NAN_OK);
      DDB.Double("Tolerance_Rel",    "RelTol",     DC_Frac, "%", &(p->EPSRel()), this, isParm|NAN_OK);
      DDB.SetVisibility(Old);

      p->BuildDataDefn(DDB, this);

      //DDB.Double("Kd",               "",           DC_, "", &(p->Kd), this, isParm|InitHidden);
      //DDB.Double("Ks",               "",           DC_, "", &(p->KScale), this, isParm|InitHidden|NAN_OK);
      /*double         RSP;//aux
      unsigned char  ModOpt;//modopt
      double         E_Term;//pt
      double         P_Term;//it
      double         I_Term;//dt
      double         D_Term;//dt*/
      DDB.Text("----------------------------------------");
      }
    }
  DDB.EndArray();

  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag PIDControl::DataXchg(DataChangeBlk & DCB)
  {
  if (FlwNode::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case idmPIDCount:
      if (DCB.rL)
        {
        if (*DCB.rL!=PIDCOUNT)
          StructureChanged(this);
        SetCount(Range(0L, *DCB.rL, MaxPIDs));
        }
      DCB.L = PIDCOUNT;
      return True;
    case idmPIDCheckBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        //AccNodeWnd::RefreshData(TRUE); ???
        //TODO: need to accept current data before processing button push!
        if (PreStartCheck())
          LogNote(Tag(), 0, "No bad external tag references or function errors");
        bAboutToStart = 0;
        }
      DCB.B=0;
      return 1;
    default:
      if (DCB.lHandle>=idmCfgTags)
        {
        int Index = (DCB.lHandle - idmCfgTags) / NoOfCfgTags;
        if (Index<PIDCOUNT)
          {
          PIDInfo* p = PID[Index];
          switch ((DCB.lHandle - idmCfgTags) - (Index * NoOfCfgTags))
            {
            case 10:
              {
              if (DCB.rpC)
                {
                bool OK=true;
                for (int i=0; OK && i<PIDCOUNT; i++)
                  if (i!=Index && PID[i]->m_sID.XStrICmp(DCB.rpC)==0)
                    OK=false;
                if (OK)
                  {
                  p->m_sID=DCB.rpC;
                  for (int j=0; j<p->m_sID.Length(); j++)
                    if (p->m_sID[j]==' ')
                      p->m_sID[j]=' ';
                  MyTagsHaveChanged();
                  }
                }
              DCB.pC = p->m_sID();
              break;
              }
            case 0:
              if (DCB.rB)
                {
                if (PIDType[Index]!=*DCB.rB)
                  {
                  PIDType[Index]=*DCB.rB;
                  PIDInfo *OldPID=PID[Index];
/*
                  if (PIDType[Index]==2)
                    PID[Index]= new PID2Info(Index);
                  else
                    PID[Index]= new PID3Info(Index);
*/
                 if (PIDType[Index]==2)
                    PID[Index]= new PID2Info(Index);
                 else if (PIDType[Index]==3)
                    PID[Index]= new PID3Info(Index);
                 else
                    PID[Index]= new PID4Info(Index);

                  PID[Index]->Init(this, Index);
                  PID[Index]->Copy(OldPID);
                  delete OldPID;
                  m_bFixIOReqd=1;
                  }
                };
                
              DCB.B = PIDType[Index];
              StructureChanged(this);
              break;
            case 1:
              if (DCB.rpC)
                {
                if (p->TagSuffixS().IsEmpty() || p->TagSuffixS().XStrICmp(DCB.rpC)!=0)
                  {
                  StructureChanged(this);
                  //bDoneExtRefs = 0;
                  }
                p->SetTagSuffix(DCB.rpC);
                }
              DCB.pC = p->TagSuffix();
              break;
            case 2:
              if (m_IOConnd[MeasIOInx(Index)])
                {
                Ctrl_GetConnIDStr(CIOWithId_Self(CtrlIOId(MeasIOInx(Index))), m_sTmpID, m_sTmpTg);
                DCB.pC=m_sTmpID();
                }
              else if (p->m_MeasVar.DoDataXchg(DCB)==1)
                {
                MyTagsHaveChanged();
                }
              break;
            case 3:
              if (m_IOConnd[OutIOInx(Index)])
                {
                Ctrl_GetConnIDStr(CIOWithId_Self(CtrlIOId(OutIOInx(Index))), m_sTmpID, m_sTmpTg);
                DCB.pC=m_sTmpID();
                }
              else if (p->m_OutputVar.DoDataXchg(DCB)==1)
                {
                MyTagsHaveChanged();
                }
              break;
            case 4:
              if (m_IOConnd[SptIOInx(Index)])
                {
                Ctrl_GetConnIDStr(CIOWithId_Self(CtrlIOId(SptIOInx(Index))), m_sTmpID, m_sTmpTg);
                DCB.pC=m_sTmpID();
                }
              else if (p->m_SetPointVar.DoDataXchg(DCB)==1)
                {
                MyTagsHaveChanged();
                }
              break;
            case 5:
              if (DCB.rB)
                {
                p->bOn = (*DCB.rB);
                MyTagsHaveChanged();
                }
              DCB.B = p->bOn;
              break;
            case 6:
              if (DCB.rD && (p->m_SetPointVar.IsBlank()))// || p->m_SetPointVar.m_bIsIO))
                p->Spt() = (*DCB.rD);
              DCB.D = p->Spt();
              break;
            case 7:
              if (DCB.rD)
                p->SetPBand(*DCB.rD);
              DCB.D = p->PBand();
              break;
            case 8:
              if (DCB.rD)
                p->SetGain(*DCB.rD);
              DCB.D = p->Gain();
              break;
            case 9:
              if (DCB.rL)
                {
                p->iPriority = *DCB.rL;
                if (SortRqd())
                  Sort();
                }
              DCB.L = p->iPriority;
              break;
           case 11:
              if (m_IOConnd[FeedFwdIOInx(Index)])
                {
                Ctrl_GetConnIDStr(CIOWithId_Self(CtrlIOId(FeedFwdIOInx(Index))), m_sTmpID, m_sTmpTg);
                DCB.pC=m_sTmpID();
                }
              else if (p->m_FeedFwdVar.DoDataXchg(DCB)==1)
                {
                MyTagsHaveChanged();
                }
              break;
            case 12:
              if (DCB.rD)
                {
                if (p->AutoMan() && !DCB.ForFileSnpScn())
                  {
                  Strng X;
                  X.Set("%s.%s", FullObjTag(), p->TagSuffix());
                  LogWarning(X(), 0, "Set ignored - Auto Mode");
                  }
                else
                  p->Out()=*DCB.rD;
                }
              DCB.D = p->Out();
              break;
            }
          }
        return True;
        }
    }

  return False;
  }

//--------------------------------------------------------------------------

flag PIDControl::ValidateData(ValidateDataBlk & VDB)
  {
  for (int i=0; i<PIDCOUNT; i++)
    {
    TaggedObject::ValidateTag(PID[i]->TagSuffixS());
    //PID[i]->TagSuffix() = PID[i]->TagSuffix());
    }
  if (SortRqd())
    Sort();
  if (m_bFixIOReqd)
    FixIOTags();
  return FlwNode::ValidateData(VDB);
  }

//--------------------------------------------------------------------------

flag PIDControl::PreStartCheck()
  {
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  if (bOn)
    {
    bAboutToStart = 1;
    }
  if (m_bFixIOReqd)
    FixIOTags();
  return FlwNode::PreStartCheck();
  }

//--------------------------------------------------------------------------

bool PIDControl::TestXRefListActive()
  { 
  //dbgpln("TestXRefListActive %s %s %s %s", GetActiveHold()?"HOLD":"    ", bOn?"ON":"  ", XRefListActive()?"ACTIVE":"      ", FullObjTag()); 
  return SetXRefListActive(!GetActiveHold() && bOn!=0); 
  }

//---------------------------------------------------------------------------

int PIDControl::UpdateXRefLists(CXRefBuildResults & Results)
  {
  //dbgpln("UpdateXRefLists  %s %s %s %s", GetActiveHold()?"HOLD":"    ", bOn?"ON":"  ", XRefListActive()?"ACTIVE":"      ", FullObjTag()); 
  if (1)//bOn)
    {
    FnMngrClear();
    int FunctNo = 0;
    for (int i=0; i<PIDCOUNT; i++)
      {
      PIDInfo* p = PID[i];
      if (p->bOn)
        {
        BXReturn Ret1 = p->m_MeasVar.UpdateXRef(p, 0, 1, FunctNo, this, CtrlIOId(MeasIOInx(i)), p->m_sTgMeas(), p->m_sIDMeas(), "PID:MeasuredTag", Results);
        BXReturn Ret2 = p->m_OutputVar.UpdateXRef(p, 1, 0, FunctNo, this, CtrlIOId(OutIOInx(i)), p->m_sTgOut(), p->m_sIDOut(), "PID:OutputTag", Results);
        BXReturn Ret3 = p->m_SetPointVar.UpdateXRef(p, 0, 1, FunctNo, this, CtrlIOId(SptIOInx(i)), p->m_sTgSpt(), p->m_sIDSpt(), "PID:OutputTag", Results);
        BXReturn Ret4 = p->m_FeedFwdVar.UpdateXRef(p, 0, 1, FunctNo, this, CtrlIOId(FeedFwdIOInx(i)), p->m_sTgFeedFwd(), p->m_sIDFeedFwd(), "PID:OutputTag", Results);

        p->bValid = (Ret1==BXR_OK) && (Ret2==BXR_OK) && (Ret3==BXR_OK);
        }
      else
        p->bValid = 0;
      p->On()=(bOn && p->bValid && p->bOn);
      }
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void PIDControl::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();
  for (int i=0; i<PIDCOUNT; i++)
    {
    PIDInfo* p = PID[i];
    p->m_MeasVar.UnlinkXRefs();
    p->m_OutputVar.UnlinkXRefs();
    p->m_SetPointVar.UnlinkXRefs();
    p->m_FeedFwdVar.UnlinkXRefs();
    }

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void PIDControl::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  dbgpln("EvalCtrlStrategy %s %s %s %s", GetActiveHold()?"HOLD":"    ", bOn?"ON":"  ", XRefListActive()?"ACTIVE":"      ", FullObjTag()); 
  if (XRefListActive() && ICGetTimeInc() > 0.0)
    {
    
    GetNearXRefValues();
    //for (int i=0; i<m_NearXRefs.GetSize(); i++)
    //  if (m_NearXRefs[i]->bMustGet)
    //    m_NearXRefs[i]->GetNearXRefValue();
    
    //solve pgm functions...
    //m_pFnctMgr->Execute(this, ICGetTime(), IC.GetTimeInc(), DoXStop, NULL, NULL, bAboutToStart, TaggedObject::GlblRunModes(), 0);
    if (FnMngrPresent())
      {
      CGExecContext ECtx(this);
      ECtx.dIC_Time = ICGetTime();
      ECtx.dIC_dTime = ICGetTimeInc();
      ECtx.OnStart = bAboutToStart;
      ECtx.HoldNearXRefXfer=true;

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

    //solve PID controllers...
    for (int i=0; i<PIDCOUNT; i++)
      {
      PID[i]->On()=(PID[i]->bValid && PID[i]->bOn);
      if (PID[i]->On())
        {
        PID[i]->m_SetPointVar.GetValue(PID[i]->Spt());
        PID[i]->m_MeasVar.GetValue(PID[i]->Meas());
        PID[i]->m_FeedFwdVar.GetValue(PID[i]->FeedFwd());
        PID[i]->Period() = ICGetTimeInc();
        double NewVal = PID[i]->ExecIns();
        PID[i]->m_OutputVar.PutValue(NewVal);
        }
      }
    SetNearXRefValues();
    //for (i=0; i<m_NearXRefs.GetSize(); i++)
    //  m_NearXRefs[i]->SetNearXRefValue();

    //for (i=0; i<NoCIOs(); i++)
    //  {
    //  int Id=IOId_Self(i);
    //  int pid=Id/3;
    //  if ((Id%3==2))
    //    SetCIO_Value(i, PID[pid]->Out());  
    //  }
    }
  }

//--------------------------------------------------------------------------

int PIDControl::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  BOOL DidChange = FALSE;
  for (int i=0; i<PIDCOUNT; i++)
    {
    if (PID[i]->m_MeasVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    if (PID[i]->m_OutputVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    if (PID[i]->m_SetPointVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;
    if (PID[i]->m_FeedFwdVar.DoChangeTag(pOldTag, pNewTag))
      DidChange = TRUE;

    }
  if (DidChange)
    {
    //PreStartCheck();
    UnlinkAllXRefs();
    bAboutToStart = 0;
    }
  return EOCT_DONE;
  }

//--------------------------------------------------------------------------

int PIDControl::DeleteTag(pchar pDelTag)
  {
  BOOL FoundOne = FALSE;
  for (int i=0; i<PIDCOUNT; i++)
    {
    if (PID[i]->m_MeasVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    if (PID[i]->m_OutputVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    if (PID[i]->m_SetPointVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    if (PID[i]->m_FeedFwdVar.ContainsTag(pDelTag))
      FoundOne = TRUE;
    }
  if (FoundOne)
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects PID Controller model '%s'.", pDelTag, Tag());
    UnlinkAllXRefs();//PreStartCheck();
    bAboutToStart = 0;
    }
  return EODT_DONE;
  }

//--------------------------------------------------------------------------
                                          
dword PIDControl::ModelStatus()
  {
  dword Status=FlwNode::ModelStatus();
  Status|=bOn ? FNS_On : FNS_Off;
  return Status;
  }

//--------------------------------------------------------------------------

flag PIDControl::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\t?????????"; return 1;
    default:                                               
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================
