//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

//#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

#include "sc_defs.h"
#include "sp_cont.h"
#define  __GSMODEL_CPP
#include "gsmodel.h"
#include "m_base.h"

#include "errorlog.h"
#include "dbgmngr.h"

//#include "optoff.h"

#define dbgGSModel       WITHDEBUG

#if dbgGSModel
static CDbgMngr dbgEvalDerivs      ("GenModel",  "XXX");
#endif

const byte AutoPriority=250/*MaxIOList*/+1;

XID xidGSModelOn     = MdlBsXID(1000);
XID xidGSOperation   = MdlBsXID(1001);
XID xidGSPhMethSpec  = MdlBsXID(1002);
XID xidGSMDmndSplts  = MdlBsXID(2000);                       // Keep MaxIOList Spare
XID xidGSMSpcSplts   = xidGSMDmndSplts+MaxIOList;  // Keep MaxSpecies*MaxIOs Spare
XID xidGSPhSplts     = xidGSMSpcSplts+MaxSpecies*MaxIOList;  // Keep MaxSpecies*MaxIOs Spare
XID xidLast          = xidGSPhSplts+MaxPhases*MaxIOList;  // Keep MaxSpecies*MaxIOs Spare

//============================================================================
//
//
//
//============================================================================

GSIOAlpha::GSIOAlpha() :
  Alphas(*(new GSAlphaArray)),
  Splits(*(new GSAlphaArray)),
  QmRqds(*(new GSAlphaArray)),
  QvRqds(*(new GSAlphaArray))
  {

  iQmMode=GSA_QmDefault;
  iMkMode=GSA_MkDefault;

  iOrder     = AutoPriority;
  iPriority  = AutoPriority;

  Qm         = 0.0f;
  QmErr      = 0.0f;
  Qv         = 0.0f;
  QvErr      = 0.0f;
  FracSplit  = 0.5f;
  DmndSplit  = 0.0f;
  LSRatio    = 1.0f;
  Density    = 1000.0f;
  SFrac      = 0.5f;
  LFrac      = 0.5f;
  VFrac      = 1.0f;
  m_SSplt      = 0.1f;
  m_LSplt      = 0.1f;
  m_VSplt      = 0.1f;
  m_SQmRqd     = 0.0f;
  m_LQmRqd     = 0.0f;
  m_VQmRqd     = 0.0f;
  m_SQvRqd     = 0.0f;
  m_LQvRqd     = 0.0f;
  m_VQvRqd     = 0.0f;
  for (int i=0; i<MaxPhases; i++)
    {
    m_PhSplt[i]   = fNAN;
    m_PhQmRqd[i]  = fNAN;
    m_PhQvRqd[i]  = fNAN;
    }
  for (i=0; i<BOT_Last; i++)
    {
    m_bPhSpltHasValid[i]=false;
    m_bPhQmRqdHasValid[i]=false;
    m_bPhQvRqdHasValid[i]=false;
    }

  Alphas.SetSize(0, SDB.Count()+1);
  Splits.SetSize(0, SDB.Count()+1);
  };

// ---------------------------------------------------------------------------

GSIOAlpha::~GSIOAlpha()
  {
  delete &Alphas;
  delete &Splits;
  delete &QmRqds;
  delete &QvRqds;
  };

// ---------------------------------------------------------------------------

GSIOAlpha & GSIOAlpha::operator=(GSIOAlpha & A)
  {
  IONo         = A.IONo;
  Desc         = A.Desc;
  iOrder       = A.iOrder; //kga 31/8/00
  iPriority    = A.iPriority; //kga 31/8/00
  iQmMode      = A.iQmMode;
  iMkMode      = A.iMkMode;
  Qm           = A.Qm;
  QmErr        = A.QmErr;
  Qv           = A.Qv;
  QvErr        = A.QvErr;
  FracSplit    = A.FracSplit;
  DmndSplit    = A.DmndSplit;
  LSRatio      = A.LSRatio;
  Density      = A.Density;
  SFrac        = A.SFrac;
  LFrac        = A.LFrac;
  VFrac        = A.VFrac;
  m_SSplt      = A.m_SSplt;
  m_LSplt      = A.m_LSplt;
  m_VSplt      = A.m_VSplt;
  m_SQmRqd     = A.m_SQmRqd;
  m_LQmRqd     = A.m_LQmRqd;
  m_VQmRqd     = A.m_VQmRqd;
  m_SQvRqd     = A.m_SQvRqd;
  m_LQvRqd     = A.m_LQvRqd;
  m_VQvRqd     = A.m_VQvRqd;
  for (int i=0; i<MaxPhases; i++)
    {
    m_PhSplt[i] = A.m_PhSplt[i];
    m_PhQmRqd[i] = A.m_PhQmRqd[i];
    m_PhQvRqd[i] = A.m_PhQvRqd[i];
    }
  Alphas.SetSize(A.Alphas.GetSize());
  Splits.SetSize(A.Splits.GetSize());
  QmRqds.SetSize(A.QmRqds.GetSize());
  QvRqds.SetSize(A.QvRqds.GetSize());
  for (int i=0; i<Alphas.GetSize(); i++)
    {
    Alphas[i]=A.Alphas[i];
    Splits[i]=A.Splits[i];
    QmRqds[i]=A.QmRqds[i];
    QvRqds[i]=A.QvRqds[i];
    }

  return *this;
  }

//--------------------------------------------------------------------------

void GSIOAlpha::BuildDataDefnOrdPri(DataDefnBlk &DDB, rGSModel Mdl)
  {
  Strng StrVal;

  DDB.Byte   ("",             "Order",      DC_,    "",     &iOrder,          &Mdl, isResult|0);

  DDBValueLstMem DDBP;
  DDBP.Add(AutoPriority, "Auto");
  for (int i=0; i<Mdl.IOs.GetSize()-1; i++)
    {
    StrVal.Set("%i", i+1);
    DDBP.Add(i, StrVal());
    }
  DDB.Byte   ("",             "Priority",   DC_,    "",     &iPriority,       &Mdl, isParm, DDBP());
  }

//--------------------------------------------------------------------------

void GSIOAlpha::BuildDataDefn(DataDefnBlk & DDB, rGSModel Mdl, flag Visible, int IONo, flag LastOne)
  {
  Strng DefLbl;
  Strng StrVal;

  DDB.Visibility(SHM_All, Visible, Visible, Visible, Visible);

  BuildDataDefnOrdPri(DDB, Mdl);

  switch (Mdl.iQmMode)
    {
    case GSA_QmDemand     : DefLbl= "(Demand)"; break;
    case GSA_QmPercSplit  : DefLbl= "(PercSplit)"; break;
    case GSA_QmMassFlow   : DefLbl= "(MassFlow)"; break;
    case GSA_QmVolumeFlow : DefLbl= "(VolumeFlow)"; break;
    default               : DefLbl= "(Unknown)"; break;
    }
  DDBValueLst DDB0[]={
    {GSA_QmDefault   ,  DefLbl()},
    {GSA_QmDemand    ,  "Demand"},
    {GSA_QmPercSplit ,  "PercSplit"},
    {GSA_QmMassFlow  ,  "MassFlow"},
    {GSA_QmVolumeFlow,  "VolumeFlow"},
    {0}};

  if (strnicmp(Mdl.FullObjTag(), "x_22", 4)==0)
    { int xxx=0; }
  DDB.Byte   ("",             "QmMode",   DC_,    "",     &iQmMode,         &Mdl, isParmStopped | AffectsStruct | SetOnChange, DDB0);

  DDB.Visibility(SHM_All, Visible && ((QmMode(Mdl)==GSA_QmMassFlow)), Visible, Visible, Visible);
  DDB.Float ("",     "Qm",          DC_Qm,      "kg/s",   &Qm,              &Mdl,
    ((QmMode(Mdl)==GSA_QmMassFlow) && !LastOne ? isParm : 0));
  DDB.Float ("",     "QmErr",          DC_Qm,      "kg/s",   &QmErr,              &Mdl, isResult|0);

  DDB.Visibility(SHM_All, Visible && ((QmMode(Mdl)==GSA_QmVolumeFlow)), Visible, Visible, Visible);
  DDB.Float ("",     "Qv",          DC_Qv,      "L/s",   &Qv,              &Mdl,
    ((QmMode(Mdl)==GSA_QmVolumeFlow) && !LastOne ? isParm : 0));
  DDB.Float ("",     "QvErr",          DC_Qv,      "L/s",   &QvErr,              &Mdl, isResult|0);

  DDB.Visibility(SHM_All, Visible && (QmMode(Mdl)==GSA_QmPercSplit), Visible, Visible, Visible);
  DDB.Float ("",     "Split",       DC_Frac,    "%",      &FracSplit,       &Mdl,
    ((QmMode(Mdl)==GSA_QmPercSplit) && !LastOne ? isParm : 0));

  DDB.Visibility(SHM_All, Visible && (QmMode(Mdl)==GSA_QmDemand), Visible, Visible, Visible);
  DDB.Float ("",     "Demand",      DC_Frac,    "%",      &DmndSplit,        &Mdl, isResult|0);

  DDB.Visibility(SHM_All, Visible, Visible, Visible, Visible);
  switch (Mdl.iMkMode)
    {
    case GSA_MkMixture   : DefLbl= "(Mixture)";     break;
    case GSA_MkDensity   : DefLbl= "(Density)";     break;
    case GSA_MkLSRatio   : DefLbl= "(LSRatio)";     break;
    case GSA_MkPhaseComp : DefLbl= "(Composition)"; break;
    case GSA_MkSpecieFrac: DefLbl= "(SpecieFracs)"; break;
    default              : DefLbl= "(Unknown)";     break;
    }
  DDBValueLst DDB1[]={
    {GSA_MkDefault   ,  DefLbl()},
    {GSA_MkMixture   ,  "Mixture"},
    {GSA_MkLSRatio   ,  "LSRatio"},
    {GSA_MkPhaseComp ,  "PhaseFracs", MDD_Hidden},
    {GSA_MkPhaseComp ,  "PhaseComp",  MDD_Hidden},
    {GSA_MkPhaseComp ,  "Composition"},
    {GSA_MkSpecieFrac ,  "SpecieFracs"},
    {0}};
  DDB.Byte   ("",             "MkMode",   DC_,    "",     &iMkMode,         &Mdl, isParmStopped|AffectsStruct | SetOnChange, DDB1);

  DDB.Visibility(SHM_All, Visible && (MkMode(Mdl)==GSA_MkLSRatio), Visible, Visible, Visible);
  DDB.Float ("",     "LSRatio",     DC_Frac,    "%",      &LSRatio,         &Mdl, isParm);
  DDB.Visibility(SHM_All, Visible && (MkMode(Mdl)==GSA_MkDensity), Visible, Visible, Visible);
  DDB.Float ("",     "Density",     DC_Rho,    "kg/m^3",  &Density,         &Mdl, isParm);
  DDB.Visibility(SHM_All, Visible && (MkMode(Mdl)==GSA_MkPhaseComp), Visible, Visible, Visible);
  DDB.Float ("",     "SolidsFrac",  DC_Frac,    "%",      &SFrac,           &Mdl, isParm);
  DDB.Float ("",     "LiquidFrac",  DC_Frac,    "%",      &LFrac,           &Mdl, isParm);
  DDB.Float ("",     "VapourFrac",  DC_Frac,    "%",      &VFrac,           &Mdl, isParm);
  DDB.Visibility(SHM_All, Visible && (MkMode(Mdl)==GSA_MkSpecieFrac), Visible, Visible, Visible);
  if (PrjFileVerNo()<22)
    {
    DDB.Float ("",     "SolidsSplt",  DC_Frac,    "%",      &m_SSplt,           &Mdl, isParm);
    DDB.Float ("",     "LiquidSplt",  DC_Frac,    "%",      &m_LSplt,           &Mdl, isParm);
    DDB.Float ("",     "VapourSplt",  DC_Frac,    "%",      &m_VSplt,           &Mdl, isParm);
    }

  if (PrjFileVerNo()<22)
    {
    Strng Tag, Sym;
    for (int s=0; s<SDB.Count(); s++)
      {
      if (strlen(SDB[s].Tag()))
        Tag.Set("%s.Splt", SDB[s].Tag());
      else
        Tag="";
      if (strlen(SDB[s].Sym()))
        Sym.Set("%s.Splt", SDB[s].Sym());
      else
        Sym="";
      DDB.Float (Tag() ? Tag() : "",   Sym(),  DC_Frac,    "%",      xidGSMSpcSplts + s + SDB.Count()*IONo,  &Mdl, isParm|NAN_OK);
      }
    }
  else
    {
    if (DDB.BeginObject(&Mdl, "Splt", "GSM_Splt", NULL, DDB_NoPage))
      {
      DDB.Float ("",     "Solids",  DC_Frac,    "%",      &m_SSplt,           &Mdl, isParm);
      DDB.Float ("",     "Liquid",  DC_Frac,    "%",      &m_LSplt,           &Mdl, isParm);
      DDB.Float ("",     "Vapour",  DC_Frac,    "%",      &m_VSplt,           &Mdl, isParm);
      for (int s=0; s<SDB.Count(); s++)
        DDB.Float (SDB[s].Tag(), SDB[s].Sym(),  DC_Frac,    "%",      xidGSMSpcSplts + s + SDB.Count()*IONo,  &Mdl, isParm|NAN_OK);
      }
    DDB.EndObject();
    }
  DDB.Visibility(SHM_All, Visible, Visible, Visible, Visible);

  DDB.Text("");
  };

//--------------------------------------------------------------------------

flag GSIOAlpha::DataXchg(DataChangeBlk & DCB)
  {

  return 0;
  };

//--------------------------------------------------------------------------

inline void Test_0_1(float &D) { if (Valid(D)) D=Range(0.0f, D, 1.0f); else D=0.0f; };
inline void Test_Inv_0_1(float &D) { if (Valid(D)) D=Range(0.0f, D, 1.0f); };

flag GSIOAlpha::ValidateData(ValidateDataBlk & VDB, byte PhMethSpec)
  {
  flag OK=1;
  Test_Inv_0_1(m_SSplt);
  Test_Inv_0_1(m_LSplt);
  Test_Inv_0_1(m_VSplt);
  for (int i=0; i<MaxPhases; i++)
    Test_Inv_0_1(m_PhSplt[i]);
  int sn=Min(SDB.Count(), Splits.GetSize());
  for (int s=0; s<sn; s++)
    Test_Inv_0_1(Splits[s]);

  for (i=0; i<BOT_Last; i++)
    {
    m_bPhSpltHasValid[i]=false;
    m_bPhQmRqdHasValid[i]=false;
    m_bPhQvRqdHasValid[i]=false;
    }

  for (int i=0; i<MaxPhases; i++)
    {
    if (Valid(m_PhSplt[i]))
      {
      m_PhSplt[i]=Range(0.0f, m_PhSplt[i], 1.0f);
      m_bPhSpltHasValid[CDB.PhaseInfo(i).m_eOcc]=true;
      }
    if (Valid(m_PhQmRqd[i]))
      m_bPhQmRqdHasValid[CDB.PhaseInfo(i).m_eOcc]=true;
    if (Valid(m_PhQvRqd[i]))
      m_bPhQvRqdHasValid[CDB.PhaseInfo(i).m_eOcc]=true;
    }
  return OK;
  };

//============================================================================
//
//
//
//============================================================================
/*#D:#T:General Model
#X:#h<General Description>#nThis is a general purpose splitter model, which can be
used in a number of units, such as a tank, tie, Thickener, etc.  This model allows
the user to specify the manner in which the material splits as it flows out of the
unit.  A number of different split methods can be specified by the user.  These
methods will be detailed below.  However, the user must be careful when configuring
the split method for the outgoing streams to allow the flow from at least one stream
to be "free".  For example, if there are three outgoing streams, one stream can be
set to take 80% of the flow, the second 5% and the model will calculate the flow from
the third stream.  The model will not allow the user to configure all three streams,
as this will remove too many degrees of freedom from the solution.
#n#n
#h<Variables to be supplied by the user>#n
#i<Mode> : This should normally be in the #i<Off> state.#n
#i<QmMode> : The mass splitting mode.  There are three methods of splitting the mass:#n
1. Demand - In this method the material will split equally between the outgoing streams
if none of the streams are set in #i<MakeUp> mode.  If a stream is set in MakeUp mode,
the required mass flow will be sent to that stream first, and the remaining material will
be split between the other streams.#n
2. PercSplit - The first stream in the list will also default to percentage split.  The user
can set any of the outgoing streams to be PercSplit, but at least one stream must remain
configured for demand.  The user can then set a percentage of the incoming material which
must flow to the required stream.#n
3. Mass flow - In this method the user may specify the actual mass flow from individual
streams.#n
#n#i<MkMode> : The material split method.  This specifies the method of splitting the
material from the unit.  There are four methods of specifying the split:#n
1. Mixture - The unit mixes all of the incoming streams to produce a homegenous mixture.
This mix is then split between the outgoing streams as specified by the Qm method.#n
2. LSRatio - The user may configure the Liquid:Solid ratio in any of the outgoing streams.#n
3. PhaseFrac - The user can then set the percentages of solids, liquids and vapours flowing
from any of the streams.#n
4. SpecieFracs - The user can configure the percentage of each specie flowing out of a stream.
#n#n
#n#h<Other>#n
Short name:GenMdl#n
Model type:Block#n
#G:Common model blocks
*/

IMPLEMENT_TAGOBJ(GSModel, "GSM_Cfg", "GSM_Cfg", "", "", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "General Model", "General Model");

GSModel::GSModel(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, TagIn, pAttach, eAttach),
  Nd(*((MdlNode*)pAttach)),
  Gammas(*(new GSGammaArray)),
  IOAlpha(*(new GSIOAlphaArray)),
  IOs(*(new GSIOIndexArray))
  {
  iOperation=GSO_General0;
  m_iPhMethSpec=GSPM_Total;

  iGMode=GSG_Off;
  iQmMode=GSA_QmDemand;
  iMkMode=GSA_MkMixture;
  bIOChanges=1;
  bTrackStatus=1;
  IOId0=0;
  IOIdN=255;
  NProds=0;

  iRecPh=0;
  iRecStrm=0;
  dRecFrac=1.0;
  dRecComp=0.5;
  };

//--------------------------------------------------------------------------

GSModel::~GSModel()
  {
  delete &Gammas;
  delete &IOAlpha;
  delete &IOs;
  };

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

void GSModel::BuildDataDefnGen0(DataDefnBlk &DDB)
  {
  static DDBValueLst DDB1[]={
    {GSA_QmDemand,     "Demand"},
    {GSA_QmPercSplit,  "PercSplit"},
    {GSA_QmMassFlow ,  "MassFlow"},
    {GSA_QmVolumeFlow ,"VolumeFlow"},
    {0}};
  DDB.Byte   ("",     "QmMode",   DC_,    "",     &iQmMode,          this, isParmStopped | AffectsStruct | SetOnChange, DDB1);

  static DDBValueLst DDB2[]={
    {GSA_MkMixture  ,  "Mixture"},
    //{GSA_MkDensity  ,  "Density"},  Arithmetic Incomplete
    {GSA_MkLSRatio  ,  "LSRatio"},
    {GSA_MkPhaseComp , "PhaseFracs", MDD_Hidden},
    {GSA_MkPhaseComp , "PhaseComp",  MDD_Hidden},
    {GSA_MkPhaseComp , "Composition"},
    {0}};
  DDB.Byte   ("",     "MkMode",   DC_,    "",     &iMkMode,          this, isParmStopped | AffectsStruct | SetOnChange, DDB2);

  DDB.CheckBox("TrackStatus", "", DC_,    "",     &bTrackStatus,     this, isParm);

  int Priority[MaxIOList+1];
  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri=FindIOEvalPriorityGen0(Priority, QmMd, MkMd);
  int NOut=0;
  for (int i=0; i<IOs.GetSize(); i++)
    if (Nd.LnkFlowDefinedOut(IOs[i]))
      NOut++;

  if (NOut>1)
    {
    Strng S;
    if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
      {
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        {
//        IOAlpha[ia].iOrder=j+1;
//        IOAlpha[ia].Desc.Set("%s:%s", Nd.IODesc_Self(IOs[ia])->pName, Nd.Nd_Rmt(IOs[ia])->Tag());

        flag Visible=Nd.LnkFlowDefinedOut(IOs[ia]);
        if (DDB.BeginElement(this, Nd.Nd_Rmt(IOs[ia])->Tag(), Nd.IODesc_Self(IOs[ia])->pName))
          IOAlpha[ia].BuildDataDefn(DDB, *this, Visible, ia, (Priority[j+1]<0));
        }

      // Old Version - for compatibility
      if ((!DDB.ForFiling() && !DDB.ForSnapShot() && !DDB.ForScenario() && !DDB.ForView()) ||
          (DDB.ForFiling() && DDB.DoingPutData()))
        for (int i=0; i<IOs.GetSize(); i++)
          {
          byte Ord=1;
          for (int j = 0;(Priority[j]>=0) && (Priority[j]!=i); j++)
              Ord++;
          IOAlpha[i].iOrder=Ord;
          if (DDB.BeginElement(this, i, IOAlpha[i].Desc()))
            {
            flag Visible=Nd.LnkFlowDefinedOut(IOs[i]);
            IOAlpha[i].BuildDataDefn(DDB, *this, True, i, False);
            }
          }
      }
    DDB.EndArray();
    }

  };

//--------------------------------------------------------------------------

flag GSModel::DataXchgGen0(DataChangeBlk & DCB)
  {

  if (DCB.lHandle>=xidGSMSpcSplts &&  DCB.lHandle<xidGSMSpcSplts+SDB.Count()*IOs.GetSize())
    {
    int i=(DCB.lHandle-xidGSMSpcSplts)/SDB.Count();
    int s=(DCB.lHandle-xidGSMSpcSplts)%SDB.Count();

    if (DCB.rF)
      {
      int sz=IOAlpha[i].Splits.GetSize();
      if (s>=sz)
        {
        IOAlpha[i].Splits.SetSize(s+1);
        for (int j=sz; j<s; j++)
          IOAlpha[i].Splits[j]=fNAN;
        }
      if (Valid(*DCB.rF))
        IOAlpha[i].Splits[s]=Range(0.0f, *DCB.rF, 1.0f);
      else
        IOAlpha[i].Splits[s]=fNAN;
      }

    if (s<IOAlpha[i].Splits.GetSize())
      DCB.F=IOAlpha[i].Splits[s];
    else
      DCB.F=fNAN;

    return 1;
    }

  return 0;
  };

//--------------------------------------------------------------------------

flag GSModel::ValidateDataGen0(ValidateDataBlk & VDB)
  {
  flag OK=1;
  for (int i=0; i<IOAlpha.GetSize(); i++)
    OK =OK && IOAlpha[i].ValidateData(VDB, m_iPhMethSpec);

  // Sort IOs in order of priority
  int Priority[MaxIOList+1];
  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri=FindIOEvalPriorityGen0(Priority, QmMd, MkMd);

  int ia;
  double QmDemandEstTot=0.0;
  for (int j = 0;((ia=Priority[j])>=0); j++)
    if (QmMd[ia]==GSA_QmDemand)
      {
      double xxx=Nd.IOQmEst_Out(IOs[ia]);
      QmDemandEstTot+=Nd.IOQmEst_Out(IOs[ia]);
      }
  for (j = 0;((ia=Priority[j])>=0); j++)
    if (QmMd[ia]==GSA_QmDemand)
      {
      double xxx=Nd.IOQmEst_Out(IOs[ia]);
      IOAlpha[ia].DmndSplit=(float)Range((double)FLT_MIN, xxx/GTZ(QmDemandEstTot), (double)FLT_MAX);
      }

  double TotalSplit=0.0f;
  flag AllPercSplit=false;
  for (j = 0;((ia=Priority[j])>=0); j++)
    {
    int ianext=Priority[j+1];
    GSIOAlpha &A=IOAlpha[ia];
    switch (QmMd[ia])
      {
      case GSA_QmPercSplit:
        {
        if (j==0)
          AllPercSplit=true;
        if (!Valid(A.FracSplit))
          A.FracSplit=0.0f;
        if (ianext>=0)
          A.FracSplit=Range(0.0f , A.FracSplit, 1.0f-(float)TotalSplit);
        else if (AllPercSplit)
          A.FracSplit=1.0f-(float)TotalSplit;
        TotalSplit+=A.FracSplit;
        break;
        }
      default:
        AllPercSplit=false;
        break;
      }
    switch (MkMd[ia])
      {
      case GSA_MkPhaseComp :
        {
        if (!Valid(A.SFrac)) A.SFrac=0.0;
        if (!Valid(A.LFrac)) A.LFrac=0.0;
        if (!Valid(A.VFrac)) A.VFrac=0.0;
        double Tot=A.SFrac+A.LFrac+A.VFrac;
        if (Tot>1.0e-10)
          {
          double Scl=1.0/Tot;
          A.SFrac*=(float)Scl;
          A.LFrac*=(float)Scl;
          A.VFrac*=(float)Scl;
          }
        else
          {
          A.SFrac=1.0;
          A.LFrac=0.0;
          A.VFrac=0.0;
          }
        break;
        }
      default:;
      }
    }

  return OK;
  };

//--------------------------------------------------------------------------

int GSModel::FindIOEvalPriorityGen0(int *Priority, byte *QmMd, byte *MkMd)
  {
  const int IOCount=IOs.GetSize();
  int NOrd=0, Order[MaxIOList+1];
  int NPri=0;
  flag Done[MaxIOList];
  Priority[0]=-1;
  Order[0]=-1;
  Done[0]=0;

  for (int ia = 0; ia < IOCount; ia++)
    {
    int io=IOs[ia];
    GSIOAlpha &A=IOAlpha[ia];
    QmMd[ia] = A.QmMode(*this);
    MkMd[ia] = A.MkMode(*this);
    Order[ia]=-1;
    Priority[ia]=-1;
    if (Nd.LnkFlowDefinedOut(io))
      {
      Order[NOrd++]=ia;
      Done[ia]=0;
      }
    else
      Done[ia]=1;
    }
  Order[IOCount]=-1;
  Priority[IOCount]=-1;

  byte ThisPri=0;
  byte NextPri=0;
  while (NextPri!=AutoPriority)
    {
    ThisPri=NextPri;
    NextPri=AutoPriority;
    for (int j = 0;((ia=Order[j])>=0); j++)
      if (!Done[ia])
        {
        GSIOAlpha &A=IOAlpha[ia];
        if (A.iPriority==ThisPri)
          {
          Priority[NPri++]=ia;
          Done[ia]=1;
          }
        else if (A.iPriority>ThisPri)
          NextPri=Min(NextPri, A.iPriority);
        }
    }

  for (int j=0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=IOAlpha[ia];
      if ((MkMd[ia]==GSA_MkLSRatio) && Valid(A.LSRatio) ||
          (MkMd[ia]==GSA_MkDensity) && Valid(A.Density) ||
          (MkMd[ia]==GSA_MkPhaseComp) && (Valid(A.SFrac) && Valid(A.LFrac) && Valid(A.VFrac)) ||
          (MkMd[ia]==GSA_MkSpecieFrac))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmMassFlow) && Valid(A.Qm))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmVolumeFlow) && Valid(A.Qv))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmPercSplit) && Valid(A.FracSplit))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmDemand))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      Priority[NPri++]=ia;
      Done[ia]=1;
      }

  for (j = 0; j<NPri; j++)
    {
    int ia=Priority[j];
    //IOAlpha[Priority[j]].iOrder=j+1;
    IOAlpha[ia].iOrder=j+1;
    IOAlpha[ia].Desc.Set("%s:%s", Nd.IODesc_Self(IOs[ia])->pName, Nd.Nd_Rmt(IOs[ia])->Tag());
    }

  ASSERT(NPri==NOrd);
  return NPri;
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOutGen0(long RunMds, double Press, SpConduit &Sd)
  {
  double Temperature=Sd.Temp();
  int nShort=0;

  // Sort IOs in order of priority
  int Priority[MaxIOList+1];
  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri=FindIOEvalPriorityGen0(Priority, QmMd, MkMd);

  // Calculate total output flows in each Output
  double QmInTotal=Sd.QMass(som_ALL);
  double QmOutAvail=QmInTotal;
  double QmDemandTot=0.0;
  double QmDemandEstTot=0.0;
  flag MakeUpRqd=False;
  for (int ia, j = 0;((ia=Priority[j])>=0); j++)
    {
    //if (j<NPri-1)
    IOAlpha[ia].QmErr=0.0;
    IOAlpha[ia].QvErr=0.0;
    if (QmMd[ia]==GSA_QmDemand)
      {
      if (Nd.IOFlange(IOs[ia])->IsMakeUpReqd())
        {
        MakeUpRqd=True;
        QmDemandTot+=Nd.IOFlange(IOs[ia])->MakeUpReqd();
        }
      else
        QmDemandEstTot+=Nd.IOQmEst_Out(IOs[ia]);
      }
    }
  //if (dbg)
  //  {
  //  dbgpln("---------------------------------- %s", Nd.FullObjTag());
  //  dbgpln("%14g %14g %14g %14g ", QmInTotal, QmOutAvail, QmDemandEstTot, Temperature);
  //  }

  for (j = 0;((ia=Priority[j])>=0); j++)
    {
    int io=IOs[ia];
    rSpConduit Cn = *(Nd.IOConduit(io));
    if (j<NPri-1)
      {
      GSIOAlpha &A=IOAlpha[ia];
      flag SetMakeUpRqdLcl=False;
      double Qo;
      switch (QmMd[ia])
        {
        case GSA_QmDemand :
          SetMakeUpRqdLcl=MakeUpRqd;
          if (MakeUpRqd)
            {
            if (Nd.IOFlange(io)->IsMakeUpReqd())
              {
              Qo=Nd.IOFlange(io)->MakeUpReqd()*QmOutAvail/GTZ(QmDemandTot);
              SetMakeUpRqdLcl=False;
              }
            else
              Qo=0.0;
            }
          else
            Qo=Nd.IOQmEst_Out(io)*QmOutAvail/GTZ(QmDemandEstTot);
          A.DmndSplit=(float)(Qo/GTZ(QmInTotal));
          break;
        case GSA_QmPercSplit :
          if (Valid(A.FracSplit))
            {
            Qo=A.FracSplit*QmInTotal;
            SetMakeUpRqdLcl=MakeUpRqd;
            }
          break;
        case GSA_QmMassFlow  :
          if (Valid(A.Qm))
            {
            Qo=A.Qm;
            SetMakeUpRqdLcl=MakeUpRqd;
            }
          break;
        case GSA_QmVolumeFlow  :
          if (Valid(A.Qv))
            {
            Qo=A.Qv*Sd.Rho();
            SetMakeUpRqdLcl=MakeUpRqd;
            }
          break;
        default:
          Qo=0.0;
          break;
        }
      if (SetMakeUpRqdLcl)
        Nd.IOFlange(io)->SetMakeUpReqdLcl(Qo);
      //else
      //  Nd.IOFlange(io)->ClrMakeUpReqdLcl();

      // Input estimates Based on Sigma Output estimates
      //...
//2811      for (int i=0; i<IOCount; i++)
//2811        if (Nd.IO_In(i))
//2811          Nd.IOFlange(i)->SetPBFlwScl(Max(1.0e-6, Nd.IOConduit(i)->QMass()/GTZ(QmInTotal)));

      double QoReqd=Qo;
      Qo=Range(0.0, Qo, QmOutAvail);
      switch (MkMd[ia])
        {
        case GSA_MkMixture   :
          //Cn.QSetM(Sd, som_ALL, Max(0.5*UsableMass, Qo), Press); //why use 0.5*UsableMass when it should be zero?
          Cn.QSetM(Sd, som_ALL, Max(0.1*UsableMass, Qo), Press);
          //Cn.QSetM(Sd, som_ALL, Qo, Press); //Wouldn't this be best! ???
          break;
        case GSA_MkDensity   :
          Cn.QSetM(Sd, som_Liq, 0.0, Press);
          break;
        case GSA_MkLSRatio   :
          if (Valid(A.LSRatio))
            {
            // Check Availability
            double LFrac=A.LSRatio/(1.0+A.LSRatio);
            double LQoAv=Sd.QMass(som_Liq);
            double SQoAv=Sd.QMass(som_Sol);
            double LQo=Qo*LFrac;
            double SQo=Qo*(1.0-LFrac);
            if (LQoAv<LQo || SQoAv<SQo)
              Qo=Min(LQoAv/GTZ(LFrac), SQoAv/GTZ(1.0-LFrac));

            Cn.QSetM(Sd, som_Liq, Qo*LFrac, Press);
            Cn.QAddM(Sd, som_Sol, Qo*(1.0-LFrac));
            }
          else
            Cn.QSetM(Sd, som_ALL, 0.0, Press);
          break;
        case GSA_MkPhaseComp :
          if (Valid(A.SFrac) && Valid(A.LFrac) && Valid(A.VFrac))
            {
            double Scl=1.0/GTZ(A.SFrac+A.LFrac+A.VFrac);
            A.SFrac*=(float)Scl;
            A.LFrac*=(float)Scl;
            A.VFrac*=(float)Scl;

            double SQoAv=Sd.QMass(som_Sol);
            double LQoAv=Sd.QMass(som_Liq);
            double VQoAv=Sd.QMass(som_Gas);
            double SQo=Qo*A.SFrac;
            double LQo=Qo*A.LFrac;
            double VQo=Qo*A.VFrac;
            if (SQoAv<SQo)
              Qo=Min(Qo, SQoAv/GTZ(A.SFrac));
            if (LQoAv<LQo)
              Qo=Min(Qo, LQoAv/GTZ(A.LFrac));
            if (VQoAv<VQo)
              Qo=Min(Qo, VQoAv/GTZ(A.VFrac));

            Cn.QSetM(Sd, som_Sol, Qo*A.SFrac, Press);
            Cn.QAddM(Sd, som_Liq, Qo*A.LFrac);
            Cn.QAddM(Sd, som_Gas, Qo*A.VFrac);
            }
          else
            Cn.QSetM(Sd, som_ALL, 0.0, Press);
          break;
        case GSA_MkSpecieFrac :
          {
          Cn.QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'

          int sn=A.Splits.GetSize();
          for (int s=0; s<SDB.Count(); s++)
            {
            double Splt=dNAN;
            if (s<sn && Valid(A.Splits[s]))
              Splt=A.Splits[s];
            else if (SDB[s].IsSol() )
              Splt=A.m_SSplt;
            else if (SDB[s].IsLiq())
              Splt=A.m_LSplt;
            else if (SDB[s].IsGas())
              Splt=A.m_VSplt;
            else
              Splt=0.0;
            Splt=Range(0.0, (Valid(Splt) ? Splt : 0.0), 1.0);
            Cn.SetVMass(s, Sd, Splt*Sd.VMass[s]);
            }
          }
          break;
        default              :
          Cn.QSetM(Sd, som_ALL, Max(0.5*UsableMass, 0.0), Press);
          break;
        }
      Cn.SetTemp(Temperature);

      const double QMassUsed = Cn.QMass(som_ALL);
      A.QmErr=(float)(QoReqd-QMassUsed);
      A.QvErr=(float)(A.QmErr/GTZ(Cn.Rho()));
      if (bTrackStatus && fabs(A.QmErr)>1.0e-6)
        nShort++;
      QmOutAvail-=QMassUsed;
      if (QmMd[ia]==GSA_QmDemand)
        {
        if (MakeUpRqd)
          {
          if (Nd.IOFlange(io)->IsMakeUpReqd())
            QmDemandTot-=Nd.IOFlange(io)->MakeUpReqd();
          }
        else
          QmDemandEstTot-=Nd.IOQmEst_Out(io);
        }

      Sd.QSubM(Cn, som_ALL, Cn.QMass());
      // Check That Species dont go negative
      for (int s=0; s<SDB.Count(); s++)
        if (Sd.VMass[s]<0.0)
          Sd.VValue[s]=0.0;

      Sd.SetTemp(Temperature);
      }
    else
      {
      // The Last Output Must take What is Left
      Cn.QCopy(Sd);
      Cn.SetTempPress(Temperature, Press);

      switch (QmMd[ia])
        {
        case GSA_QmDemand :
          {
          GSIOAlpha &A=IOAlpha[ia];
          double Qo=Cn.QMass();
          A.DmndSplit=(float)(Qo/GTZ(QmInTotal));
          break;
          }
        }

      }
    }
  if (bTrackStatus && nShort)
    SetCI(1, "W\tRequirements not met in %i stream%s", nShort, nShort>1 ? "s":"");
  else
    ClrCI(1);
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

void GSModel::BuildDataDefnV5(DataDefnBlk &DDB)
  {
  };

//--------------------------------------------------------------------------

flag GSModel::DataXchgV5(DataChangeBlk & DCB)
  {

  return 0;
  };

//--------------------------------------------------------------------------

flag GSModel::ValidateDataV5(ValidateDataBlk & VDB)
  {
  flag OK=1;

  return OK;
  };

//--------------------------------------------------------------------------

int GSModel::FindIOEvalPriorityV5(int *Priority, byte *QmMd, byte *MkMd)
  {
  int NPri=0;
  Priority[0]=-1;
  return NPri;
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOutV5(long RunMds, double Press, SpConduit &Sd)
  {
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------


void GSModel::BuildDataDefnSplit(DataDefnBlk &DDB)
  {
  Strng T, Tt, Ts;
  int j;
  int I[MaxIOList+1];
  GSIOAlpha * A[MaxIOList+1];
  int NPri=FindIOEvalPrioritySplit(I, A);

  static DDBValueLst DDBPhMeth[]=
    {
      {GSPM_Total,        "Total"},
      {GSPM_Individual,   "Individual"},
      {GSPM_Both,         "Both"},
      {0}
    };

  // Calculate total output flows in each Output
  switch (iOperation)
    {
    case GSO_MassFracSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          DDB.Float ("", "Split", DC_Frac, "%", &A[j]->FracSplit, this, j<NPri-1 ? isParm : 0);
          }
      DDB.EndArray();
      break;
    case GSO_MassFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            DDB.Float ("", "Flow", DC_Qm, "kg/s", &A[j]->Qm, this, j<NPri-1 ? isParm : 0);
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Float ("", "QmErr", DC_Qm,  "kg/s", &A[j]->QmErr, this, isResult);
          }
      DDB.EndArray();
      break;
     case GSO_VolumeFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            DDB.Float ("", "VFlow", DC_Qv, "L/s", &A[j]->Qv, this, j<NPri-1 ? isParm : 0);
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Float ("", "QvErr", DC_Qv,  "L/s", &A[j]->QvErr, this, isResult);
          }
      DDB.EndArray();
      break;
    case GSO_PhMassFracSplit:
//      DDB.Byte   ("", "PhaseSpec",   DC_, "", &m_iPhMethSpec, this, isParmStopped | AffectsStruct | SetOnChange, DDBPhMeth);
      DDB.Byte   ("", "PhaseSpec",   DC_, "", xidGSPhMethSpec, this, isParmStopped | AffectsStruct | SetOnChange, DDBPhMeth);
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (PrjFileVerNo()<22)
            {
            DDB.Float ("Solids.Splt", "",  DC_Frac, "%",  &A[j]->m_SSplt,  this, j<NPri-1 ? isParm : 0);
            DDB.Float ("Liquids.Splt","",  DC_Frac, "%",  &A[j]->m_LSplt,  this, j<NPri-1 ? isParm : 0);
            DDB.Float ("Gasses.Splt", "",  DC_Frac, "%",  &A[j]->m_VSplt,  this, j<NPri-1 ? isParm : 0);
            }
          else
            {
            //TODO Fix problem in project recovery in array with object for 'numeric' tags
            //KGA 6/6/2002 : needs fixing??? or disalow numeric named tags???
            if (1)
              {//this code does NOT work for numeric tag pipe eg "X_1.GM.IOs.[2].Splt.Solids (%)"...
              if (DDB.BeginObject(this, "Splt", "GSM_Splt", NULL, DDB_NoPage))
                {
                for (int c=BOT_Solid; c<=BOT_Gas; c++)
                  {
                  static const LPSTR Nm[]={"Solids","Liquids",/*"Aqueous",*/"Gasses"};
                  if (m_iPhMethSpec&GSPM_Total)
                    DDB.Float (Nm[c], "",  DC_Frac, "%",  &A[j]->m_Splt[c],  this, j<NPri-1 ? isParm : 0);
                  if (m_iPhMethSpec&GSPM_Individual)
                    {
                    for (int p=0; p<CDB.PhaseCount(); p++)
                      {
                      CPhaseInfo &P=CDB.PhaseInfo(p);
                      if (P.m_eOcc==c)
                        DDB.Float (P.m_Tag(), P.m_Sym(),  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                        (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                      }
                    }
                  }
                //
                //if (m_iPhMethSpec&GSPM_Total)
                //  DDB.Float ("Solids", "",  DC_Frac, "%",  &A[j]->m_SSplt,  this, j<NPri-1 ? isParm : 0);
                //if (m_iPhMethSpec&GSPM_Individual)
                //  {
                //  for (int p=0; p<CDB.PhaseCount(); p++)
                //    {
                //    CPhaseInfo &P=CDB.PhaseInfo(p);
                //    if (P.m_eOcc==BOT_Solid)
                //      DDB.Float (P.m_Nm(), "",  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                //      (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                //    }
                //  }

                //if (m_iPhMethSpec&GSPM_Total)
                //  DDB.Float ("Liquids","",  DC_Frac, "%",  &A[j]->m_LSplt,  this, j<NPri-1 ? isParm : 0);
                //if (m_iPhMethSpec&GSPM_Individual)
                //  {
                //  for (int p=0; p<CDB.PhaseCount(); p++)
                //    {
                //    CPhaseInfo &P=CDB.PhaseInfo(p);
                //    if (P.m_eOcc==BOT_Liquid)
                //      DDB.Float (P.m_Nm(), "",  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                //      (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                //    }
                //  }
                //
                //if (m_iPhMethSpec&GSPM_Total)
                //  DDB.Float ("Gasses", "",  DC_Frac, "%",  &A[j]->m_VSplt,  this, j<NPri-1 ? isParm : 0);
                //if (m_iPhMethSpec&GSPM_Individual)
                //  {
                //  for (int p=0; p<CDB.PhaseCount(); p++)
                //    {
                //    CPhaseInfo &P=CDB.PhaseInfo(p);
                //    if (P.m_eOcc==BOT_Gas)
                //      DDB.Float (P.m_Nm(), "",  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                //      (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                //    }
                //  }
                }
              DDB.EndObject();
              }
            else
              {//this code DOES work for numeric tag pipe eg "X_1.GM.IOs.[2].Splt.Solids (%)"...
              DDB.Float ("Splt.Solids", "",  DC_Frac, "%",  &A[j]->m_SSplt,  this, j<NPri-1 ? isParm : 0);
              DDB.Float ("Splt.Liquids","",  DC_Frac, "%",  &A[j]->m_LSplt,  this, j<NPri-1 ? isParm : 0);
              DDB.Float ("Splt.Gasses", "",  DC_Frac, "%",  &A[j]->m_VSplt,  this, j<NPri-1 ? isParm : 0);
              }
            }
          }
      DDB.EndArray();
      break;
    case GSO_PhMassFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            {
            if (PrjFileVerNo()<22)
              {
              DDB.Float ("Solids.QmRqd", "",  DC_Qm, "kg/s",  &A[j]->m_SQmRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Float ("Liquids.QmRqd","",  DC_Qm, "kg/s",  &A[j]->m_LQmRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Float ("Gasses.QmRqd", "",  DC_Qm, "kg/s",  &A[j]->m_VQmRqd,  this, j<NPri-1 ? isParm : 0);
              }
            else
              {
              if (DDB.BeginObject(this, "QmRqd", "GSM_QmRqd", NULL, DDB_NoPage))
                {
                DDB.Float ("Solids", "",  DC_Qm, "kg/s",  &A[j]->m_SQmRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Float ("Liquids","",  DC_Qm, "kg/s",  &A[j]->m_LQmRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Float ("Gasses", "",  DC_Qm, "kg/s",  &A[j]->m_VQmRqd,  this, j<NPri-1 ? isParm : 0);
                }
              DDB.EndObject();
              }
            }
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Float ("", "QmErr", DC_Qm,  "kg/s", &A[j]->QmErr, this, isResult|0);
          }
      DDB.EndArray();
      break;
    case GSO_PhVolumeFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            {
            if (PrjFileVerNo()<22)
              {
              DDB.Float ("Solids.QvRqd", "",  DC_Qv, "L/s",  &A[j]->m_SQvRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Float ("Liquids.QvRqd","",  DC_Qv, "L/s",  &A[j]->m_LQvRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Float ("Gasses.QvRqd", "",  DC_Qv, "L/s",  &A[j]->m_VQvRqd,  this, j<NPri-1 ? isParm : 0);
              }
            else
              {
              if (DDB.BeginObject(this, "QmRqd", "GSM_QmRqd", NULL, DDB_NoPage))
                {
                DDB.Float ("Solids", "",  DC_Qv, "L/s",  &A[j]->m_SQvRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Float ("Liquids","",  DC_Qv, "L/s",  &A[j]->m_LQvRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Float ("Gasses", "",  DC_Qv, "L/s",  &A[j]->m_VQvRqd,  this, j<NPri-1 ? isParm : 0);
                }
              DDB.EndObject();
              }
            }
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Float ("", "QvErr", DC_Qv,  "L/s", &A[j]->QvErr, this, isResult|0);
          }
      DDB.EndArray();
      break;
    case GSO_SpMassFracSplit:
      {
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          int ns=A[j]->Splits.GetSize();
          if (PrjFileVerNo()<22)
            {
            for (int s=0; s<ns; s++)
              {
              Tt.Set("%s.Splt", SDB[s].Tag());
              Ts.Set("%s.Splt", SDB[s].Sym());
              DDB.Float (Tt(), Ts(),  DC_Frac, "%",  &A[j]->Splits[s],  this, j<NPri-1 ? isParm : 0);
              }
            }
          else
            {
            if (DDB.BeginObject(this, "Splt", "GSM_Splt", NULL, DDB_NoPage))
              {
              for (int s=0; s<ns; s++)
                DDB.Float (SDB[s].Tag(), SDB[s].Sym(),  DC_Frac, "%",  &A[j]->Splits[s],  this, j<NPri-1 ? isParm : 0);
              }
            DDB.EndObject();
            }
          }
      DDB.EndArray();
      }
      break;
    case GSO_SpMassFlowSplit:
      {
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
      for (j = 0;j<NPri; j++)
        {
        if (j>0)
          DDB.Page("..", DDB_OptPage);
        DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
        A[j]->BuildDataDefnOrdPri(DDB, *this);
        if (j<NPri-1 || !DDB.ForView())
          {
          int ns=A[j]->QmRqds.GetSize();
          if (PrjFileVerNo()<22)
            {
            for (int s=0; s<ns; s++)
              {
              Tt.Set("%s.QmRqd", SDB[s].Tag());
              Ts.Set("%s.QmRqd", SDB[s].Sym());
              DDB.Float (Tt(), Ts(),  DC_Qm, "kg/s",  &A[j]->QmRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            }
          else
            {
            if (DDB.BeginObject(this, "QmRqd", "GSM_QmRqd", NULL, DDB_NoPage))
              {
              for (int s=0; s<ns; s++)
                DDB.Float (SDB[s].Tag(), SDB[s].Sym(),  DC_Qm, "kg/s",  &A[j]->QmRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            DDB.EndObject();
            }
          }
        //else if (j==(NPri-1))
        if (j<(NPri-1))
          DDB.Float ("", "QmErr", DC_Qm,  "kg/s", &A[j]->QmErr, this, isResult|0);
        }
      DDB.EndArray();
      }
      break;
    case GSO_SpVolumeFlowSplit:
      {
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", IOs.GetSize(), 0))
      for (j = 0;j<NPri; j++)
        {
        if (j>0)
          DDB.Page("..", DDB_OptPage);
        DDB.BeginElement(this, Nd.Nd_Rmt(I[j])->Tag(), Nd.IODesc_Self(I[j])->pName);
        A[j]->BuildDataDefnOrdPri(DDB, *this);
        if (j<NPri-1 || !DDB.ForView())
          {
          int ns=A[j]->QvRqds.GetSize();
          if (PrjFileVerNo()<22)
            {
            for (int s=0; s<ns; s++)
              {
              Tt.Set("%s.QvRqd", SDB[s].Tag());
              Ts.Set("%s.QvRqd", SDB[s].Sym());
              DDB.Float (Tt(), Ts(),  DC_Qv, "L/s",  &A[j]->QvRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            }
          else
            {
            if (DDB.BeginObject(this, "QvRqd", "GSM_QvRqd", NULL, DDB_NoPage))
              {
              for (int s=0; s<ns; s++)
                DDB.Float (SDB[s].Tag(), SDB[s].Sym(),  DC_Qv, "L/s",  &A[j]->QvRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            DDB.EndObject();
            }
          }
        //else if (j==(NPri-1))
        if (j<(NPri-1))
          DDB.Float ("", "QvErr", DC_Qv,  "L/s", &A[j]->QvErr, this, isResult|0);
        }
      DDB.EndArray();
      }
      break;
    case GSO_PhaseRecovery:
      static DDBValueLst DDBPh[]={
        {0,  "Solid"},
        {1,  "Liquid"},
        {2,  "Vapour"},
        {0}};
      static DDBValueLst DDBSt[]={
        {0,  ""},
        {1,  ""},
        {0}};
      if (NPri>=1) DDBSt[0].m_pStr=Nd.Nd_Rmt(I[0])->Tag();
      if (NPri>=2) DDBSt[1].m_pStr=Nd.Nd_Rmt(I[1])->Tag();

      if (DDB.BeginStruct(this, "Recovery", NULL, DDB_NoPage))
        {
        DDB.Byte  ("Phase",       "",  DC_,     "",  &iRecPh,   this, isParm, DDBPh);
        DDB.Byte  ("Stream",      "",  DC_,     "",  &iRecStrm, this, isParm, DDBSt);
        DDB.Float ("Fraction",    "",  DC_Frac, "%", &dRecFrac, this, isParm);
        DDB.Float ("Composition", "",  DC_Frac, "%", &dRecComp, this, isParm);
        }
      DDB.EndStruct();
      break;
    }
  };

//--------------------------------------------------------------------------

flag GSModel::DataXchgSplit(DataChangeBlk & DCB)
  {
  return 0;
  };

//--------------------------------------------------------------------------

flag GSModel::ValidateDataSplit(ValidateDataBlk & VDB)
  {
  flag OK=1;
  int j;
  int I[MaxIOList+1];
  GSIOAlpha * A[MaxIOList+1];
  int NPri=FindIOEvalPrioritySplit(I, A);//, QmMd, MkMd);
  SetCI(3, NPri<1);
  if (NPri<1)
    return false;

  // Calculate total output flows in each Output
  switch (iOperation)
    {
    case GSO_MassFracSplit:
      {
      double Tot=0.0;
      for (j = 0;j<NPri-1; j++)
        {
        float &F=A[j]->FracSplit;
        F=Range(0.0f, F, 1.0f);
        F=Min(F, 1.0f-(float)Tot);
        Tot+=F;
        }
      A[NPri-1]->FracSplit=1.0f-(float)Tot;
      }
      break;
    case GSO_MassFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        {
        float &F=A[j]->Qm;
        F=Valid(F) ? Max(0.0f, F) : 0.0f;
        }
      }
      break;
    case GSO_VolumeFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        {
        float &F=A[j]->Qv;
        F=Valid(F) ? Max(0.0f, F) : 0.0f;
        }
      }
      break;
    case GSO_PhMassFracSplit:
      {
      double STot=0.0;
      double LTot=0.0;
      double VTot=0.0;
      double PhTot[MaxPhases]={0.0,0.0};
      for (j = 0;j<NPri-1; j++)
        {
        A[j]->ValidateData(VDB, m_iPhMethSpec);
        float &SF=A[j]->m_SSplt;
        SF=Range(0.0f, SF, 1.0f);
        SF=Min(SF, 1.0f-(float)STot);
        STot+=SF;
        float &LF=A[j]->m_LSplt;
        LF=Range(0.0f, LF, 1.0f);
        LF=Min(LF, 1.0f-(float)LTot);
        LTot+=LF;
        float &VF=A[j]->m_VSplt;
        VF=Range(0.0f, VF, 1.0f);
        VF=Min(VF, 1.0f-(float)VTot);
        VTot+=VF;
        for (int i=0; i<CDB.PhaseCount(); i++)
          {
          if (m_iPhMethSpec==GSPM_Individual)
            if (!Valid(A[j]->m_PhSplt[i]))
              A[j]->m_PhSplt[i]=0;

          if (Valid(A[j]->m_PhSplt[i]))
            {
            float &F=A[j]->m_PhSplt[i];
            //F=Range(0.0f, F, 1.0f);
            F=Range(0.0f, F, 1.0f-(float)PhTot[i]);
            PhTot[i]+=F;
            }
          else if (m_iPhMethSpec & GSPM_Total)
            PhTot[i]+=A[j]->m_Splt[CDB.PhaseInfo(i).m_eOcc];
          else
            PhTot[i]+=0.0f;
          }
        }
      A[NPri-1]->m_SSplt=(float)(1.0-STot);
      A[NPri-1]->m_LSplt=(float)(1.0-LTot);
      A[NPri-1]->m_VSplt=(float)(1.0-VTot);
      for (int i=0; i<CDB.PhaseCount(); i++)
        A[NPri-1]->m_PhSplt[i]=(float)GEZ(1.0-PhTot[i]); // fix trunc/round errors
      }
      break;
    case GSO_PhMassFlowSplit:
      for (j = 0;j<NPri; j++)
        {
        A[j]->ValidateData(VDB, m_iPhMethSpec);
        float &SF=A[j]->m_SQmRqd;
        SF=Valid(SF) ? Max(0.0f, SF) : 0.0f;
        float &LF=A[j]->m_LQmRqd;
        LF=Valid(LF) ? Max(0.0f, LF) : 0.0f;
        float &VF=A[j]->m_VQmRqd;
        VF=Valid(VF) ? Max(0.0f, VF) : 0.0f;
        }
      break;
    case GSO_PhVolumeFlowSplit:
      for (j = 0;j<NPri; j++)
        {
        A[j]->ValidateData(VDB, m_iPhMethSpec);
        float &SF=A[j]->m_SQvRqd;
        SF=Valid(SF) ? Max(0.0f, SF) : 0.0f;
        float &LF=A[j]->m_LQvRqd;
        LF=Valid(LF) ? Max(0.0f, LF) : 0.0f;
        float &VF=A[j]->m_VQvRqd;
        VF=Valid(VF) ? Max(0.0f, VF) : 0.0f;
        }
      break;
    case GSO_SpMassFracSplit:
      {
      for (j = 0;j<NPri; j++)
        A[j]->Splits.SetSize(SDB.Count());
      for (int s=0; s<SDB.Count(); s++)
        {
        double Tot=0.0;
        for (j = 0;j<NPri-1; j++)
          {
          float &F=A[j]->Splits[s];
          F=Valid(F) ? Range(0.0f, F, 1.0f) : 0.0f;
          F=Min(F, 1.0f-(float)Tot);
          Tot+=F;
          }
        A[NPri-1]->Splits[s]=(float)(1.0-Tot);
        }
      }
      break;
    case GSO_SpMassFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        A[j]->QmRqds.SetSize(SDB.Count());
      for (int s=0; s<SDB.Count(); s++)
        {
        for (j = 0;j<NPri; j++)
          {
          float &F=A[j]->QmRqds[s];
          F=Valid(F) ? Max(0.0f, F) : 0.0f;
          }
        }
      }
      break;
    case GSO_SpVolumeFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        A[j]->QvRqds.SetSize(SDB.Count());
      for (int s=0; s<SDB.Count(); s++)
        {
        for (j = 0;j<NPri; j++)
          {
          float &F=A[j]->QvRqds[s];
          F=Valid(F) ? Max(0.0f, F) : 0.0f;
          }
        }
      }
      break;
    case GSO_PhaseRecovery:
      iRecPh=Range((byte)0, iRecPh, (byte)2);
      iRecStrm=Range((byte)0, iRecStrm, (byte)1);
      dRecFrac=Range(0.0f, dRecFrac, 1.0f);
      dRecComp=Range(0.0f, dRecComp, 1.0f);
      break;
    }
  return OK;
  };

//--------------------------------------------------------------------------

int GSModel::FindIOEvalPrioritySplit(int *I, GSIOAlpha **A)
  {
  const int IOCount=IOs.GetSize();
//  int NOrd=0, Order[MaxIOList+1];
//  int NPri=0;
  bool Done[MaxIOList];
//  Iriority[0]=-1;
//  Order[0]=-1;
//  Done[0]=0;

  for (int ia = 0; ia < IOCount; ia++)
//    {
//    int io=IOs[ia];
//    GSIOAlpha &A=IOAlpha[ia];
//    //QmMd[ia] = A.QmMode(*this);
//    //MkMd[ia] = A.MkMode(*this);
//    Order[ia]=-1;
//    Priority[ia]=-1;
//    if (Nd.LnkFlowDefinedOut(io))
//      {
//      Order[NOrd++]=ia;
      Done[ia]=false;
//      }
//    else
//      Done[ia]=1;
//    }
//  Order[IOCount]=-1;
//  Priority[IOCount]=-1;


  int NOrd=0;


  byte ThisPri=0;
  byte NextPri=0;
  while (NextPri!=AutoPriority)
    {
    ThisPri=NextPri;
    NextPri=AutoPriority;
    for (int ia = 0; ia < IOCount; ia++)
      if (!Done[ia] && Nd.LnkFlowDefinedOut(IOs[ia]))
  //  for (int j = 0;((ia=Order[j])>=0); j++)
  //    if (!Done[ia])
        {
        //GSIOAlpha &A=IOAlpha[ia];
        if (IOAlpha[ia].iPriority==ThisPri)
          {
          I[NOrd]=ia;
          A[NOrd]=&IOAlpha[ia];
          NOrd++;
//          Priority[NPri++]=ia;
          Done[ia]=true;
          }
        else if (IOAlpha[ia].iPriority>ThisPri)
          NextPri=Min(NextPri, IOAlpha[ia].iPriority);
        }
    }


  for (ia = 0; ia < IOCount; ia++)
    {
    if (!Done[ia] && Nd.LnkFlowDefinedOut(IOs[ia]))
      {
      I[NOrd]=ia;
      A[NOrd]=&IOAlpha[ia];
      NOrd++;
      Done[ia]=true;
      }
    }

  for (int j = 0; j<NOrd; j++)
    {
    int ia=I[j];
    //IOAlpha[Priority[j]].iOrder=j+1;
    A[j]->iOrder=j+1;
    A[j]->Desc.Set("%s:%s", Nd.IODesc_Self(IOs[ia])->pName, Nd.Nd_Rmt(IOs[ia])->Tag());
    }

  return NOrd;
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOutSplit(long RunMds, double Press, SpConduit &Sd)
  {
  int I[MaxIOList+1];
  GSIOAlpha * A[MaxIOList+1];
  int NPri=FindIOEvalPrioritySplit(I, A);//, QmMd, MkMd);

  if (NPri<1)
    return;

  // Calculate total output flows in each Output
  double Temperature=Sd.Temp();
  double QmInTotal=Sd.QMass(som_ALL);
  double QmOutAvail=QmInTotal;
  int nShort=0;

  switch (iOperation)
    {
    case GSO_MassFracSplit:
      {
      for (int j = 0;j<NPri; j++)
        Nd.IOConduit(I[j])->QSetF(Sd, som_ALL, A[j]->FracSplit, Sd.Press());
      }
      break;
    case GSO_MassFlowSplit:
      {
      double TotAv=Sd.QMass();
      double TotRqd=TotAv;
      for (int j=0; j<NPri-1; j++)
        {
        SpConduit & Cd=*Nd.IOConduit(I[j]);
        double F=A[j]->Qm;
        Cd.QSetM(Sd, som_ALL, Min(TotAv, F), Sd.Press());
        A[j]->QmErr=(float)(Cd.QMass()-F);
        if (fabs(A[j]->QmErr)>1.0e-6)
          nShort++;
        TotAv=GEZ(TotAv-F);
        TotRqd=TotRqd-F;
        }
      Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, TotAv, Sd.Press());
      A[j]->QmErr=(float)(TotRqd-TotAv);
      //if (fabs(A[j]->QmErr)>1.0e-6)
      //  nShort++;
      }
      break;
    case GSO_VolumeFlowSplit:
      {
      double Dens=Sd.Rho();
      double TotAv=Sd.QMass();
      double TotRqd=TotAv;
      for (int j=0; j<NPri-1; j++)
        {
        SpConduit & Cd=*Nd.IOConduit(I[j]);
        double F=A[j]->Qv*Dens;
        Cd.QSetM(Sd, som_ALL, Min(TotAv, F), Sd.Press());
        A[j]->QvErr=(float)(Cd.QVolume()-A[j]->Qv);
        if (fabs(A[j]->QvErr)>1.0e-6)
          nShort++;
        TotAv=GEZ(TotAv-F);
        TotRqd=TotRqd-F;
        }
      Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, TotAv, Sd.Press());
      A[j]->QvErr=(float)((TotRqd-TotAv)/GTZ(Dens));
      //if (fabs(A[j]->QmErr)>1.0e-6)
      //  nShort++;
      }
      break;
    case GSO_PhMassFracSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        SpConduit &Cd=*Nd.IOConduit(I[j]);
        bool SetReqd=true;
        for (int c=BOT_Solid; c<=BOT_Gas; c++)
          {
          //dbgpln("===================== %i",c);
          if ((m_iPhMethSpec&GSPM_Individual))// && A[j]->m_bPhSpltHasValid[c])
            {
            int i1=CDB.PhaseFirst(c);
            int i2=CDB.PhaseLast(c);
            if (SetReqd && i2>=i1)
              {
              double Splt=Valid(A[j]->m_PhSplt[i1]) ? A[j]->m_PhSplt[i1] : A[j]->m_Splt[c];
              Splt=Range(0.0, Splt, 1.0);
              Cd.QSetF(Sd, CDB.PhaseInfo(i1).m_PhMsk, Splt, Sd.Press());
              //dbgpln("ISet  %2i %04x %-5.5s %10.4f %10.4f", i1, CDB.PhaseInfo(i1).m_PhMsk, CDB.PhaseInfo(i1).m_Sym(), Splt, Cd.QMass());
              //dbgpln("=====================");
              SetReqd=false;
              i1++;
              }
            if ((m_iPhMethSpec&GSPM_Total)==0)
              {
              for (int i=i1; i<=i2; i++)
                {
                double Splt=Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : 0;
                Splt=Range(0.0, Splt, 1.0);
                Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Splt);
                //dbgpln("IAddI %2i %04x %-5.5s %10.4f %10.4f ", i, CDB.PhaseInfo(i).m_PhMsk, CDB.PhaseInfo(i).m_Sym(), Splt, Cd.QMass());
                //dbgpln("=====================");
                }
              }
            else
              {
              for (int i=i1; i<=i2; i++)
                {
                double Splt=Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : A[j]->m_Splt[c];
                Splt=Range(0.0, Splt, 1.0);
                Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Splt);
                //dbgpln("IAddT %2i %04x %-5.5s %10.4f %10.4f", i, CDB.PhaseInfo(i).m_PhMsk, CDB.PhaseInfo(i).m_Sym(), Splt, Cd.QMass());
                //dbgpln("=====================");
                }
              }
            }
          else
            {
            static PhMask Which[] = {som_Sol, som_Liq, som_Gas};
            if (SetReqd)
              {
              Cd.QSetF(Sd, Which[c], Range(0.0f, A[j]->m_Splt[c], 1.0f), Sd.Press());
              SetReqd=false;
              }
            else
              Cd.QAddF(Sd, Which[c], Range(0.0f, A[j]->m_Splt[c], 1.0f));
            }
          }

        //if ((m_iPhMethSpec&GSPM_Individual) && A[j]->m_bPhSpltHasValid[BOT_Solid])
        //  {
        //  int i1=CDB.PhaseFirst(BOT_Solid);
        //  int i2=CDB.PhaseLast(BOT_Solid);
        //  Cd.QSetF(Sd, CDB.PhaseInfo(i1).m_PhMsk, Valid(A[j]->m_PhSplt[i1]) ? A[j]->m_PhSplt[i1] : A[j]->m_SSplt, Sd.Press());
        //  if ((m_iPhMethSpec&GSPM_Total)==0)
        //    {
        //    for (int i=i1+1; i<=i2; i++)
        //      Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : 0);
        //    }
        //  else
        //    {
        //    for (int i=i1+1; i<=i2; i++)
        //      Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : A[j]->m_SSplt);
        //    }
        //  }
        //else
        //  Cd.QSetF(Sd, som_Sol, A[j]->m_SSplt, Sd.Press());

        //if ((m_iPhMethSpec&GSPM_Individual) && A[j]->m_bPhSpltHasValid[BOT_Liquid])
        //  {
        //  int i1=CDB.PhaseFirst(BOT_Liquid);
        //  int i2=CDB.PhaseLast(BOT_Liquid);
        //  if ((m_iPhMethSpec&GSPM_Total)==0)
        //    {
        //    for (int i=i1; i<=i2; i++)
        //      Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : 0);
        //    }
        //  else
        //    {
        //    for (int i=i1; i<=i2; i++)
        //      Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : A[j]->m_LSplt);
        //    }
        //  }
        //else
        //  Cd.QAddF(Sd, som_Liq, A[j]->m_LSplt);

        //if ((m_iPhMethSpec&GSPM_Individual) && A[j]->m_bPhSpltHasValid[BOT_Gas])
        //  {
        //  int i1=CDB.PhaseFirst(BOT_Gas);
        //  int i2=CDB.PhaseLast(BOT_Gas);
        //  if ((m_iPhMethSpec&GSPM_Total)==0)
        //    {
        //    for (int i=i1; i<=i2; i++)
        //      Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : 0);
        //    }
        //  else
        //    {
        //    for (int i=i1; i<=i2; i++)
        //      Cd.QAddF(Sd, CDB.PhaseInfo(i).m_PhMsk, Valid(A[j]->m_PhSplt[i]) ? A[j]->m_PhSplt[i] : A[j]->m_VSplt);
        //    }
        //  }
        //else
        //  Cd.QAddF(Sd, som_Gas, A[j]->m_VSplt);
        }
      }
      break;
    case GSO_PhMassFlowSplit:
      {
      double STotAv=Sd.QMass(som_Sol);
      double LTotAv=Sd.QMass(som_Liq);
      double VTotAv=Sd.QMass(som_Gas);
      double TotRqd=STotAv+LTotAv+VTotAv;
      for (int j=0; j<NPri-1; j++)
        {
        SpConduit & Cd=*Nd.IOConduit(I[j]);
        double SF=A[j]->m_SQmRqd;
        Cd.QSetM(Sd, som_Sol, Min(STotAv, SF), Sd.Press());
        STotAv=GEZ(STotAv-SF);
        TotRqd=TotRqd-SF;
        double LF=A[j]->m_LQmRqd;
        Cd.QAddM(Sd, som_Liq, Min(LTotAv, LF));
        LTotAv=GEZ(LTotAv-LF);
        TotRqd=TotRqd-LF;
        double VF=A[j]->m_VQmRqd;
        Cd.QAddM(Sd, som_Gas, Min(VTotAv, VF));
        A[j]->QmErr=(float)(Cd.QMass()-(SF+LF+VF));
        if (fabs(A[j]->QmErr)>1.0e-6)
          nShort++;
        VTotAv=GEZ(VTotAv-VF);
        TotRqd=TotRqd-VF;
        }
      Nd.IOConduit(I[NPri-1])->QSetM(Sd, som_Sol, STotAv, Sd.Press());
      Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Liq, LTotAv);
      Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Gas, VTotAv);
      A[j]->QmErr=(float)(TotRqd-(STotAv+LTotAv+VTotAv));
      }
      break;
    case GSO_PhVolumeFlowSplit:
      {
      double STotAv=Sd.QMass(som_Sol);
      double LTotAv=Sd.QMass(som_Liq);
      double VTotAv=Sd.QMass(som_Gas);
      double TotRqd=STotAv+LTotAv+VTotAv;
      double SDens=Sd.Rho(som_Sol);
      double LDens=Sd.Rho(som_Liq);
      double VDens=Sd.Rho(som_Gas);
      for (int j=0; j<NPri-1; j++)
        {
        SpConduit & Cd=*Nd.IOConduit(I[j]);
        double SF=A[j]->m_SQvRqd*SDens;
        Cd.QSetM(Sd, som_Sol, Min(STotAv, SF), Sd.Press());
        STotAv=GEZ(STotAv-SF);
        TotRqd=TotRqd-SF;
        double LF=A[j]->m_LQvRqd*LDens;
        Cd.QAddM(Sd, som_Liq, Min(LTotAv, LF));
        LTotAv=GEZ(LTotAv-LF);
        TotRqd=TotRqd-LF;
        double VF=A[j]->m_VQvRqd*VDens;
        Cd.QAddM(Sd, som_Gas, Min(VTotAv, VF));
        A[j]->QvErr=(float)((Cd.QVolume()-(SF/GTZ(SDens)+LF/GTZ(LDens)+VF/GTZ(VDens))));
        if (fabs(A[j]->QvErr)>1.0e-6)
          nShort++;
        VTotAv=GEZ(VTotAv-VF);
        TotRqd=TotRqd-VF;
        }
      Nd.IOConduit(I[NPri-1])->QSetM(Sd, som_Sol, STotAv, Sd.Press());
      Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Liq, LTotAv);
      Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Gas, VTotAv);
      //TODO Fix This!
      A[j]->QvErr=(float)(TotRqd-(STotAv+LTotAv+VTotAv));
      }
      break;
    case GSO_SpMassFracSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        SpConduit &Cn=*Nd.IOConduit(I[j]);
        Cn.QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'
        for (int s=0; s<SDB.Count(); s++)
          Cn.SetVMass(s, Sd, A[j]->Splits[s]*Sd.VMass[s]);
        //Cn.SetTempPress(Sd.Temp(), Sd.Press());
        }
      }
      break;
    case GSO_SpMassFlowSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'
        A[j]->QmErr=0.0;
        }
      double TotErr=0.0;
      for (int s=0; s<SDB.Count(); s++)
        {
        double TotAv=Sd.VMass[s];
        double TotRqd=TotAv;
        for (j = 0;j<NPri-1; j++)
          {
          SpConduit & Cd=*Nd.IOConduit(I[j]);
          double F=A[j]->QmRqds[s];
          Cd.SetVMass(s, Cd, Min(TotAv, F));
          A[j]->QmErr+=(float)(Cd.VMass[s]-F);
          TotAv=GEZ(TotAv-F);
          TotRqd=TotRqd-F;
          }
        Nd.IOConduit(I[NPri-1])->SetVMass(s, Sd, TotAv);
        TotErr+=TotAv-TotRqd;
        }
      for (j = 0;j<NPri-1; j++)
        if (fabs(A[j]->QmErr)>1.0e-6)
          nShort++;
      //for (int j = 0;j<NPri; j++)
      //  Nd.IOConduit(I[j])->SetTempPress(Sd.Temp(), Sd.Press());
      }
      break;
    case GSO_SpVolumeFlowSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'
        A[j]->QvErr=0.0;
        }
      double TotErr=0.0;
      for (int s=0; s<SDB.Count(); s++)
        {
        double Dens=SDB[s].Density(Sd.Fidelity(), Sd.Temp(), Sd.Press(), NULL, Sd.SVData());
        double TotAv=Sd.VMass[s];
        double TotRqd=TotAv;
        for (j = 0;j<NPri-1; j++)
          {
          SpConduit & Cd=*Nd.IOConduit(I[j]);
          double F=A[j]->QvRqds[s]*Dens;
          Cd.SetVMass(s, Sd, Min(TotAv, F));
          A[j]->QvErr+=(float)((Cd.VMass[s]-F)/GTZ(Dens));
          TotAv=GEZ(TotAv-F);
          TotRqd=TotRqd-F;
          }
        Nd.IOConduit(I[NPri-1])->SetVMass(s, Sd, TotAv);
        TotErr+=TotAv-TotRqd;
        }
      for (j = 0;j<NPri-1; j++)
        if (fabs(A[j]->QvErr)>1.0e-6)
          nShort++;
      //for (int j = 0;j<NPri; j++)
      //  Nd.IOConduit(I[j])->SetTempPress(Sd.Temp(), Sd.Press());
      }
      break;
    case GSO_PhaseRecovery:
      {
      PhMask som_Phases[]={som_Sol, som_Liq, som_Gas};
      PhMask som_Phase, som_Other;
      som_Phase=som_Phases[iRecPh];
      som_Other=som_ALL&(~som_Phase);
      double MassPhase=Sd.QMass(som_Phase);
      double MassOther=Sd.QMass(som_Other);
      SpConduit &PCd=*Nd.IOConduit(I[iRecStrm]);
      SpConduit &SCd=*Nd.IOConduit(I[(iRecStrm+1)%2]);

      PCd.QSetM(Sd, som_Phase, MassPhase*dRecFrac, Press);
      SCd.QSetM(Sd, som_Phase, MassPhase*(1.0-dRecFrac), Press);

      double RqdOtherMass=(1.0-dRecComp)/GTZ(dRecComp)*MassPhase*dRecFrac;
      SetCI(2, bTrackStatus && QmInTotal>UsableMass && !InRange(0.0, RqdOtherMass, MassOther));
      RqdOtherMass=Range(0.0, RqdOtherMass, MassOther);

      PCd.QAddM(Sd, som_Other, RqdOtherMass);
      SCd.QAddM(Sd, som_Other, MassOther-RqdOtherMass);


//      case Range((byte)0, iRecPh, (byte)2);
//      iRecStrm=Range((byte)0, iRecStrm, (byte)1);
//      dRecFrac=Range(0.0f, dRecFrac, 1.0f);
//      dRecComp=Range(0.0f, dRecComp, 1.0f);
//      ?
      break;
      }
    }
  if (bTrackStatus && nShort && QmInTotal>UsableMass)
    SetCI(1, "W\tRequirements not met in %i stream%s", nShort, nShort>1 ? "s":"");
  else
    ClrCI(1);
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

void GSModel::CheckConfig()
  {
  if (bIOChanges)
    {
    Gammas.SetSize(SDB.Count()+1);

    NProds=0;
    const int IOsSize = IOs.GetSize();
    for (int i=0; i<IOsSize; i++)
      {
      int io=IOs[i];
      if (i>=IOAlpha.GetSize() || io>IOAlpha[i].IONo)
        {
        GSIOAlpha A;
        A.IONo=io;
        IOAlpha.InsertAt(i, A);
        }
      else
        {
        while (IOAlpha[i].IONo>io && i<IOAlpha.GetSize())
          IOAlpha.RemoveAt(i);
        }
      if (Nd.LnkFlowDefinedOut(IOs[i]))
        NProds++;
      }

    //fix priority numbers...
    bool FixRqd = false;
    for (int j=0; j<IOsSize && !FixRqd; j++)
      FixRqd = (IOAlpha[j].iPriority!=AutoPriority && IOAlpha[j].iPriority>=IOsSize-1);
    if (FixRqd)
      {
      int LastLow = -1;
      bool Fnd = true;
      while (Fnd)
        {
        int Low = 999;
        int LowIndex = -1;
        for (j=0; j<IOsSize; j++)
          if (IOAlpha[j].iPriority!=AutoPriority && IOAlpha[j].iPriority>LastLow && IOAlpha[j].iPriority<Low)
            Low = IOAlpha[j].iPriority;
        LastLow++;
        Fnd = (Low!=999);
        if (Fnd)
          for (j=0; j<IOsSize; j++)
            if (IOAlpha[j].iPriority==Low)
              IOAlpha[j].iPriority = LastLow;
        }
      }

    bIOChanges=0;
    }
  }

//--------------------------------------------------------------------------

void GSModel::PrepareDefinedData()
  {
  };

// -------------------------------------------------------------------------

/*void GSModel::PostConnect(int IONo)
  {
  for (int i=0; i<IOs.GetSize(); i++)
    if (IOs[i]>=IONo)
      break;
  byte Id=Nd.IOId_Self(IONo);
  if (Id>=IOId0 && Id<=IOIdN)
    {
    IOs.InsertAt(i, IONo);
    i++;
    }

  for (i ; i<IOs.GetSize(); i++)
    IOs[i]++;
  bIOChanges=1;
  CheckConfig();
  };*/

void GSModel::PostConnect(int IONo)
  {
  for (int i=0; i<IOs.GetSize(); i++)
    if (IOs[i]>=IONo)
      break;
  int Id = Nd.IOId_Self(IONo);
  if (Id>=IOId0 && Id<=IOIdN)
    {
    IOs.InsertAt(i, IONo);
    //maybe instead of inserting new GSIOAlpha, check end of IOAlpha for unused item and use it so that "old/previous" values are used?
    GSIOAlpha A;
    A.IONo = IOs[i];
    IOAlpha.InsertAt(i, A);
    i++;
    }

  const int IOsSize = IOs.GetSize();
  if (i<IOsSize)
    {
    int ii = i;
    for (i ; i<IOs.GetSize(); i++)
      IOs[i]++;

    //shift everything up...
    const int AlphaSize = IOAlpha.GetSize();
    for (int j=ii; j<IOsSize && j<AlphaSize; j++)
      IOAlpha[j].IONo = IOs[j];
    }
  bIOChanges=1;
  CheckConfig();
  };

// -------------------------------------------------------------------------

/*void GSModel::PreDisConnect(int IONo)
  {
  for (int i=0; i<IOs.GetSize(); i++)
    if (IOs[i]==IONo)
      {
      IOs.RemoveAt(i);
      for ( ; i<IOs.GetSize(); i++)
        IOs[i]--;
      }
  bIOChanges=1;
  CheckConfig();
  };*/

void GSModel::PreDisConnect(int IONo)
  {
  for (int i=0; i<IOs.GetSize(); i++)
    if (IOs[i]==IONo)
      {
      IOs.RemoveAt(i);
      const int IOsSize = IOs.GetSize();
      if (i<IOsSize)
        {
        int ii = i;
        for ( ; i<IOsSize; i++)
          IOs[i]--;

        //shift everything down...
        GSIOAlpha A;
        A = IOAlpha[ii];
        const int AlphaSize = IOAlpha.GetSize();
        for (int j=ii; j<IOsSize && j<AlphaSize; j++)
          {
          IOAlpha[j] = IOAlpha[j+1];
          IOAlpha[j].IONo = IOs[j];
          }
        if (j<AlphaSize)
          {
          IOAlpha[j] = A;
          IOAlpha[j].IONo = AlphaSize-1;//IOs[j];
          }
        }
      }
  bIOChanges=1;
  CheckConfig();
  };

//--------------------------------------------------------------------------

void GSModel::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this, "GM", NULL, DDB_NoPage))
    {
    //TODO Temp fix to force GS Model to CheckConfig() always!
    //KGA 4/6/98 : temporary fix: set flag (bIOChanges) to 1 to
    //force CheckConfig() to recalculate NProds. In theory bIOChanges
    //should be set after a link has been deleted or added so that
    //the access page for the 'general split model' changes its
    //spots when it should!
    bIOChanges=1; //KGA temp fix

    CheckConfig();

    static DDBValueLst DDB0[]=
      {
        {GSO_General0         ,"General"},
          //      {GSO_V5               ,"V5"},
        {GSO_MassFracSplit    ,"MassFrac"},
        {GSO_MassFlowSplit    ,"MassFlow"},
        {GSO_VolumeFlowSplit  ,"VolumeFlow"},
        {GSO_PhMassFracSplit  ,"Phase_MassFrac"},
        {GSO_PhMassFlowSplit  ,"Phase_MassFlow"},
        {GSO_PhVolumeFlowSplit,"Phase_VolumeFlow"},
        {GSO_SpMassFracSplit  ,"Specie_MassFrac"},
        {GSO_SpMassFlowSplit  ,"Specie_MassFlow"},
        {GSO_SpVolumeFlowSplit,"Specie_VolumeFlow"},
        {GSO_PhaseRecovery    ,"Phase_Recovery"},
        {0}
      };
    static DDBValueLst DDB1[]=
      {
        {GSO_General0         ,"General"},
          //      {GSO_V5               ,"V5"},
        {GSO_MassFracSplit    ,"MassFrac"},
        {GSO_MassFlowSplit    ,"MassFlow"},
        {GSO_VolumeFlowSplit  ,"VolumeFlow"},
        {GSO_PhMassFracSplit  ,"Phase_MassFrac"},
        {GSO_PhMassFlowSplit  ,"Phase_MassFlow"},
        {GSO_PhVolumeFlowSplit,"Phase_VolumeFlow"},
        {GSO_SpMassFracSplit  ,"Specie_MassFrac"},
        {GSO_SpMassFlowSplit  ,"Specie_MassFlow"},
        {GSO_SpVolumeFlowSplit,"Specie_VolumeFlow"},
        {0}
      };


    DDB.Byte   ("", "Operation",   DC_, "", xidGSOperation, this, isParmStopped | AffectsStruct | SetOnChange, (NProds==2) ? DDB0 : DDB1);
  //  DDB.Byte   ("", "Operation",   DC_, "", &iOperation, this, isParmStopped | AffectsStruct, DDB1);

    switch (iOperation)
      {
      case GSO_General0:
        BuildDataDefnGen0(DDB);
        break;
      case GSO_V5:
        BuildDataDefnV5(DDB);
        break;
      case GSO_MassFracSplit:
      case GSO_MassFlowSplit:
      case GSO_VolumeFlowSplit:
      case GSO_PhMassFracSplit:
      case GSO_PhMassFlowSplit:
      case GSO_PhVolumeFlowSplit:
      case GSO_SpMassFracSplit:
      case GSO_SpMassFlowSplit:
      case GSO_SpVolumeFlowSplit:
      case GSO_PhaseRecovery:
        BuildDataDefnSplit(DDB);
        break;
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag GSModel::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidGSOperation:
      if (DCB.rB)
        {
        iOperation=*DCB.rB;
        switch (iOperation)
          {
          case GSO_General0:
          case GSO_V5:
          case GSO_MassFracSplit:
          case GSO_MassFlowSplit:
          case GSO_VolumeFlowSplit:
          case GSO_PhMassFracSplit:
          case GSO_PhMassFlowSplit:
          case GSO_PhVolumeFlowSplit:
            break;
          case GSO_SpMassFracSplit:
          case GSO_SpMassFlowSplit:
          case GSO_SpVolumeFlowSplit:
            {
            for (int i=0; i<IOs.GetSize(); i++)
              if (Nd.LnkFlowDefinedOut(IOs[i]))
                {
                IOAlpha[i].Splits.SetSize(SDB.Count());
                IOAlpha[i].QmRqds.SetSize(SDB.Count());
                IOAlpha[i].QvRqds.SetSize(SDB.Count());
                }
            }
            break;
          case GSO_PhaseRecovery:
            {
            int N=0;
            for (int i=0; i<IOs.GetSize(); i++)
              if (Nd.LnkFlowDefinedOut(IOs[i]))
                N++;
            if (N!=2)
              iOperation=GSO_MassFracSplit;
            }
            break;
          }
        }
      DCB.B=iOperation;
      return 1;
    case xidGSPhMethSpec:
      if (DCB.rB)
        m_iPhMethSpec=*DCB.rB;
      DCB.B=m_iPhMethSpec;
      return 1;
    default:;
    }

  switch (iOperation)
    {
    case GSO_General0:
      return DataXchgGen0(DCB);
    case GSO_V5:
      return DataXchgV5(DCB);
    case GSO_MassFracSplit:
    case GSO_MassFlowSplit:
    case GSO_VolumeFlowSplit:
    case GSO_PhMassFracSplit:
    case GSO_PhMassFlowSplit:
    case GSO_PhVolumeFlowSplit:
    case GSO_SpMassFracSplit:
    case GSO_SpMassFlowSplit:
    case GSO_SpVolumeFlowSplit:
    case GSO_PhaseRecovery:
      return DataXchgSplit(DCB);
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag GSModel::ValidateData(ValidateDataBlk & VDB)
  {
  if (!TaggedObject::NumericTagsBad)
    {
    //KGA 6/5/2002 : For temp "fix", warn user about numeric tags in pipe names!
    const int IOCount=IOs.GetSize();
    for (int ia = 0; ia < IOCount; ia++)
      {
      const int io=IOs[ia];
    /*GSIOAlpha &A=IOAlpha[ia];
    QmMd[ia] = A.QmMode(*this);
    MkMd[ia] = A.MkMode(*this);
    Order[ia]=-1;
    Priority[ia]=-1;*/
    if (Nd.LnkFlowDefinedOut(io))
    //for (int j=0; j<NPri; j++)
      {
      Strng s;
      //s = Nd.Nd_Rmt(I[j])->Tag();
      s = Nd.Nd_Rmt(io)->Tag();
      flag Err=true;
      for (int i=0; i<s.Len(); i++)
        if (!isdigit(s[i]))
          Err=false;
      SetCI(4, Err);
      }
    }
    }

  switch (iOperation)
    {
    case GSO_General0:
      return ValidateDataGen0(VDB);
    case GSO_V5:
      return ValidateDataV5(VDB);
    case GSO_MassFracSplit:
    case GSO_MassFlowSplit:
    case GSO_VolumeFlowSplit:
    case GSO_PhMassFracSplit:
    case GSO_PhMassFlowSplit:
    case GSO_PhVolumeFlowSplit:
    case GSO_SpMassFracSplit:
    case GSO_SpMassFlowSplit:
    case GSO_SpVolumeFlowSplit:
    case GSO_PhaseRecovery:
      return ValidateDataSplit(VDB);
    }
  return 0;
  };

//--------------------------------------------------------------------------

void GSModel::MakeUpNodeTransferAvail(int JoinId)
  {
  };

//--------------------------------------------------------------------------

void GSModel::MakeUpNodeTransferReqd(int JoinId)
  {
  };

//--------------------------------------------------------------------------

void GSModel::EvalProducts(long RunMds, double Press, SpConduit *pQf, CReactionBlock *pRB, double Flash_Press, CEnvironHX * pEHX)
  {
  const int IOCount=IOs.GetSize();

  SpConduit &Sd = (pQf!=NULL) ? *pQf : *Nd.GetWrkConduit("GSMdl", chLINEID());//SdLcl();

  // Measure Total inputs and outputs
  CSpMdlSlct Slct;
  double PMin=10000.0;
  for (int ia = 0; ia < IOCount; ia++)
    {
    int io=IOs[ia];
    if (Nd.IO_In(io))
      {
      PMin=Min(PMin, Nd.IOP_Self(io));
      Slct.Add(Nd.IOConduit(io)->Model(), Nd.IOQm_In(io));
      }
    }
  // Sum all  material from IOs to Workspace
  if (RunMds==SM_Direct)
    Press=PMin;
  Sd.QZero();
  Sd.SetPress(Press);
  Sd.SelectModel(Slct);
  int NFeeds=0;
  for (ia = 0; ia < IOCount; ia++)
    {
    int io=IOs[ia];
    if (Nd.IO_In(io))
      {
//?
//      Sd.SelectModel(Nd.IOConduit(io), NFeeds==0);
      Sd.QAddF(*Nd.IOConduit(io), som_ALL, 1.0);
      NFeeds++;
      }
    }

  dbgpln("Must Pass and Solve VLE");
  if (pRB)
    {
  //  if (bFlashIt)
  //    Sd.QPFlash(Flash_Press, 0.0);
    pRB->EvalProducts(Sd);
    }
  //if (bFlashIt)
  //  Sd.QPFlash(Flash_Press, 0.0);

  // Environmental Exchange
  if (pEHX)
    pEHX->EvalProducts(Sd);

  EvalProductsOut(RunMds, Press, Sd);
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOut(long RunMds, double Press, SpConduit &Sd)
  {
  switch (iOperation)
    {
    case GSO_General0:
      EvalProductsOutGen0(RunMds, Press, Sd);
      break;
    case GSO_V5:
      EvalProductsOutV5(RunMds, Press, Sd);
      break;
    case GSO_MassFracSplit:
    case GSO_MassFlowSplit:
    case GSO_VolumeFlowSplit:
    case GSO_PhMassFracSplit:
    case GSO_PhMassFlowSplit:
    case GSO_PhVolumeFlowSplit:
    case GSO_SpMassFracSplit:
    case GSO_SpMassFlowSplit:
    case GSO_SpVolumeFlowSplit:
    case GSO_PhaseRecovery:
      EvalProductsOutSplit(RunMds, Press, Sd);
      break;
    }
  }

//--------------------------------------------------------------------------

flag GSModel::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tRequirements not met in 1 stream"; return 1;
    case  2: pS="W\tRecovery Specification not met";   return 1;
    case  3: pS="W\tNo Outputs";                       return 1;
    case  4: pS="W\tConnected outlet pipes should NOT have 'numeric' tags!"; return 1;
    default:
      return TaggedObject::CIStrng(No, pS);
    }
  };

//============================================================================
//
//
//
//============================================================================

flag GSModelBase::Open()
  {
  fEnabled=True;

  if (pGSM)
    return True;

  pGSM=new GSModel(&GSModelClass, "GM", pNd, TOA_Embedded);
  pNd->StructureChanged(NULL);
  pGSM->IOId0=MdlIOId0;
  pGSM->IOIdN=MdlIOIdN;

  for (int i=0; i<pNd->NoFlwIOs(); i++)
    {
    int Id=pNd->IOId_Self(i);
    if (Id>=MdlIOId0 && Id<=MdlIOIdN)
      pGSM->IOs.Add(i);
      //pGSM->IOs.InsertAt(i, i);
    }
  pGSM->bIOChanges=1;
  pGSM->CheckConfig();
  return fEnabled;
  };

//--------------------------------------------------------------------------

void GSModelBase::Close()
  {
  if (pGSM==NULL)
    return;
  pGSM->StructureChanged(NULL);
  //pGSM->Gammas.SetSize(0);
  //pGSM->IOAlpha.SetSize(0);
  //delete pGSM;
  //pGSM=NULL;
  fEnabled=False;
  };

//--------------------------------------------------------------------------
/**
byte GSModelBase::GMode(int iIONo)
  {
  return pGSM->iGMode;
  };
**/
//--------------------------------------------------------------------------

byte GSModelBase::QmMode(int iIONo)
  {
  int Priority[MaxIOList+1];
  //byte QmMd[MaxIOList];//, MkMd[MaxIOList];
  //int NPri=pGSM->FindIOEvalPriority(Priority, QmMd, MkMd);
  int NPri0;//pGSM->FindIOEvalPriority(Priority, QmMd, MkMd);

  switch (pGSM->iOperation)
    {
    case GSO_General0:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=pGSM->FindIOEvalPriorityGen0(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (pGSM->IOs[ia]==iIONo)
          return QmMd[ia];
      break;
      }
    case GSO_V5:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=pGSM->FindIOEvalPriorityV5(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (pGSM->IOs[ia]==iIONo)
          return QmMd[ia];
      break;
      }
    case GSO_MassFracSplit:
    case GSO_MassFlowSplit:
    case GSO_VolumeFlowSplit:
    case GSO_PhMassFracSplit:
    case GSO_PhMassFlowSplit:
    case GSO_PhVolumeFlowSplit:
    case GSO_SpMassFracSplit:
    case GSO_SpMassFlowSplit:
    case GSO_SpVolumeFlowSplit:
    case GSO_PhaseRecovery:
      {
      GSIOAlpha * A[MaxIOList+1];
      NPri0=pGSM->FindIOEvalPrioritySplit(Priority, A);
      break;
      }
    }

  return GSA_QmInValid;
  };

//--------------------------------------------------------------------------

byte GSModelBase::MkMode(int iIONo)
  {
  int Priority[MaxIOList+1];
//  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri0;//=pGSM->FindIOEvalPriority(Priority, QmMd, MkMd);
  switch (pGSM->iOperation)
    {
    case GSO_General0:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=pGSM->FindIOEvalPriorityGen0(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (pGSM->IOs[ia]==iIONo)
          return MkMd[ia];
      break;
      }
    case GSO_V5:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=pGSM->FindIOEvalPriorityV5(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (pGSM->IOs[ia]==iIONo)
          return MkMd[ia];
      break;
      }
    case GSO_MassFracSplit:
    case GSO_MassFlowSplit:
    case GSO_VolumeFlowSplit:
    case GSO_PhMassFracSplit:
    case GSO_PhMassFlowSplit:
    case GSO_PhVolumeFlowSplit:
    case GSO_SpMassFracSplit:
    case GSO_SpMassFlowSplit:
    case GSO_SpVolumeFlowSplit:
    case GSO_PhaseRecovery:
      {
      GSIOAlpha * A[MaxIOList+1];
      NPri0=pGSM->FindIOEvalPrioritySplit(Priority, A);
      break;
      }
    }

//  int ia;
//  for (int j = 0;((ia=Priority[j])>=0); j++)
//    if (pGSM->IOs[ia]==iIONo)
//      return MkMd[ia];
  return GSA_MkInValid;
  };

//--------------------------------------------------------------------------

GSGammaArray & GSModelBase::Gammas()
  {
  return pGSM->Gammas;
  };

//--------------------------------------------------------------------------

GSIOAlphaArray & GSModelBase::IOAlpha()
  {
  return pGSM->IOAlpha;
  };

//--------------------------------------------------------------------------

void GSModelBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
  DDB.CheckBoxBtn("",     "General_Model",  DC_,    "",     xidGSModelOn,  pNd, Flags, DDBOnOff);
  }

//--------------------------------------------------------------------------

void GSModelBase::BuildDataDefn(DataDefnBlk &DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(SHM_All, fEnabled);

  if (pGSM)
    DDB.Object(pGSM, pNd, NULL, NULL, DDB_RqdPage);//

  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

flag GSModelBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidGSModelOn:
      if (DCB.rB)
        {
        if (*DCB.rB)
          Open();
        else
          Close();
        //pNd->StructureChanged(pNd); Done By Open /Close
        }
      DCB.B=(Enabled());
      return 1;
    }
  return 0;//return pGSM ? pGSM->DataXchg(DCB) : 0;
  }

//============================================================================
//
//
//
//============================================================================
