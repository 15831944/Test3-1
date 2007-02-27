//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*#M:PGM classes.*/

#include "stdafx.h"
#define  __PGM_C_CPP
#include "pgm_e.h"
#include "pgm_i.h"
#include "profiler.h"
#include "pgm_n.h"
#include "pgm_c.h"
#include "pgm_s.h"
#include "executiv.h"
#include "PGM_ELEC.h"
//#include "optoff.h"

//===========================================================================

#define dbgPID         0

//===========================================================================
                    
#define AtLimitCount   4L

//===========================================================================

template <class T> int VDifferent(T a, T b)
  {
  flag av=Valid(a);
  flag bv=Valid(b);
  if (av && bv)
    return (a != b);
  return (av != bv);
  }

//===========================================================================
// Some PGM classes ...
//===========================================================================
//
// CPID2
//
//===========================================================================

CPID2::CPID2(CPID2Data* pData) :
//<PID>  TearPosBlk(TT_SystemTear),
  m_bOn(pData->bOn),  
  m_Meas(pData->Meas),  
  m_Spt(pData->Spt),
  m_SptTrk(pData->SptTrk),
  m_TrackMinMaxMeas(pData->TrackMinMaxMeas),
  m_TrackMinMaxOut(pData->TrackMinMaxOut),
  m_AutoMan(pData->AutoMan),
  m_PBand(pData->PBand),
  m_Ic(pData->Ic),
  m_Dc(pData->Dc),
  m_RevActing(pData->RevActing),
  m_TuneRule(pData->TuneRule),
  m_TuneOut(pData->TuneOut),
  m_DeTune(pData->DeTune),
  m_KScale(pData->KScale),
  m_InMn(pData->InMn),
  m_InMx(pData->InMx),
  m_OutMn(pData->OutMn),
  m_OutMx(pData->OutMx),
  m_Out(pData->Out),
  m_ETerm(pData->ETerm),
  m_PTerm(pData->PTerm),
  m_ITerm(pData->ITerm),
  m_DTerm(pData->DTerm),
  m_Bias(pData->Bias),
  m_EPSAbs(pData->EPSAbs),
  m_EPSRel(pData->EPSRel)
  {
  Clear();
  }

//---------------------------------------------------------------------------

CPID2::CPID2( pGCClassVar pClassVar) :
//<PID>  TearPosBlk(TT_SystemTear),
  m_bOn(((pGCByteVar)pClassVar->GetVarByName("On"))->m_var),  
  m_Meas(((pGCDoubleVar)pClassVar->GetVarByName("Meas"))->m_var),  
  m_Spt(((pGCDoubleVar)pClassVar->GetVarByName("Spt"))->m_var),   
  //RSP(((pGCDoubleVar)pClassVar->GetVarByName("RSP"))->m_var),   
  m_SptTrk(((pGCByteVar)pClassVar->GetVarByName("SptTrk"))->m_var),  
  m_TrackMinMaxMeas(((pGCByteVar)pClassVar->GetVarByName("TrackMinMaxMeas"))->m_var),   
  m_TrackMinMaxOut(((pGCByteVar)pClassVar->GetVarByName("TrackMinMaxOut"))->m_var),   
  m_AutoMan(((pGCByteVar)pClassVar->GetVarByName("Auto"))->m_var),  
  m_PBand(((pGCDoubleVar)pClassVar->GetVarByName("PB"))->m_var),    
  m_Ic(((pGCDoubleVar)pClassVar->GetVarByName("Ic"))->m_var),    
  m_Dc(((pGCDoubleVar)pClassVar->GetVarByName("Dc"))->m_var),    
  m_RevActing(((pGCByteVar)pClassVar->GetVarByName("IncOpt"))->m_var),
  m_TuneRule(((pGCByteVar)pClassVar->GetVarByName("TuneRule"))->m_var),
  m_TuneOut(((pGCDoubleVar)pClassVar->GetVarByName("TuneOut"))->m_var),
  m_DeTune(((pGCDoubleVar)pClassVar->GetVarByName("DeTune"))->m_var),  
  //Kd(((pGCDoubleVar)pClassVar->GetVarByName("Kd"))->m_var),    
  m_KScale(((pGCDoubleVar)pClassVar->GetVarByName("Ks"))->m_var),    
  m_InMn(((pGCDoubleVar)pClassVar->GetVarByName("InMn"))->m_var),  
  m_InMx(((pGCDoubleVar)pClassVar->GetVarByName("InMx"))->m_var),  
  m_OutMn(((pGCDoubleVar)pClassVar->GetVarByName("OutMn"))->m_var), 
  m_OutMx(((pGCDoubleVar)pClassVar->GetVarByName("OutMx"))->m_var), 
  //ModOpt(((pGCByteVar)pClassVar->GetVarByName("ModOpt"))->m_var),
  m_Out(((pGCDoubleVar)pClassVar->GetVarByName("Out"))->m_var),   
  //OutRqd(((pGCDoubleVar)pClassVar->GetVarByName("OutRqd"))->m_var),   
  m_ETerm(((pGCDoubleVar)pClassVar->GetVarByName("Et"))->m_var),    
  m_PTerm(((pGCDoubleVar)pClassVar->GetVarByName("Pt"))->m_var),    
  m_ITerm(((pGCDoubleVar)pClassVar->GetVarByName("It"))->m_var),    
  m_DTerm(((pGCDoubleVar)pClassVar->GetVarByName("Dt"))->m_var),
  m_Bias(((pGCDoubleVar)pClassVar->GetVarByName("Bias"))->m_var),    
  //m_Hold(((pGCByteVar)pClassVar->GetVarByName("Hold"))->m_var)
  m_EPSAbs(((pGCDoubleVar)pClassVar->GetVarByName("EPSAbs"))->m_var),    
  m_EPSRel(((pGCDoubleVar)pClassVar->GetVarByName("EPSRel"))->m_var)    
  {
  Clear();
  pXRM = NULL;
  }

//---------------------------------------------------------------------------

CPID2::~CPID2()
  {
  }

//---------------------------------------------------------------------------

void CPID2::Clear()
  {
//  sTagName = "";
  m_Meas=0.0;
  m_Spt=0.5;   
  //RSP=0.0;   
  m_bOn=1;  
  //m_SptTrk=PrjFileVerNo()<60?0:1; //?
  m_SptTrk=0; //kga 03/05 always default to 0 until we fix this!!!
  m_TrackMinMaxMeas=1;
  m_TrackMinMaxOut=1;
  m_AutoMan=1;  
  m_PBand=1.0;    
  m_Ic=0.0;    
  m_Dc=0.0;    
  m_RevActing=0;
  m_TuneRule=TR_PI20;
  m_TuneOut=dNAN;
  m_DeTune=1.0;
  //Kd=0.0;    
  m_KScale=dNAN;
  m_InMn=0.0;  
  m_InMx=1.0;  
  m_OutMn=0.0; 
  m_OutMx=1.0; 
  //ModOpt=4;
  m_Out=0.0;   
  //OutRqd=0.0;   
  m_ETerm=0.0;    
  m_PTerm=0.0;    
  m_ITerm=0.0;    
  m_DTerm=0.0;   
  m_Bias=0.0;
  m_FeedFwd=0.0;
  
  m_PrvScaledError=dNAN;
  //m_PrvMeas=dNAN;
  m_PrvOutput=dNAN;

  m_Hold=false;
  m_HoldRqst=false;
  m_HoldCvgCount=0;
  //m_Gain=0.1;
  m_InitHold=true;

  m_dwExecCount=0; // CNM

  m_EPSAbs=dNAN;
  m_EPSRel=dNAN;

  //m_EstDOut=0;
  }

//---------------------------------------------------------------------------

double CPID2::ExecIns()
  {
  #if dbgPID
  double OI_Term=m_ITerm;
  #endif

  m_dwExecCount=CtrlSequencer::ExecCount();

  double Ks = (Valid(m_KScale) ? m_KScale : GTZ(m_OutMx-m_OutMn)/GTZ(m_InMx-m_InMn));
  //double Aux_Val = Range((double)m_InMn, 0.0/*SrcAux*/, (double)m_InMx) ;
  double Input, SetPoint;
  const bool TrackMinMaxMeas = (m_TrackMinMaxMeas || DefNetProbalMode());
  const bool TrackMinMaxOut = (m_TrackMinMaxOut || DefNetProbalMode());
  if (Finite(m_Meas))
    {
    SetCI(1, TrackMinMaxMeas && (m_Meas<m_InMn));
    SetCI(2, TrackMinMaxMeas && (m_Meas>m_InMx));
    Input = Range((double)m_InMn, m_Meas, (double)m_InMx);
    // ??? Input = ((Add_Aux > 0) ? Input-Aux_Val : Aux_Val+Input);
    }
  else
    {
    SetCI(1, true);
    SetCI(2, true);
    Input = m_InMn;
    }
  if (Finite(m_Spt))
    {
    SetCI(5, (m_Spt<m_InMn));
    SetCI(6, (m_Spt>m_InMx));
    SetPoint = Range((double)m_InMn, m_Spt, (double)m_InMx);
    }
  else
    {
    SetCI(5, true);
    SetCI(6, true);
    SetPoint = 0.5*(m_InMn+m_InMx);
    }

  if (!DefNetProbalMode())
    {//disable SptTrk in ProBal for now!!!
    if (!m_AutoMan && m_SptTrk)
      SetPoint=Input;
    }
  
  m_ETerm = SetPoint-Input;                                         // PID-16
  m_PBand = Max(m_PBand, 0.001);

  double OutEntry=m_Out;
  flag DoIt=m_AutoMan;
  if (DoIt)
    {
    m_ExecCnt++; // Mark as Active
    if (CtrlSeq().SequencerActive() && (m_Hold || m_InitHold))
      DoIt=false;
    //m_InitHold=false;
    }
  
  //bool InActive= TearVarBlk::SolverActive() && XSolvingPB();
  flag BumpLess=false;
  flag OutChgd=VDifferent(m_PrvOutput, m_Out);
  //double OutRng=(m_OutMx-m_OutMn); 
  //flag RangeChgd=VDifferent(m_PrvRange, OutRng);
  //m_PrvRange=OutRng;

  //for (int i=TV_MaxEstDOut-1; i>0; i--)
  //  m_EstDOut[i]=m_EstDOut[i-1];
  //m_EstDOut[0]=0.0;
  
  if (DoIt)// && !InActive)
    {
    flag SetPtChg=VDifferent(m_PrvSetPoint, SetPoint);
    //flag SetPtChg=(!Valid(SetPoint) || (m_PrvSetPoint!=SetPoint));
    // A SetPtChg must not result in Bumpless XFer
    if (m_PrvAuto!=m_AutoMan /*|| SetPtChg*/ || m_PrvPBand!=m_PBand || m_PrvDc!=m_Dc || OutChgd)// || RangeChgd)
      BumpLess=true;

    double ScaledError=Ks*m_ETerm;
    double ScaledBias=0.0;
    //if (Valid(m_PrvScaledError) && !SetPtChg)
    //  m_DTerm=m_Dc*(ScaledError-m_PrvScaledError)/GTZ(m_Period);
    //else 
    m_DTerm=0.0;
    m_PrvScaledError=ScaledError;

    double CompositeError=ScaledError+m_DTerm;             // PID-35
//    double CompositeError=ScaledError-m_DTerm;             // PID-35

    //double m_Period=1;/*CNM*IC.dTime;*/
    double ScaledTau=m_Ic/GTZ(m_Period);                      // PID-34
    double FBK=m_Out; // ???? m_Out  OR OutRqd CNM            // Not Cascaded

    double O=m_Out;
    m_PTerm = CompositeError / m_PBand;                  // PID-33
    if (m_RevActing)
      m_PTerm=-m_PTerm;

    if (BumpLess)
      m_ITerm = m_Out-(m_PTerm+ScaledBias);               // Reconstruct m_ITerm to prevent Windup
    m_ITerm += (FBK-ScaledBias-m_ITerm)/(ScaledTau+1.0);// PID-35
    
    m_Out = m_PTerm+m_ITerm+ScaledBias;
    //m_EstDOut[0]=m_Out-OutEntry;
    // Taken out not need for SPT, PB, IC etc canges 
    if (/*BumpLess || CNM */(m_Out < m_OutMn) || (m_Out> m_OutMx))
      {
      m_Out=Range(m_OutMn, m_Out, m_OutMx);
      m_ITerm = m_Out-(m_PTerm+ScaledBias);               // Reconstruct m_ITerm to prevent Windup
      }

    #if dbgPID
    dbgpln("PID I:%11.4f Ks:%11.4f e:%11.4f O:%11.4f P:%11.4f I:%11.4f oI:%11.4f",Input, Ks, m_ETerm, m_Out, m_PTerm, m_ITerm, OI_Term);
    if (BumpLess)
      dbgpln("    Bumpless %s",sTagName());
    #endif
    }
  else
    {
    }

  if (!m_AutoMan)
    m_HoldCvgCount=-1;
  else 
    {
    if ((m_Out <= m_OutMn) || (m_Out >= m_OutMx))// || OutChgd)
      m_HoldCvgCount++;
    else
      m_HoldCvgCount--;

    m_HoldCvgCount=Range(0L, m_HoldCvgCount, AtLimitCount);
    }

  SetCI(3, TrackMinMaxOut && (m_Out<=m_OutMn));
  SetCI(4, TrackMinMaxOut && (m_Out>=m_OutMx));

  if (CtrlSeq().SequencerActive())
    {
    double D0=0.1*fabs(m_OutMx-m_OutMn);
    m_Out=Range(OutEntry-D0, m_Out, OutEntry+D0);
    m_InitHold=false;

    if (0)
      {
      Strng T;
      T.Set("%s.%s", TagName(), m_sTagSuffix());
      dbgpln("%s: %-20.20s [%2i] P:%10.4f R:%10.4f M:%10.4f  E:%10.4f  O:%10.4f >> %10.4f", 
              DoIt ? "SOLVE":"HOLD ",
              T(), m_HoldCvgCount, m_PBand, m_Ic, (m_Meas-m_InMn)/NZ(m_InMx-m_InMn), m_ETerm/NZ(m_InMx-m_InMn), 
              (OutEntry-m_OutMn)/NZ(m_OutMx-m_OutMn), (m_Out-m_OutMn)/NZ(m_OutMx-m_OutMn));
      }
    }
  
  m_Out=Range(m_OutMn, m_Out, m_OutMx);
  m_PrvOutput=m_Out;
  m_PrvAuto=m_AutoMan;
  m_PrvSetPoint=SetPoint;
  m_PrvPBand=m_PBand;
  m_PrvDc=m_Dc;

  //m_Out=OutRqd;
  return m_Out;
  }

//---------------------------------------------------------------------------

void CPID2::Copy(CPID2* p)
  {
  m_dwExecCount=0; // CNM

  m_Period = p->m_Period;
  m_Reset = p->m_Reset;
  m_PrvAuto = p->m_PrvAuto;
  m_PrvSetPoint = p->m_PrvSetPoint;
  m_PrvOutput = p->m_PrvOutput;
  m_PrvPBand = p->m_PrvPBand;
  m_PrvDc = p->m_PrvDc;
  m_PrvScaledError = p->m_PrvScaledError;
  m_HoldCvgCount = p->m_HoldCvgCount; //??
  }

//---------------------------------------------------------------------------

flag CPID2::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="Wp\tPID Input at Low limit";           return 1;
    case 2: pS="Wp\tPID Input at High limit";          return 1;
    case 3: pS="Wp\tPID Output at Low limit";          return 1;
    case 4: pS="Wp\tPID Output at High limit";         return 1;
    case 5: pS="Wp\tPID SetPoint below Low limit";     return 1;
    case 6: pS="Wp\tPID SetPoint above High limit";    return 1;
    default:
      return ConditionBlk::CIStrng(No, pS);
    }
  };

// --------------------------------------------------------------------------

int CPID2::TestConverged()//TearVarArray & TV)
  {
  if (CtrlSequencer::ExecCount()!=m_dwExecCount) // CNM Not Evaluated
    return -1; 

  if (m_HoldCvgCount==0)
    return 1; // Relative to Input
  else if (m_HoldCvgCount>0 && m_HoldCvgCount<AtLimitCount)
    return -2; // Not Converged
  return -1;  // Converged
  };

//==========================================================================

const short Idf_PID2Solve = 1;
const short Idf_PID2Init = 2;

//---------------------------------------------------------------------------

GCPID2::GCPID2(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "PID2", VarClassDefn)
  {
  //all variables added here are automatically saved and restored when neccessary
  AddVar(IB, "On",      &GCByte);
  AddVar(IB, "Meas",    &GCDouble, VarConst);
  AddVar(IB, "Spt",     &GCDouble);
  AddVar(IB, "SptTrk",  &GCByte); 
  AddVar(IB, "TrackMinMaxMeas", &GCByte); 
  AddVar(IB, "TrackMinMaxOut", &GCByte); 
  AddVar(IB, "Auto",    &GCByte); 
  AddVar(IB, "Gain",    &GCDouble);
  AddVar(IB, "PB",      &GCDouble);
  AddVar(IB, "Ic",      &GCDouble);
  AddVar(IB, "Dc",      &GCDouble);
  AddVar(IB, "Bias",    &GCDouble);
  AddVar(IB, "IncOpt",  &GCByte); 
  AddVar(IB, "TuneRule",&GCByte); 
  AddVar(IB, "TuneOut", &GCDouble);
  AddVar(IB, "DeTune",  &GCDouble);
  AddVar(IB, "Ks",      &GCDouble);
  AddVar(IB, "InMn",    &GCDouble);
  AddVar(IB, "InMx",    &GCDouble);
  AddVar(IB, "OutMn",   &GCDouble);
  AddVar(IB, "OutMx",   &GCDouble);
  AddVar(IB, "Out",     &GCDouble);
  AddVar(IB, "EPSAbs",  &GCDouble);
  AddVar(IB, "EPSRel",  &GCDouble);
  AddVar(IB, "Et",      &GCDouble, VarConst);
  AddVar(IB, "Pt",      &GCDouble, VarConst);
  AddVar(IB, "It",      &GCDouble, VarConst);
  AddVar(IB, "Dt",      &GCDouble, VarConst);
  AddVar(IB, "Hold",    &GCByte,   VarConst);
  AddVar(IB, "ExecCnt", &GCLong,   VarConst);
  AddFunct(IB, "Solve", 1, False, Idf_PID2Solve);
  AddFunct(IB, "Init", 9, False, Idf_PID2Init);
  }

//---------------------------------------------------------------------------

void GCPID2::Init(pGCClassVar pClassVar)
  {
  CPID2* pPID2 = new CPID2(pClassVar);
  pClassVar->m_pSubClass = (void*)pPID2;
  
  
  //pPID2->sTagName = m_pName ? m_pName : "?";
  //pPID2->sTagName += ".";
//  pPID2->sTagName = ".";
//  pPID2->sTagName += pClassVar->Name();
  pPID2->m_sTagSuffix = pClassVar->Name();
  }
  
//---------------------------------------------------------------------------

void GCPID2::Done(pGCClassVar pClassVar)
  {
  delete (CPID2*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCPID2::CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CPID2* pPID2 = (CPID2*)pSubClass;
//  if (pPID2->sTagName[0]=='.')
//    {
//    Strng s(pPID2->sTagName());
//    pPID2->sTagName = IB.m_pXRM->GetOwnerTag();
//    pPID2->sTagName += s;
//    }
  switch (FunctId)
    {
    case Idf_PID2Solve:
      pPID2->m_Meas = IB.GetDParm(); 
      pPID2->m_Period = IB.m_dIC_dTime.Seconds;
      return pPID2->ExecIns();
      break;
    case Idf_PID2Init:
      pPID2->m_OutMx = IB.GetDParm();
      pPID2->m_OutMn = IB.GetDParm();
      pPID2->m_InMx = IB.GetDParm();
      pPID2->m_InMn = IB.GetDParm();
      pPID2->m_Dc = IB.GetDParm();
      pPID2->m_Ic = IB.GetDParm();
      pPID2->m_PBand = IB.GetDParm();
      pPID2->m_RevActing= IB.GetBParm();
      pPID2->m_Spt = IB.GetDParm();
      return 0.0;
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCPID2::OnSave(FilingControlBlock &FCB, void* pSubClass)
  {
  GCFCBAppendRec(FCB, '{', "PID2", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Period", ((CPID2*)pSubClass)->m_Period);
  SaveVal(FCB, "Reset", ((CPID2*)pSubClass)->m_Reset);
  SaveVal(FCB, "PrvAuto", ((CPID2*)pSubClass)->m_PrvAuto);
  SaveVal(FCB, "PrvSetPoint", ((CPID2*)pSubClass)->m_PrvSetPoint);
  SaveVal(FCB, "PrvOutput", ((CPID2*)pSubClass)->m_PrvOutput);
  SaveVal(FCB, "PrvPBand", ((CPID2*)pSubClass)->m_PrvPBand);
  SaveVal(FCB, "PrvDc", ((CPID2*)pSubClass)->m_PrvDc);
  SaveVal(FCB, "HoldCvgCount", ((CPID2*)pSubClass)->m_HoldCvgCount);

  GCFCBAppendRec(FCB, '}', "PID2", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCPID2::OnLoad(FilingControlBlock &FCB, void* pSubClass)
  {
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        if (Buff.Try("Period", ((CPID2*)pSubClass)->m_Period)) break;
        if (Buff.Try("Reset", ((CPID2*)pSubClass)->m_Reset)) break;
        if (Buff.Try("PrvAuto", ((CPID2*)pSubClass)->m_PrvAuto)) break;
        if (Buff.Try("PrvSetPoint", ((CPID2*)pSubClass)->m_PrvSetPoint)) break;
        if (Buff.Try("PrvOutput", ((CPID2*)pSubClass)->m_PrvOutput)) break;
        if (Buff.Try("PrvPBand", ((CPID2*)pSubClass)->m_PrvPBand)) break;
        if (Buff.Try("PrvDc", ((CPID2*)pSubClass)->m_PrvDc)) break;
        if (Buff.Try("HoldCvgCount", ((CPID2*)pSubClass)->m_HoldCvgCount)) break;
        break;
      }
    }
  return False;
  }
  
//---------------------------------------------------------------------------

void GCPID2::OnRestore(void* pOldSubClass, void* pNewSubClass)
  {
  CPID2* pNew = (CPID2*)pNewSubClass;
  pNew->Copy((CPID2*)pOldSubClass);
  }

//---------------------------------------------------------------------------

void GCPID2::OnOptimize(CNodeXRefMngr * pXRM, void* pClssVar, void* pSubClass) 
  {
  CPID2* pPID2 = (CPID2*)pSubClass;
  pPID2->pXRM = pXRM;
  };

//===========================================================================
//
// CPID3
//
//===========================================================================

CPID3::CPID3(CPID3Data* pData) :
  m_bOn(pData->bOn),  
  m_Meas(pData->Meas),  
  m_Spt(pData->Spt),
  m_SptTrk(pData->SptTrk),
  m_TrackMinMaxMeas(pData->TrackMinMaxMeas),
  m_TrackMinMaxOut(pData->TrackMinMaxOut),
  m_AutoMan(pData->AutoMan),
  m_Gn(pData->Gn),
  m_Ic(pData->Ic),
  m_Dc(pData->Dc),
  m_RevActing(pData->RevActing),
  m_TuneRule(pData->TuneRule),
  m_TuneOut(pData->TuneOut),
  m_DeTune(pData->DeTune),
  m_InMn(pData->InMn),
  m_InMx(pData->InMx),
  m_OutMn(pData->OutMn),
  m_OutMx(pData->OutMx),
  m_Out(pData->Out),
  m_ETerm(pData->ETerm),
  m_PTerm(pData->PTerm),
  m_ITerm(pData->ITerm),
  m_DTerm(pData->DTerm),
  m_Bias(pData->Bias),
  m_EPSAbs(pData->EPSAbs),
  m_EPSRel(pData->EPSRel)
  {
  Clear();
  }

//---------------------------------------------------------------------------

CPID3::CPID3( pGCClassVar pClassVar) :
  m_bOn(((pGCByteVar)pClassVar->GetVarByName("On"))->m_var),  
  m_Meas(((pGCDoubleVar)pClassVar->GetVarByName("Meas"))->m_var),  
  m_Spt(((pGCDoubleVar)pClassVar->GetVarByName("Spt"))->m_var),   
  m_SptTrk(((pGCByteVar)pClassVar->GetVarByName("SptTrk"))->m_var),   
  m_TrackMinMaxMeas(((pGCByteVar)pClassVar->GetVarByName("TrackMinMaxMeas"))->m_var),   
  m_TrackMinMaxOut(((pGCByteVar)pClassVar->GetVarByName("TrackMinMaxOut"))->m_var),   
  m_AutoMan(((pGCByteVar)pClassVar->GetVarByName("Auto"))->m_var),  
  m_Gn(((pGCDoubleVar)pClassVar->GetVarByName("Gain"))->m_var),    
  m_Ic(((pGCDoubleVar)pClassVar->GetVarByName("Ic"))->m_var),    
  m_Dc(((pGCDoubleVar)pClassVar->GetVarByName("Dc"))->m_var),    
  m_RevActing(((pGCByteVar)pClassVar->GetVarByName("RevAct"))->m_var),
  m_TuneRule(((pGCByteVar)pClassVar->GetVarByName("TuneRule"))->m_var),
  m_TuneOut(((pGCDoubleVar)pClassVar->GetVarByName("TuneOut"))->m_var),
  m_DeTune(((pGCDoubleVar)pClassVar->GetVarByName("DeTune"))->m_var),    
  m_InMn(((pGCDoubleVar)pClassVar->GetVarByName("InMn"))->m_var),  
  m_InMx(((pGCDoubleVar)pClassVar->GetVarByName("InMx"))->m_var),  
  m_OutMn(((pGCDoubleVar)pClassVar->GetVarByName("OutMn"))->m_var), 
  m_OutMx(((pGCDoubleVar)pClassVar->GetVarByName("OutMx"))->m_var), 
  m_Out(((pGCDoubleVar)pClassVar->GetVarByName("Out"))->m_var),   
  m_ETerm(((pGCDoubleVar)pClassVar->GetVarByName("Et"))->m_var),    
  m_PTerm(((pGCDoubleVar)pClassVar->GetVarByName("Pt"))->m_var),    
  m_ITerm(((pGCDoubleVar)pClassVar->GetVarByName("It"))->m_var),    
  m_DTerm(((pGCDoubleVar)pClassVar->GetVarByName("Dt"))->m_var),
  m_Bias(((pGCDoubleVar)pClassVar->GetVarByName("Bias"))->m_var),    
  m_EPSAbs(((pGCDoubleVar)pClassVar->GetVarByName("EPSAbs"))->m_var),    
  m_EPSRel(((pGCDoubleVar)pClassVar->GetVarByName("EPSRel"))->m_var)    
  {
  Clear();
  pXRM = NULL;
  }

//---------------------------------------------------------------------------

CPID3::~CPID3()
  {
  }

//---------------------------------------------------------------------------

void CPID3::Clear()
  {
//  sTagName = "";
  m_Meas=0.0;
  m_Spt=0.5;   
  //RSP=0.0;   
  m_bOn=1;  
  //m_SptTrk=PrjFileVerNo()<60?0:1; //?
  m_SptTrk=0; //kga 03/05 always default to 0 until we fix this!!!
  m_TrackMinMaxMeas=1;
  m_TrackMinMaxOut=1;
  m_AutoMan=1;  
  m_Gn=1.0;    
  m_Ic=0.0;    
  m_Dc=0.0;    
  m_RevActing=0;
  m_TuneRule=TR_PI20;
  m_TuneOut=dNAN;
  m_DeTune=1.0;
  //Kd=0.0;    
  //m_KScale=dNAN;
  m_InMn=0.0;  
  m_InMx=1.0;  
  m_OutMn=0.0; 
  m_OutMx=1.0; 
  //ModOpt=4;
  m_Out=0.0;   
  //OutRqd=0.0;   
  m_ETerm=0.0;    
  m_PTerm=0.0;    
  m_ITerm=0.0;    
  m_DTerm=0.0;
  m_Bias=0.0;
  m_FeedFwd=0.0;

  
  m_PrvMeas=dNAN;
  m_PrvOutput=dNAN;
  m_PrvRange=dNAN;

  m_Hold=false;
  m_HoldRqst=false;
  m_HoldCvgCount=0;
  //m_Gain=0.1;
  m_InitHold=true;

  m_dwExecCount=0; // CNM

  m_EPSAbs=dNAN;
  m_EPSRel=dNAN;
  //m_EstDOut=0;
  }

//---------------------------------------------------------------------------
//  Algorithm Ref:
//    PID Algorithms and their computer implementation.
//    D.W. Clarke.
//    Dept of Engineering Science, University of Oxford. 
//    Trans Inst M C Vol 6 No 6 Oct-Dec 1984

double CPID3::ExecIns()
  {
  #if dbgPID
  double OI_Term=m_ITerm;
  #endif

  m_dwExecCount=CtrlSequencer::ExecCount();

  if (!DefNetProbalMode())
    {//disable SptTrk in ProBal for now!!!
    if (!m_AutoMan && m_SptTrk)
      m_Spt=m_Meas;
    }

  double MeasScaled=(m_Meas-m_InMn)/NZ(m_InMx-m_InMn);
  double SptScaled=(m_Spt-m_InMn)/NZ(m_InMx-m_InMn);
  double OutRng=(m_OutMx-m_OutMn); 
  double OutScaled=(m_Out-m_OutMn)/NZ(OutRng);
  flag RangeChgd=(Valid(m_PrvRange) ? m_PrvRange!=OutRng : true);
  m_PrvRange=OutRng;

  const bool TrackMinMaxMeas = (m_TrackMinMaxMeas || DefNetProbalMode());
  const bool TrackMinMaxOut = (m_TrackMinMaxOut || DefNetProbalMode());
  if (Valid(MeasScaled))
    {
    SetCI(1, TrackMinMaxMeas && (MeasScaled<0.0));
    SetCI(2, TrackMinMaxMeas && (MeasScaled>1.0));
    MeasScaled = Range(0.0, MeasScaled, 1.0);
    }
  else
    {
    SetCI(1, true);
    SetCI(2, true);
    MeasScaled = 0.0;
    }
  if (Valid(SptScaled))
    {
    SetCI(5, (SptScaled<0.0));
    SetCI(6, (SptScaled>1.0));
    //SptScaled = Range(0.0, SptScaled, 1.0);//does this need scaling??????
    }
  else
    {
    SetCI(5, true);
    SetCI(6, true);
    SptScaled = 0.0;
    }

  double Prv_MeasScaled=Valid(m_PrvMeas) ? (m_PrvMeas-m_InMn)/NZ(m_InMx-m_InMn) : MeasScaled;

  m_ETerm = SptScaled-MeasScaled;                                         // PID-16
  m_Gn = fabs(m_Gn);

  double OutEntry=OutScaled;
  flag DoIt=m_AutoMan;
  if (DoIt && CtrlSeq().SequencerActive())
    {
    m_ExecCnt++; // Mark as Active
    if (m_Hold || m_InitHold)
      DoIt=false;
    }
  
  flag BumpLess=false;
  flag OutChgd=VDifferent(m_PrvOutput, m_Out);
  //flag OutChgd=(Valid(m_PrvOutput) ? m_PrvOutput!=m_Out : true);

  flag UseDc = (m_Dc>1.0e-6);

  double IcUse=Max(m_Ic, 0.5*m_Period);
  if (!DefNetProbalMode() && fabs(m_Ic)<1.0e-9)
    m_Ic = IcUse; //do we only want to remember this adjustment if m_Ic was zero???
  double GainP=m_Gn*(1.0+m_Period/(2.0*IcUse));
  double Beta=(2.0*IcUse-m_Period)/(2.0*IcUse+m_Period);
  
  double DcUse=m_Dc;///Max(m_Dc, 5.0*m_Period);
  double Gamma=DcUse/m_Period;
  double D1=(0.2*Gamma-1.0)/(0.2*Gamma+1.0);
  double D2=2.0*Gamma/(0.2*Gamma+1.0);

  double O=m_Out;
  m_PTerm = m_ETerm * m_Gn;//GainP;//m_Gn;
  if (m_RevActing)
    m_PTerm=-m_PTerm;
  
  if (DoIt)
    {
    flag SetPtChg=VDifferent(m_PrvSetPoint, m_Spt);
    // A SetPtChg must not result in Bumpless XFer
    if (/*SetPtChg ||*/ OutChgd || RangeChgd || m_PrvAuto!=m_AutoMan || m_PrvGn!=m_Gn || m_PrvDc!=m_Dc)
      {
      BumpLess=true;
      m_ITerm = OutScaled-m_PTerm; 
      m_DTerm = 0.0;
      // Reapply Integral
      m_ITerm = Beta*m_ITerm+(1.0-Beta)*OutScaled;
      }
    OutScaled = m_PTerm+m_ITerm;
    OutScaled=Range(0.0, OutScaled, 1.0);
    
    // Integral
    m_ITerm = Beta*m_ITerm+(1.0-Beta)*OutScaled;

    // Derivative
    if (UseDc)
      {
      m_DTerm = D1*m_DTerm+D2*(MeasScaled-Prv_MeasScaled);
      //OutScaled=OutScaled+GainP*m_DTerm;
      OutScaled=OutScaled-m_DTerm;
      OutScaled=Range(0.0, OutScaled, 1.0);
      }
    else
      m_DTerm = 0.0;

    #if dbgPID
    dbgpln("PID I:%11.4f Ks:%11.4f e:%11.4f O:%11.4f P:%11.4f I:%11.4f oI:%11.4f",Input, Ks, m_ETerm, m_Out, m_PTerm, m_ITerm, OI_Term);
    if (BumpLess)
      dbgpln("    Bumpless %s",sTagName());
    #endif
    }
  else
    m_ITerm = OutScaled-m_PTerm; // Reconstruct m_ITerm to prevent Windup

  if (!m_AutoMan)
    m_HoldCvgCount=-1;
  else 
    {
    if ((OutScaled <= 0.0) || (OutScaled>= 1.0))// || OutChgd)
      m_HoldCvgCount++;
    else
      m_HoldCvgCount--;

    m_HoldCvgCount=Range(0L, m_HoldCvgCount, AtLimitCount);
    }

  SetCI(3, TrackMinMaxOut && (OutScaled<=0.0));
  SetCI(4, TrackMinMaxOut && (OutScaled>=1.0));

  if (CtrlSeq().SequencerActive())
    {
    double D0=0.1;//*fabs(m_OutMx-m_OutMn);
    OutScaled=Range(OutEntry-D0, OutScaled, OutEntry+D0);
    m_InitHold=false;

    if (0)
      {
      Strng T;
      T.Set("%s.%s", TagName(), m_sTagSuffix());
      dbgpln("%s: %-20.20s [%2i] G:%10.4f I:%10.4f D:%10.4f M:%10.4f  E:%10.4f  O:%10.4f >> %10.4f", 
              DoIt ? "SOLVE":"HOLD ",
              T(), m_HoldCvgCount, m_Gn, IcUse, m_Dc, MeasScaled, m_ETerm, 
              OutEntry, OutScaled);
      }
    }
  
//  m_PrvOutput=OutScaled;
  m_PrvAuto=m_AutoMan;
  m_PrvSetPoint=m_Spt;
  m_PrvGn=m_Gn;
  m_PrvDc=m_Dc;
  m_PrvMeas=m_Meas;

  m_Out=m_OutMn+OutScaled*(m_OutMx-m_OutMn);
  m_PrvOutput=m_Out;

  return m_Out;
  }

//---------------------------------------------------------------------------

void CPID3::Copy(CPID3* p)
  {
  m_dwExecCount=0; // CNM

  m_Period = p->m_Period;
  m_Reset = p->m_Reset;
  m_PrvAuto = p->m_PrvAuto;
  m_PrvSetPoint = p->m_PrvSetPoint;
  m_PrvOutput = p->m_PrvOutput;
  m_PrvGn= p->m_PrvGn;
  m_PrvDc = p->m_PrvDc;
  m_PrvMeas = p->m_PrvMeas;
  m_PrvRange = p->m_PrvRange;
  m_HoldCvgCount = p->m_HoldCvgCount; //??
  }

//---------------------------------------------------------------------------

flag CPID3::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="Wp\tPID Input at Low limit";           return 1;
    case 2: pS="Wp\tPID Input at High limit";          return 1;
    case 3: pS="Wp\tPID Output at Low limit";          return 1;
    case 4: pS="Wp\tPID Output at High limit";         return 1;
    case 5: pS="Wp\tPID SetPoint below Low limit";     return 1;
    case 6: pS="Wp\tPID SetPoint above High limit";    return 1;
    default:
      return ConditionBlk::CIStrng(No, pS);
    }
  };

// --------------------------------------------------------------------------

int CPID3::TestConverged()//TearVarArray & TV)
  {
  if (CtrlSequencer::ExecCount()!=m_dwExecCount) // CNM Not Evaluated
    return -1; 

  if (m_HoldCvgCount==0)
    return 1; // Relative to Input
  else if (m_HoldCvgCount>0 && m_HoldCvgCount<AtLimitCount)
    return -2; // Not Converged
  return -1;  // Converged
  };

//==========================================================================

const short Idf_PID3Solve = 1;
const short Idf_PID3Init = 2;

//---------------------------------------------------------------------------

GCPID3::GCPID3(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "PID3", VarClassDefn)
  {
  //all variables added here are automatically saved and restored when neccessary
  AddVar(IB, "On",      &GCByte);
  AddVar(IB, "Meas",    &GCDouble, VarConst);
  AddVar(IB, "Spt",     &GCDouble);
  AddVar(IB, "SptTrk",  &GCByte); 
  AddVar(IB, "TrackMinMaxMeas", &GCByte); 
  AddVar(IB, "TrackMinMaxOut", &GCByte); 
  AddVar(IB, "Auto",    &GCByte); 
  AddVar(IB, "Gain",    &GCDouble);
  AddVar(IB, "Ic",      &GCDouble);
  AddVar(IB, "Dc",      &GCDouble);
  AddVar(IB, "Bias",    &GCDouble);
  AddVar(IB, "RevAct",  &GCByte); 
  AddVar(IB, "TuneRule",&GCByte); 
  AddVar(IB, "TuneOut", &GCDouble); 
  AddVar(IB, "DeTune",  &GCDouble);
  AddVar(IB, "InMn",    &GCDouble);
  AddVar(IB, "InMx",    &GCDouble);
  AddVar(IB, "OutMn",   &GCDouble);
  AddVar(IB, "OutMx",   &GCDouble);
  AddVar(IB, "Out",     &GCDouble);
  AddVar(IB, "EPSAbs",  &GCDouble);
  AddVar(IB, "EPSRel",  &GCDouble);
  AddVar(IB, "Et",      &GCDouble, VarConst);
  AddVar(IB, "Pt",      &GCDouble, VarConst);
  AddVar(IB, "It",      &GCDouble, VarConst);
  AddVar(IB, "Dt",      &GCDouble, VarConst);
  AddVar(IB, "Hold",    &GCByte,   VarConst);
  AddVar(IB, "ExecCnt", &GCLong,   VarConst);
  AddFunct(IB, "Solve", 1, False, Idf_PID3Solve);
  AddFunct(IB, "Init", 9, False, Idf_PID3Init);
  }

//---------------------------------------------------------------------------

void GCPID3::Init(pGCClassVar pClassVar)
  {
  CPID3* pPID3 = new CPID3(pClassVar);
  pClassVar->m_pSubClass = (void*)pPID3;
  pPID3->m_sTagSuffix = pClassVar->Name();
  }
  
//---------------------------------------------------------------------------

void GCPID3::Done(pGCClassVar pClassVar)
  {
  delete (CPID3*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCPID3::CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CPID3* pPID3 = (CPID3*)pSubClass;
  switch (FunctId)
    {
    case Idf_PID3Solve:
      pPID3->m_Meas = IB.GetDParm(); 
      pPID3->m_Period = IB.m_dIC_dTime.Seconds;
      return pPID3->ExecIns();
      break;
    case Idf_PID3Init:
      pPID3->m_OutMx = IB.GetDParm();
      pPID3->m_OutMn = IB.GetDParm();
      pPID3->m_InMx = IB.GetDParm();
      pPID3->m_InMn = IB.GetDParm();
      pPID3->m_Dc = IB.GetDParm();
      pPID3->m_Ic = IB.GetDParm();
      pPID3->m_Gn = IB.GetDParm();
      pPID3->m_RevActing = IB.GetBParm();
      pPID3->m_Spt = IB.GetDParm();
      return 0.0;
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCPID3::OnSave(FilingControlBlock &FCB, void* pSubClass)
  {
  GCFCBAppendRec(FCB, '{', "PID3", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Period", ((CPID3*)pSubClass)->m_Period);
  SaveVal(FCB, "Reset", ((CPID3*)pSubClass)->m_Reset);
  SaveVal(FCB, "PrvAuto", ((CPID3*)pSubClass)->m_PrvAuto);
  SaveVal(FCB, "PrvSetPoint", ((CPID3*)pSubClass)->m_PrvSetPoint);
  SaveVal(FCB, "PrvOutput", ((CPID3*)pSubClass)->m_PrvOutput);
  SaveVal(FCB, "Prv_Gain", ((CPID3*)pSubClass)->m_PrvGn);
  SaveVal(FCB, "PrvDc", ((CPID3*)pSubClass)->m_PrvDc);
  SaveVal(FCB, "PrvMeas", ((CPID3*)pSubClass)->m_PrvMeas);
  SaveVal(FCB, "PrvRange", ((CPID3*)pSubClass)->m_PrvRange);
  SaveVal(FCB, "HoldCvgCount", ((CPID3*)pSubClass)->m_HoldCvgCount);

  GCFCBAppendRec(FCB, '}', "PID3", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCPID3::OnLoad(FilingControlBlock &FCB, void* pSubClass)
  {
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        if (Buff.Try("Period", ((CPID3*)pSubClass)->m_Period)) break;
        if (Buff.Try("Reset", ((CPID3*)pSubClass)->m_Reset)) break;
        if (Buff.Try("PrvAuto", ((CPID3*)pSubClass)->m_PrvAuto)) break;
        if (Buff.Try("PrvSetPoint", ((CPID3*)pSubClass)->m_PrvSetPoint)) break;
        if (Buff.Try("PrvOutput", ((CPID3*)pSubClass)->m_PrvOutput)) break;
        if (Buff.Try("Prv_Gain", ((CPID3*)pSubClass)->m_PrvGn)) break;
        if (Buff.Try("PrvDc", ((CPID3*)pSubClass)->m_PrvDc)) break;
        if (Buff.Try("PrvMeas", ((CPID3*)pSubClass)->m_PrvMeas)) break;
        if (Buff.Try("PrvRange", ((CPID3*)pSubClass)->m_PrvRange)) break;
        if (Buff.Try("HoldCvgCount", ((CPID3*)pSubClass)->m_HoldCvgCount)) break;

        break;
      }
    }
  return False;
  }
  
//---------------------------------------------------------------------------

void GCPID3::OnRestore(void* pOldSubClass, void* pNewSubClass)
  {
  CPID3* pNew = (CPID3*)pNewSubClass;
  pNew->Copy((CPID3*)pOldSubClass);
  }

//---------------------------------------------------------------------------

void GCPID3::OnOptimize(CNodeXRefMngr * pXRM, void* pClssVar, void* pSubClass) 
  {
  CPID3* pPID3 = (CPID3*)pSubClass;
  pPID3->pXRM = pXRM;
  };

//===========================================================================
//
// CPID4
// Same as PID3 except RevActing corrected and BIAS added M.R.W 15/11/4
//
//===========================================================================

CPID4::CPID4(CPID4Data* pData) :
  m_bOn(pData->bOn),  
  m_Meas(pData->Meas),  
  m_Spt(pData->Spt),
  m_SptTrk(pData->SptTrk),
  m_TrackMinMaxMeas(pData->TrackMinMaxMeas),
  m_TrackMinMaxOut(pData->TrackMinMaxOut),
  m_AutoMan(pData->AutoMan),
  m_Gn(pData->Gn),
  m_Ic(pData->Ic),
  m_Dc(pData->Dc),
  m_RevActing(pData->RevActing),
  m_TuneRule(pData->TuneRule),
  m_TuneOut(pData->TuneOut),
  m_DeTune(pData->DeTune),
  m_InMn(pData->InMn),
  m_InMx(pData->InMx),
  m_OutMn(pData->OutMn),
  m_OutMx(pData->OutMx),
  m_Out(pData->Out),
  m_ETerm(pData->ETerm),
  m_PTerm(pData->PTerm),
  m_ITerm(pData->ITerm),
  m_DTerm(pData->DTerm),
  m_Bias(pData->Bias),
  m_FeedFwd(pData->FeedFwd),
  m_EPSAbs(pData->EPSAbs),
  m_EPSRel(pData->EPSRel)
  {
  Clear();
  }

//---------------------------------------------------------------------------

CPID4::CPID4( pGCClassVar pClassVar) :
  m_bOn(((pGCByteVar)pClassVar->GetVarByName("On"))->m_var),  
  m_Meas(((pGCDoubleVar)pClassVar->GetVarByName("Meas"))->m_var),  
  m_Spt(((pGCDoubleVar)pClassVar->GetVarByName("Spt"))->m_var),   
  m_SptTrk(((pGCByteVar)pClassVar->GetVarByName("SptTrk"))->m_var),   
  m_TrackMinMaxMeas(((pGCByteVar)pClassVar->GetVarByName("TrackMinMaxMeas"))->m_var),   
  m_TrackMinMaxOut(((pGCByteVar)pClassVar->GetVarByName("TrackMinMaxOut"))->m_var),   
  m_AutoMan(((pGCByteVar)pClassVar->GetVarByName("Auto"))->m_var),  
  m_Gn(((pGCDoubleVar)pClassVar->GetVarByName("Gain"))->m_var),    
  m_Ic(((pGCDoubleVar)pClassVar->GetVarByName("Ic"))->m_var),    
  m_Dc(((pGCDoubleVar)pClassVar->GetVarByName("Dc"))->m_var),    
  m_RevActing(((pGCByteVar)pClassVar->GetVarByName("RevAct"))->m_var),
  m_TuneRule(((pGCByteVar)pClassVar->GetVarByName("TuneRule"))->m_var),
  m_TuneOut(((pGCDoubleVar)pClassVar->GetVarByName("TuneOut"))->m_var),
  m_DeTune(((pGCDoubleVar)pClassVar->GetVarByName("DeTune"))->m_var),    
  m_InMn(((pGCDoubleVar)pClassVar->GetVarByName("InMn"))->m_var),  
  m_InMx(((pGCDoubleVar)pClassVar->GetVarByName("InMx"))->m_var),  
  m_OutMn(((pGCDoubleVar)pClassVar->GetVarByName("OutMn"))->m_var), 
  m_OutMx(((pGCDoubleVar)pClassVar->GetVarByName("OutMx"))->m_var), 
  m_Out(((pGCDoubleVar)pClassVar->GetVarByName("Out"))->m_var),   
  m_ETerm(((pGCDoubleVar)pClassVar->GetVarByName("Et"))->m_var),    
  m_PTerm(((pGCDoubleVar)pClassVar->GetVarByName("Pt"))->m_var),    
  m_ITerm(((pGCDoubleVar)pClassVar->GetVarByName("It"))->m_var),    
  m_DTerm(((pGCDoubleVar)pClassVar->GetVarByName("Dt"))->m_var),
  m_Bias(((pGCDoubleVar)pClassVar->GetVarByName("Bias"))->m_var),
  m_FeedFwd(((pGCDoubleVar)pClassVar->GetVarByName("FeedFwd"))->m_var),
  m_EPSAbs(((pGCDoubleVar)pClassVar->GetVarByName("EPSAbs"))->m_var),    
  m_EPSRel(((pGCDoubleVar)pClassVar->GetVarByName("EPSRel"))->m_var)    
  {
  Clear();
  pXRM = NULL;
  }

//---------------------------------------------------------------------------

CPID4::~CPID4()
  {
  }

//---------------------------------------------------------------------------

void CPID4::Clear()
  {
//  sTagName = "";
  m_Meas=0.0;
  m_Spt=0.5;   
  //RSP=0.0;   
  m_bOn=1;  
  //m_SptTrk=PrjFileVerNo()<60?0:1; //?
  m_SptTrk=0; //kga 03/05 always default to 0 until we fix this!!!
  m_TrackMinMaxMeas=1;
  m_TrackMinMaxOut=1;
  m_AutoMan=1;  
  m_Gn=1.0;    
  m_Ic=0.0;    
  m_Dc=0.0;    
  m_RevActing=0;
  m_TuneRule=TR_PI20;
  m_TuneOut=dNAN;
  m_DeTune=1.0;
  //Kd=0.0;    
  //m_KScale=dNAN;
  m_InMn=0.0;  
  m_InMx=1.0;  
  m_OutMn=0.0; 
  m_OutMx=1.0; 
  //ModOpt=4;
  m_Out=0.0;   
  //OutRqd=0.0;   
  m_ETerm=0.0;    
  m_PTerm=0.0;    
  m_ITerm=0.0;  
  m_DTerm=0.0;    
  m_Bias=0.0;
  m_FeedFwd=0.0;

  m_PrvMeas=dNAN;
  m_PrvOutput=dNAN;
  m_PrvRange=dNAN;

  m_Hold=false;
  m_HoldRqst=false;
  m_HoldCvgCount=0;
  //m_Gain=0.1;
  m_InitHold=true;

  m_dwExecCount=0; // CNM

  m_EPSAbs=dNAN;
  m_EPSRel=dNAN;
  //m_EstDOut=0;
  }

//---------------------------------------------------------------------------
//  Algorithm Ref:
//    PID Algorithms and their computer implementation.
//    D.W. Clarke.
//    Dept of Engineering Science, University of Oxford. 
//    Trans Inst M C Vol 6 No 6 Oct-Dec 1984

double CPID4::ExecIns()
  {
  #if dbgPID
  double OI_Term=m_ITerm;
  #endif

  m_dwExecCount=CtrlSequencer::ExecCount();

  if (!DefNetProbalMode())
    {//disable SptTrk in ProBal for now!!!
    if (!m_AutoMan && m_SptTrk)
      m_Spt=m_Meas;
    }

  double MeasScaled=(m_Meas-m_InMn)/NZ(m_InMx-m_InMn);
  double SptScaled=(m_Spt-m_InMn)/NZ(m_InMx-m_InMn);
  double OutRng=(m_OutMx-m_OutMn); 
  double OutScaled=(m_Out-m_OutMn)/NZ(OutRng);

  double BiasScaled=(m_Bias-m_OutMn)/NZ(m_OutMx-m_OutMn);
  double FeedFwdScaled=(m_FeedFwd-m_OutMn)/NZ(m_OutMx-m_OutMn);

  flag RangeChgd=(Valid(m_PrvRange) ? m_PrvRange!=OutRng : true);
  m_PrvRange=OutRng;

  const bool TrackMinMaxMeas = (m_TrackMinMaxMeas || DefNetProbalMode());
  const bool TrackMinMaxOut = (m_TrackMinMaxOut || DefNetProbalMode());
  if (Valid(MeasScaled))
    {
    SetCI(1, TrackMinMaxMeas && (MeasScaled<0.0));
    SetCI(2, TrackMinMaxMeas && (MeasScaled>1.0));
    MeasScaled = Range(0.0, MeasScaled, 1.0);
    }
  else
    {
    SetCI(1, true);
    SetCI(2, true);
    MeasScaled = 0.0;
    }
  if (Valid(SptScaled))
    {
    SetCI(5, (SptScaled<0.0));
    SetCI(6, (SptScaled>1.0));
    //SptScaled = Range(0.0, SptScaled, 1.0);//does this need scaling??????
    }
  else
    {
    SetCI(5, true);
    SetCI(6, true);
    SptScaled = 0.0;
    }

  if (Valid(BiasScaled))
    {
    SetCI(9, (BiasScaled<0.0));
    SetCI(10, (BiasScaled>1.0));
    BiasScaled = Range(0.0, BiasScaled, 1.0);
    }
  else
    {
    SetCI(9, true);
    SetCI(10, true);
    BiasScaled = 0.0;
    }

  if (Valid(FeedFwdScaled))
    {
    SetCI(7, (FeedFwdScaled<0.0));
    SetCI(8, (FeedFwdScaled>1.0));
    FeedFwdScaled = Range(0.0, FeedFwdScaled, 1.0);
    }
  else
    {
    SetCI(7, true);
    SetCI(8, true);
    FeedFwdScaled = 0.0;
    }

  double Prv_MeasScaled=Valid(m_PrvMeas) ? (m_PrvMeas-m_InMn)/NZ(m_InMx-m_InMn) : MeasScaled;

  m_ETerm = SptScaled-MeasScaled;                                         // PID-16
  m_Gn = fabs(m_Gn);

  double OutEntry=OutScaled;
  flag DoIt=m_AutoMan;
  if (DoIt && CtrlSeq().SequencerActive())
    {
    m_ExecCnt++; // Mark as Active
    if (m_Hold || m_InitHold)
      DoIt=false;
    }
  
  flag BumpLess=false;
  flag OutChgd=VDifferent(m_PrvOutput, m_Out);
  //flag OutChgd=(Valid(m_PrvOutput) ? m_PrvOutput!=m_Out : true);

  flag UseDc = (m_Dc>1.0e-6);

  double IcUse;
  
  if (m_Ic != 0.0) 
	  IcUse = Max(m_Ic, 0.5*m_Period);
  else
	  IcUse = 0.0;
  m_Ic = IcUse; //do we want this to change as period changes!?!?!?!
  double GainP;
  
  if (IcUse!=0.0)
	  GainP = m_Gn*(1.0+m_Period/(2.0*IcUse));
  else
    GainP = m_Gn;

  double Beta=(2.0*IcUse-m_Period)/(2.0*IcUse+m_Period);
  
  double DcUse=m_Dc;///Max(m_Dc, 5.0*m_Period);
  double Gamma=DcUse/m_Period;
  double D1=(0.2*Gamma-1.0)/(0.2*Gamma+1.0);
  double D2=2.0*Gamma/(0.2*Gamma+1.0);

  double O=m_Out;
  m_PTerm = m_ETerm * m_Gn;//GainP;//m_Gn;

  //
  // M.R.W - RevActing changed to be correct 
  if (!m_RevActing)
    m_PTerm=-m_PTerm;
  
  if (DoIt)
    {
    flag SetPtChg=VDifferent(m_PrvSetPoint, m_Spt);
    // A SetPtChg must not result in Bumpless XFer
    if (/*SetPtChg ||*/ OutChgd || RangeChgd || m_PrvAuto!=m_AutoMan || m_PrvGn!=m_Gn || m_PrvDc!=m_Dc)
      {
      if (IcUse != 0.0)
        {
        BumpLess=true;
        m_ITerm = OutScaled-m_PTerm-BiasScaled-FeedFwdScaled; 
        m_DTerm = 0.0;
        // Reapply Integral
        m_ITerm = Beta*m_ITerm+(1.0-Beta)*OutScaled;
        }
      }

    OutScaled = m_PTerm+m_ITerm;

    // Integral
	if (IcUse == 0.0)
		m_ITerm = 0.0;
	else
		m_ITerm = Beta*m_ITerm+(1.0-Beta)*OutScaled;

    OutScaled += BiasScaled + FeedFwdScaled;

    // Integral Anti-windup
    // If OP is greated than 100% modify integral term
    // so OP is 100%. If it is less than 0% then modify integral
    // term so that it is 0%
    if (IcUse != 0.0)
      {
      if (OutScaled > 1.0)
	      m_ITerm = m_ITerm - (OutScaled-1.0);
      else if (OutScaled < 0.0)
	      m_ITerm = m_ITerm - OutScaled;
      OutScaled=Range(0.0, OutScaled, 1.0);
      }

    // Derivative
    if (UseDc)
      {
      m_DTerm = D1*m_DTerm+D2*(MeasScaled-Prv_MeasScaled);
      //OutScaled=OutScaled+GainP*m_DTerm;
      OutScaled=OutScaled-m_DTerm;
      OutScaled=Range(0.0, OutScaled, 1.0);
      }
    else
      m_DTerm = 0.0;

    #if dbgPID
    dbgpln("PID I:%11.4f Ks:%11.4f e:%11.4f O:%11.4f P:%11.4f I:%11.4f oI:%11.4f",Input, Ks, m_ETerm, m_Out, m_PTerm, m_ITerm, OI_Term);
    if (BumpLess)
      dbgpln("    Bumpless %s",sTagName());
    #endif
    }
  else
    m_ITerm = OutScaled-m_PTerm; // Reconstruct m_ITerm to prevent Windup

  if (!m_AutoMan)
    m_HoldCvgCount=-1;
  else 
    {
    if ((OutScaled <= 0.0) || (OutScaled>= 1.0))// || OutChgd)
      m_HoldCvgCount++;
    else
      m_HoldCvgCount--;

    m_HoldCvgCount=Range(0L, m_HoldCvgCount, AtLimitCount);
    }

  SetCI(3, TrackMinMaxOut && (OutScaled<=0.0));
  SetCI(4, TrackMinMaxOut && (OutScaled>=1.0));

  if (CtrlSeq().SequencerActive())
    {
    double D0=0.1;//*fabs(m_OutMx-m_OutMn);
    OutScaled=Range(OutEntry-D0, OutScaled, OutEntry+D0);
    m_InitHold=false;

    if (0)
      {
      Strng T;
      T.Set("%s.%s", TagName(), m_sTagSuffix());
      dbgpln("%s: %-20.20s [%2i] G:%10.4f I:%10.4f D:%10.4f M:%10.4f  E:%10.4f  O:%10.4f >> %10.4f", 
              DoIt ? "SOLVE":"HOLD ",
              T(), m_HoldCvgCount, m_Gn, IcUse, m_Dc, MeasScaled, m_ETerm, 
              OutEntry, OutScaled);
      }
    }
  
//  m_PrvOutput=OutScaled;
  m_PrvAuto=m_AutoMan;
  m_PrvSetPoint=m_Spt;
  m_PrvGn=m_Gn;
  m_PrvDc=m_Dc;
  m_PrvMeas=m_Meas;

  m_Out=m_OutMn+OutScaled*(m_OutMx-m_OutMn);
  m_PrvOutput=m_Out;

  return m_Out;
  }

//---------------------------------------------------------------------------

void CPID4::Copy(CPID4* p)
  {
  m_dwExecCount=0; // CNM

  m_Period = p->m_Period;
  m_Reset = p->m_Reset;
  m_PrvAuto = p->m_PrvAuto;
  m_PrvSetPoint = p->m_PrvSetPoint;
  m_PrvOutput = p->m_PrvOutput;
  m_PrvGn= p->m_PrvGn;
  m_PrvDc = p->m_PrvDc;
  m_PrvMeas = p->m_PrvMeas;
  m_PrvRange = p->m_PrvRange;
  m_HoldCvgCount = p->m_HoldCvgCount; //??
  }

//---------------------------------------------------------------------------

flag CPID4::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="Wp\tPID Input at Low limit";           return 1;
    case 2: pS="Wp\tPID Input at High limit";          return 1;
    case 3: pS="Wp\tPID Output at Low limit";          return 1;
    case 4: pS="Wp\tPID Output at High limit";         return 1;
    case 5: pS="Wp\tPID SetPoint below Low limit";     return 1;
    case 6: pS="Wp\tPID SetPoint above High limit";    return 1;
    case 7: pS="Wp\tPID Feed Forward below Low limit";     return 1;
    case 8: pS="Wp\tPID Feed Forward above High limit";    return 1;
    case 9: pS="Wp\tPID Bias below Low limit";     return 1;
    case 10: pS="Wp\tPID Bias above High limit";    return 1;
    default:
      return ConditionBlk::CIStrng(No, pS);
    }
  };

// --------------------------------------------------------------------------

int CPID4::TestConverged()//TearVarArray & TV)
  {
  if (CtrlSequencer::ExecCount()!=m_dwExecCount) // CNM Not Evaluated
    return -1; 

  if (m_HoldCvgCount==0)
    return 1; // Relative to Input
  else if (m_HoldCvgCount>0 && m_HoldCvgCount<AtLimitCount)
    return -2; // Not Converged
  return -1;  // Converged
  };

//==========================================================================

const short Idf_PID4Solve = 1;
const short Idf_PID4Init = 2;

//---------------------------------------------------------------------------

GCPID4::GCPID4(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "PID4", VarClassDefn)
  {
  //all variables added here are automatically saved and restored when neccessary
  AddVar(IB, "On",      &GCByte);
  AddVar(IB, "Meas",    &GCDouble, VarConst);
  AddVar(IB, "Spt",     &GCDouble);
  AddVar(IB, "SptTrk",  &GCByte); 
  AddVar(IB, "TrackMinMaxMeas", &GCByte); 
  AddVar(IB, "TrackMinMaxOut", &GCByte); 
  AddVar(IB, "Auto",    &GCByte); 
  AddVar(IB, "Gain",    &GCDouble);
  AddVar(IB, "Ic",      &GCDouble);
  AddVar(IB, "Dc",      &GCDouble);
  AddVar(IB, "Bias",    &GCDouble);
  AddVar(IB, "RevAct",  &GCByte); 
  AddVar(IB, "TuneRule",&GCByte); 
  AddVar(IB, "TuneOut", &GCDouble); 
  AddVar(IB, "DeTune",  &GCDouble);
  AddVar(IB, "InMn",    &GCDouble);
  AddVar(IB, "InMx",    &GCDouble);
  AddVar(IB, "OutMn",   &GCDouble);
  AddVar(IB, "OutMx",   &GCDouble);
  AddVar(IB, "Out",     &GCDouble);
  AddVar(IB, "EPSAbs",  &GCDouble);
  AddVar(IB, "EPSRel",  &GCDouble);
  AddVar(IB, "Et",      &GCDouble, VarConst);
  AddVar(IB, "Pt",      &GCDouble, VarConst);
  AddVar(IB, "It",      &GCDouble, VarConst);
  AddVar(IB, "Dt",      &GCDouble, VarConst);
  AddVar(IB, "Hold",    &GCByte,   VarConst);
  AddVar(IB, "ExecCnt", &GCLong,   VarConst);
  AddFunct(IB, "Solve", 1, False, Idf_PID4Solve);
  AddFunct(IB, "Init", 9, False, Idf_PID4Init);
  }

//---------------------------------------------------------------------------

void GCPID4::Init(pGCClassVar pClassVar)
  {
  CPID4* pPID4 = new CPID4(pClassVar);
  pClassVar->m_pSubClass = (void*)pPID4;
  pPID4->m_sTagSuffix = pClassVar->Name();
  }
  
//---------------------------------------------------------------------------

void GCPID4::Done(pGCClassVar pClassVar)
  {
  delete (CPID4*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCPID4::CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  CPID4* pPID4 = (CPID4*)pSubClass;
  switch (FunctId)
    {
    case Idf_PID4Solve:
      pPID4->m_Meas = IB.GetDParm(); 
      pPID4->m_Period = IB.m_dIC_dTime.Seconds;
      return pPID4->ExecIns();
      break;
    case Idf_PID4Init:
      pPID4->m_OutMx = IB.GetDParm();
      pPID4->m_OutMn = IB.GetDParm();
      pPID4->m_InMx = IB.GetDParm();
      pPID4->m_InMn = IB.GetDParm();
      pPID4->m_Dc = IB.GetDParm();
      pPID4->m_Ic = IB.GetDParm();
      pPID4->m_Gn = IB.GetDParm();
      pPID4->m_RevActing = IB.GetBParm();
      pPID4->m_Spt = IB.GetDParm();
      return 0.0;
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCPID4::OnSave(FilingControlBlock &FCB, void* pSubClass)
  {
  GCFCBAppendRec(FCB, '{', "PID4", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Period", ((CPID4*)pSubClass)->m_Period);
  SaveVal(FCB, "Reset", ((CPID4*)pSubClass)->m_Reset);
  SaveVal(FCB, "PrvAuto", ((CPID4*)pSubClass)->m_PrvAuto);
  SaveVal(FCB, "PrvSetPoint", ((CPID4*)pSubClass)->m_PrvSetPoint);
  SaveVal(FCB, "PrvOutput", ((CPID4*)pSubClass)->m_PrvOutput);
  SaveVal(FCB, "Prv_Gain", ((CPID4*)pSubClass)->m_PrvGn);
  SaveVal(FCB, "PrvDc", ((CPID4*)pSubClass)->m_PrvDc);
  SaveVal(FCB, "PrvMeas", ((CPID4*)pSubClass)->m_PrvMeas);
  SaveVal(FCB, "PrvRange", ((CPID4*)pSubClass)->m_PrvRange);
  SaveVal(FCB, "HoldCvgCount", ((CPID4*)pSubClass)->m_HoldCvgCount);

  GCFCBAppendRec(FCB, '}', "PID4", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCPID4::OnLoad(FilingControlBlock &FCB, void* pSubClass)
  {
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        if (Buff.Try("Period", ((CPID4*)pSubClass)->m_Period)) break;
        if (Buff.Try("Reset", ((CPID4*)pSubClass)->m_Reset)) break;
        if (Buff.Try("PrvAuto", ((CPID4*)pSubClass)->m_PrvAuto)) break;
        if (Buff.Try("PrvSetPoint", ((CPID4*)pSubClass)->m_PrvSetPoint)) break;
        if (Buff.Try("PrvOutput", ((CPID4*)pSubClass)->m_PrvOutput)) break;
        if (Buff.Try("Prv_Gain", ((CPID4*)pSubClass)->m_PrvGn)) break;
        if (Buff.Try("PrvDc", ((CPID4*)pSubClass)->m_PrvDc)) break;
        if (Buff.Try("PrvMeas", ((CPID4*)pSubClass)->m_PrvMeas)) break;
        if (Buff.Try("PrvRange", ((CPID4*)pSubClass)->m_PrvRange)) break;
        if (Buff.Try("HoldCvgCount", ((CPID4*)pSubClass)->m_HoldCvgCount)) break;

        break;
      }
    }
  return False;
  }
  
//---------------------------------------------------------------------------

void GCPID4::OnRestore(void* pOldSubClass, void* pNewSubClass)
  {
  CPID4* pNew = (CPID4*)pNewSubClass;
  pNew->Copy((CPID4*)pOldSubClass);
  }

//---------------------------------------------------------------------------

void GCPID4::OnOptimize(CNodeXRefMngr * pXRM, void* pClssVar, void* pSubClass) 
  {
  CPID4* pPID4 = (CPID4*)pSubClass;
  pPID4->pXRM = pXRM;
  };


//===========================================================================
//
// CPGMTime
//
//===========================================================================

class CPGMTime
  {
  friend class GCTime;
  public:
    CPGMTime(pGCClassVar pClassVar);
  protected:
    //parameters accessable from PGM
    unsigned char  &HSec;
    unsigned char  &Sec;
    unsigned char  &Min;
    unsigned char  &Hour;
    unsigned char  &Day;
    unsigned char  &Mon;
    long           &Year;
    unsigned char  &WDay;
  public:
    double CPGMTime::SplitTime(double t, flag UseGMT);
  };

//---------------------------------------------------------------------------

CPGMTime::CPGMTime(pGCClassVar pClassVar) :
  HSec(((pGCByteVar)pClassVar->GetVarByName("HSec"))->m_var),  
  Sec(((pGCByteVar)pClassVar->GetVarByName("Sec"))->m_var),  
  Min(((pGCByteVar)pClassVar->GetVarByName("Min"))->m_var),  
  Hour(((pGCByteVar)pClassVar->GetVarByName("Hour"))->m_var),  
  Day(((pGCByteVar)pClassVar->GetVarByName("Day"))->m_var),  
  Mon(((pGCByteVar)pClassVar->GetVarByName("Mon"))->m_var),  
  Year(((pGCLongVar)pClassVar->GetVarByName("Year"))->m_var),  
  WDay(((pGCByteVar)pClassVar->GetVarByName("WDay"))->m_var)
  {
  HSec=0;
  Sec=0;
  Min=0;
  Hour=0;
  Day=0;
  Mon=0;
  Year=0;
  WDay=0;
  }

//---------------------------------------------------------------------------

double CPGMTime::SplitTime(double t, flag UseGMT)
  {
  __time64_t tt = (__time64_t)(t);
  tm* ptm;
  if (UseGMT)
    ptm = _gmtime64(&tt);
  else
    ptm = _localtime64(&tt);
  HSec = (unsigned char)((t-tt)*100.0);
  if (ptm)
    {
    Day = ptm->tm_mday;
    Mon = ptm->tm_mon+1;
    Year = ptm->tm_year+1900;
    Hour = ptm->tm_hour;
    Min = ptm->tm_min;
    Sec = ptm->tm_sec;
    WDay = ptm->tm_wday;
    return t;
    }
  else
    return 0.0;
  }

//==========================================================================

const short Idf_TimeSplit = 1;
const short Idf_TimeGMTSplit = 2;

//---------------------------------------------------------------------------

GCTime::GCTime(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "TimeClass", VarClassDefn)
  {
  AddVar(IB, "HSec",   &GCByte, VarConst);
  AddVar(IB, "Sec",    &GCByte, VarConst);
  AddVar(IB, "Min",    &GCByte, VarConst);
  AddVar(IB, "Hour",   &GCByte, VarConst);
  AddVar(IB, "Day",    &GCByte, VarConst);
  AddVar(IB, "Mon",    &GCByte, VarConst);
  AddVar(IB, "Year",   &GCLong, VarConst);
  AddVar(IB, "WDay",   &GCByte, VarConst);
  AddFunct(IB, "SplitTime", 1, False, Idf_TimeSplit);
  AddFunct(IB, "GMTSplitTime", 1, False, Idf_TimeGMTSplit);
  }

//---------------------------------------------------------------------------

void GCTime::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (void*)new CPGMTime(pClassVar);
  }
  
//---------------------------------------------------------------------------

void GCTime::Done(pGCClassVar pClassVar)
  {
  delete (CPGMTime*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCTime::CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  switch (FunctId)
    {
    case Idf_TimeSplit:
      return ((CPGMTime*)pSubClass)->SplitTime(IB.GetDParm(), False);
      break;
    case Idf_TimeGMTSplit:
      return ((CPGMTime*)pSubClass)->SplitTime(IB.GetDParm(), True);
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCTime::OnSave(FilingControlBlock &FCB, void* pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCTime::OnLoad(FilingControlBlock &FCB, void* pSubClass)
  {
  return True;
  }

//==========================================================================

const short Idf_ArraySetLen = 1;
const short Idf_ArrayGetLen = 2;
const short Idf_ArrayGetAt = 3;
const short Idf_ArraySetAt = 4;
const short Idf_ArrayIncAt = 5;
const short Idf_ArrayDecAt = 6;
const short Idf_ArraySetAll = 7;
const short Idf_ArrayLoad = 8;

//---------------------------------------------------------------------------

GCArray::GCArray(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "Array", VarClassDefn)
  {
  AddFunct(IB, "SetLen", 1, False, Idf_ArraySetLen);
  AddFunct(IB, "GetLen", 0, False, Idf_ArrayGetLen);
  AddFunct(IB, "SetAt", 2, False, Idf_ArraySetAt);
  AddFunct(IB, "GetAt", 1, False, Idf_ArrayGetAt);
  AddFunct(IB, "IncAt", 2, False, Idf_ArrayIncAt);
  AddFunct(IB, "DecAt", 2, False, Idf_ArrayDecAt);
  AddFunct(IB, "SetAll", 1, False, Idf_ArraySetAll);
  AddFunct(IB, "Load", 1, True, Idf_ArrayLoad);
  }

//---------------------------------------------------------------------------

void GCArray::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (void*)new CDVector();
  pClassVar->m_flags |= VarArrayClass;
  }
  
//---------------------------------------------------------------------------

void GCArray::Done(pGCClassVar pClassVar)
  {
  delete (pCDVector)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCArray::CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  switch (FunctId)
    {
    case Idf_ArraySetLen:
      ((pCDVector)pSubClass)->SetSize(IB.GetLParm()); 
      return 0.0;
      break;
    case Idf_ArrayGetLen:
      return (double)((pCDVector)pSubClass)->GetLen();
      break;
    case Idf_ArraySetAt:
      {//get parameters in reverse order
	    double d = IB.GetDParm();
      long Index = IB.GetLParm();
      if (Index<0 || Index>=((pCDVector)pSubClass)->GetLen())
        {
        char Buff[256];
        sprintf(Buff, "Invalid array index %d for %s.SetAt", Index, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      (*((pCDVector)pSubClass))[Index] = d;
      return 0.0;
      break;
      }
    case Idf_ArrayGetAt:
      {
      long Index = IB.GetLParm();
      if (Index<0 || Index>=((pCDVector)pSubClass)->GetLen())
        {
        char Buff[256];
        sprintf(Buff, "Invalid array index %d for %s.GetAt", Index, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      return (*((pCDVector)pSubClass))[Index];
      break;
      }
    case Idf_ArraySetAll:
      ((pCDVector)pSubClass)->SetAll(IB.GetDParm());
      return 0.0;
      break;
    case Idf_ArrayIncAt:
      {//get parameters in reverse order
	    double d = IB.GetDParm();
      long Index = IB.GetLParm();
      if (Index<0 || Index>=((pCDVector)pSubClass)->GetLen())
        {
        char Buff[256];
        sprintf(Buff, "Invalid array index %d for %s.IncAt", Index, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      (*((pCDVector)pSubClass))[Index] += d;
      return 0.0;
      break;
      }
    case Idf_ArrayDecAt:
      {//get parameters in reverse order
	    double d = IB.GetDParm();
      long Index = IB.GetLParm();
      if (Index<0 || Index>=((pCDVector)pSubClass)->GetLen())
        {
        char Buff[256];
        sprintf(Buff, "Invalid array index %d for %s.DecAt", Index, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      (*((pCDVector)pSubClass))[Index] -= d;
      return 0.0;
      break;
      }
    case Idf_ArrayLoad:
      {
      CVMLoadHelper H;
      strcpy(H.FName, IB.GetSParm());
      if (((pCDVector)pSubClass)->Load(H))
        return dGCtrue;
      else
        return dGCfalse;
      break;
      }
    default:
      ASSERT(FALSE);  //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCArray::OnSave(FilingControlBlock &FCB, void* pSubClass)
  {
  GCFCBAppendRec(FCB, '{', "Array", NULL, 0); // Start Internal Vars
  pCDVector a = (pCDVector)pSubClass;
  SaveVal(FCB, "Length", a->GetLen());
  for (long i=0; i<a->GetLen(); i++)
    SaveVal(FCB, "Item", a->m_d[i]);
  GCFCBAppendRec(FCB, '}', "Array", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCArray::OnLoad(FilingControlBlock &FCB, void* pSubClass)
  {
  pCDVector a = (pCDVector)pSubClass;
  long Len = 0;
  a->SetSize(Len);
  double d;
  long i = 0;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        return True;
      case '{':
        break;
      default:
        if (Buff.Try("Length", Len)) 
          {
          a->SetSize(Len);
          break;
          }
        if (Buff.Try("Item", d)) 
          {
          if (i<Len)
            a->m_d[i] = d;
          i++;
          break;
          }
        break;
      }
    }
  return False;
  }
  
//---------------------------------------------------------------------------

void GCArray::OnRestore(void* pOldSubClass, void* pNewSubClass)
  {
  pCDVector pNew = (pCDVector)pNewSubClass;
  (*pNew) = (*((pCDVector)pOldSubClass));
  }

//==========================================================================

const short Idf_MatrixSetSize = 1;
const short Idf_MatrixGetRowCount = 2;
const short Idf_MatrixGetColCount = 3;
const short Idf_MatrixSetAt = 4;
const short Idf_MatrixGetAt = 5;
const short Idf_MatrixIncAt = 6;
const short Idf_MatrixDecAt = 7;
const short Idf_MatrixSetAll = 8;
const short Idf_MatrixLoad = 9;

//---------------------------------------------------------------------------

GCMatrix::GCMatrix(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, "Matrix", VarClassDefn)
  {
  AddFunct(IB, "SetSize", 2, False, Idf_MatrixSetSize);
  AddFunct(IB, "GetRowCount", 0, False, Idf_MatrixGetRowCount);
  AddFunct(IB, "GetColCount", 0, False, Idf_MatrixGetColCount);
  AddFunct(IB, "SetAt", 3, False, Idf_MatrixSetAt);
  AddFunct(IB, "GetAt", 2, False, Idf_MatrixGetAt);
  AddFunct(IB, "IncAt", 3, False, Idf_MatrixIncAt);
  AddFunct(IB, "DecAt", 3, False, Idf_MatrixDecAt);
  AddFunct(IB, "SetAll", 1, False, Idf_MatrixSetAll);
  AddFunct(IB, "Load", 1, True, Idf_MatrixLoad);
  }

//---------------------------------------------------------------------------

void GCMatrix::Init(pGCClassVar pClassVar)
  {
  pClassVar->m_pSubClass = (void*)new CDMatrix();
  pClassVar->m_flags |= VarMatrixClass;
  }
  
//---------------------------------------------------------------------------

void GCMatrix::Done(pGCClassVar pClassVar)
  {
  delete (pCDMatrix)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCMatrix::CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  switch (FunctId)
    {
    case Idf_MatrixSetSize:
      {//get parameters in reverse order
      long Cols = IB.GetLParm();
      long Rows = IB.GetLParm();
      ((pCDMatrix)pSubClass)->SetSize(Rows, Cols); 
      return 0.0;
      break;
      }
    case Idf_MatrixGetRowCount:
      return (double)((pCDMatrix)pSubClass)->GetRows();
      break;
    case Idf_MatrixGetColCount:
      return (double)((pCDMatrix)pSubClass)->GetCols();
      break;
    case Idf_MatrixSetAt:
      {//get parameters in reverse order
	    double d = IB.GetDParm();
      long Col = IB.GetLParm();
      long Row = IB.GetLParm();
      if (Row<0 || Row>=((pCDMatrix)pSubClass)->GetRows())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix row index %d for %s.SetAt", Row, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      if (Col<0 || Col>=((pCDMatrix)pSubClass)->GetCols())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix columnn index %d for %s.SetAt", Col, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      (*((pCDMatrix)pSubClass))[Row][Col] = d;
      return 0.0;
      break;
      }
    case Idf_MatrixGetAt:
      {//get parameters in reverse order
      long Col = IB.GetLParm();
      long Row = IB.GetLParm();
      if (Row<0 || Row>=((pCDMatrix)pSubClass)->GetRows())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix row index %d for %s.GetAt", Row, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      if (Col<0 || Col>=((pCDMatrix)pSubClass)->GetCols())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix columnn index %d for %s.GetAt", Col, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      return (*((pCDMatrix)pSubClass))[Row][Col];
      break;
      }
    case Idf_MatrixSetAll:
      ((pCDMatrix)pSubClass)->SetAll(IB.GetDParm());
      return 0.0;
      break;
    case Idf_MatrixIncAt:
      {//get parameters in reverse order
	    double d = IB.GetDParm();
      long Col = IB.GetLParm();
      long Row = IB.GetLParm();
      if (Row<0 || Row>=((pCDMatrix)pSubClass)->GetRows())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix row index %d for %s.IncAt", Row, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      if (Col<0 || Col>=((pCDMatrix)pSubClass)->GetCols())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix columnn index %d for %s.IncAt", Col, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      (*((pCDMatrix)pSubClass))[Row][Col] += d;
      return 0.0;
      break;
      }
    case Idf_MatrixDecAt:
      {//get parameters in reverse order
	    double d = IB.GetDParm();
      long Col = IB.GetLParm();
      long Row = IB.GetLParm();
      if (Row<0 || Row>=((pCDMatrix)pSubClass)->GetRows())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix row index %d for %s.DecAt", Row, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      if (Col<0 || Col>=((pCDMatrix)pSubClass)->GetCols())
        {
        char Buff[256];
        sprintf(Buff, "Invalid matrix columnn index %d for %s.DecAt", Col, pClassVar->Name());
        IB.Err(ErrOtherRuntime, 5, Buff);
        return 0.0;
        }
      (*((pCDMatrix)pSubClass))[Row][Col] -= d;
      return 0.0;
      break;
      }
    case Idf_MatrixLoad:
      {
      CVMLoadHelper H;
      Strng Fn = IB.GetSParm();
      strcpy(H.FName, Fn());
      if (((pCDMatrix)pSubClass)->Load(H))
        return dGCtrue;
      else
        return dGCfalse;
      break;
      }
    default:
      ASSERT(FALSE);  //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCMatrix::OnSave(FilingControlBlock &FCB, void* pSubClass)
  {
  GCFCBAppendRec(FCB, '{', "Matrix", NULL, 0); // Start Internal Vars
  pCDMatrix a = (pCDMatrix)pSubClass;
  SaveVal(FCB, "Rows", a->GetRows());
  SaveVal(FCB, "Cols", a->GetCols());
  for (long i=0; i<a->GetRows(); i++)
    for (long j=0; j<a->GetCols(); j++)
      SaveVal(FCB, "d", a->m_d[i][j]);
  GCFCBAppendRec(FCB, '}', "Matrix", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCMatrix::OnLoad(FilingControlBlock &FCB, void* pSubClass)
  {
  pCDMatrix a = (pCDMatrix)pSubClass;
  long Rows = 0;
  long Cols = 0;
  bool DoneSetSize = false;
  double d;
  long i = 0;
  long j = 0;
  for (;;)
    {
    GCFCBBuff Buff;
    GCFCBReadBuff(FCB, Buff);

    //dbgpln("..%5i: %c %3i %s",FCB.SetFilePointer(0, FILE_CURRENT), Buff.Hd.Id, Buff.Hd.nTotalLen, Buff.Name());

    switch (Buff.Hd.Id)
      {
      case '}':
        if (!DoneSetSize)
          a->SetSize(Rows, Cols);
        return True;
      case '{':
        break;
      default:
        if (Buff.Try("d", d)) 
          {
          if (!DoneSetSize)
            {
            a->SetSize(Rows, Cols);
            DoneSetSize = true;
            }
          if (i<Rows && j<Cols)
            a->m_d[i][j] = d;
          j++;
          if (j>=Cols)
            {
            i++;
            j=0;
            }
          break;
          }
        if (Buff.Try("Rows", Rows)) 
          {
          break;
          }
        if (Buff.Try("Cols", Cols)) 
          {
          break;
          }
        break;
      }
    }
  if (!DoneSetSize)
    a->SetSize(Rows, Cols);
  return False;
  }
  
//---------------------------------------------------------------------------

void GCMatrix::OnRestore(void* pOldSubClass, void* pNewSubClass)
  {
  pCDMatrix pNew = (pCDMatrix)pNewSubClass;
  (*pNew) = (*((pCDMatrix)pOldSubClass));
  }

//==========================================================================

void DefinePGMClasses(rGCInsMngr IB)
  {//implemented to allow PGM Class Definitions to be added
  TRY
    {
    new GCPID2(IB);
    new GCPID3(IB);
    new GCArray(IB);
    new GCMatrix(IB);
    new GCTime(IB);
    //new GCXYFn(IB);
    new GCProfiler(IB);
    //new GCRandomProfiler(IB);
    new GCNoise(IB);
    // Electrical Stuff    
    new GCETerminal(IB);
    new GCTermStrip(IB);
    // Specie DB
    new GCSDB(IB);

    #ifdef PMC
    //optimiser equations used at PMC 1995...
    new GCQuadEqn(IB);
    new GCCircEqn(IB);
    new GCPartEqn(IB);
    new GCAveEqn(IB);
    #endif
    }
  CATCH(GCException, thePGMException)
    {
    TRACE("ERROR: Check PGM classes!!!\n");
    ASSERT(FALSE); //error defining PGM classes, NOT a PGM code error.
    }
  END_CATCH
  }

//==========================================================================
