//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
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
XID xidGSThermal     = MdlBsXID(1002);
XID xidGSPhMethSpec  = MdlBsXID(1003);
XID xidGSMDmndSplts  = MdlBsXID(2000);                        // Keep MaxIOList Spare
XID xidGSMSpcSplts   = xidGSMDmndSplts+MaxIOList;             // Keep MaxSpecies*MaxIOs Spare
XID xidGSPhSplts     = xidGSMSpcSplts+MaxSpecies*MaxIOList;   // Keep MaxSpecies*MaxIOs Spare
XID xidGSTermMode    = xidGSPhSplts+MaxPhases*MaxIOList;      // Keep MaxSpecies*MaxIOs Spare
XID xidLast          = xidGSTermMode+MaxIOList;               // Keep MaxIOs Spare

//============================================================================
//
//
//
//============================================================================

GSIOAlpha::GSIOAlpha() :
  m_Alphas(*(new GSAlphaArray)),
  m_Splits(*(new GSAlphaArray)),
  m_QmRqds(*(new GSAlphaArray)),
  m_QvRqds(*(new GSAlphaArray))
  {

  m_iQmMode=GSA_QmDefault;
  m_iMkMode=GSA_MkDefault;

  m_iOrder     = AutoPriority;
  m_iPriority  = AutoPriority;

  m_Qm         = 0.0;
  m_QmErr      = 0.0;
  m_Qv         = 0.0;
  m_QvErr      = 0.0;
  m_FracSplit  = 0.5;
  m_DmndSplit  = 0.0;
  m_LSRatio    = 1.0;
  m_Density    = 1000.0;
  m_SFrac      = 0.5;
  m_LFrac      = 0.5;
  m_VFrac      = 1.0;
  m_SSplt      = 0.1;
  m_LSplt      = 0.1;
  m_VSplt      = 0.1;
  m_SQmRqd     = 0.0;
  m_LQmRqd     = 0.0;
  m_VQmRqd     = 0.0;
  m_SQvRqd     = 0.0;
  m_LQvRqd     = 0.0;
  m_VQvRqd     = 0.0;
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

  m_Alphas.SetSize(0, SDB.Count()+1);
  m_Splits.SetSize(0, SDB.Count()+1);
  };

// ---------------------------------------------------------------------------

GSIOAlpha::~GSIOAlpha()
  {
  delete &m_Alphas;
  delete &m_Splits;
  delete &m_QmRqds;
  delete &m_QvRqds;
  };

// ---------------------------------------------------------------------------

GSIOAlpha & GSIOAlpha::operator=(GSIOAlpha & A)
  {
  m_IONo       = A.m_IONo;
  m_Desc       = A.m_Desc;
  m_iOrder     = A.m_iOrder; //kga 31/8/00
  m_iPriority  = A.m_iPriority; //kga 31/8/00
  m_iQmMode    = A.m_iQmMode;
  m_iMkMode    = A.m_iMkMode;
  m_Qm         = A.m_Qm;
  m_QmErr      = A.m_QmErr;
  m_Qv         = A.m_Qv;
  m_QvErr      = A.m_QvErr;
  m_FracSplit  = A.m_FracSplit;
  m_DmndSplit  = A.m_DmndSplit;
  m_LSRatio    = A.m_LSRatio;
  m_Density    = A.m_Density;
  m_SFrac      = A.m_SFrac;
  m_LFrac      = A.m_LFrac;
  m_VFrac      = A.m_VFrac;
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
  m_Alphas.SetSize(A.m_Alphas.GetSize());
  m_Splits.SetSize(A.m_Splits.GetSize());
  m_QmRqds.SetSize(A.m_QmRqds.GetSize());
  m_QvRqds.SetSize(A.m_QvRqds.GetSize());
  for (int i=0; i<m_Alphas.GetSize(); i++)
    {
    m_Alphas[i]=A.m_Alphas[i];
    m_Splits[i]=A.m_Splits[i];
    m_QmRqds[i]=A.m_QmRqds[i];
    m_QvRqds[i]=A.m_QvRqds[i];
    }

  return *this;
  }

//--------------------------------------------------------------------------

void GSIOAlpha::BuildDataDefnOrdPri(DataDefnBlk &DDB, GSModel & Mdl)
  {
  Strng StrVal;

  DDB.Byte   ("",             "Order",      DC_,    "",     &m_iOrder,          &Mdl, isResult|0);

  DDBValueLstMem DDBP;
  DDBP.Add(AutoPriority, "Auto");
  for (int i=0; i<Mdl.m_IOs.GetSize()-1; i++)
    {
    StrVal.Set("%i", i+1);
    DDBP.Add(i, StrVal());
    }
  DDB.Byte   ("",             "Priority",   DC_,    "",     &m_iPriority,       &Mdl, isParm, DDBP());
  }

//--------------------------------------------------------------------------

void GSIOAlpha::BuildDataDefn(DataDefnBlk & DDB, GSModel & Mdl, flag Visible, int IONo, flag LastOne)
  {
  Strng DefLbl;
  Strng StrVal;

  DDB.Visibility(NSHM_All, Visible, Visible, Visible, Visible);

  BuildDataDefnOrdPri(DDB, Mdl);

  switch (Mdl.m_iQmMode)
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

  DDB.Byte   ("",             "QmMode",   DC_,    "",     &m_iQmMode,         &Mdl, isParmStopped | AffectsStruct | SetOnChange, DDB0);

  DDB.Visibility(NSHM_All, Visible && ((QmMode(Mdl)==GSA_QmMassFlow)), Visible, Visible, Visible);
  DDB.Double("",     "Qm",          DC_Qm,      "kg/s",   &m_Qm,              &Mdl,
    ((QmMode(Mdl)==GSA_QmMassFlow) && !LastOne ? isParm : 0));
  DDB.Double("",     "QmErr",          DC_Qm,      "kg/s",   &m_QmErr,              &Mdl, isResult|0);

  DDB.Visibility(NSHM_All, Visible && ((QmMode(Mdl)==GSA_QmVolumeFlow)), Visible, Visible, Visible);
  DDB.Double("",     "Qv",          DC_Qv,      "L/s",   &m_Qv,              &Mdl,
    ((QmMode(Mdl)==GSA_QmVolumeFlow) && !LastOne ? isParm : 0));
  DDB.Double("",     "QvErr",          DC_Qv,      "L/s",   &m_QvErr,              &Mdl, isResult|0);

  DDB.Visibility(NSHM_All, Visible && (QmMode(Mdl)==GSA_QmPercSplit), Visible, Visible, Visible);
  DDB.Double ("",     "Split",       DC_Frac,    "%",      &m_FracSplit,       &Mdl,
    ((QmMode(Mdl)==GSA_QmPercSplit) && !LastOne ? isParm : 0));

  DDB.Visibility(NSHM_All, Visible && (QmMode(Mdl)==GSA_QmDemand), Visible, Visible, Visible);
  DDB.Double("",     "Demand",      DC_Frac,    "%",      &m_DmndSplit,        &Mdl, isResult|0);

  DDB.Visibility(NSHM_All, Visible, Visible, Visible, Visible);
  switch (Mdl.m_iMkMode)
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
  DDB.Byte   ("",             "MkMode",   DC_,    "",     &m_iMkMode,         &Mdl, isParmStopped|AffectsStruct | SetOnChange, DDB1);

  DDB.Visibility(NSHM_All, Visible && (MkMode(Mdl)==GSA_MkLSRatio), Visible, Visible, Visible);
  DDB.Double("",     "LSRatio",     DC_Frac,    "%",      &m_LSRatio,         &Mdl, isParm);
  DDB.Visibility(NSHM_All, Visible && (MkMode(Mdl)==GSA_MkDensity), Visible, Visible, Visible);
  DDB.Double("",     "Density",     DC_Rho,    "kg/m^3",  &m_Density,         &Mdl, isParm);
  DDB.Visibility(NSHM_All, Visible && (MkMode(Mdl)==GSA_MkPhaseComp), Visible, Visible, Visible);
  DDB.Double("",     "SolidsFrac",  DC_Frac,    "%",      &m_SFrac,           &Mdl, isParm);
  DDB.Double("",     "LiquidFrac",  DC_Frac,    "%",      &m_LFrac,           &Mdl, isParm);
  DDB.Double("",     "VapourFrac",  DC_Frac,    "%",      &m_VFrac,           &Mdl, isParm);
  DDB.Visibility(NSHM_All, Visible && (MkMode(Mdl)==GSA_MkSpecieFrac), Visible, Visible, Visible);
  if (PrjFileVerNo()<22)
    {
    DDB.Double("",     "SolidsSplt",  DC_Frac,    "%",      &m_SSplt,           &Mdl, isParm);
    DDB.Double("",     "LiquidSplt",  DC_Frac,    "%",      &m_LSplt,           &Mdl, isParm);
    DDB.Double("",     "VapourSplt",  DC_Frac,    "%",      &m_VSplt,           &Mdl, isParm);
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
      DDB.Double(Tag() ? Tag() : "",   Sym(),  DC_Frac,    "%",      xidGSMSpcSplts + s + SDB.Count()*IONo,  &Mdl, isParm|NAN_OK);
      }
    }
  else
    {
    if (DDB.BeginObject(&Mdl, "Splt", "GSM_Splt", NULL, DDB_NoPage))
      {
      DDB.Double("",     "Solids",  DC_Frac,    "%",      &m_SSplt,           &Mdl, isParm);
      DDB.Double("",     "Liquid",  DC_Frac,    "%",      &m_LSplt,           &Mdl, isParm);
      DDB.Double("",     "Vapour",  DC_Frac,    "%",      &m_VSplt,           &Mdl, isParm);
      for (int s=0; s<SDB.Count(); s++)
        DDB.Double(SDB[s].Tag(), SDB[s].Sym(),  DC_Frac,    "%",      xidGSMSpcSplts + s + SDB.Count()*IONo,  &Mdl, isParm|NAN_OK);
      }
    DDB.EndObject();
    }
  DDB.Visibility(NSHM_All, Visible, Visible, Visible, Visible);

  DDB.Text("");
  };

//--------------------------------------------------------------------------

flag GSIOAlpha::DataXchg(DataChangeBlk & DCB)
  {

  return 0;
  };

//--------------------------------------------------------------------------

inline void Test_0_1(double &D) { if (Valid(D)) D=Range(0.0, D, 1.0); else D=0.0; };
inline void Test_Inv_0_1(double &D) { if (Valid(D)) D=Range(0.0, D, 1.0); };

flag GSIOAlpha::ValidateData(ValidateDataBlk & VDB, byte PhMethSpec)
  {
  flag OK=1;
  Test_Inv_0_1(m_SSplt);
  Test_Inv_0_1(m_LSplt);
  Test_Inv_0_1(m_VSplt);
  for (int i=0; i<MaxPhases; i++)
    Test_Inv_0_1(m_PhSplt[i]);
  int sn=Min(SDB.Count(), m_Splits.GetSize());
  for (int s=0; s<sn; s++)
    Test_Inv_0_1(m_Splits[s]);

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
      m_PhSplt[i]=Range(0.0, m_PhSplt[i], 1.0);
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
  m_Nd(*((MdlNode*)pAttach)),
  m_Gammas(*(new GSGammaArray)),
  m_IOAlpha(*(new GSIOAlphaArray)),
  m_IOs(*(new GSIOIndexArray)),
  m_IOThermal(*(new GSIOThermalArray))
  {
  m_iOperation=GSO_General0;
#if WITHTHERMAL 
  m_bThermal      = false;
  m_bAllowHX      = false;
  m_TempIn        = StdT;
  m_PressIn       = StdP;
  m_PowerIn       = 0.0;
  m_FinalTempMin  = CDB.MinT;
  m_FinalTempMax  = CDB.MaxT;
  m_ChangeTst     = false;
  m_ChangeNeg     = -50;
  m_ChangePos     = 50;
#endif
  m_iPhMethSpec=GSPM_Total;

  m_iGMode=GSG_Off;
  m_iQmMode=GSA_QmDemand;
  m_iMkMode=GSA_MkMixture;
  m_bIOChanges=1;
  m_bTrackStatus=1;
  m_IOId0=0;
  m_IOIdN=255;
  m_NProds=0;

  m_iRecPh=0;
  m_iRecStrm=0;
  m_dRecFrac=1.0;
  m_dRecComp=0.5;
  m_pWrkCd=NULL;
  };

//--------------------------------------------------------------------------

GSModel::~GSModel()
  {
  delete m_pWrkCd;
  delete &m_Gammas;
  delete &m_IOAlpha;
  delete &m_IOs;
  delete &m_IOThermal;
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
  DDB.Byte   ("",     "QmMode",   DC_,    "",     &m_iQmMode,          this, isParmStopped | AffectsStruct | SetOnChange, DDB1);

  static DDBValueLst DDB2[]={
    {GSA_MkMixture  ,  "Mixture"},
    //{GSA_MkDensity  ,  "Density"},  Arithmetic Incomplete
    {GSA_MkLSRatio  ,  "LSRatio"},
    {GSA_MkPhaseComp , "PhaseFracs", MDD_Hidden},
    {GSA_MkPhaseComp , "PhaseComp",  MDD_Hidden},
    {GSA_MkPhaseComp , "Composition"},
    {0}};
  DDB.Byte   ("",     "MkMode",   DC_,    "",     &m_iMkMode,          this, isParmStopped | AffectsStruct | SetOnChange, DDB2);

  DDB.CheckBox("TrackStatus", "", DC_,    "",     &m_bTrackStatus,     this, isParm);

  int Priority[MaxIOList+1];
  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri=FindIOEvalPriorityGen0(Priority, QmMd, MkMd);
  int NOut=0;
  for (int i=0; i<m_IOs.GetSize(); i++)
    if (m_Nd.LnkFlowDefinedOut(m_IOs[i]))
      NOut++;

  if (NOut>1)
    {
    Strng S;
    if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
      {
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        {
//        m_IOAlpha[ia].iOrder=j+1;
//        m_IOAlpha[ia].Desc.Set("%s:%s", m_Nd.IODesc_Self(m_IOs[ia])->pName, m_Nd.Nd_Rmt(m_IOs[ia])->Tag());

        //IORECBUG
        flag Visible=m_Nd.LnkFlowDefinedOut(m_IOs[ia]);
        if (DDB.BeginElement(this, m_Nd.Nd_Rmt(m_IOs[ia])->Tag(), m_Nd.IODesc_Self(m_IOs[ia])->DefaultIOName(OrigPrjFileVerNo())))
          m_IOAlpha[ia].BuildDataDefn(DDB, *this, Visible, ia, (Priority[j+1]<0));
        }

      // Old Version - for compatibility
      if ((!DDB.ForFiling() && !DDB.ForSnapShot() && !DDB.ForScenario() && !DDB.ForView()) ||
          (DDB.ForFiling() && DDB.DoingPutData()))
        for (int i=0; i<m_IOs.GetSize(); i++)
          {
          byte Ord=1;
          for (int j = 0;(Priority[j]>=0) && (Priority[j]!=i); j++)
              Ord++;
          m_IOAlpha[i].m_iOrder=Ord;
          if (DDB.BeginElement(this, i, m_IOAlpha[i].m_Desc()))
            {
            flag Visible=m_Nd.LnkFlowDefinedOut(m_IOs[i]);
            m_IOAlpha[i].BuildDataDefn(DDB, *this, True, i, False);
            }
          }
      }
    DDB.EndArray();
    }

  };

//--------------------------------------------------------------------------

flag GSModel::DataXchgGen0(DataChangeBlk & DCB)
  {

  if (DCB.lHandle>=xidGSMSpcSplts &&  DCB.lHandle<xidGSMSpcSplts+SDB.Count()*m_IOs.GetSize())
    {
    int i=(DCB.lHandle-xidGSMSpcSplts)/SDB.Count();
    int s=(DCB.lHandle-xidGSMSpcSplts)%SDB.Count();

    if (DCB.rD)
      {
      int sz=m_IOAlpha[i].m_Splits.GetSize();
      if (s>=sz)
        {
        m_IOAlpha[i].m_Splits.SetSize(s+1);
        for (int j=sz; j<s; j++)
          m_IOAlpha[i].m_Splits[j]=fNAN;
        }
      if (Valid(*DCB.rD))
        m_IOAlpha[i].m_Splits[s]=Range(0.0, *DCB.rD, 1.0);
      else
        m_IOAlpha[i].m_Splits[s]=fNAN;
      }

    if (s<m_IOAlpha[i].m_Splits.GetSize())
      DCB.D=m_IOAlpha[i].m_Splits[s];
    else
      DCB.D=dNAN;

    return 1;
    }

  return 0;
  };

//--------------------------------------------------------------------------

flag GSModel::ValidateDataGen0(ValidateDataBlk & VDB)
  {
  flag OK=1;
  for (int i=0; i<m_IOAlpha.GetSize(); i++)
    OK =OK && m_IOAlpha[i].ValidateData(VDB, m_iPhMethSpec);

  // Sort m_IOs in order of priority
  int Priority[MaxIOList+1];
  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri=FindIOEvalPriorityGen0(Priority, QmMd, MkMd);

  int ia;
  double QmDemandEstTot=0.0;
  for (int j = 0;((ia=Priority[j])>=0); j++)
    if (QmMd[ia]==GSA_QmDemand)
      {
      double xxx=m_Nd.IOQmEst_Out(m_IOs[ia]);
      QmDemandEstTot+=m_Nd.IOQmEst_Out(m_IOs[ia]);
      }
  for (j = 0;((ia=Priority[j])>=0); j++)
    if (QmMd[ia]==GSA_QmDemand)
      {
      double xxx=m_Nd.IOQmEst_Out(m_IOs[ia]);
      m_IOAlpha[ia].m_DmndSplit=Range((double)FLT_MIN, xxx/GTZ(QmDemandEstTot), (double)FLT_MAX);
      }

  double TotalSplit=0.0;
  flag AllPercSplit=false;
  for (j = 0;((ia=Priority[j])>=0); j++)
    {
    int ianext=Priority[j+1];
    GSIOAlpha &A=m_IOAlpha[ia];
    switch (QmMd[ia])
      {
      case GSA_QmPercSplit:
        {
        if (j==0)
          AllPercSplit=true;
        if (!Valid(A.m_FracSplit))
          A.m_FracSplit=0.0;
        if (ianext>=0)
          A.m_FracSplit=Range(0.0 , A.m_FracSplit, 1.0-TotalSplit);
        else if (AllPercSplit)
          A.m_FracSplit=1.0-TotalSplit;
        TotalSplit+=A.m_FracSplit;
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
        if (!Valid(A.m_SFrac)) A.m_SFrac=0.0;
        if (!Valid(A.m_LFrac)) A.m_LFrac=0.0;
        if (!Valid(A.m_VFrac)) A.m_VFrac=0.0;
        double Tot=A.m_SFrac+A.m_LFrac+A.m_VFrac;
        if (Tot>1.0e-10)
          {
          double Scl=1.0/Tot;
          A.m_SFrac*=Scl;
          A.m_LFrac*=Scl;
          A.m_VFrac*=Scl;
          }
        else
          {
          A.m_SFrac=1.0;
          A.m_LFrac=0.0;
          A.m_VFrac=0.0;
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
  const int IOCount=m_IOs.GetSize();
  int NOrd=0, Order[MaxIOList+1];
  int NPri=0;
  flag Done[MaxIOList];
  Priority[0]=-1;
  Order[0]=-1;
  Done[0]=0;

  for (int ia = 0; ia < IOCount; ia++)
    {
    int io=m_IOs[ia];
    GSIOAlpha &A=m_IOAlpha[ia];
    QmMd[ia] = A.QmMode(*this);
    MkMd[ia] = A.MkMode(*this);
    Order[ia]=-1;
    Priority[ia]=-1;
    if (m_Nd.LnkFlowDefinedOut(io))
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
        GSIOAlpha &A=m_IOAlpha[ia];
        if (A.m_iPriority==ThisPri)
          {
          Priority[NPri++]=ia;
          Done[ia]=1;
          }
        else if (A.m_iPriority>ThisPri)
          NextPri=Min(NextPri, A.m_iPriority);
        }
    }

  for (int j=0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=m_IOAlpha[ia];
      if ((MkMd[ia]==GSA_MkLSRatio) && Valid(A.m_LSRatio) ||
          (MkMd[ia]==GSA_MkDensity) && Valid(A.m_Density) ||
          (MkMd[ia]==GSA_MkPhaseComp) && (Valid(A.m_SFrac) && Valid(A.m_LFrac) && Valid(A.m_VFrac)) ||
          (MkMd[ia]==GSA_MkSpecieFrac))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=m_IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmMassFlow) && Valid(A.m_Qm))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=m_IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmVolumeFlow) && Valid(A.m_Qv))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=m_IOAlpha[ia];
      if ((QmMd[ia]==GSA_QmPercSplit) && Valid(A.m_FracSplit))
        {
        Priority[NPri++]=ia;
        Done[ia]=1;
        }
      }

  for (j = 0;((ia=Order[j])>=0); j++)
    if (!Done[ia])
      {
      GSIOAlpha &A=m_IOAlpha[ia];
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
    //IORECBUG
    int ia=Priority[j];
    //m_IOAlpha[Priority[j]].iOrder=j+1;
    m_IOAlpha[ia].m_iOrder=j+1;
    m_IOAlpha[ia].m_Desc.Set("%s:%s", m_Nd.IODesc_Self(m_IOs[ia])->IOName(), m_Nd.Nd_Rmt(m_IOs[ia])->Tag());
    }

  ASSERT(NPri==NOrd);
  return NPri;
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOutGen0(long SolveMethod, double Press, SpConduit &Sd)
  {
  if (m_pWrkCd==NULL)
    m_pWrkCd= new SpConduit(&SpConduitClass, "Work", NULL, TOA_Free);

  SpConduit & WrkCd = *m_pWrkCd;
  WrkCd.QCopy(Sd);

  double Temperature=WrkCd.Temp();
  int nShort=0;
    
  // Sort m_IOs in order of priority
  int Priority[MaxIOList+1];
  byte QmMd[MaxIOList], MkMd[MaxIOList];
  int NPri=FindIOEvalPriorityGen0(Priority, QmMd, MkMd);

  // Calculate total output flows in each Output
  double QmInTotal=WrkCd.QMass(som_ALL);
  double QmOutAvail=QmInTotal;
  double QmDemandTot=0.0;
  double QmDemandEstTot=0.0;
  flag MakeUpRqd=False;
  for (int ia, j = 0;((ia=Priority[j])>=0); j++)
    {
    //if (j<NPri-1)
    m_IOAlpha[ia].m_QmErr=0.0;
    m_IOAlpha[ia].m_QvErr=0.0;
    if (QmMd[ia]==GSA_QmDemand)
      {
      if (m_Nd.IOFlange(m_IOs[ia])->IsMakeUpReqd())
        {
        MakeUpRqd=True;
        QmDemandTot+=m_Nd.IOFlange(m_IOs[ia])->MakeUpReqd();
        }
      else
        QmDemandEstTot+=m_Nd.IOQmEst_Out(m_IOs[ia]);
      }
    }
  //if (dbg)
  //  {
  //  dbgpln("---------------------------------- %s", m_Nd.FullObjTag());
  //  dbgpln("%14g %14g %14g %14g ", QmInTotal, QmOutAvail, QmDemandEstTot, Temperature);
  //  }

  for (j = 0;((ia=Priority[j])>=0); j++)
    {
    int io=m_IOs[ia];
    rSpConduit Cn = *(m_Nd.IOConduit(io));
    if (j<NPri-1)
      {
      GSIOAlpha &A=m_IOAlpha[ia];
      flag SetMakeUpRqdLcl=False;
      double Qo;
      switch (QmMd[ia])
        {
        case GSA_QmDemand :
          SetMakeUpRqdLcl=MakeUpRqd;
          if (MakeUpRqd)
            {
            if (m_Nd.IOFlange(io)->IsMakeUpReqd())
              {
              Qo=m_Nd.IOFlange(io)->MakeUpReqd()*QmOutAvail/GTZ(QmDemandTot);
              SetMakeUpRqdLcl=False;
              }
            else
              Qo=0.0;
            }
          else
            Qo=m_Nd.IOQmEst_Out(io)*QmOutAvail/GTZ(QmDemandEstTot);
          A.m_DmndSplit=(Qo/GTZ(QmInTotal));
          break;
        case GSA_QmPercSplit :
          if (Valid(A.m_FracSplit))
            {
            Qo=A.m_FracSplit*QmInTotal;
            SetMakeUpRqdLcl=MakeUpRqd;
            }
          break;
        case GSA_QmMassFlow  :
          if (Valid(A.m_Qm))
            {
            Qo=A.m_Qm;
            SetMakeUpRqdLcl=MakeUpRqd;
            }
          break;
        case GSA_QmVolumeFlow  :
          if (Valid(A.m_Qv))
            {
            Qo=A.m_Qv*WrkCd.Rho();
            SetMakeUpRqdLcl=MakeUpRqd;
            }
          break;
        default:
          Qo=0.0;
          break;
        }
      if (SetMakeUpRqdLcl)
        m_Nd.IOFlange(io)->SetMakeUpReqdLcl(Qo);
      //else
      //  m_Nd.IOFlange(io)->ClrMakeUpReqdLcl();

      // Input estimates Based on Sigma Output estimates
      //...
//2811      for (int i=0; i<IOCount; i++)
//2811        if (m_Nd.IO_In(i))
//2811          m_Nd.IOFlange(i)->SetPBFlwScl(Max(1.0e-6, m_Nd.IOConduit(i)->QMass()/GTZ(QmInTotal)));

      double QoReqd=Qo;
      Qo=Range(0.0, Qo, QmOutAvail);
      switch (MkMd[ia])
        {
        case GSA_MkMixture   :
          //Cn.QSetM(WrkCd, som_ALL, Max(0.5*UsableMass, Qo), Press); //why use 0.5*UsableMass when it should be zero?
          Cn.QSetM(WrkCd, som_ALL, Max(0.1*UsableMass, Qo), Press);
          //Cn.QSetM(WrkCd, som_ALL, Qo, Press); //Wouldn't this be best! ???
          break;
        case GSA_MkDensity   :
          Cn.QSetM(WrkCd, som_Liq, 0.0, Press);
          break;
        case GSA_MkLSRatio   :
          if (Valid(A.m_LSRatio))
            {
            // Check Availability
            double LFrac=A.m_LSRatio/(1.0+A.m_LSRatio);
            double LQoAv=WrkCd.QMass(som_Liq);
            double SQoAv=WrkCd.QMass(som_Sol);
            double LQo=Qo*LFrac;
            double SQo=Qo*(1.0-LFrac);
            if (LQoAv<LQo || SQoAv<SQo)
              Qo=Min(LQoAv/GTZ(LFrac), SQoAv/GTZ(1.0-LFrac));

            Cn.QSetM(WrkCd, som_Liq, Qo*LFrac, Press);
            Cn.QAddM(WrkCd, som_Sol, Qo*(1.0-LFrac));
            }
          else
            Cn.QSetM(WrkCd, som_ALL, 0.0, Press);
          break;
        case GSA_MkPhaseComp :
          if (Valid(A.m_SFrac) && Valid(A.m_LFrac) && Valid(A.m_VFrac))
            {
            double Scl=1.0/GTZ(A.m_SFrac+A.m_LFrac+A.m_VFrac);
            A.m_SFrac*=Scl;
            A.m_LFrac*=Scl;
            A.m_VFrac*=Scl;

            double SQoAv=WrkCd.QMass(som_Sol);
            double LQoAv=WrkCd.QMass(som_Liq);
            double VQoAv=WrkCd.QMass(som_Gas);
            double SQo=Qo*A.m_SFrac;
            double LQo=Qo*A.m_LFrac;
            double VQo=Qo*A.m_VFrac;
            if (SQoAv<SQo)
              Qo=Min(Qo, SQoAv/GTZ(A.m_SFrac));
            if (LQoAv<LQo)
              Qo=Min(Qo, LQoAv/GTZ(A.m_LFrac));
            if (VQoAv<VQo)
              Qo=Min(Qo, VQoAv/GTZ(A.m_VFrac));

            Cn.QSetM(WrkCd, som_Sol, Qo*A.m_SFrac, Press);
            Cn.QAddM(WrkCd, som_Liq, Qo*A.m_LFrac);
            Cn.QAddM(WrkCd, som_Gas, Qo*A.m_VFrac);
            }
          else
            Cn.QSetM(WrkCd, som_ALL, 0.0, Press);
          break;
        case GSA_MkSpecieFrac :
          {
          Cn.QSetM(WrkCd, som_ALL, 1.0, Press); // Transfer Qualities etc'

          int sn=A.m_Splits.GetSize();
          for (int s=0; s<SDB.Count(); s++)
            {
            double Splt=dNAN;
            if (s<sn && Valid(A.m_Splits[s]))
              Splt=A.m_Splits[s];
            else if (SDB[s].IsSol() )
              Splt=A.m_SSplt;
            else if (SDB[s].IsLiq())
              Splt=A.m_LSplt;
            else if (SDB[s].IsGas())
              Splt=A.m_VSplt;
            else
              Splt=0.0;
            Splt=Range(0.0, (Valid(Splt) ? Splt : 0.0), 1.0);
            Cn.SetVMass(s, WrkCd, Splt*WrkCd.VMass[s]);
            }
          }
          break;
        default              :
          Cn.QSetM(WrkCd, som_ALL, Max(0.5*UsableMass, 0.0), Press);
          break;
        }
      Cn.SetTemp(Temperature);

      const double QMassUsed = Cn.QMass(som_ALL);
      A.m_QmErr=(QoReqd-QMassUsed);
      A.m_QvErr=(A.m_QmErr/GTZ(Cn.Rho()));
      if (m_bTrackStatus && fabs(A.m_QmErr)>1.0e-6)
        nShort++;
      QmOutAvail-=QMassUsed;
      if (QmMd[ia]==GSA_QmDemand)
        {
        if (MakeUpRqd)
          {
          if (m_Nd.IOFlange(io)->IsMakeUpReqd())
            QmDemandTot-=m_Nd.IOFlange(io)->MakeUpReqd();
          }
        else
          QmDemandEstTot-=m_Nd.IOQmEst_Out(io);
        }

      WrkCd.QSubM(Cn, som_ALL, Cn.QMass());
      // Check That Species dont go negative
      for (int s=0; s<SDB.Count(); s++)
        if (WrkCd.VMass[s]<0.0)
          WrkCd.VValue[s]=0.0;

      WrkCd.SetTemp(Temperature);
      }
    else
      {
      // The Last Output Must take What is Left
      Cn.QCopy(WrkCd);
      Cn.SetTempPress(Temperature, Press);

      switch (QmMd[ia])
        {
        case GSA_QmDemand :
          {
          GSIOAlpha &A=m_IOAlpha[ia];
          double Qo=Cn.QMass();
          A.m_DmndSplit=(Qo/GTZ(QmInTotal));
          break;
          }
        }

      }
    }
  if (m_bTrackStatus && nShort)
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

void GSModel::EvalProductsOutV5(long SolveMethod, double Press, SpConduit &Sd)
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
  switch (m_iOperation)
    {
    case GSO_MassFracSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          DDB.Double ("", "Split", DC_Frac, "%", &A[j]->m_FracSplit, this, j<NPri-1 ? isParm : 0);
          }
      DDB.EndArray();
      break;
    case GSO_MassFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            DDB.Double("", "Flow", DC_Qm, "kg/s", &A[j]->m_Qm, this, j<NPri-1 ? isParm : 0);
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Double("", "QmErr", DC_Qm,  "kg/s", &A[j]->m_QmErr, this, isResult);
          }
      DDB.EndArray();
      break;
     case GSO_VolumeFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            DDB.Double("", "VFlow", DC_Qv, "L/s", &A[j]->m_Qv, this, j<NPri-1 ? isParm : 0);
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Double("", "QvErr", DC_Qv,  "L/s", &A[j]->m_QvErr, this, isResult);
          }
      DDB.EndArray();
      break;
    case GSO_PhMassFracSplit:
//      DDB.Byte   ("", "PhaseSpec",   DC_, "", &m_iPhMethSpec, this, isParmStopped | AffectsStruct | SetOnChange, DDBPhMeth);
      DDB.Byte   ("", "PhaseSpec",   DC_, "", xidGSPhMethSpec, this, isParmStopped | AffectsStruct | SetOnChange, DDBPhMeth);
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (PrjFileVerNo()<22)
            {
            DDB.Double("Solids.Splt", "",  DC_Frac, "%",  &A[j]->m_SSplt,  this, j<NPri-1 ? isParm : 0);
            DDB.Double("Liquids.Splt","",  DC_Frac, "%",  &A[j]->m_LSplt,  this, j<NPri-1 ? isParm : 0);
            DDB.Double("Gasses.Splt", "",  DC_Frac, "%",  &A[j]->m_VSplt,  this, j<NPri-1 ? isParm : 0);
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
                    DDB.Double(Nm[c], "",  DC_Frac, "%",  &A[j]->m_Splt[c],  this, j<NPri-1 ? isParm : 0);
                  if (m_iPhMethSpec&GSPM_Individual)
                    {
                    for (int p=0; p<CDB.PhaseCount(); p++)
                      {
                      CPhaseInfo &P=CDB.PhaseInfo(p);
                      if (P.m_eOcc==c)
                        DDB.Double(P.m_Tag(), P.m_Sym(),  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                        (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                      }
                    }
                  }
                //
                //if (m_iPhMethSpec&GSPM_Total)
                //  DDB.Double("Solids", "",  DC_Frac, "%",  &A[j]->m_SSplt,  this, j<NPri-1 ? isParm : 0);
                //if (m_iPhMethSpec&GSPM_Individual)
                //  {
                //  for (int p=0; p<CDB.PhaseCount(); p++)
                //    {
                //    CPhaseInfo &P=CDB.PhaseInfo(p);
                //    if (P.m_eOcc==BOT_Solid)
                //      DDB.Double(P.m_Nm(), "",  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                //      (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                //    }
                //  }

                //if (m_iPhMethSpec&GSPM_Total)
                //  DDB.Double("Liquids","",  DC_Frac, "%",  &A[j]->m_LSplt,  this, j<NPri-1 ? isParm : 0);
                //if (m_iPhMethSpec&GSPM_Individual)
                //  {
                //  for (int p=0; p<CDB.PhaseCount(); p++)
                //    {
                //    CPhaseInfo &P=CDB.PhaseInfo(p);
                //    if (P.m_eOcc==BOT_Liquid)
                //      DDB.Double(P.m_Nm(), "",  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                //      (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                //    }
                //  }
                //
                //if (m_iPhMethSpec&GSPM_Total)
                //  DDB.Double("Gasses", "",  DC_Frac, "%",  &A[j]->m_VSplt,  this, j<NPri-1 ? isParm : 0);
                //if (m_iPhMethSpec&GSPM_Individual)
                //  {
                //  for (int p=0; p<CDB.PhaseCount(); p++)
                //    {
                //    CPhaseInfo &P=CDB.PhaseInfo(p);
                //    if (P.m_eOcc==BOT_Gas)
                //      DDB.Double(P.m_Nm(), "",  DC_Frac, "%",  &A[j]->m_PhSplt[p],  this,
                //      (j<NPri-1 ? isParm : 0)|(m_iPhMethSpec&GSPM_Total?NAN_OK:0)|ChildLevel(1));
                //    }
                //  }
                }
              DDB.EndObject();
              }
            else
              {//this code DOES work for numeric tag pipe eg "X_1.GM.IOs.[2].Splt.Solids (%)"...
              DDB.Double("Splt.Solids", "",  DC_Frac, "%",  &A[j]->m_SSplt,  this, j<NPri-1 ? isParm : 0);
              DDB.Double("Splt.Liquids","",  DC_Frac, "%",  &A[j]->m_LSplt,  this, j<NPri-1 ? isParm : 0);
              DDB.Double("Splt.Gasses", "",  DC_Frac, "%",  &A[j]->m_VSplt,  this, j<NPri-1 ? isParm : 0);
              }
            }
          }
      DDB.EndArray();
      break;
    case GSO_PhMassFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            {
            if (PrjFileVerNo()<22)
              {
              DDB.Double("Solids.QmRqd", "",  DC_Qm, "kg/s",  &A[j]->m_SQmRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Double("Liquids.QmRqd","",  DC_Qm, "kg/s",  &A[j]->m_LQmRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Double("Gasses.QmRqd", "",  DC_Qm, "kg/s",  &A[j]->m_VQmRqd,  this, j<NPri-1 ? isParm : 0);
              }
            else
              {
              if (DDB.BeginObject(this, "QmRqd", "GSM_QmRqd", NULL, DDB_NoPage))
                {
                DDB.Double("Solids", "",  DC_Qm, "kg/s",  &A[j]->m_SQmRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Double("Liquids","",  DC_Qm, "kg/s",  &A[j]->m_LQmRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Double("Gasses", "",  DC_Qm, "kg/s",  &A[j]->m_VQmRqd,  this, j<NPri-1 ? isParm : 0);
                }
              DDB.EndObject();
              }
            }
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Double("", "QmErr", DC_Qm,  "kg/s", &A[j]->m_QmErr, this, isResult|0);
          }
      DDB.EndArray();
      break;
    case GSO_PhVolumeFlowSplit:
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          if (j<NPri-1 || !DDB.ForView())
            {
            if (PrjFileVerNo()<22)
              {
              DDB.Double("Solids.QvRqd", "",  DC_Qv, "L/s",  &A[j]->m_SQvRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Double("Liquids.QvRqd","",  DC_Qv, "L/s",  &A[j]->m_LQvRqd,  this, j<NPri-1 ? isParm : 0);
              DDB.Double("Gasses.QvRqd", "",  DC_Qv, "L/s",  &A[j]->m_VQvRqd,  this, j<NPri-1 ? isParm : 0);
              }
            else
              {
              if (DDB.BeginObject(this, "QmRqd", "GSM_QmRqd", NULL, DDB_NoPage))
                {
                DDB.Double("Solids", "",  DC_Qv, "L/s",  &A[j]->m_SQvRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Double("Liquids","",  DC_Qv, "L/s",  &A[j]->m_LQvRqd,  this, j<NPri-1 ? isParm : 0);
                DDB.Double("Gasses", "",  DC_Qv, "L/s",  &A[j]->m_VQvRqd,  this, j<NPri-1 ? isParm : 0);
                }
              DDB.EndObject();
              }
            }
          //else if (j==(NPri-1))
          if (j<(NPri-1))
            DDB.Double("", "QvErr", DC_Qv,  "L/s", &A[j]->m_QvErr, this, isResult|0);
          }
      DDB.EndArray();
      break;
    case GSO_SpMassFracSplit:
      {
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
        for (j = 0;j<NPri; j++)
          {
          if (j>0)
            DDB.Page("..", DDB_OptPage);
          //IORECBUG
          DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
          A[j]->BuildDataDefnOrdPri(DDB, *this);
          int ns=A[j]->m_Splits.GetSize();
          if (PrjFileVerNo()<22)
            {
            for (int s=0; s<ns; s++)
              {
              Tt.Set("%s.Splt", SDB[s].Tag());
              Ts.Set("%s.Splt", SDB[s].Sym());
              DDB.Double(Tt(), Ts(),  DC_Frac, "%",  &A[j]->m_Splits[s],  this, j<NPri-1 ? isParm : 0);
              }
            }
          else
            {
            if (DDB.BeginObject(this, "Splt", "GSM_Splt", NULL, DDB_NoPage))
              {
              for (int s=0; s<ns; s++)
                DDB.Double(SDB[s].Tag(), SDB[s].Sym(),  DC_Frac, "%",  &A[j]->m_Splits[s],  this, j<NPri-1 ? isParm : 0);
              }
            DDB.EndObject();
            }
          }
      DDB.EndArray();
      }
      break;
    case GSO_SpMassFlowSplit:
      {
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
      for (j = 0;j<NPri; j++)
        {
        if (j>0)
          DDB.Page("..", DDB_OptPage);
        //IORECBUG
        DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
        A[j]->BuildDataDefnOrdPri(DDB, *this);
        if (j<NPri-1 || !DDB.ForView())
          {
          int ns=A[j]->m_QmRqds.GetSize();
          if (PrjFileVerNo()<22)
            {
            for (int s=0; s<ns; s++)
              {
              Tt.Set("%s.QmRqd", SDB[s].Tag());
              Ts.Set("%s.QmRqd", SDB[s].Sym());
              DDB.Double(Tt(), Ts(),  DC_Qm, "kg/s",  &A[j]->m_QmRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            }
          else
            {
            if (DDB.BeginObject(this, "QmRqd", "GSM_QmRqd", NULL, DDB_NoPage))
              {
              for (int s=0; s<ns; s++)
                DDB.Double(SDB[s].Tag(), SDB[s].Sym(),  DC_Qm, "kg/s",  &A[j]->m_QmRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            DDB.EndObject();
            }
          }
        //else if (j==(NPri-1))
        if (j<(NPri-1))
          DDB.Double("", "QmErr", DC_Qm,  "kg/s", &A[j]->m_QmErr, this, isResult|0);
        }
      DDB.EndArray();
      }
      break;
    case GSO_SpVolumeFlowSplit:
      {
      if (DDB.BeginArray(this, "IOs", "GSM_IOs", m_IOs.GetSize(), 0))
      for (j = 0;j<NPri; j++)
        {
        if (j>0)
          DDB.Page("..", DDB_OptPage);
        //IORECBUG
        DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
        A[j]->BuildDataDefnOrdPri(DDB, *this);
        if (j<NPri-1 || !DDB.ForView())
          {
          int ns=A[j]->m_QvRqds.GetSize();
          if (PrjFileVerNo()<22)
            {
            for (int s=0; s<ns; s++)
              {
              Tt.Set("%s.QvRqd", SDB[s].Tag());
              Ts.Set("%s.QvRqd", SDB[s].Sym());
              DDB.Double(Tt(), Ts(),  DC_Qv, "L/s",  &A[j]->m_QvRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            }
          else
            {
            if (DDB.BeginObject(this, "QvRqd", "GSM_QvRqd", NULL, DDB_NoPage))
              {
              for (int s=0; s<ns; s++)
                DDB.Double(SDB[s].Tag(), SDB[s].Sym(),  DC_Qv, "L/s",  &A[j]->m_QvRqds[s],  this, j<NPri-1 ? isParm : 0);
              }
            DDB.EndObject();
            }
          }
        //else if (j==(NPri-1))
        if (j<(NPri-1))
          DDB.Double("", "QvErr", DC_Qv,  "L/s", &A[j]->m_QvErr, this, isResult|0);
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
      if (NPri>=1) DDBSt[0].m_pStr=m_Nd.Nd_Rmt(I[0])->Tag();
      if (NPri>=2) DDBSt[1].m_pStr=m_Nd.Nd_Rmt(I[1])->Tag();

      if (DDB.BeginStruct(this, "Recovery", NULL, DDB_NoPage))
        {
        DDB.Byte  ("Phase",       "",  DC_,     "",  &m_iRecPh,   this, isParm, DDBPh);
        DDB.Byte  ("Stream",      "",  DC_,     "",  &m_iRecStrm, this, isParm, DDBSt);
        DDB.Double("Fraction",    "",  DC_Frac, "%", &m_dRecFrac, this, isParm);
        DDB.Double("Composition", "",  DC_Frac, "%", &m_dRecComp, this, isParm);
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
  switch (m_iOperation)
    {
    case GSO_MassFracSplit:
      {
      double Tot=0.0;
      for (j = 0;j<NPri-1; j++)
        {
        double &F=A[j]->m_FracSplit;
        F=Range(0.0, F, 1.0);
        F=Min(F, 1.0-Tot);
        Tot+=F;
        }
      A[NPri-1]->m_FracSplit=1.0-Tot;
      }
      break;
    case GSO_MassFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        {
        double &F=A[j]->m_Qm;
        F=Valid(F) ? Max(0.0, F) : 0.0;
        }
      }
      break;
    case GSO_VolumeFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        {
        double &F=A[j]->m_Qv;
        F=Valid(F) ? Max(0.0, F) : 0.0;
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
        double &SF=A[j]->m_SSplt;
        SF=Range(0.0, SF, 1.0);
        SF=Min(SF, 1.0-STot);
        STot+=SF;
        double &LF=A[j]->m_LSplt;
        LF=Range(0.0, LF, 1.0);
        LF=Min(LF, 1.0-LTot);
        LTot+=LF;
        double &VF=A[j]->m_VSplt;
        VF=Range(0.0, VF, 1.0);
        VF=Min(VF, 1.0-VTot);
        VTot+=VF;
        for (int i=0; i<CDB.PhaseCount(); i++)
          {
          if (m_iPhMethSpec==GSPM_Individual)
            if (!Valid(A[j]->m_PhSplt[i]))
              A[j]->m_PhSplt[i]=0;

          if (Valid(A[j]->m_PhSplt[i]))
            {
            double &F=A[j]->m_PhSplt[i];
            //F=Range(0.0, F, 1.0);
            F=Range(0.0, F, 1.0-PhTot[i]);
            PhTot[i]+=F;
            }
          else if (m_iPhMethSpec & GSPM_Total)
            PhTot[i]+=A[j]->m_Splt[CDB.PhaseInfo(i).m_eOcc];
          else
            PhTot[i]+=0.0;
          }
        }
      A[NPri-1]->m_SSplt=(1.0-STot);
      A[NPri-1]->m_LSplt=(1.0-LTot);
      A[NPri-1]->m_VSplt=(1.0-VTot);
      for (int i=0; i<CDB.PhaseCount(); i++)
        A[NPri-1]->m_PhSplt[i]=GEZ(1.0-PhTot[i]); // fix trunc/round errors
      }
      break;
    case GSO_PhMassFlowSplit:
      for (j = 0;j<NPri; j++)
        {
        A[j]->ValidateData(VDB, m_iPhMethSpec);
        double &SF=A[j]->m_SQmRqd;
        SF=Valid(SF) ? Max(0.0, SF) : 0.0;
        double &LF=A[j]->m_LQmRqd;
        LF=Valid(LF) ? Max(0.0, LF) : 0.0;
        double &VF=A[j]->m_VQmRqd;
        VF=Valid(VF) ? Max(0.0, VF) : 0.0;
        }
      break;
    case GSO_PhVolumeFlowSplit:
      for (j = 0;j<NPri; j++)
        {
        A[j]->ValidateData(VDB, m_iPhMethSpec);
        double &SF=A[j]->m_SQvRqd;
        SF=Valid(SF) ? Max(0.0, SF) : 0.0;
        double &LF=A[j]->m_LQvRqd;
        LF=Valid(LF) ? Max(0.0, LF) : 0.0;
        double &VF=A[j]->m_VQvRqd;
        VF=Valid(VF) ? Max(0.0, VF) : 0.0;
        }
      break;
    case GSO_SpMassFracSplit:
      {
      for (j = 0;j<NPri; j++)
        A[j]->m_Splits.SetSize(SDB.Count());
      for (int s=0; s<SDB.Count(); s++)
        {
        double Tot=0.0;
        for (j = 0;j<NPri-1; j++)
          {
          double &F=A[j]->m_Splits[s];
          F=Valid(F) ? Range(0.0, F, 1.0) : 0.0;
          F=Min(F, 1.0-Tot);
          Tot+=F;
          }
        A[NPri-1]->m_Splits[s]=(1.0-Tot);
        }
      }
      break;
    case GSO_SpMassFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        A[j]->m_QmRqds.SetSize(SDB.Count());
      for (int s=0; s<SDB.Count(); s++)
        {
        for (j = 0;j<NPri; j++)
          {
          double &F=A[j]->m_QmRqds[s];
          F=Valid(F) ? Max(0.0, F) : 0.0;
          }
        }
      }
      break;
    case GSO_SpVolumeFlowSplit:
      {
      for (j = 0;j<NPri; j++)
        A[j]->m_QvRqds.SetSize(SDB.Count());
      for (int s=0; s<SDB.Count(); s++)
        {
        for (j = 0;j<NPri; j++)
          {
          double &F=A[j]->m_QvRqds[s];
          F=Valid(F) ? Max(0.0, F) : 0.0;
          }
        }
      }
      break;
    case GSO_PhaseRecovery:
      m_iRecPh=Range((byte)0, m_iRecPh, (byte)2);
      m_iRecStrm=Range((byte)0, m_iRecStrm, (byte)1);
      m_dRecFrac=Range(0.0, m_dRecFrac, 1.0);
      m_dRecComp=Range(0.0, m_dRecComp, 1.0);
      break;
    }
  return OK;
  };

//--------------------------------------------------------------------------

int GSModel::FindIOEvalPrioritySplit(int *I, GSIOAlpha **A)
  {
  const int IOCount=m_IOs.GetSize();
  bool Done[MaxIOList];

  for (int ia = 0; ia < IOCount; ia++)
    Done[ia]=false;

  int NOrd=0;


  byte ThisPri=0;
  byte NextPri=0;
  while (NextPri!=AutoPriority)
    {
    ThisPri=NextPri;
    NextPri=AutoPriority;
    for (int ia = 0; ia < IOCount; ia++)
      {
      if (!Done[ia] && m_Nd.LnkFlowDefinedOut(m_IOs[ia]))
        {
        if (m_IOAlpha[ia].m_iPriority==ThisPri)
          {
          I[NOrd]=ia;
          A[NOrd]=&m_IOAlpha[ia];
          NOrd++;
          Done[ia]=true;
          }
        else if (m_IOAlpha[ia].m_iPriority>ThisPri)
          NextPri=Min(NextPri, m_IOAlpha[ia].m_iPriority);
        }
      }
    }


  for (ia = 0; ia < IOCount; ia++)
    {
    if (!Done[ia] && m_Nd.LnkFlowDefinedOut(m_IOs[ia]))
      {
      I[NOrd]=ia;
      A[NOrd]=&m_IOAlpha[ia];
      NOrd++;
      Done[ia]=true;
      }
    }

  for (int j = 0; j<NOrd; j++)
    {
    int ia=I[j];
    A[j]->m_iOrder=j+1;
    A[j]->m_Desc.Set("%s:%s", m_Nd.IODesc_Self(m_IOs[ia])->IOName(), m_Nd.Nd_Rmt(m_IOs[ia])->Tag());
    }

  return NOrd;
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOutSplit(long SolveMethod, double Press, SpConduit &Sd)
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

  switch (m_iOperation)
    {
    case GSO_MassFracSplit:
      {
      for (int j = 0;j<NPri; j++)
        m_Nd.IOConduit(I[j])->QSetF(Sd, som_ALL, A[j]->m_FracSplit, Sd.Press());
      }
      break;
    case GSO_MassFlowSplit:
      {
      double TotAv=Sd.QMass();
      double TotRqd=TotAv;
      for (int j=0; j<NPri-1; j++)
        {
        SpConduit & Cd=*m_Nd.IOConduit(I[j]);
        double F=A[j]->m_Qm;
        Cd.QSetM(Sd, som_ALL, Min(TotAv, F), Sd.Press());
        A[j]->m_QmErr=(Cd.QMass()-F);
        if (fabs(A[j]->m_QmErr)>1.0e-6)
          nShort++;
        TotAv=GEZ(TotAv-F);
        TotRqd=TotRqd-F;
        }
      m_Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, TotAv, Sd.Press());
      A[j]->m_QmErr=(TotRqd-TotAv);
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
        SpConduit & Cd=*m_Nd.IOConduit(I[j]);
        double F=A[j]->m_Qv*Dens;
        Cd.QSetM(Sd, som_ALL, Min(TotAv, F), Sd.Press());
        A[j]->m_QvErr=(Cd.QVolume()-A[j]->m_Qv);
        if (fabs(A[j]->m_QvErr)>1.0e-6)
          nShort++;
        TotAv=GEZ(TotAv-F);
        TotRqd=TotRqd-F;
        }
      m_Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, TotAv, Sd.Press());
      A[j]->m_QvErr=((TotRqd-TotAv)/GTZ(Dens));
      //if (fabs(A[j]->QmErr)>1.0e-6)
      //  nShort++;
      }
      break;
    case GSO_PhMassFracSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        SpConduit &Cd=*m_Nd.IOConduit(I[j]);
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
              Cd.QSetF(Sd, Which[c], Range(0.0, A[j]->m_Splt[c], 1.0), Sd.Press());
              SetReqd=false;
              }
            else
              Cd.QAddF(Sd, Which[c], Range(0.0, A[j]->m_Splt[c], 1.0));
            }
          }
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
        SpConduit & Cd=*m_Nd.IOConduit(I[j]);
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
        A[j]->m_QmErr=(Cd.QMass()-(SF+LF+VF));
        if (fabs(A[j]->m_QmErr)>1.0e-6)
          nShort++;
        VTotAv=GEZ(VTotAv-VF);
        TotRqd=TotRqd-VF;
        }
      m_Nd.IOConduit(I[NPri-1])->QSetM(Sd, som_Sol, STotAv, Sd.Press());
      m_Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Liq, LTotAv);
      m_Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Gas, VTotAv);
      A[j]->m_QmErr=(TotRqd-(STotAv+LTotAv+VTotAv));
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
        SpConduit & Cd=*m_Nd.IOConduit(I[j]);
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
        A[j]->m_QvErr=((Cd.QVolume()-(SF/GTZ(SDens)+LF/GTZ(LDens)+VF/GTZ(VDens))));
        if (fabs(A[j]->m_QvErr)>1.0e-6)
          nShort++;
        VTotAv=GEZ(VTotAv-VF);
        TotRqd=TotRqd-VF;
        }
      m_Nd.IOConduit(I[NPri-1])->QSetM(Sd, som_Sol, STotAv, Sd.Press());
      m_Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Liq, LTotAv);
      m_Nd.IOConduit(I[NPri-1])->QAddM(Sd, som_Gas, VTotAv);
      //TODO Fix This!
      A[j]->m_QvErr=(TotRqd-(STotAv+LTotAv+VTotAv));
      }
      break;
    case GSO_SpMassFracSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        SpConduit &Cn=*m_Nd.IOConduit(I[j]);
        Cn.QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'
        for (int s=0; s<SDB.Count(); s++)
          Cn.SetVMass(s, Sd, A[j]->m_Splits[s]*Sd.VMass[s]);
        //Cn.SetTempPress(Sd.Temp(), Sd.Press());
        }
      }
      break;
    case GSO_SpMassFlowSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        m_Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'
        A[j]->m_QmErr=0.0;
        }
      double TotErr=0.0;
      for (int s=0; s<SDB.Count(); s++)
        {
        double TotAv=Sd.VMass[s];
        double TotRqd=TotAv;
        for (j = 0;j<NPri-1; j++)
          {
          SpConduit & Cd=*m_Nd.IOConduit(I[j]);
          double F=A[j]->m_QmRqds[s];
          Cd.SetVMass(s, Cd, Min(TotAv, F));
          A[j]->m_QmErr+=(Cd.VMass[s]-F);
          TotAv=GEZ(TotAv-F);
          TotRqd=TotRqd-F;
          }
        m_Nd.IOConduit(I[NPri-1])->SetVMass(s, Sd, TotAv);
        TotErr+=TotAv-TotRqd;
        }
      for (j = 0;j<NPri-1; j++)
        if (fabs(A[j]->m_QmErr)>1.0e-6)
          nShort++;
      //for (int j = 0;j<NPri; j++)
      //  m_Nd.IOConduit(I[j])->SetTempPress(Sd.Temp(), Sd.Press());
      }
      break;
    case GSO_SpVolumeFlowSplit:
      {
      for (int j = 0;j<NPri; j++)
        {
        m_Nd.IOConduit(I[j])->QSetM(Sd, som_ALL, 1.0, Press); // Transfer Qualities etc'
        A[j]->m_QvErr=0.0;
        }
      double TotErr=0.0;
      for (int s=0; s<SDB.Count(); s++)
        {
        double Dens=SDB[s].Density(Sd.Fidelity(), Sd.Temp(), Sd.Press(), NULL, Sd.SVData());
        double TotAv=Sd.VMass[s];
        double TotRqd=TotAv;
        for (j = 0;j<NPri-1; j++)
          {
          SpConduit & Cd=*m_Nd.IOConduit(I[j]);
          double F=A[j]->m_QvRqds[s]*Dens;
          Cd.SetVMass(s, Sd, Min(TotAv, F));
          A[j]->m_QvErr+=((Cd.VMass[s]-F)/GTZ(Dens));
          TotAv=GEZ(TotAv-F);
          TotRqd=TotRqd-F;
          }
        m_Nd.IOConduit(I[NPri-1])->SetVMass(s, Sd, TotAv);
        TotErr+=TotAv-TotRqd;
        }
      for (j = 0;j<NPri-1; j++)
        if (fabs(A[j]->m_QvErr)>1.0e-6)
          nShort++;
      //for (int j = 0;j<NPri; j++)
      //  m_Nd.IOConduit(I[j])->SetTempPress(Sd.Temp(), Sd.Press());
      }
      break;
    case GSO_PhaseRecovery:
      {
      PhMask som_Phases[]={som_Sol, som_Liq, som_Gas};
      PhMask som_Phase, som_Other;
      som_Phase=som_Phases[m_iRecPh];
      som_Other=som_ALL&(~som_Phase);
      double MassPhase=Sd.QMass(som_Phase);
      double MassOther=Sd.QMass(som_Other);
      SpConduit &PCd=*m_Nd.IOConduit(I[m_iRecStrm]);
      SpConduit &SCd=*m_Nd.IOConduit(I[(m_iRecStrm+1)%2]);

      PCd.QSetM(Sd, som_Phase, MassPhase*m_dRecFrac, Press);
      SCd.QSetM(Sd, som_Phase, MassPhase*(1.0-m_dRecFrac), Press);

      double RqdOtherMass=(1.0-m_dRecComp)/GTZ(m_dRecComp)*MassPhase*m_dRecFrac;
      SetCI(2, m_bTrackStatus && QmInTotal>UsableMass && !InRange(0.0, RqdOtherMass, MassOther));
      RqdOtherMass=Range(0.0, RqdOtherMass, MassOther);

      PCd.QAddM(Sd, som_Other, RqdOtherMass);
      SCd.QAddM(Sd, som_Other, MassOther-RqdOtherMass);


//      case Range((byte)0, iRecPh, (byte)2);
//      iRecStrm=Range((byte)0, iRecStrm, (byte)1);
//      dRecFrac=Range(0.0, dRecFrac, 1.0);
//      dRecComp=Range(0.0, dRecComp, 1.0);
//      ?
      break;
      }
    }
  if (m_bTrackStatus && nShort && QmInTotal>UsableMass)
    SetCI(1, "W\tRequirements not met in %i stream%s", nShort, nShort>1 ? "s":"");
  else
    ClrCI(1);
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

template <class T> void SortPriority(T & IOA, int IOsSize)
  {
  bool FixRqd = false;
  for (int j=0; j<IOsSize && !FixRqd; j++)
    FixRqd = (IOA[j].m_iPriority!=AutoPriority && IOA[j].m_iPriority>=IOsSize-1);
  if (FixRqd)
    {
    int LastLow = -1;
    bool Fnd = true;
    while (Fnd)
      {
      int Low = 999;
      int LowIndex = -1;
      for (j=0; j<IOsSize; j++)
        {
        if (IOA[j].m_iPriority!=AutoPriority && IOA[j].m_iPriority>LastLow && IOA[j].m_iPriority<Low)
          Low = IOA[j].m_iPriority;
        }
      LastLow++;
      Fnd = (Low!=999);
      if (Fnd)
        {
        for (j=0; j<IOsSize; j++)
          {
          if (IOA[j].m_iPriority==Low)
            IOA[j].m_iPriority = LastLow;
          }
        }
      }
    }
  }

//--------------------------------------------------------------------------

void GSModel::CheckConfig()
  {
  if (m_bIOChanges)
    {
    //dbgpln("CheckConfig %s",m_Nd.FullObjTag());

    m_Gammas.SetSize(SDB.Count()+1);

    //dbgpln("IOCount   %i cf %i  %i %i",m_IOs.GetSize(), m_Nd.NoProcLnkIOs(), m_Nd.NoProcDirectIOs(), m_Nd.NoRealDirectIOs());

    m_NProds=0;
    const int IOsSize = m_IOs.GetSize();
    for (int i=0; i<IOsSize; i++)
      {
      int io=m_IOs[i];
      //dbgpln("m_IOAlpha i:%3i io:%3i Id:%5i Sz:%i", i, io, -1/*m_Nd.IOId_Self(io)*/, m_IOAlpha.GetSize());
      if (i>=m_IOAlpha.GetSize() || io>m_IOAlpha[i].m_IONo)
        {
        GSIOAlpha A;
        A.m_IONo=io;
        m_IOAlpha.InsertAt(i, A);
        GSIOThermal T;
        T.m_IONo=io;
        m_IOThermal.InsertAt(i, T);
        }
      else
        {
        while (m_IOAlpha[i].m_IONo>io && i<m_IOAlpha.GetSize())
          {
          m_IOAlpha.RemoveAt(i);
          m_IOThermal.RemoveAt(i);
          }
        }
      if (m_Nd.LnkFlowDefinedOut(m_IOs[i]))
        m_NProds++;
      }

    SortPriority(m_IOAlpha, IOsSize);
    SortPriority(m_IOThermal, IOsSize);

    ////fix priority numbers...
    //bool FixRqd = false;
    //for (int j=0; j<IOsSize && !FixRqd; j++)
    //  FixRqd = (m_IOAlpha[j].m_iPriority!=AutoPriority && m_IOAlpha[j].m_iPriority>=IOsSize-1);
    //if (FixRqd)
    //  {
    //  int LastLow = -1;
    //  bool Fnd = true;
    //  while (Fnd)
    //    {
    //    int Low = 999;
    //    int LowIndex = -1;
    //    for (j=0; j<IOsSize; j++)
    //      if (m_IOAlpha[j].m_iPriority!=AutoPriority && m_IOAlpha[j].m_iPriority>LastLow && m_IOAlpha[j].m_iPriority<Low)
    //        Low = m_IOAlpha[j].m_iPriority;
    //    LastLow++;
    //    Fnd = (Low!=999);
    //    if (Fnd)
    //      for (j=0; j<IOsSize; j++)
    //        if (m_IOAlpha[j].m_iPriority==Low)
    //          m_IOAlpha[j].m_iPriority = LastLow;
    //    }
    //  }

    m_bIOChanges=0;
    }
  }

//--------------------------------------------------------------------------

void GSModel::PrepareDefinedData()
  {
  };

// -------------------------------------------------------------------------

/*void GSModel::PostConnect(int IONo)
  {
  for (int i=0; i<m_IOs.GetSize(); i++)
    if (m_IOs[i]>=IONo)
      break;
  int Id=m_Nd.IOId_Self(IONo);
  if (Id>=IOId0 && Id<=IOIdN)
    {
    m_IOs.InsertAt(i, IONo);
    i++;
    }

  for (i ; i<m_IOs.GetSize(); i++)
    m_IOs[i]++;
  m_bIOChanges=1;
  CheckConfig();
  };*/

void GSModel::PostConnect(int IONo)
  {
  int Id = m_Nd.IOId_Self(IONo);
  //dbgp("GSModel::PostConnect io:%3i Id:%5i", IONo, Id);
  if (Id>=m_IOId0 && Id<=m_IOIdN)
    {
    //Check Length of IO ect
    int IOsLenRqd=0;
    for (int i=0; i<m_Nd.NoProcLnkIOs(); i++)
      {
      if (m_Nd.IOId_Self(i)>=m_IOId0 && m_Nd.IOId_Self(i)<=m_IOIdN)
        IOsLenRqd++;
      }

    if (m_IOs.GetCount()<IOsLenRqd)
      {

      //dbgp(" AddIt");
      ASSERT_ALWAYS(IONo<250, "Max of 250 IOs in a GenModel", __FILE__, __LINE__);
      int iPos=-1;
      for (int i=0; i<m_IOs.GetSize(); i++)
        if (m_IOs[i]>=IONo)
          break;

      m_IOs.InsertAt(i, IONo);
      //maybe instead of inserting new GSIOAlpha, check end of m_IOAlpha for unused item and use it so that "old/previous" values are used?
      GSIOAlpha A;
      A.m_IONo = m_IOs[i];
      m_IOAlpha.InsertAt(i, A);
      GSIOThermal T;
      T.m_IONo = m_IOs[i];
      m_IOThermal.InsertAt(i, T);
      i++;
      //}

      const int IOsSize = m_IOs.GetSize();
      if (i<IOsSize)
        {
        int ii = i;
        for (i ; i<m_IOs.GetSize(); i++)
          m_IOs[i]++;

        //shift everything up...
        const int AlphaSize = m_IOAlpha.GetSize();
        for (int j=ii; j<IOsSize && j<AlphaSize; j++)
          {
          m_IOAlpha[j].m_IONo = m_IOs[j];
          m_IOThermal[j].m_IONo = m_IOs[j];
          }
        }
      m_bIOChanges=1;
      }
    else
      {
      int XX=0;
      }
    }
  //dbgpln(" %s", m_Nd.FullObjTag());
  CheckConfig();
  };

// -------------------------------------------------------------------------

/*void GSModel::PreDisConnect(int IONo)
  {
  for (int i=0; i<m_IOs.GetSize(); i++)
    if (m_IOs[i]==IONo)
      {
      m_IOs.RemoveAt(i);
      for ( ; i<m_IOs.GetSize(); i++)
        m_IOs[i]--;
      }
  m_bIOChanges=1;
  CheckConfig();
  };*/

void GSModel::PreDisConnect(int IONo)
  {
  int Id = m_Nd.IOId_Self(IONo);
  //dbgp("GSModel::PreDisConnect io:%3i Id:%5i", IONo, Id);
  for (int i=0; i<m_IOs.GetSize(); i++)
    {
    if (m_IOs[i]==IONo)
      {
      //dbgp(" RemIt");
      m_IOs.RemoveAt(i);
      const int IOsSize = m_IOs.GetSize();
      if (i<IOsSize)
        {
        int ii = i;
        for ( ; i<IOsSize; i++)
          m_IOs[i]--;

        //shift everything down...
        GSIOAlpha A;
        A = m_IOAlpha[ii];
        const int AlphaSize = m_IOAlpha.GetSize();
        for (int j=ii; j<IOsSize && j<AlphaSize; j++)
          {
          m_IOAlpha[j] = m_IOAlpha[j+1];
          m_IOAlpha[j].m_IONo = m_IOs[j];
          }
        if (j<AlphaSize)
          {
          m_IOAlpha[j] = A;
          m_IOAlpha[j].m_IONo = AlphaSize-1;//m_IOs[j];
          }

        GSIOThermal T;
        T = m_IOThermal[ii];
        const int ThermalSize = m_IOThermal.GetSize();
        for (int j=ii; j<IOsSize && j<ThermalSize; j++)
          {
          m_IOThermal[j] = m_IOThermal[j+1];
          m_IOThermal[j].m_IONo = m_IOs[j];
          }
        if (j<ThermalSize)
          {
          m_IOThermal[j] = T;
          m_IOThermal[j].m_IONo = ThermalSize-1;//m_IOs[j];
          }

        }
      }
    }
  //dbgpln(" %s", m_Nd.FullObjTag());
  m_bIOChanges=1;
  CheckConfig();
  };

//--------------------------------------------------------------------------

void GSModel::BuildDataDefn(DataDefnBlk &DDB)
  {
  if (DDB.BeginStruct(this, "GM", NULL, DDB_NoPage))
    {
    //TODO Temp fix to force GS Model to CheckConfig() always!
    //KGA 4/6/98 : temporary fix: set flag (m_bIOChanges) to 1 to
    //force CheckConfig() to recalculate NProds. In theory m_bIOChanges
    //should be set after a link has been deleted or added so that
    //the access page for the 'general split model' changes its
    //spots when it should!
    m_bIOChanges=1; //KGA temp fix

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


    DDB.Byte   ("", "Operation",   DC_, "", xidGSOperation, this, isParmStopped | AffectsStruct | SetOnChange, (m_NProds==2) ? DDB0 : DDB1);
  //  DDB.Byte   ("", "Operation",   DC_, "", &m_iOperation, this, isParmStopped | AffectsStruct, DDB1);

    switch (m_iOperation)
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
#if WITHTHERMAL 
    DDB.Text(" ");
    DDB.Page("..", DDB_OptPage);
    DDB.CheckBoxBtn("", "Thermal",   DC_, "", xidGSThermal, this, isParmStopped | AffectsStruct | SetOnChange);//, (NProds==2) ? DDB0 : DDB1);
    if (m_bThermal)
      BuildDataDefnThermal(DDB);
#endif
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
        m_iOperation=*DCB.rB;
        switch (m_iOperation)
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
            for (int i=0; i<m_IOs.GetSize(); i++)
              if (m_Nd.LnkFlowDefinedOut(m_IOs[i]))
                {
                m_IOAlpha[i].m_Splits.SetSize(SDB.Count());
                m_IOAlpha[i].m_QmRqds.SetSize(SDB.Count());
                m_IOAlpha[i].m_QvRqds.SetSize(SDB.Count());
                }
            }
            break;
          case GSO_PhaseRecovery:
            {
            int N=0;
            for (int i=0; i<m_IOs.GetSize(); i++)
              if (m_Nd.LnkFlowDefinedOut(m_IOs[i]))
                N++;
            if (N!=2)
              m_iOperation=GSO_MassFracSplit;
            }
            break;
          }
        }
      DCB.B=m_iOperation;
      return 1;
    case xidGSPhMethSpec:
      if (DCB.rB)
        m_iPhMethSpec=*DCB.rB;
      DCB.B=m_iPhMethSpec;
      return 1;
#if WITHTHERMAL 
    case xidGSThermal:
      if (DCB.rB)
        m_bThermal=*DCB.rB!=0;
      DCB.B=m_bThermal;
      return 1;
#endif
    default:;
    }

  switch (m_iOperation)
    {
    case GSO_General0:
      if (DataXchgGen0(DCB))
        return true;
      break;
    case GSO_V5:
      if (DataXchgV5(DCB))
        return true;
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
      if (DataXchgSplit(DCB))
        return true;
      break;
    }

#if WITHTHERMAL 
  if (m_bThermal && DataXchgThermal(DCB))
    return true;
#endif

  return 0;
  }

//--------------------------------------------------------------------------

flag GSModel::ValidateData(ValidateDataBlk & VDB)
  {
  if (!TaggedObject::NumericTagsBad)
    {
    //KGA 6/5/2002 : For temp "fix", warn user about numeric tags in pipe names!
    const int IOCount=m_IOs.GetSize();
    for (int ia = 0; ia < IOCount; ia++)
      {
      const int io=m_IOs[ia];
      /*GSIOAlpha &A=m_IOAlpha[ia];
      QmMd[ia] = A.QmMode(*this);
      MkMd[ia] = A.MkMode(*this);
      Order[ia]=-1;
      Priority[ia]=-1;*/
      if (m_Nd.LnkFlowDefinedOut(io))
        //for (int j=0; j<NPri; j++)
        {
        Strng s;
        //s = m_Nd.Nd_Rmt(I[j])->Tag();
        s = m_Nd.Nd_Rmt(io)->Tag();
        flag Err=true;
        for (int i=0; i<s.Len(); i++)
          if (!isdigit(s[i]))
            Err=false;
        SetCI(4, Err);
        }
      }
    }

#if WITHTHERMAL 
  if (m_bThermal)
    ValidateDataThermal(VDB);
#endif

  switch (m_iOperation)
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

void GSModel::EvalProducts(long SolveMethod, double Press, SpConduit *pQf, CReactionBlock *pRB, double Flash_Press, CEnvironHX * pEHX)
  {
  const int IOCount=m_IOs.GetSize();

  SpConduit &Sd = (pQf!=NULL) ? *pQf : *m_Nd.GetWrkConduit("GSMdl", chLINEID());//SdLcl();

  // Measure Total inputs and outputs
  CSpMdlSlct Slct;
  double PMin=10000.0;
  for (int ia = 0; ia < IOCount; ia++)
    {
    int io=m_IOs[ia];
    if (m_Nd.IO_In(io))
      {
      PMin=Min(PMin, m_Nd.IOP_Self(io));
      Slct.Add(m_Nd.IOConduit(io)->Model(), m_Nd.IOQm_In(io));
      }
    }
  // Sum all  material from m_IOs to Workspace
  if (SolveMethod==SM_Direct)
    Press=PMin;
  Sd.QZero();
  Sd.SetPress(Press);
  Sd.SelectModel(Slct);
  int NFeeds=0;
  for (ia = 0; ia < IOCount; ia++)
    {
    int io=m_IOs[ia];
    if (m_Nd.IO_In(io))
      {
      Sd.QAddF(*m_Nd.IOConduit(io), som_ALL, 1.0);
      NFeeds++;
      }
    }

  dbgpln("Must Pass and Solve VLE");
  if (pRB)
    pRB->EvalProducts(Sd);

  // Environmental Exchange
  if (pEHX)
    pEHX->EvalProducts(Sd);

  EvalProductsOut(SolveMethod, Press, Sd);
  
  }

//--------------------------------------------------------------------------

void GSModel::EvalProductsOut(long SolveMethod, double Press, SpConduit &Sd)
  {
  switch (m_iOperation)
    {
    case GSO_General0:
      EvalProductsOutGen0(SolveMethod, Press, Sd);
      break;
    case GSO_V5:
      EvalProductsOutV5(SolveMethod, Press, Sd);
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
      EvalProductsOutSplit(SolveMethod, Press, Sd);
      break;
    }

#if WITHTHERMAL 
  if (m_bThermal)
    EvalProductsThermal();
#endif
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------

#if WITHTHERMAL 


GSIOThermal::GSIOThermal() 
  {
  m_iMode      = GST_None;

  m_iOrder     = AutoPriority;
  m_iPriority  = AutoPriority;

  m_TempReqd   = StdT;
  m_TempRise   = 0.0;
  m_TempDrop   = 0.0;
  m_TempOut    = StdT;

  m_bLast      = false;
  };

// ---------------------------------------------------------------------------

GSIOThermal::~GSIOThermal()
  {
  };

// ---------------------------------------------------------------------------

GSIOThermal & GSIOThermal::operator=(GSIOThermal & A)
  {
  m_IONo       = A.m_IONo;
  m_Desc       = A.m_Desc;
  m_iOrder     = A.m_iOrder; //kga 31/8/00
  m_iPriority  = A.m_iPriority; //kga 31/8/00
  m_iMode      = A.m_iMode;

  m_TempReqd   = A.m_TempReqd;
  m_TempRise   = A.m_TempRise;
  m_TempDrop   = A.m_TempDrop;
  m_TempOut    = A.m_TempOut;

  return *this;
  }

//--------------------------------------------------------------------------

void GSIOThermal::BuildDataDefnOrdPri(DataDefnBlk &DDB, GSModel & Mdl)
  {
  Strng StrVal;

  DDB.Byte   ("",             "Order",      DC_,    "",     &m_iOrder,          &Mdl, isResult|0);

  DDBValueLstMem DDBP;
  DDBP.Add(AutoPriority, "Auto");
  for (int i=0; i<Mdl.m_IOs.GetSize()-1; i++)
    {
    StrVal.Set("%i", i+1);
    DDBP.Add(i, StrVal());
    }
  DDB.Byte   ("",             "Priority",   DC_,    "",     &m_iPriority,       &Mdl, isParm, DDBP());
  }

//--------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------


void GSModel::BuildDataDefnThermal(DataDefnBlk &DDB)
  {
  DDB.Double     ("TempIn",         "", DC_T,   "C",    &m_TempIn,       this, 0);
  DDB.Double     ("FinalT.Min",     "", DC_T,   "C",    &m_FinalTempMin, this, isParm);
  DDB.Double     ("FinalT.Max",     "", DC_T,   "C",    &m_FinalTempMax, this, isParm);
  DDB.CheckBoxBtn("Change.Test",    "", DC_,    "",     &m_ChangeTst,    this, isParm|SetOnChange);
  DDB.Visibility(NSHM_All,  m_ChangeTst);
  DDB.Double     ("Change.MaxNeg",  "", DC_dT,  "C",    &m_ChangeNeg,    this, isParm);
  DDB.Double     ("Change.MaxPos",  "", DC_dT,  "C",    &m_ChangePos,    this, isParm);
  DDB.Visibility();
  DDB.CheckBoxBtn("AllowHX",        "", DC_,    "",     &m_bAllowHX,     this, isParm|SetOnChange);
  DDB.Visibility(NSHM_All,  m_bAllowHX);
  DDB.Double     ("PowerInput",     "", DC_Pwr, "kW",   &m_PowerIn,      this, 0);
  DDB.Visibility();

  Strng T, Tt, Ts;
  int j;
  int I[MaxIOList+1];
  GSIOThermal * A[MaxIOList+1];
  int NPri=FindIOEvalPriorityThermal(I, A);

  if (DDB.BeginArray(this, "TIOs", "GSM_TIOs", m_IOs.GetSize(), 0))
    {
    for (j = 0;j<NPri; j++)
      {
      //IORECBUG
      DDB.BeginElement(this, m_Nd.Nd_Rmt(I[j])->Tag(), m_Nd.IODesc_Self(I[j])->DefaultIOName(OrigPrjFileVerNo()));
      if (A[j]->Mode(this)!=GST_None || !m_bAllowHX || !DDB.ForView())
        A[j]->BuildDataDefnOrdPri(DDB, *this);

      bool CfgAble = j<NPri-1||m_bAllowHX;

      DDBValueLst DDB0[]={
        {GST_None,    "None"      },
        {GST_TReqd,   "Required"  },
        {GST_TRise,   "Rise"      },
        {GST_TDrop,   "Drop"      },
        {0}};

        DDB.Byte   ("",             "Mode",   DC_,    "",     xidGSTermMode+A[j]->m_IONo,         this, CfgAble ? (isParm | SetOnChange) : 0, DDB0);
      switch (A[j]->Mode(this))
        {
        case GST_TReqd:
          DDB.Double ("", "TempReqd", DC_T,  "C", &A[j]->m_TempReqd, this, CfgAble ? isParm : 0);
          break;
        case GST_TRise:
          DDB.Double ("", "TempRise", DC_dT, "C", &A[j]->m_TempRise, this, CfgAble ? isParm : 0);
          break;
        case GST_TDrop:
          DDB.Double ("", "TempDrop", DC_dT, "C", &A[j]->m_TempDrop, this, CfgAble ? isParm : 0);
          break;
        }

      DDB.Double ("", "TempOut", DC_T, "C", &A[j]->m_TempOut, this, 0);
      }
    }
  DDB.EndArray();
  }

//--------------------------------------------------------------------------

flag GSModel::DataXchgThermal(DataChangeBlk & DCB)
  {
  if (DCB.lHandle>=xidGSTermMode && DCB.lHandle<xidGSTermMode+MaxIOList)
    {
    int IONo = DCB.lHandle-xidGSTermMode;
    if (DCB.rB)
      m_IOThermal[IONo].m_iMode = *DCB.rB;
    DCB.B = m_IOThermal[IONo].Mode(this);
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

flag GSModel::ValidateDataThermal(ValidateDataBlk & VDB)
  {
  if (!m_bAllowHX)
    m_PowerIn = 0.0;
  m_FinalTempMin = Max(1.0, m_FinalTempMin  );
  m_FinalTempMax = Range(m_FinalTempMin, m_FinalTempMax, C2K(10000.0));
  m_ChangeNeg    = LEZ(m_ChangeNeg);
  m_ChangePos    = GEZ(m_ChangePos);

  flag OK=1;
  int I[MaxIOList+1];
  GSIOThermal * A[MaxIOList+1];
  int NPri=FindIOEvalPriorityThermal(I, A);
  //SetCI(3, NPri<1);
  if (NPri<1)
    return false;

  return true;
  }

//--------------------------------------------------------------------------

int GSModel::FindIOEvalPriorityThermal(int *I, GSIOThermal **A)
  {
  const int IOCount=m_IOs.GetSize();
  bool Done[MaxIOList];

  for (int ia = 0; ia < IOCount; ia++)
      Done[ia]=false;

  int NOrd=0;
  byte ThisPri=0;
  byte NextPri=0;
  while (NextPri!=AutoPriority)
    {
    ThisPri=NextPri;
    NextPri=AutoPriority;
    for (int ia = 0; ia < IOCount; ia++)
      {
      if (!Done[ia] && m_Nd.LnkFlowDefinedOut(m_IOs[ia]))// && (m_IOThermal[ia].m_iMode!=GST_None))
        {
        if (m_IOThermal[ia].m_iPriority==ThisPri)
          {
          I[NOrd]=ia;
          A[NOrd]=&m_IOThermal[ia];
          NOrd++;
          Done[ia]=true;
          }
        else if (m_IOThermal[ia].m_iPriority>ThisPri)
          NextPri=Min(NextPri, m_IOThermal[ia].m_iPriority);
        }
      }
    }


  for (ia = 0; ia < IOCount; ia++)
    {
    if (!Done[ia] && m_Nd.LnkFlowDefinedOut(m_IOs[ia]))
      {
      I[NOrd]=ia;
      A[NOrd]=&m_IOThermal[ia];
      NOrd++;
      Done[ia]=true;
      }
    }

  for (int j = 0; j<NOrd; j++)
    {
    //IORECBUG
    int ia=I[j];
    A[j]->m_iOrder=j+1;
    A[j]->m_bLast=(j==NOrd-1);
    A[j]->m_Desc.Set("%s:%s", m_Nd.IODesc_Self(m_IOs[ia])->IOName(), m_Nd.Nd_Rmt(m_IOs[ia])->Tag());
    }

  return NOrd;
  }

//--------------------------------------------------------------------------

class CTNoneFinder : public MRootFinderBase
  {
  friend class GSModel;
  public:
    CTNoneFinder(GSModel & GS, int * I, GSIOThermal ** A, int NPri) : m_GS(GS), m_I(I), m_A(A), m_NPri(NPri), MRootFinderBase("TNoneFinder", s_Tol)
      {
      m_NoneHf = 0.0;
      };
    LPCTSTR ObjTag() { return (LPCTSTR)m_GS.FullObjTag(); };
    double Function(double x)
      {
      // x is Temp
      m_NoneHf=0.0;
      for (int j = 0;j<m_NPri; j++)
        {
        SpConduit & Cd = *m_GS.m_Nd.IOConduit(m_I[j]);
        GSIOThermal & TOp = *m_A[j];
        if (TOp.Mode(&m_GS)==GST_None)
          {
          Cd.SetTempPress(x, m_GS.m_PressIn);
          m_NoneHf+=Cd.totHf();
          }
        }

      //dbgpln("RhoHFinder %8.6f %16.8f %16.8f %16.8f", x, pGrp->m_P_Applied, pGrp->m_Sum_dP, pGrp->m_P_Applied+pGrp->m_Sum_dP);
      return m_NoneHf;
      };

  protected:
    GSModel      & m_GS;
    int          * m_I;
    GSIOThermal ** m_A;
    int            m_NPri;
    double         m_NoneHf;
    
    static CToleranceBlock s_Tol;
  };

CToleranceBlock CTNoneFinder::s_Tol(TBF_BothSys, "GM:TempFinder", 0, 1.0e-9, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

//--------------------------------------------------------------------------

void GSModel::EvalProductsThermal()
  {
  dbgpln("GSModel::EvalProductsThermal");

  if (!m_bAllowHX)
    m_PowerIn = 0.0;

  int I[MaxIOList+1];
  GSIOThermal * A[MaxIOList+1];
  int NPri=FindIOEvalPriorityThermal(I, A);

  if (NPri<1)
    return;

  double BigFlow = 0;
  m_TempIn       = StdT;
  m_PressIn      = StdP;

  double totHf=0.0;
  for (int j = 0;j<NPri; j++)
    {
    SpConduit & Cd = *m_Nd.IOConduit(I[j]);
    totHf+=Cd.totHf();
    if (Cd.QMass()>BigFlow)
      {
      BigFlow=Cd.QMass();
      m_TempIn=Cd.Temp();
      m_PressIn=Cd.Press();
      }
    }

  bool ReqdOutOfRange   = false;
  bool AdjustOutOfRange = false;
  bool PowerOutOfRange  = false;

  double TMin = m_ChangeTst ? Max(m_FinalTempMin, m_TempIn + m_ChangeNeg) : m_FinalTempMin;
  double TMax = m_ChangeTst ? Min(m_FinalTempMax, m_TempIn + m_ChangePos) : m_FinalTempMax;

  int NFree=0;
  double RqdHf=0;
  for (int j = 0;j<NPri; j++)
    {
    SpConduit & Cd = *m_Nd.IOConduit(I[j]);
    GSIOThermal & TOp = *A[j];
    double PReqd = Cd.Press();
    double TReqd = Cd.Temp();
    switch (TOp.Mode(this))
      {
      case GST_TReqd:
        TReqd = TOp.m_TempReqd;
        break;
      case GST_TRise:
        TReqd =Cd.Temp()+TOp.m_TempRise;
        break;  
      case GST_TDrop:
        TReqd = Cd.Temp()-TOp.m_TempDrop;
        break;
      case GST_None:
        NFree++;
        continue;
      }

    double TSet=Range(TMin, TReqd, TMax);
    if (TSet!=TReqd)
      ReqdOutOfRange=true;

    Cd.SetTempPress(TSet, m_PressIn);
    RqdHf+=Cd.totHf();
    }

  m_PowerIn=0.0;
  double OtherHf=totHf-RqdHf;
  if (NFree>0)
    {
    CTNoneFinder TNF(*this, I, A, NPri);
    TNF.SetTarget(OtherHf);
    switch (TNF.Start(TMin, TMax))
      {
      case RF_OK:
        if (TNF.Solve_Brent()==RF_OK)
          break;
        m_PowerIn=totHf-TNF.m_NoneHf;
        if (!m_bAllowHX)
          AdjustOutOfRange=true;
        break;
      case RF_LoLimit:
        TNF.Function(TMin);
        m_PowerIn=totHf-TNF.m_NoneHf;
        if (!m_bAllowHX)
          AdjustOutOfRange=true;
        break;
      case RF_HiLimit:
        TNF.Function(TMax);
        m_PowerIn=totHf-TNF.m_NoneHf;
        if (!m_bAllowHX)
          AdjustOutOfRange=true;
        break;
      }
    }
  else if (fabs(OtherHf)>1e-6)
    {
    m_PowerIn=OtherHf;
    if (!m_bAllowHX)
      PowerOutOfRange =true;
    }

  for (int j = 0;j<NPri; j++)
    A[j]->m_TempOut = m_Nd.IOConduit(I[j])->Temp();

  SetCI(5, ReqdOutOfRange);
  SetCI(6, AdjustOutOfRange);
  SetCI(7, PowerOutOfRange);
  }

#endif

//--------------------------------------------------------------------------
//
//
//
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
    case  5: pS="W\tThermal Requirements Out of Range";   return 1;
    case  6: pS="W\tThermal Adjustments Out of Range";    return 1;
    case  7: pS="W\tPower Requirements Out of Range";     return 1;
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
  m_fEnabled=True;

  if (m_pGSM)
    return True;

  m_pGSM=new GSModel(&GSModelClass, "GM", m_pNd, TOA_Embedded);
  m_pNd->StructureChanged(NULL);
  m_pGSM->m_IOId0=m_MdlIOId0;
  m_pGSM->m_IOIdN=m_MdlIOIdN;

  //dbgpln("----- %s",pNd->FullObjTag());
  for (int i=0; i<m_pNd->NoProcLnkIOs(); i++)
    {
    int Id=m_pNd->IOId_Self(i);
    //dbgp("%3i Id:%5i %-25s", i, Id, pNd->Nd_Rmt(i)?pNd->Nd_Rmt(i)->FullObjTag():"NULL");
    if (Id>=m_MdlIOId0 && Id<=m_MdlIOIdN)
      {
      m_pGSM->m_IOs.Add(i);
      //dbgp(" Add");
      }
      //m_pGSM->m_IOs.InsertAt(i, i);
    //dbgpln("");
    }
  m_pGSM->m_bIOChanges=1;
  m_pGSM->CheckConfig();
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void GSModelBase::Close()
  {
  if (m_pGSM==NULL)
    return;
  m_pGSM->StructureChanged(NULL);
  m_fEnabled=False;
  };

//--------------------------------------------------------------------------

byte GSModelBase::QmMode(int iIONo)
  {
  int Priority[MaxIOList+1];
  int NPri0;

  switch (m_pGSM->m_iOperation)
    {
    case GSO_General0:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=m_pGSM->FindIOEvalPriorityGen0(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (m_pGSM->m_IOs[ia]==iIONo)
          return QmMd[ia];
      break;
      }
    case GSO_V5:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=m_pGSM->FindIOEvalPriorityV5(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (m_pGSM->m_IOs[ia]==iIONo)
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
      NPri0=m_pGSM->FindIOEvalPrioritySplit(Priority, A);
      break;
      }
    }

  return GSA_QmInValid;
  };

//--------------------------------------------------------------------------

byte GSModelBase::MkMode(int iIONo)
  {
  int Priority[MaxIOList+1];
  int NPri0;
  switch (m_pGSM->m_iOperation)
    {
    case GSO_General0:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=m_pGSM->FindIOEvalPriorityGen0(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (m_pGSM->m_IOs[ia]==iIONo)
          return MkMd[ia];
      break;
      }
    case GSO_V5:
      {
      byte QmMd[MaxIOList], MkMd[MaxIOList];
      NPri0=m_pGSM->FindIOEvalPriorityV5(Priority, QmMd, MkMd);
      int ia;
      for (int j = 0;((ia=Priority[j])>=0); j++)
        if (m_pGSM->m_IOs[ia]==iIONo)
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
      NPri0=m_pGSM->FindIOEvalPrioritySplit(Priority, A);
      break;
      }
    }

  return GSA_MkInValid;
  };

//--------------------------------------------------------------------------

GSGammaArray & GSModelBase::Gammas()
  {
  return m_pGSM->m_Gammas;
  };

//--------------------------------------------------------------------------

GSIOAlphaArray & GSModelBase::IOAlpha()
  {
  return m_pGSM->m_IOAlpha;
  };

//--------------------------------------------------------------------------

void GSModelBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
  DDB.CheckBoxBtn("",     "General_Model",  DC_,    "",     xidGSModelOn,  m_pNd, Flags, DDBOnOff);
  }

//--------------------------------------------------------------------------

void GSModelBase::BuildDataDefn(DataDefnBlk &DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, m_fEnabled);

  if (m_pGSM)
    DDB.Object(m_pGSM, m_pNd, NULL, NULL, DDB_RqdPage);//

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
        }
      DCB.B=(Enabled());
      return 1;
    }
  return 0;
  }

//============================================================================
//
//
//
//============================================================================
