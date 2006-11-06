//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"

#define  __CTRLSEQ_CPP
#include "ctrlseq.h"

#include "errorlog.h"
#include "dbgmngr.h"

#define dbgCtrlSeq     01
#if dbgCtrlSeq
static CDbgMngr dbgSequence("CtrlSeq", "Sequence");
static CDbgMngr dbgSolve("CtrlSeq", "Solve");
#endif

#ifndef __TAGOBJ_H
  #include "tagobj.h"
#endif  

XID xidNTearVars    = SysXID(100);
XID xidNTearHistory = SysXID(101);

//#include "optoff.h"

static CtrlSeqInfoArray   m_CSBs;
static CCriticalSection   m_CSBSect;

//===========================================================================

CtrlSeqInfoArray & CtrlSequencer::CSBs() { return m_CSBs; };
int  CtrlSequencer::NCSBs() { return m_CSBs.GetSize(); };
CtrlSeqBlk & CtrlSequencer::CSBlk(int i) { return *m_CSBs[i].pBlk; };

void CtrlSequencer::LockCSBs()   { m_CSBSect.Lock(); };
void CtrlSequencer::UnlockCSBs() { m_CSBSect.Unlock(); };

//===========================================================================
//
//
//
//===========================================================================

flag CtrlSeqBlk::fListChgd=false;
//static long s_nCtrlSeqBlks=0;

CtrlSeqBlk::CtrlSeqBlk()
  {
  fListChgd=true;

  CtrlSequencer::LockCSBs();
  CtrlSeqInfoArray & CSBs=CtrlSequencer::CSBs();
  int size=CSBs.GetSize();
  CSBs.SetSize(size+1);
  CSBs[size].pBlk=this;
  CtrlSequencer::UnlockCSBs();
  
  m_TuneOutLo=1;
  m_TuneOutHi=0;
  m_nGain=0;

  m_CtrlActive=true;
  m_TuneRqst=false;
  m_TuneBusy=0;
  m_Hold=false;
  m_HoldRqst=false;
  m_ExecCnt=1;
  m_ExecCntP=0;

  m_Tag.Set("Value");
  m_Sym.Set("V");

  //dbgpln("CtrlSeqBlk %i [%i]", ++s_nCtrlSeqBlks, CtrlSequencer::NCSBs());
  }

// ------------------------------------------------------------------------

CtrlSeqBlk::~CtrlSeqBlk() 
  { 

  fListChgd=true;
  CtrlSequencer::LockCSBs();
  CtrlSeqInfoArray & CSBs=CtrlSequencer::CSBs();
  int size=CSBs.GetSize();
  for (int i=0; i<size; i++)
    if (CSBs[i].pBlk==this)
      break;
  for ( ; i<size-1; i++)
    CSBs[i]=CSBs[i+1];
  CSBs.SetSize(size-1);
  CtrlSequencer::UnlockCSBs();

  //dbgpln("~CtrlSeqBlk %i [%i]", s_nCtrlSeqBlks--, CtrlSequencer::NCSBs());
  }

// ------------------------------------------------------------------------

flag CtrlSeqBlk::InitTuneRqd()
  {
  flag Rqd=TuneOutHi()<TuneOutLo();
  #if dbgCtrlSeq
  if (dbgSolve())
    if (Rqd)
      dbgpln("INITTUNERQD: %s", m_Tag());
  #endif
  return Rqd;
  };

// ------------------------------------------------------------------------

flag CtrlSeqBlk::TuneRqd(double Tol)
  {
  if (!CtrlSequencer::m_fAllowAutoRetune)
    return false;
#if !MultiGainValues
  flag Rqd=(OutputN() < TuneOutLo()-Tol) || (OutputN() > TuneOutHi()+Tol);
#else
  Tol=1.0/TV_NGainSegs;
  flag Rqd=(OutputN() < TuneOutLo()-Tol) || (OutputN() > TuneOutHi()+Tol);
#endif
  //double D=1.0/TV_NGainSegs;
  //double D=EqnSlv().m_RetuneOutDelta;//1.0/TV_NGainSegs;
  //flag Rqd=(OutputN() < TuneOutLo()-Tol) || (OutputN() > TuneOutHi()+Tol);
  #if dbgCtrlSeq
  if (dbgSolve())
    if (Rqd)
      dbgpln("TuneRqd: D:%10.4f   Lo:%10.4f O:%10.4f Hi:%10.4f %s",
              Tol, TuneOutLo()-Tol, OutputN(), TuneOutHi()+Tol, m_Tag());
  #endif
  return Rqd;
  };

// ------------------------------------------------------------------------

void CtrlSeqBlk::ClearGain()
  {
  m_nGain=0;
  m_TuneOutLo=1;
  m_TuneOutHi=0;
  //for (int i=0; i<TV_MaxEstDOut; i++)
  //  m_EstDOut[i]=0.0;
  }

// ------------------------------------------------------------------------

flag CtrlSeqBlk::GainOK() { return m_nGain>0; }
/*
// ------------------------------------------------------------------------
#if MultiGainValues 
flag CtrlSeqBlk::GetGain(double O, double &P, double &R)
  {
  int nGain=m_nGain;
  TVC_GainArray &Gn=m_Gain;
  //CDArray &Ic=m_IntConst;
  //CDArray &GnO=m_GainO;
  switch (nGain)
    {
    case 0:
      P=10000;
      R=100.0;
      return false; // No Data
    case 1:
      P=Gn[0].m_PropBand;
      R=Gn[0].m_ResetTime;
      return true;
    default:
      {
      for (int i=1; i<nGain-1; i++)
        if (O<=Gn[i].m_Out)
          break;
      TVC_GainItem & G0=m_Gain[i-1];
      TVC_GainItem & G1=m_Gain[i];

      P=G0.m_PropBand+(O-G0.m_Out)/(G1.m_Out-G0.m_Out)*(G1.m_PropBand-G0.m_PropBand);
      R=G0.m_ResetTime+(O-G0.m_Out)/(G1.m_Out-G0.m_Out)*(G1.m_ResetTime-G0.m_ResetTime);

      if (G0.m_PropBand<G1.m_PropBand)
        P=Range(G0.m_PropBand, P, G1.m_PropBand);
      else
        P=Range(G1.m_PropBand, P, G0.m_PropBand);
      if (G0.m_ResetTime<G1.m_ResetTime)
        R=Range(G0.m_ResetTime, R, G1.m_ResetTime);
      else
        R=Range(G1.m_ResetTime, R, G0.m_ResetTime);
dbgpln("Gain: O:%10.4f P:%10.4f R:%10.4f", O, P, R);
      return true;
      }
    }
  };
#else
  */
flag CtrlSeqBlk::GetEstGain(double &G, double &I, double &D)
  {
  if (m_nGain==0)
    {
    G=1.0e-6;
    I=100.0;
    D=100.0;
    return false; // No Data
    }

  G=m_EstGain.m_Gain;
  I=m_EstGain.m_Ic;
  D=m_EstGain.m_Dc;
  return true;
  };

// ------------------------------------------------------------------------
/*
#if MultiGainValues 
int CtrlSeqBlk::SetGain(double O, double P, double R, double PG, double PD, double PP)
  {
  int &nGain=m_nGain;
  TVC_GainArray &Gn=m_Gain;
//  CDArray &Gn=m_Gain;
//  CDArray &Ic=m_IntConst;
//  CDArray &GnO=m_GainO;

  switch (nGain)
    {
    case 0: 
      nGain++;
#if !MultiGainValues
    case 1: 
#endif
      Gn[0].m_PropBand=P;
      Gn[0].m_ResetTime=R;
      Gn[0].m_PlantGain=PG;
      Gn[0].m_PlantDelay=PD;
      Gn[0].m_PlantPeriod=PP;
      Gn[0].m_Out=O;
      break;
    default:
      {
      if (nGain>=Gn.GetSize())
        DoBreak();
      for (int i=0; i<nGain && O>Gn[i].m_Out; i++) 
        {};
      for (int j=nGain; j>i; j--)
        //{
        Gn[j]=Gn[j-1];
        //Ic[j]=Ic[j-1];
        //GnO[j]=GnO[j-1];
        //}
      nGain++;
      Gn[i].m_PropBand=P;
      Gn[i].m_ResetTime=R;
      Gn[i].m_PlantGain=PG;
      Gn[i].m_PlantDelay=PD;
      Gn[i].m_PlantPeriod=PP;
      Gn[i].m_Out=O;
//      Gn[i]=G;
//      Ic[i]=I;
//      GnO[i]=O;
      }
    }
  return m_nGain;
  };
#else
  */
int CtrlSeqBlk::SetEstGain(double G, double I, double D, double PG, double PD, double PP)
  {
  m_nGain=1;
  m_EstGain.m_Gain=G;
  m_EstGain.m_Ic=I;
  m_EstGain.m_Dc=D;
  m_EstGain.m_PlantGain=PG;
  m_EstGain.m_PlantDelay=PD;
  m_EstGain.m_PlantPeriod=PP;
  //m_Gain.m_Out=O;
  return m_nGain;
  };

// ------------------------------------------------------------------------

int CtrlSeqBlk::SetEstGain(double G, double I, double D)
  {
  m_nGain=1;
  m_EstGain.m_Gain=G;
  m_EstGain.m_Ic=I;
  m_EstGain.m_Dc=D;
  //m_EstGain.m_Out=O;
  return m_nGain;
  };
//#endif

// ------------------------------------------------------------------------

#if MultiGainValues 
flag CtrlSeqBlk::GetPlant(double O, double &G, double &D, double &P)
  {
  int nGain=m_nGain;
  TVC_GainArray &Gn=m_EstGain;
  //CDArray &Ic=m_IntConst;
  //CDArray &GnO=m_EstGainO;
  switch (nGain)
    {
    case 0:
      G=0;
      D=0;
      P=0;
      return false; // No Data
    case 1:
      G=Gn[0].m_PlantGain;
      D=Gn[0].m_PlantDelay;
      P=Gn[0].m_PlantPeriod;
      return true;
    default:
      {
      for (int i=1; i<nGain-1; i++)
        if (O<=Gn[i].m_Out)
          break;
      TVC_GainItem & G0=m_Gain[i-1];
      TVC_GainItem & G1=m_Gain[i];

      G=G0.m_PlantGain+(O-G0.m_Out)/(G1.m_Out-G0.m_Out)*(G1.m_PlantGain-G0.m_PlantGain);
      D=G0.m_PlantDelay+(O-G0.m_Out)/(G1.m_Out-G0.m_Out)*(G1.m_PlantDelay-G0.m_PlantDelay);
      P=G0.m_PlantPeriod+(O-G0.m_Out)/(G1.m_Out-G0.m_Out)*(G1.m_PlantPeriod-G0.m_PlantPeriod);

//      if (G0.m_PropBand<G1.m_PropBand)
//        P=Range(G0.m_PropBand, P, G1.m_PropBand);
//      else
//        P=Range(G1.m_PropBand, P, G0.m_PropBand);
//      if (G0.m_ResetTime<G1.m_ResetTime)
//        R=Range(G0.m_ResetTime, R, G1.m_ResetTime);
//      else
//        R=Range(G1.m_ResetTime, R, G0.m_ResetTime);
      #if dbgCtrlSeq
      if (dbgSolve())
        dbgpln("Plant: O:%10.4f G:%10.4f D:%10.4f P:%10.4f", O, G, D, P);
      #endif
      return true;
      }
    }
  };
#else
flag CtrlSeqBlk::GetPlant(double &G, double &D, double &P)
  {
  if (m_nGain==0)
    {
    G=0;
    D=0;
    P=0;
    return false; // No Data
    }
  G=m_EstGain.m_PlantGain;
  D=m_EstGain.m_PlantDelay;
  P=m_EstGain.m_PlantPeriod;
  return true;
  };
#endif

//// ------------------------------------------------------------------------
//
//int CtrlSeqBlk::SetGain(double O, double P, double R)
//  {
//  /**/
//  int &nGain=m_nGain;
//  TVC_GainArray &Gn=m_Gain;
////  CDArray &Gn=m_Gain;
////  CDArray &Ic=m_IntConst;
////  CDArray &GnO=m_GainO;
//  switch (nGain)
//    {
//    case 0: 
//      Gn[0].m_PropBand=P;
//      Gn[0].m_ResetTime=R;
//      Gn[0].m_Out=O;
//      nGain++;
//      break;
//    default:
//      {
//      if (nGain>=Gn.GetSize())
//        DoBreak();
//      for (int i=0; i<nGain && O>Gn[i].m_Out; i++) 
//        {};
//      for (int j=nGain; j>i; j--)
//        //{
//        Gn[j]=Gn[j-1];
//        //Ic[j]=Ic[j-1];
//        //GnO[j]=GnO[j-1];
//        //}
//      nGain++;
//      Gn[i].m_PropBand=P;
//      Gn[i].m_ResetTime=R;
//      Gn[i].m_Out=O;
////      Gn[i]=G;
////      Ic[i]=I;
////      GnO[i]=O;
//      }
//    }
//  /**/
//  return m_nGain;
//  };
/*
const dword CSW_On        = 0x00000001;
const dword CSW_AutoMan   = 0x00000002;
const dword CSW_Active    = 0x00000004;
const dword CSW_Hold      = 0x00000008;
const dword CSW_TuneBusy  = 0x00000010;
const dword CSW_TuneRqst  = 0x00000020;
*/

// ------------------------------------------------------------------------

dword CtrlSeqBlk::StatusWord() 
  { 
  dword dw=0;
  if (On()) dw |= CSW_On;      
  if (AutoMan()) dw |= CSW_AutoMan;
  if (CtrlActive()) dw |= CSW_Active;
  if (Hold()) dw |= CSW_Hold;
  if (TuneRqst()) dw |=CSW_TuneRqst;
  if (TuneBusy()==1) dw |= CSW_TuneBusy1;
  if (TuneBusy()==2) dw |= CSW_TuneBusy2;
  return dw;
  }

// ------------------------------------------------------------------------

void CtrlSeqBlk::SetStatusWord(dword Values, dword Mask) 
  { 
  if (Mask & CSW_On)
    On() = ((Values&CSW_On)!=0);
  if (Mask & CSW_AutoMan)
    {
    if (Values&CSW_AutoMan)
      AutoMan() = true;
    else
      {
      AutoMan() = false;
      SetTuneRqst(false);
      SetTuneBusy(0);
      }
    }
  if (Mask & CSW_TuneRqst)
    {
    if (Values&CSW_TuneRqst)
      SetTuneRqst(true && AutoMan());
    else
      {
      SetTuneRqst(false);
      SetTuneBusy(0);
      }
    }
  };

//==========================================================================
//
//
//
//==========================================================================

flag CtrlSequencer::m_fAllowAutoRetune=false;
dword CtrlSequencer::dwExecCount=1;
    
CtrlSequencer::CtrlSequencer()
  {
  m_MinSettleCount=5;//25;
  m_MaxSettleCount=1000;
  
  m_MaxPBDerate=3.0;
  m_RetuneOutDelta=0.2;
  m_TestDelta=0.01;

  m_iSequence=SQN_Start;
  m_iCount=-1;
  m_iVar=-1;

  m_fSequencerActive=false;
  m_fSequencerWasActive=false;

  m_fHoldAll=false;
  m_fTuneWithHold=true; 

  m_SlopePrior=0.0;
  m_ScaleLimit=5.0;
  };

//--------------------------------------------------------------------------

CtrlSequencer::~CtrlSequencer()
  {
  };

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

void CtrlSequencer::BeginStep()//EqnSlvCtrlBlk & EqnCB)
  {
  };

//--------------------------------------------------------------------------

void CtrlSequencer::EndStep()//EqnSlvCtrlBlk & EqnCB)
  {
  };

//--------------------------------------------------------------------------

void CtrlSequencer::Initialise()//EqnSlvCtrlBlk & EqnCB)
  {
//  EqnCB.SetNUnknowns(0);

  //m_ToSolve.SetSize(0);
  m_nSolveVars=0;
  //CtrlSeqBlkIter CSBs(CtrlSeqBlk::List);
  //for (CtrlSeqBlk* pT=CSBs.First(); pT; pT=CSBs.Next())
  for (int i=0; i<NCSBs(); i++)
    {
    //m_ToSolve.Add(pT);
    m_nSolveVars++;
    CtrlSeqBlk & Ti=CSBlk(i);
    Ti.m_InitHold=true;
    Ti.SetExecCnt(1);
    Ti.SetExecCntP(0);
    m_CSBs[i].m_Alpha.SetSize(NCSBs());
    }
  m_iSequence=SQN_Start;

  //m_Output.SetSize(m_nSolveVars);
  //m_ErrorChg.SetSize(m_nSolveVars);
  //m_ErrorChgPos.SetSize(m_nSolveVars);
  //m_ErrorChgNeg.SetSize(m_nSolveVars);
  //m_ErrorChgAvg.SetSize(m_nSolveVars);

  //m_Error.SetSize(m_nSolveVars);
  //m_PrevError.SetSize(m_nSolveVars);
  //m_OutMemory.SetSize(m_nSolveVars);

  //m_SettleIters.SetSize(m_nSolveVars);
  //m_DelayIters.SetSize(m_nSolveVars);

  //m_AlphaLinear.SetSize(m_nSolveVars);

  //m_Alpha.SetSizes(m_nSolveVars, m_nSolveVars);
  m_iCount=-1;
  m_iVar=-1;
  }

//--------------------------------------------------------------------------

void CtrlSequencer::SetOutput(long iVar)
  {
  CtrlSeqBlk & T=*m_CSBs[iVar].Blk();

  if (Valid(T.TuneOut()))
    {
    double O=(T.TuneOut()-T.OutMin())/T.OutSpan();
    #if dbgCtrlSeq
    if (dbgSolve())
      dbgpln("SET  : %s   O:%10.4f >> %10.4f", T.m_Tag(), T.OutputN(), O);
    #endif
    T.SetOutputN(O);
    }
  }

//--------------------------------------------------------------------------

void CtrlSequencer::TweakOutput(long iVar)
  {
  CtrlSeqBlk & T=*m_CSBs[iVar].Blk();
  double D=(T.MeasureN()<0.5) ? m_TestDelta : -m_TestDelta;
  m_CSBs[iVar].m_OutMemory=T.OutputN();

  m_TestOutput=T.OutputN()+D;
  m_TestOutChg=D;
  m_CSBs[iVar].m_DelayIters=0;
  m_CSBs[iVar].m_SettleIters=0;

  #if dbgCtrlSeq
  if (dbgSolve())
    dbgpln("TWEAK: %s   O:%10.4f >> %10.4f", T.m_Tag(), T.OutputN(), m_TestOutput);
  #endif
  T.SetOutputN(m_TestOutput);
  }

//--------------------------------------------------------------------------

void CtrlSequencer::RestoreOutput(long iVar)
  {
  }

//--------------------------------------------------------------------------

void CtrlSequencer::ResetHistory()
  {
  for (long e=0; e<m_CSBs.GetSize(); e++)
    {
    CtrlSeqBlk & Tsb=*m_CSBs[e].Blk();
    Tsb.m_ErrHistPrior.SetSize(0, 64);
    Tsb.m_ErrHistPost.SetSize(0, 64);
    }
  }

//--------------------------------------------------------------------------

void CtrlSequencer::CollectHistoryPrior()
  {
  for (long e=0; e<NCSBs(); e++)
    {
    CtrlSeqBlk & Tsb=CSBlk(e);
    double E=Tsb.ErrorN();
    Tsb.m_ErrHistPrior.Add(E);
    }
  }

//--------------------------------------------------------------------------

void CtrlSequencer::CollectHistoryPost()
  {
  for (long e=0; e<NCSBs(); e++)
    {
    CtrlSeqBlk & Tsb=CSBlk(e);
    double E=Tsb.ErrorN();
    Tsb.m_ErrHistPost.Add(E);
    }
  }

//--------------------------------------------------------------------------

int  CtrlSequencer::AnalyseHistory(long iVar, TVC_GainItem & GI)
  {
  int i,j,k, ret=1;
  CtrlSeqBlk & Tsb=*m_CSBs[iVar].Blk();
  //TearVar & TV=T.TV[0];
  CDArray &HP=Tsb.m_ErrHistPrior;
  CDArray &H=Tsb.m_ErrHistPost;
  double HMin=1e100;
  double HMax=-1e100;
  for (i=0; i<HP.GetSize(); i++)
    {
    HMin=Min(HMin, HP[i]);
    HMax=Max(HMax, HP[i]);
    }
  for (i=0; i<H.GetSize(); i++)
    {
    HMin=Min(HMin, H[i]);
    HMax=Max(HMax, H[i]);
    }
  
  double E0=H[0];
  double EN=H[H.GetUpperBound()];
  double EChg=EN-E0;
  GI.m_PlantGain=EChg/m_TestOutChg;
  GI.m_PlantDelay=0;
  GI.m_PlantPeriod=0;
  double Ratio=0;

  int    iMaxDiff=-1;
  double MaxADiff=0;
  for (i=1; i<H.GetSize(); i++)
    {
    double Diff=H[i]-H[i-1];
    if (fabs(Diff)>MaxADiff)
      {
      iMaxDiff=i;
      MaxADiff=fabs(Diff);
      }
    }

  #if dbgCtrlSeq
  if (dbgSolve())
    for (int l=0; l<2; l++)
      {
      CDArray & rH = l==0 ? HP : H;
      for (i=0; i<rH.GetSize(); i++)
        {
        dbgp("+%3i) %10.4f", i, rH[i]);
        if (i>0)
          {
          double Diff=rH[i]-rH[i-1];
          dbgp(" %10.4f  %3s", Diff, (i==iMaxDiff && l>0) ? "<->":"");
          }
        else
          dbgp(" %10s  %3s", "", "");

        j=(int)(20*(rH[i]-HMin)/NZ(HMax-HMin));
        k=20-j;
        dbgp(" |");
        if (j)
          dbgp("%*s", j, "");
        dbgp("*");
        if (k)
          dbgp("%*s", k, "");
        dbgpln("|");
        }
      }
  #endif
  
  if (iMaxDiff>=0)
    {
    int i=iMaxDiff;
    int j=iMaxDiff-1;

    double Ti=1+i;
    double Tj=1+j;
    double Ei=H[i];
    double Ej=H[j];
    double Tx=Ti+(E0-Ei)*(Ti-Tj)/(Ei-Ej);
    double Ty=Ti+(EN-Ei)*(Ti-Tj)/(Ei-Ej);

    #if dbgCtrlSeq
    if (dbgSolve())
      {
      dbgpln("Ti %10.4f", Ti);
      dbgpln("Tj %10.4f", Tj);
      dbgpln("Ei %10.4f", Ei);
      dbgpln("Ej %10.4f", Ej);
      dbgpln("Tx %10.4f", Tx);
      dbgpln("Ty %10.4f", Ty);
      }
    #endif
    GI.m_PlantDelay=Tx;
    GI.m_PlantPeriod=Ty-Tx;
    Ratio=GI.m_PlantPeriod/GI.m_PlantDelay;
    ret=0;
    }

  #if dbgCtrlSeq
  if (dbgSolve())
    {
    dbgpln("PlantGain %10.4f", GI.m_PlantGain);
    dbgpln("Delay     %10.4f", GI.m_PlantDelay);
    dbgpln("Period    %10.4f", GI.m_PlantPeriod);
    dbgpln("Ratio     %10.4f", Ratio);
    dbgpln("Return    %i", ret);
    }
  #endif
  return ret;
  }

//--------------------------------------------------------------------------

void CtrlSequencer::ClearErrors()
  {
  for (long e=0; e<NCSBs(); e++)
    {
    CtrlSeqBlk & T=CSBlk(e);
    m_CSBs[e].m_Error=T.ErrorN();
    m_CSBs[e].m_ErrorChgPos=0.0;
    m_CSBs[e].m_ErrorChgNeg=0.0;
    m_CSBs[e].m_ErrorChgAvg=0.0;
    }
  m_iTestErrChgCnt=0;
  }

//--------------------------------------------------------------------------

void CtrlSequencer::MonitorErrors(long iVar)
  {
  for (long e=0; e<NCSBs(); e++)
    if (CSBlk(e).CtrlActive())
      {
      double ErrChg=CSBlk(e).ErrorN()-m_CSBs[e].m_Error;
      m_CSBs[e].m_ErrorChgPos=Max(m_CSBs[e].m_ErrorChgPos, ErrChg);
      m_CSBs[e].m_ErrorChgNeg=Min(m_CSBs[e].m_ErrorChgNeg, ErrChg);
      m_CSBs[e].m_ErrorChgAvg=(m_iTestErrChgCnt*m_CSBs[e].m_ErrorChgAvg+ErrChg)/(m_iTestErrChgCnt+1);
      }
  m_iTestErrChgCnt++;

  double E=fabs(CSBlk(iVar).ErrorN()-m_CSBs[iVar].m_Error);
  if (E>0.01*m_TestDelta && m_CSBs[iVar].m_DelayIters==0)
    m_CSBs[iVar].m_DelayIters=m_iCount;
  }

//--------------------------------------------------------------------------

flag CtrlSequencer::TestSettledPrior(long iVar)
  {
  m_MinSettleCount=Max(5L,m_MinSettleCount);
  //m_ToSolve[iVar]->SetTuneBusy(true);
  flag OK=false;
  if (m_iCount > m_MaxSettleCount)
    OK=true;
  else if (m_iCount >= m_MinSettleCount)
    {
    OK=true;
    
    CtrlSeqBlk & Tsb=*m_CSBs[iVar].Blk();
    CDArray &EH=Tsb.m_ErrHistPrior;

    ASSERT_ALWAYS(EH.GetSize()>=m_MinSettleCount, "Bad Error History");
    int EHN=EH.GetSize();
    int N=Min(1+m_MinSettleCount/2L, EHN/2L);
    

    LeastSquareFit LSF;
    for (int p=EHN-2*N+1; p<=EHN-N; p++)
      LSF.AddPt(p, EH[p]);

    double Slope0=LSF.Param(1,1); // ParamNo, Order; 

    LSF.Initialise();
    for (p=EHN-N; p<EHN; p++)
      LSF.AddPt(p, EH[p]);

    double Slope1=LSF.Param(1,1); // ParamNo, Order; 
   
    if (ConvergedVV(Slope0, Slope1, 1.0e-6, 1.0e-4))
      m_SlopePrior=Slope1;
    else
      OK=false;
    }

  for (long e=0; e<NCSBs(); e++)
     m_CSBs[e].m_PrevError=CSBlk(e).ErrorN();
  return OK;
  }

//--------------------------------------------------------------------------

flag CtrlSequencer::TestSettledPost(long iVar)
  {
  //m_ToSolve[iVar]->SetTuneBusy(true);
  flag OK=false;
  if (m_iCount > m_MaxSettleCount)
    OK=true;
  else if (m_iCount >= m_MinSettleCount)
    {
    OK=true;

    CtrlSeqBlk & Tsb=*m_CSBs[iVar].Blk();
    CDArray &EH=Tsb.m_ErrHistPost;

    ASSERT_ALWAYS(EH.GetSize()>=m_MinSettleCount, "Bad Error History");
    LeastSquareFit LSF;
    for (int p=EH.GetSize()-m_MinSettleCount; p<EH.GetSize(); p++)
      LSF.AddPt(p, EH[p]);

    double OffSt=LSF.Param(0,1); // ParamNo, Order; 
    double Slope=LSF.Param(1,1); // ParamNo, Order; 
    if (!ConvergedVV(Slope, m_SlopePrior, 1.0e-6, 1.0e-4))
      OK=false;

    }

  for (long e=0; e<NCSBs(); e++)
     m_CSBs[e].m_PrevError=CSBlk(e).ErrorN();
  return OK;
  }

//--------------------------------------------------------------------------

void CtrlSequencer::CalcAlpha(long iVar)
  {
  CtrlSeqBlk & Ti=*m_CSBs[iVar].Blk();

  double A;
  for (long e=0; e<NCSBs(); e++)
    if (CSBlk(e).CtrlActive())
      {
      m_CSBs[e].m_ErrorChg=CSBlk(e).ErrorN()-m_CSBs[e].m_Error;
      if (0)
        {
        if (m_CSBs[e].m_ErrorChg>0.0)
          A=m_CSBs[e].m_ErrorChgPos/m_TestOutChg;
        else if (m_CSBs[e].m_ErrorChg<0.0)
          A=m_CSBs[e].m_ErrorChgNeg/m_TestOutChg;
        else
          A=m_CSBs[e].m_ErrorChg/m_TestOutChg;
        }
      else
        A=m_CSBs[e].m_ErrorChgAvg/m_TestOutChg;
      m_CSBs[iVar].m_Alpha[e]=A;
      #if dbgCtrlSeq
      if (dbgSolve())
        dbgpln("calc : [%i,%i]  A:%10.3g  ", iVar, e,  m_CSBs[iVar].m_Alpha[e]);
      #endif
      }

  m_CSBs[iVar].m_SettleIters=m_iCount;

#if !MultiGainValues
  Ti.SetTuneOutLo(m_CSBs[iVar].m_OutMemory); 
  Ti.SetTuneOutHi(m_CSBs[iVar].m_OutMemory); 
#else
  Ti.SetTuneOutLo(Min(Ti.TuneOutLo(), m_CSBs[iVar].m_OutMemory)); 
  Ti.SetTuneOutHi(Max(Ti.TuneOutHi(), m_CSBs[iVar].m_OutMemory)); 
#endif
  
  }

//--------------------------------------------------------------------------

DDBValueLst DDBTuneRules[] =
  {
//    {TR_None , ""}, 
    {TR_P    , "P"},
    {TR_PI   , "PI"},
    {TR_PID  , "PID"},
    {TR_P20  , "P-20"},
    {TR_PI20 , "PI-20"},
    {TR_PID20, "PID-20"},
    {0, NULL}
  };

void CtrlSequencer::CalcGains(long iVar)
  {
  CtrlSeqBlk & Tsb=CSBlk(iVar);
  //TearVar & TV=Tsb.TV[0];

  Tsb.SetTuneRqst(false);
  Tsb.SetTuneBusy(0);

  TVC_GainItem GI;

  flag OK=fabs(m_CSBs[iVar].m_Alpha[iVar])>1.0e-9;
  if (AnalyseHistory(iVar, GI)!=0)
    OK=false;
  ResetHistory();

  if (OK)
    {
    // Calcs according to:
    // 'Instrumentation for Process Measurement and Control' 2nd Ed
    // Norman A. Anderson
    // The Foxboro Company

    //#if SingleGainValue
    //Tsb.m_nGain=OK;
    //#endif
    Tsb.RevActing()=GI.m_PlantGain<0;
    //Tsb.m_PropBandScl=1.0;
    //GI.m_PlantPeriod=Max(GI.m_PlantPeriod, 1.0);
    double Ratio=GI.m_PlantPeriod/Max(1.0e-6, GI.m_PlantDelay);
//  
    //double PropBand=2.86*fabs(GI.m_PlantGain)/Ratio/Tsb.GainScale();
    double Gain=1.0;
    double Ti=0.0;
    double Td=0.0;
    switch (Tsb.TuneRule())
      {
      case TR_None    :
        break;
      case TR_P       :
        Gain=Tsb.DeTune()/(3.33*fabs(GI.m_PlantGain)/Ratio);
        break;
      case TR_PI      :
        Gain=Tsb.DeTune()/(2.86*fabs(GI.m_PlantGain)/Ratio);
        Ti=fabs(3.33*GI.m_PlantDelay*fabs(GI.m_PlantGain));
        break;
      case TR_PID     :
        Gain=Tsb.DeTune()/(1.67*fabs(GI.m_PlantGain)/Ratio);
        Ti=fabs(1.67*GI.m_PlantDelay*fabs(GI.m_PlantGain));
        Td=fabs(0.3*Ratio*GI.m_PlantDelay/fabs(GI.m_PlantGain));
        break;
      case TR_P20     :
        Gain=Tsb.DeTune()/(1.43*fabs(GI.m_PlantGain)/Ratio);
        break;
      case TR_PI20    :
        Gain=Tsb.DeTune()/(1.67*fabs(GI.m_PlantGain)/Ratio);
        Ti=fabs(1.67*GI.m_PlantDelay*fabs(GI.m_PlantGain));
        break;
      case TR_PID20   :
        Gain=Tsb.DeTune()/(1.052*fabs(GI.m_PlantGain)/Ratio);
        Ti=fabs(1.43*GI.m_PlantDelay*fabs(GI.m_PlantGain));
        Td=fabs(0.45*Ratio*GI.m_PlantDelay/fabs(GI.m_PlantGain));
        break;

      }
    
    #if dbgCtrlSeq
    if (dbgSolve())
      {
      dbgpln("Sign      %+i", Tsb.RevActing());
      dbgpln("Gain      %10.4f", Gain);
      dbgpln("Ic        %10.4f", Ti);
      dbgpln("Dc        %10.4f", Td);
      }
    #endif

    //#if MultiGainValues 
    //Tsb.SetGain(Tsb.OutputN(), Gain, Ti, Td, GI.m_PlantGain, GI.m_PlantDelay, GI.m_PlantPeriod);
    //#else
    Tsb.SetEstGain(Gain, Ti, Td, GI.m_PlantGain, GI.m_PlantDelay, GI.m_PlantPeriod);
    //#endif
    Tsb.SetOpGain(Gain, Ti, Td);

    // Calc/Recalc all Deratings
    for (int i=0; i<NCSBs(); i++)
      {
      //TearVar & TV=CSBlk(i).TV[0];
      double MaxNormAlpha=0.0;
      for (int j=0; j<NCSBs(); j++)
        if (j!=i && (fabs(m_CSBs[j].m_Alpha[j])>1.0e-9))
          {
          double NormAlpha=fabs(m_CSBs[j].m_Alpha[i]/m_CSBs[j].m_Alpha[j]);
          MaxNormAlpha=Max(MaxNormAlpha, NormAlpha);
          }    
      //Tsb.SetDeTune(1.0+Range(0.0, MaxNormAlpha, 1.0)*m_MaxPBDerate);
      }
    }
  else
    {
    // Assume Previous Gain OK ???????
    LogWarning(Tsb.m_Tag(), 0, "Bad Gain: Output does not influence Measured");
    }
  
  #if dbgCtrlSeq
  if (dbgSolve())
    dbgpln("CALC : %s [Iters:%5i/%5i] A:%10.3g  DE:%10.4f  DO:%10.4f %s", 
             OK ? "OK":"  ", m_CSBs[iVar].m_DelayIters, m_CSBs[iVar].m_SettleIters,
             m_CSBs[iVar].m_Alpha[iVar], m_CSBs[iVar].m_ErrorChg, m_TestOutChg, Tsb.m_Tag());
  #endif

  for (long i=0; i<m_nSolveVars; i++)
    if (CSBlk(i).CtrlActive() && CSBlk(i).GainOK())
      CSBlk(i).SetHold(this, false);//m_fHoldAll);
  }

//--------------------------------------------------------------------------

void CtrlSequencer::DumpAlpha()
  {
  #if dbgCtrlSeq
  if (dbgSolve())
    {
    dbgpln("Alpha ------------- DeltaErr/DeltaOut ------------------");
    for (long i=0; i<m_nSolveVars; i++)
      if (CSBlk(i).CtrlActive())
        {
        for (long j=0; j<m_nSolveVars; j++)
          if (CSBlk(i).CtrlActive())
            dbgp("%16.6f ", m_CSBs[i].m_Alpha[j]);
        dbgpln(" %s", CSBlk(i).m_Tag());
        }
    dbgpln("--------------------------------------------------------");
    }
  #endif
  }

//--------------------------------------------------------------------------

long CtrlSequencer::DoControl()
  {
  LockCSBs();
  double OutDiff=0;
  for (long e=0; e<NCSBs(); e++)
    {
    CtrlSeqBlk & Ti=CSBlk(e);
    Ti.SetHold(this, false);//m_fHoldAll);
    if (Ti.CtrlActive() && Ti.AutoMan())
      {
      if (Ti.TuneRqst())
        {
        Ti.ClearGain();
        UnlockCSBs();
        return e;
        }
      else if (Ti.TuneRqd(m_RetuneOutDelta))
        {
        UnlockCSBs();
        return e;
        }
      }
    }

  UnlockCSBs();
  return -1;
  }

//--------------------------------------------------------------------------

int CtrlSequencer::StepSequencer()//EqnSlvCtrlBlk & EqnCB)
  {
  if (dbgSequence())
    dbgpln(" StepSequencer ===============================");
  
  LockCSBs();

  if (CtrlSeqBlk::fListChgd)
    {
    Initialise();
    CtrlSeqBlk::fListChgd=false;
    }

  for (long s=0; s<NCSBs(); s++)
    {
    CtrlSeqBlk & Tsb=CSBlk(s);
    Tsb.SetCtrlActive(Tsb.ExecCnt()>0);
    if (Tsb.ExecCnt()>Tsb.ExecCntP())
      Tsb.SetExecCnt(Min(10L, Tsb.ExecCnt()));
    else
      Tsb.SetExecCnt(Max(0L, Tsb.ExecCnt()-1L));
    Tsb.SetExecCntP(Tsb.ExecCnt());
    }
  
  if ((m_nSolveVars<=0) || !SequencerActive())
    {
    m_iSequence=SQN_Control;
    for (long s=0; s<NCSBs(); s++)
      {
      CSBlk(s).SetHold(this, false);//m_fHoldAll);
      CSBlk(s).SetTuneBusy(0);
      }
    
    if (m_fSequencerWasActive)
      for (long s=0; s<NCSBs(); s++)
        CSBlk(s).SetTuneRqst(false);
    m_fSequencerWasActive=false;
    UnlockCSBs();
    return 0;
    }
  m_fSequencerWasActive=SequencerActive();
  
  for (s=0; s<NCSBs(); s++)
    {
    CtrlSeqBlk & Tsb=CSBlk(s);
    //Tsb.SetExecCntP(Tsb.ExecCnt());
    if (!Tsb.CtrlActive() || !Tsb.AutoMan() || !Tsb.TuneRqst())
      {
      Tsb.SetTuneRqst(false);
      Tsb.SetTuneBusy(0);
      if (m_iVar==s)
        {
        m_iSequence=SQN_Control;
        m_iCount=0;
        m_iVar=DoControl();
        if (m_iVar>=0)
          {
          m_iSequence=SQN_SettleAndTweak;
          for (int i=0; i<m_nSolveVars; i++)
            CSBlk(i).SetHold(this, m_fTuneWithHold || (i==m_iVar));//m_fHoldAll || m_fTuneWithHold || (i==m_iVar));
          }
        }
      }        
    }
  
  #if dbgCtrlSeq
  if (dbgSolve())
    {
    dbgp("%*sSEQUENCE  [ ",100,"");
    switch (m_iSequence)
      {
      case SQN_Start         : dbgp("SQN_Start         "); break;
      case SQN_SettleAndTweak: dbgp("SQN_SettleAndTweak"); break;
      case SQN_SettleAndCalc : dbgp("SQN_SettleAndCalc "); break;
      case SQN_Control       : dbgp("SQN_Control       "); break;
      }
    dbgpln(" %4i %4i ]",m_iCount, m_iVar);
    }
  #endif

  long i;
  switch (m_iSequence)
    {
    case SQN_Start:
      for (i=0; i<m_nSolveVars; i++)
        {
        CtrlSeqBlk &Tsb=CSBlk(i);
        if (Tsb.InitTuneRqd())
          {
          #if MultiGainValues
          double O=Tsb.OutputN();
          int i=(int)(O*TV_NGainSegs);
          O=(1.0+i*2)/(2.0*TV_NGainSegs);
          #endif
          }
        }
      m_iSequence=SQN_Control;
      m_iCount=0;
      // Fall Thru

    case SQN_Control: // Do Control;
      m_iVar=DoControl();
      if (m_iVar>=0)
        {
        m_iSequence=SQN_SettleAndTweak;
        for (i=0; i<m_nSolveVars; i++)
          CSBlk(i).SetHold(this, m_fTuneWithHold || (i==m_iVar));//m_fHoldAll || m_fTuneWithHold || (i==m_iVar));
        SetOutput(m_iVar);
        m_iCount=0;
        }
      break;

    case SQN_SettleAndTweak: // Wait to Settle
      if (m_iCount==0)
        ResetHistory();
      CollectHistoryPrior();
      m_iCount++;
      CSBlk(m_iVar).SetTuneBusy(1);
      if (TestSettledPrior(m_iVar))
        {
        ClearErrors();
        TweakOutput(m_iVar);
        m_iSequence=SQN_SettleAndCalc;
        m_iCount=0;
        }
      break;

    case SQN_SettleAndCalc: // Subsequent Retune of Controllers;
      CollectHistoryPost();
      m_iCount++;
      MonitorErrors(m_iVar);
      CSBlk(m_iVar).SetTuneBusy(2);
      if (TestSettledPost(m_iVar))
        {
        CalcAlpha(m_iVar);
        CalcGains(m_iVar);
        DumpAlpha();
        m_iSequence=SQN_Control;
        m_iCount=0;
        m_iVar=DoControl();
        if (m_iVar>=0)
          {
          m_iSequence=SQN_SettleAndTweak;
          for (i=0; i<m_nSolveVars; i++)
            CSBlk(i).SetHold(this, m_fTuneWithHold || (i==m_iVar));//m_fHoldAll || m_fTuneWithHold || (i==m_iVar));
          }
        }
      break;

    }

  for (s=0; s<NCSBs(); s++)
    CSBlk(s).m_InitHold=false;

  UnlockCSBs();

  return (m_iSequence==SQN_Control) ? 2 : 1;
  }

//--------------------------------------------------------------------------

void CtrlSequencer::SetHoldAll(flag On)
  {
  if (CtrlSeqBlk::fListChgd)
    {
    Initialise();
    CtrlSeqBlk::fListChgd=false;
    }

  m_fHoldAll=On; 
  for (int s=0; s<NCSBs(); s++)
    CSBlk(s).UpdateHold(this);

  if (m_fHoldAll)
    {
    m_iSequence=SQN_Control;
    for (long s=0; s<NCSBs(); s++)
      {
      CSBlk(s).SetHold(this, false);//m_fHoldAll);
      CSBlk(s).SetTuneRqst(false);
      CSBlk(s).SetTuneBusy(0);
      }
    }
  };

//--------------------------------------------------------------------------

flag CtrlSequencer::TestConverged(EqnSlvCtrlBlk & EqnCB)
  {
  if (dbgSequence())
    dbgpln(" TestTearConvergence ===============================");
  
  LockCSBs();

  int OK=EqnCB.Converged();

  if (SequencerActive() && SequencerTuning())
    OK=false;

  // Check Converged
  //CtrlSeqBlkIter CSBs(CtrlSeqBlk::List);
  //for (CtrlSeqBlk* pT=CSBs.First(); pT; pT=CSBs.Next())
  for (int i=0; i<NCSBs(); i++)
    {
    CtrlSeqBlk* pT=m_CSBs[i].Blk();
    int LclOK=true;
    switch (pT->TestConverged())
      {
      case -2:// consider the Var Blk to be NOT converged
        LclOK=false;
        continue;
      case -1:// consider the Var Blk to be converged
        continue;
      //case 0: // Errors Not supplied
      //  for (iVar=0; iVar<nVariables; iVar++)
      //    {
      //    TV[iVar].m_Error[0] = (TV[iVar].Y[0]-TV[iVar].X[0]);
      //    TV[iVar].m_Meas[0]  = TV[iVar].Y[0];
      //    }
      //  break;
      case 1: // Data Supplied
        break;
      }

    EqnCB.BumpNUnknowns();
    double Err=0.0;
    double RelTol=Valid(pT->EPSRel()) ? pT->EPSRel() : EqnCB.Cfg.dEps_Rel;
    double AbsTol=Valid(pT->EPSAbs()) ? pT->EPSAbs() : EqnCB.Cfg.dEps_Abs;
    //if (!ConvergedVV(pT->SetPoint(), pT->Measure(), AbsTol, RelTol, Err))
    if (!Finite(pT->SetPoint()) || !Finite(pT->Measure()) || !ConvergedVV(pT->SetPoint(), pT->Measure(), AbsTol, RelTol, Err))
      {
      LclOK=false;
      if (EqnCB.NoteBadEqnError(Err, RelTol))
        {
        EqnCB.CollectBadEqnError(Err, pT->Measure(), AbsTol, RelTol, -1, pT->PIDTag(), TTT_PID);
        }
      }
    
    #if dbgCtrlSeq
    if (dbgSolve())
      dbgpln("PID %16.8f %-12s %s", Err, LclOK?"Cnvgd":"NOTCnvrgd", pT->PIDTag());
    #endif
    if (!LclOK)
      OK=false;
    }

  if (!OK)
    EqnCB.SetConverged(false);

  BumpExecCount();
  
  UnlockCSBs();

  return OK;
  }


//===========================================================================
