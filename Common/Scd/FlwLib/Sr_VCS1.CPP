//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sc_defs.h"

#define  __SR_VCS1_CPP

#include "sp_db.h"
#include "sr_vcs1.h"
//#include "optoff.h"

#define dbgVCS 01
#if dbgVCS
#include "dbgmngr.h"
static CDbgMngr dbgSolve      ("RctVCS", "Solve");
static CDbgMngr dbgSolveSeq   ("RctVCS", "SolveSeq");
static CDbgMngr dbgIterTemp   ("RctVCS", "IterTemp");
static CDbgMngr dbgIterMole   ("RctVCS", "IterMole");
static CDbgMngr dbgMatrix     ("RctVCS", "Matrix");
static CDbgMngr dbgIterDFE    ("RctVCS", "IterDFE");
static CDbgMngr dbgIterThermo ("RctVCS", "IterThermo");
static CDbgMngr dbgElemAbund  ("RctVCS", "ElemAbund");
static CDbgMngr dbgElCorr     ("RctVCS", "ElCorr");
static CDbgMngr dbgELAB       ("RctVCS", "ELAB");
static CDbgMngr dbgResult     ("RctVCS", "Result");

//static CDbgMngr dbgSolveSimplex("RctVCS", "SolveSimplex");
//static CDbgMngr dbgDumpSimplex("RctVCS", "DumpSimplex");
#endif

inline double kPa2Atm(double x) { return x/101.325;};
inline double Atm2kPa(double x) { return x*101.325;};

const long    MaxStateCnts  = 20000;
const double  TempTol       = 1.0e-3;

//===========================================================================
//===========================================================================
//
//  This VCS (Villars Cruise Smith) algorithm was originally implmented
//  from the book:
//
//  Chemical Reaction Equilibrium Analysis
//    William R Smith
//    Ronald W Missen
//
//  John Wiley & Sons
//  ISBN 0-471-09347-5  AACR2
//
//  Initially translated by Bruce Botes of Kenwalt
//
//  Rearranged to fit into SysCAD during August 2001
//
//===========================================================================
//===========================================================================

// Error codes

#if dbgVCS
char  *dbgRouteTxt[] = {
  "LInitData",
  "LReInitData",
  "L1955-Init()",
  "L429-BasOpt()",
  "L6714- bad exit",
  "L430-DeltaG(0,1,m_NR)",
  "L093-init for each m_Iter.",
  "L090- Calculate()",
  "L096-ElemAbund()",
  "L441-ChkOptBasis()",
  "L212-EquiChkMaj()",
  "L227-EquiChkMin()",
  "L24192-EvaIInitVect()",
  "L1366-ChkDelSpecie()",
  "L857-DumpChanges()",
  "LXferResult",
  "LAdjTemp",
  "LFinished",
  0
  };

static void SetMyDebug(int AllowTrunc)
  {
  int DoDbg=1;//(CnvrgeStatesCnt++>=1000)?1:0;
  if (AllowTrunc)
    dbgfiletrunc(0);
  dbgSolve.bOn=DoDbg;
  dbgSolveSeq.bOn=DoDbg;
  dbgIterTemp.bOn=DoDbg;
  dbgIterMole.bOn=DoDbg;
  //dbgMatrix.bOn=DoDbg;
  //dbgIterDFE.bOn=DoDbg;
  dbgIterThermo.bOn=DoDbg;
  dbgElemAbund.bOn=DoDbg;
  dbgElCorr.bOn=DoDbg;
  dbgELAB.bOn=DoDbg;
  dbgResult.bOn=DoDbg;
  }

#define WITHRESTART (1)
#define WITHFORCERESTART (0 && dbgVCS)
#if WITHFORCERESTART
static int ForceRestart=0;
#endif

#else

#define WITHRESTART (0)
#define WITHFORCERESTART (0)

#endif

long CVCSSolver::Solve(SpModel * pModel, SpVector & Moles, double TempStart, double TempFinal, double Press, flag FindTemp)
  {
  long ret=VCS_NO_ERR;

  try
    {
    long Pass=0;
    m_pModel=pModel;
    m_bFindTemp=FindTemp;

#if WITHRESTART
ReTry:
#endif

    long StateCnt=0;
    #if WITHFORCERESTART
    ForceRestart=0;
    #endif
    #if dbgVCS
    long Both = 0;
    if (Pass>0 || Both)
      {
      SetMyDebug(1 && ((!Both && Pass>0) || (Both==1 && Pass==0)));
      if (dbgSolve() && Pass>0)
        dbgp("\n\n\n=================================== RESTART ====================================\n\n");
      }

    if (dbgSolveSeq() || dbgMatrix() || dbgIterMole() || dbgIterTemp())
      dbgSolve.bOn=1;

    if (dbgSolve())
      dbgp("================= Villars Cruise Smith Free Energy Minimisation ================");
    #endif

    // Initialise VCS Algorithm
    m_M=m_nMaxSpecie-1;
    m_NE=m_nMaxElement-1;
    // Copy Info

    // Get data & set options
    m_nMaxIt = 200;
    m_Alpha = 0.0;
    m_IEst = 1; // 0=User Estimate / 1=algorithm to do initial estimate
    m_Type = 2; // kJ/kmol
    m_nNotDeleteItCnt=0;
    m_nMaxNotDeleteItCnt=20;

    m_TempKStart=TempStart;
    m_TempKFinal=TempFinal;
    m_Press=kPa2Atm(Press);

    for (int s=0; s<SVSpcCount(); s++)
      {
      m_SpMoles.VValue[s]=Moles[s];
      m_SpMass.VValue[s]=Moles[s]*SDB[s].MoleWt();
      }

    // now we must enter the activity formalism
    if (m_Alpha != 0.0)   // only do this for non_ideal cases
      {
      //VCS_act[k] = current_opcode = ptr->act_c[i]; // assign the index into the opcode_array
      //if (current_opcode != MARKER)
      //  { // this compound must have a activity formalism
      //  // now we must go through our local copy of the opcodes and adjust the references
      //  while (opcode[current_opcode++].mnem != RTN)
      //    {
      //    if (opcode[current_opcode].mnem == GET)
      //       opcode[current_opcode].x.entry += (INDEX)(k - i); // now the opcode will point to the correct specie in VCS_specie
      //    }
      //  }
      }

      //memcpy(opcode, opcode_array, sizeof(opcode)); /* make a local copy of the opcode array */

    InitialiseDataPhase();

    m_NoPhases = 0;
    for (int p = 1; p < m_nMaxPhase; p++)
      if (m_NoSpeciePhase[p] != 0)
        m_NoPhases++; // Count the number of phases
    if (m_NoPhases == 0)
      return VCS_PHASE_ERR;

    //InitialiseData();
    m_Route = LInitData;
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\n[%7i] ------------------------------------- %s",StateCnt, dbgRouteTxt[m_Route]);
    #endif
    m_TLoop=0;
    do
      {
      switch (m_Route)
        {
        case LInitData:
          InitialiseData();
          m_Route = L1955;
          break;
        case LReInitData:
          ReInitialiseData();
          m_Route = L1955;
          break;
        case  L1955   :
          Init();
          break;
        case  L429    :
          // determine basis species, evaluate sioichiomerty
          BasOpt(FALSE);
          if (! m_CONV)
            {
            m_Route = L24192;
            break;
            }
          #if dbgVCS
          else
            {
            m_Route = L6714;
            break;
            }
          #endif
          // else fall through
        case  L6714   :
          m_ICONV1=TRUE;
          m_Route = L857;     // print results
          break;
        case  L430    :
          DeltaG(0,1,m_NR);   // evaluate all reaction free energy changes
          if (!m_LBO)
            {
            m_Route = L212;
            break;
            }
          m_LBO = FALSE;
          if (m_IM)
            {    // reaction entirely between minors
            m_Iti = 0;
            m_Route = L090;
            break;
            }
          #if dbgVCS
          else
            {
            m_Route = L093;
            break;
            }
          #endif
          // else fall through
        case  L093    :
          m_Iti = 4*(m_It1/4)-m_It1;  // set initial values for iteration
          if (m_Iti != 0)
            {
            m_Route = L090;
            break;
            }
          DFE(m_W,0,1);     // for m_TMole and all components + m_Minor reactions only
          DeltaG(1,1,m_NR);     // minors only
          #if dbgVCS
          if (dbgSolveSeq())
            dbgp("\nCalled 'DFE()' and 'DeltaG()' for minors");
          m_Route = L090;
          break;
          #endif
          // fall through
        case  L090    :
          Calculate();
          // THIS IS A BDB MOD TO THE CODE TO STOP THE PROGRAM AFTER m_nMaxIt
          // AS WITH ALPHA SET IT SOMEHOW MISSES 'EquiChkMaj' IF ALL THE
          // REACTIONS ARE AMONG THE MINORS
          if(m_Iter > m_nMaxIt || m_nNotDeleteItCnt>m_nMaxNotDeleteItCnt)
            {
            m_ICONV = TRUE;
            m_Route = L857;
            break;
            }
          break;
        case  L096    :
          ElemAbund(); // evaluate final/intermediate element abundances
          break;
        case  L441    :
          ChkOptBasis();
          break;
        case  L212    : // Check for equilibrium among the reactions
          if (m_IM)
            {           // true if all the reactions are m_Minor
            m_Route = L227;   // thus only check minors for equilib
            break;
            }
          EquiChkMaj(); // check equil of major species
          break;
        case  L227    :
          EquiChkMin(); // check equil of m_Minor species
          break;
        case  L24192    :
          EvalInitVect();   // evaluate initial major-m_Minor vector
          break;
        case  L1366   :
          ChkDelSpecie(); // re-check deleted spesies
          break;
        case  L857    :
          if (m_bFindTemp)
            {
            #if dbgVCS
            if (dbgSolveSeq())
              dbgp("\n-------------------------------------%s = %i","m_TLoop",m_TLoop);
            #endif
            double OldTemp=m_pModel->Temp();
            AdjustTemp();
            #if dbgVCS
            if (dbgSolveSeq())
              dbgp("\n-------------------------------------%s %12.6f > %12.6f","Final Temp", OldTemp, m_TempKFinal);
            #endif
            if (fabs(OldTemp-m_TempKFinal)<TempTol)
              DumpChanges();
            else
              {
              if (m_TLoop++<50)
                m_Route=LReInitData;
              else
                {
                ret=VCS_NO_TCONV;
                DumpChanges();
                }
              }
            }
          else
            {
            DumpChanges();    // final delta G/RT values
            }
          break;
        case LXferResult:
          #if dbgVCS
          TransferResult(dbgResult()!=0);
          #else
          TransferResult(false);
          #endif
          CalcFinalInfo();
          m_Route=LFinished;
          break;
        }
      #if dbgVCS
      if (dbgSolveSeq())
        dbgp("\n[%7i] ------------------------------------- %s",StateCnt, dbgRouteTxt[m_Route]);
      #endif
      }
#if WITHFORCERESTART
    while((m_Route != LFinished) && !ForceRestart && StateCnt++<MaxStateCnts);
    if (((m_Route != LFinished) || ForceRestart) && Pass++==0)
      goto ReTry;
#elif WITHRESTART
    while((m_Route != LFinished) && StateCnt++<MaxStateCnts);
    if ((m_Route != LFinished) && Pass++==0)
      goto ReTry;
#else
    while((m_Route != LFinished) && StateCnt++<MaxStateCnts);
#endif
    }

  catch ( CVCSException *e)
    {
    LogError(m_pModel->FullObjTag(), 0, "FEM-VCS %s", e->Msg);
    ret=VCS_NO_CONV;
    goto Done;
    }

  if (m_ICONV)
    {
    LogNote(m_pModel->FullObjTag(), 0, "FEM-VCS Conversion criterion not statisfied");
    ret=VCS_NO_CONV;
    goto Done;
    }
  else if (m_ICONV1)
    {
    LogNote(m_pModel->FullObjTag(), 0, "FEM-VCS Convergence to number of positive N(I) less than C.");
    goto Done;
    }

Done:
  #if dbgVCS
  if (dbgSolve())
    dbgp("\n================================================================================\n");
  #endif
  return ret;
  }

// ----------- start of iteration loop --------------
CVCSSolver::VCSPath CVCSSolver::Init(void)
  {
  int i;

  for (i = 1; i <= m_NoPhases; i++)
    m_TMole[i] = m_TInert[i];
  for (i = 1; i <= m_MR; i++)
    m_TMole[m_SI[i]] += m_W[i];    // add moles of species for each Phase
  m_TMole[0] = 0.0;                // total moles for Single specie Phase
                                   // is unimportant
  DFE(m_W,0,0);                    // for m_TMole and all components and reactions.
  if (m_Iter > 0)
    return (m_Route = L430);       // evaluate all reaction free energy changes
  if (m_IEst != 0)
    return (m_Route = L441);       // chk for optimum basis when machine estimate
  return (m_Route = L429);
  }


// ---------- evaluate initial major-m_Minor vector -----------
CVCSSolver::VCSPath CVCSSolver::EvalInitVect(void)
  {
  int L,i;

  m_It1 = 1;
  m_ILT = FALSE;
  m_Minor = 0;
  for (i = 1; i <= m_NR; i++)
    {                              // check each reaction for major / minors
    L = m_IR[i];
    if (m_W[L] > 0.0)
      m_IC[i] = MAJOR;
    else
      {
      m_Minor++;
      m_IC[i] = NMINOR;
      }
    }
  m_LEC = FALSE;                   // Set equilibrium check flag
  m_LBO = TRUE;
  if (m_Iter > 0)
    return (m_Route = L096);       // not the first iteration
  return (m_Route = L430);         // evaluate all reaction free energy changes
  }


// ----------------- This is the actual reaction solver ----------------------
// --- calculates m_Wt[L] = m_W[L] + par * m_DS[L] ---------------------------------
CVCSSolver::VCSPath CVCSSolver::Calculate(void)
  {
  int i,lk,                        // current stoichiometric reaction
      L,k,xm;

  double  par,xx,c,dgg,ww,dx;

  for (i = 1; i <= m_MR; i++)
    m_FEL[i] = m_FE[i];
  if (!(m_ILT || m_IM))    // m_IM => True if all reactions are minors
    {
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\nCalling 'ST2(SOLDEL) from 'calculate()'");
    #endif
    if (FALSE == ST2())                   // calculates m_DS[] - delta sigma
      {
      #if dbgVCS
      if (dbgSolveSeq())
        dbgp("\nNOT FOLLOWING CONVENTIONAL m_Route - exiting 'calculate()'");
      #endif
      return (m_Route = L429);            // do basis optimisation again
      }
    }
  m_ITL = FALSE;                              // Set equilibrium check flag
  m_LEC = FALSE;
  for (i = 1; i <= m_NoPhases; i++)
    m_DeltaMole[i] = 0.0;                  // clear the mole differences per Phase
  for (i = 1; i <= m_NC; i++)
    m_DS[i] = 0.0;    // clear the extents of the components
  i = 1;
  do
    {                                         // do for each reaction i
    L = m_IR[i];
    switch(m_IC[i])
      {
      case MAJOR  :
        if (fabs(m_DG[i]) <= 1e-6)
          {                           // reaction in equilibrium
          m_Wt[L] = m_W[L];     // cancel adjustments
          m_DS[L] = 0.0;
          break;
          }
        else
          {                           // reaction not in equilibrium
          dx = m_DS[L];
          m_Wt[L] = m_W[L] + dx;      // adjust reaction by extent
          if (m_Wt[L] > 0.0)      // check for positive major species
            {
              // adjust component moles of the reaction accordingly
            for (k = 1; k <= m_NC; k++)
              m_DS[k] += m_SC[k][i]*dx;
            for (k = 1; k <= m_NoPhases; k++)
              m_DeltaMole[k] += m_DMole[k][i] * dx;
            break;
            }
          else
            {            // non positive major species
            if (m_SI[L] != SinglePhase)
              {
              m_DS[L] = dx = -0.9*m_W[L]; // cut reaction adustment to 1/3
              m_Wt[L] = 0.1*m_W[L];
              // adjust component moles of the reaction accordingly
              for (k = 1; k <= m_NC; k++)
                m_DS[k] += m_SC[k][i]*dx;
              for (k = 1; k <= m_NoPhases; k++)
                m_DeltaMole[k] += m_DMole[k][i] * dx;
              // is reaction greater than smallest component/100
              if (m_W[L] >= 0.01*m_W[m_NC])
                {
                break;  // do next reaction
                }
              else
                {
                m_IC[i] = MINOR;     // change major to m_Minor
                #if dbgVCS
                if (dbgSolveSeq())
                  dbgp("\nChanged major %3d to m_Minor",i);
                #endif
                // fall through to handel minors
                }
              }
            else
              {                  // set single-specie Phase to zero
              dx = -m_W[L];
              // calculate extent so that the components stay positive
              for (k = 1; k <= m_NC; k++)
                {
                m_Wt[k] = m_W[k] + m_SC[k][i]*dx;
                if (m_Wt[k] <= 0.0)       // reset loop counter and do it again
                  {
                  k = 1;
                  dx = dx/2;
                  }
                }
              // set final moles immediately
                m_W[L] += dx;
              for (k = 1; k <= m_NC; k++)
                m_W[k] = m_Wt[k];
              for (k = 1; k <= m_NoPhases; k++)
                m_TMole[k] += m_DMole[k][i] * dx;
              m_Wt[L] = m_W[L];
              m_DS[L] = 0.0;
              if (m_W[L] > 0.0)           // if posetive mass exit
                break;
              m_IC[i] = NMINOR;             // negative -> change major to neg. m_Minor
              #if dbgVCS
              if (dbgSolveSeq())
                dbgp("\nChanged major %3d to negative m_Minor",i);
              #endif
              DFE(m_W,0,m_Iti);   // for m_TMole and components + all reactions/majors only
              DeltaG(m_Iti,1,m_NR);   // majors or all reactions
              for (k = 1; k <= m_MR; k++)
                m_FEL[k] = m_FE[k];
              }
            // if we got here we changed a major to a m_Minor specie
            m_Minor++;          // correct total minors
            m_IM = (m_Minor == m_NR);
            if (m_IM && (m_Iti != 0))   // all reactions are minors
              return (m_Route = L227);  // exit & do equilib check for MINOR
            break;
            }
          }
        // a MINOR reaction is a specie smaller
        //              than smallest component/100
        //              which is part of a multi specie Phase
      case MINOR  :
        if (m_Iti == 0)  // we do this only every fourth iteration
          {
          dgg = m_DG[i];
          ww = m_W[L];
          if (dgg < 82.0)
            {
            if (fabs(dgg) <= 1e-5)
              {                          // reactions in equilibrium
              m_Wt[L] = m_W[L];     // delete adjustments
              m_DS[L] = 0.0;
              break;
              }
            else
              {
              c = log((double)ww)-dgg;
              if (c > - 73.6)
                {  // limit m_Wt[L] to a minimum of e-32 moles
                m_Wt[L] = exp(c); // equation 9.5-1 in book
                if (m_Wt[L] > m_W[1])
                  m_Wt[L] = ww * 1e6; // new m_Minor moles greater
                                      // than biggest component
                m_DS[L] = dx = m_Wt[L] - ww;
                if (ww <= (-dx))
                   m_ITL = TRUE;      // step too large - Force
                // adjust component moles of the reaction accordingly
                for (k = 1; k <= m_NC; k++) m_DS[k] += m_SC[k][i]*dx;
                for (k = 1; k <= m_NoPhases; k++)
                  m_DeltaMole[k] += m_DMole[k][i] * dx;
                break;
                }
              }
            }
          // thus m_DG[i] >= 82 or (m_DG[i] < 82 and c < -73.6)
          if (ww >= 1e-26)
            {
            m_Wt[L] = ww * 1e-6;
            m_DS[L] = dx = -0.999999*m_W[L];
            // adjust component moles of the reaction accordingly
            for (k = 1; k <= m_NC; k++) m_DS[k] += m_SC[k][i]*dx;
            for (k = 1; k <= m_NoPhases; k++)
              m_DeltaMole[k] += m_DMole[k][i] * dx;
            break;
            }
          else
            {  // delete reaction specie because < 1e-26
            #if dbgVCS
            if (dbgSolveSeq())
              dbgp("\nCalling 'delete(L,0)' from 'calculate'");
            #endif
            if (!DeleteIt(L,0))
              {
              // cannot delete because it was the last reaction
              #if dbgVCS
              if (dbgSolveSeq())
                dbgp("\n   EXITING 'calculate()' to re-check deleted specie");
              #endif
              return (m_Route = L1366); // exit & re-check deleted species
              }
            #if dbgVCS
            if (dbgSolveSeq())
              dbgp("\nReturned from 'delete(L,0)' to 'calculate'");
            #endif
            m_Minor--;
            break;
            }
          }
        else m_DS[L] = 0.0 ; // only calculate MINORS every 4 th iteration
          break;
        // NMINOR is a zero mole single-specie Phase
      case NMINOR :
        if  (m_DG[i] < 0.0)
          {  // Check if we can make it a major
          m_Minor--;
          m_IC[i] = MAJOR;
          m_IM = FALSE;
          m_ILT = FALSE;
          #if dbgVCS
          if (dbgSolveSeq())
            dbgp("\nChanged nminor %3d to major",i);
          #endif
          }
        m_Wt[L] = m_W[L];   // delete any adjustments
        m_DS[L] = 0.0;
        break;
      }  // end of switch()
    i++;
    }
  while (i <= m_NR);
  // -- limit reduction of components to 99 % of its original moles -
  // -- thus calculate the step size parameter such that the components -
  // -- stay positive --
  par = 0.5;
  for (i = 1; i <= m_NC; i++)
    {
    xx = -m_DS[i]/Max(1.0e-70, m_W[i]);
    if (par < xx)
      par = xx;
    }
  par = 1.0/par;
  if (par <= 1.01 && par > 0.0)         // reduction in basis too large
    {                                    // par was greater than 1
    par = 0.99*par;                     // reduce overall step size
    m_ITL = FALSE;            // step too large -> Force
    for (i = 1; i <= m_MR; i++)
      m_DS[i] *= par;
    for (i = 1; i <= m_NoPhases; i++)
      m_DeltaMole[i] *= par;
    lk = m_MR;
    }
  else
    {
    lk = m_NC;
    }
   // At last adjust the reaction specie mass by the calculated extent
  for (i = 1; i <= lk; i++)
    m_Wt[i] = GEZ(m_W[i] + m_DS[i]);
  for (i = 1; i <= m_NoPhases; i++)            // also do it for the phases
    m_TMole1[i] = GEZ(m_TMole[i] + m_DeltaMole[i]); // new total moles
  // ------- temporary chemical potentials --------
  DFE(m_Wt,1,m_Iti);  // for m_TMole1 and components + all reactions/majors only
  m_FORCED = FALSE;
  if (!(m_IM || m_ILT || m_ITL))
    {
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\nCalling 'Force()' from 'calculate()'");
    #endif
    Force();  // convergence forcer
    }
  // --------- reset values at end of iteration ---------------------
  // --------- calculate new reaction free energy changes -----------
  if (!m_FORCED)
    {
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\nCONVERGENCE WAS NOT m_FORCED!!!!!!!");
    #endif
    for (i = 1; i <= m_NoPhases; i++)
      m_TMole[i] = m_TMole1[i];
    for (k = 1; k <= m_MR; k++)
      if (m_DS[k] != 0.0)
        m_W[k] = m_Wt[k];
    }
  #if dbgVCS
  if (dbgIterMole())
    {
    prin_immed(); // print intermediate results
    dbgp("\nTransfering data to spreadsheet");
    }
  #endif

  if (m_Alpha != 0.0)
    GetActivityCoef(); // getting new chem. pot. in m_FF[]
  if (m_bFindTemp || m_Alpha != 0.0)
    DFE(m_W,0,m_Iti); // for m_TMole and components + all reactions/majors only

// exit  if escape key was pressed
//  if (kbhit() && (getch() == 27))
//   {
//    m_ICONV = TRUE;
//    return (m_Route = L857);
//  }

  DeltaG(m_Iti,1,m_NR); // m_Iti = 0 ->all reactions from 1 to m_NR
            // else for major reactions only
  m_Iter++;
  m_It1++;
  // ----------- set  microscopic multispecies Phase (m_TMole < 1e-10) to zero ------
  xm = 0;
  for (i = 1; i <= m_NoPhases; i++)
    {
    if(((m_TMole[i]-m_TInert[i]) < 1e-10) && m_NoSpeciePhase[i] != 0 && m_TMole[i] != 0.0)
      {
      #if dbgVCS
      if (dbgSolveSeq())
        dbgp("\n DELETING PHASE %3d - total moles less than 1e-10",i);
      #endif
      for (k = 1; k <= m_MR; k++)
           {
        if (m_SI[k] == i)
               {
          m_W[k] = 0.0;
          m_Wt[k] = 0.0;
          m_DS[k] = 0.0;
        }
      }
      m_TMole[i] = 0.0;
      m_TMole1[i] = 0.0;
      m_DeltaMole[i] = 0.0;
      xm = 1;
      }
    }
  if (xm == 0)
    return (m_Route = L441);      // no Phase deleted
                  // exit and check for optimum basis, convergence
  // a Phase was deleted, check the reactions
  for (k = 1; k <= m_NR; k++)
    if (m_W[(m_IR[k])] == 0.0)
      {
      m_IC[k] = NMINOR;
      #if dbgVCS
      if (dbgSolveSeq())
        dbgp("\nChanged major %3d to negative m_Minor",k);
      #endif
      }
  BasOpt(FALSE);  // do basis optimisation and calculate stioch. matrix
  DFE(m_W,0,0);     // for m_TMole and components + all reactions
  DeltaG(0,1,m_NR); // all reactions from 1 to m_NR
  if(m_CONV)
    return (m_Route = L6714); // the biggest specie is zero
                                // - no optimum basis in BasOpt
  else
    return(m_Route = L441);       // exit and check for optimum basis
  }

// ----------- check for optimum basis ---------------------
// ----------- then check for convergence ------------------
CVCSSolver::VCSPath CVCSSolver::ChkOptBasis(void)
  {
  int i,j,k,jj,         // counters
      L;

  k = 2;
  while ((m_W[k-1] >= m_W[k]) && (k <= m_NC))
    k++;
  if ((k > m_NC) && (m_NC != 1))
    {
    for (k = 1; k <= m_NR; k++)
      {                               // check if reactions mass is larger than
      L = m_IR[k];                  // component mass
      j = m_NC;
      while ((m_W[L] > m_W[j]) && (j > 0))
        {
        if (m_SC[j][k] != 0.0)              // no optimum basis
          return (m_Route = L429);      // recalc basis
        j--;
        }
      }
    }
  else
    {
    for (j = 1; j <= m_NR; j++)
      {                               // check if components are sorted in decreasing
      L = m_IR[j];                  // masses
      for (jj = 1; jj <= m_NC; jj++)
        {
        if ((m_W[L] > m_W[jj]) && (m_SC[jj][j] != 0.0))  // no optimum basis
          return (m_Route = L429);            // recalc basis
        }
      }
    }
  if (m_Iter == 0)
    return (m_Route = L24192);                // this will only occur when user estimate is used
                          // so evaluate the major-m_Minor vector
  if (m_Iti != 0)
    return (m_Route = L212);                  // check for equilibrium among major reactions
  // ------------- re-evaluate major-m_Minor vector when minors are evaluated -------------
  m_Minor = 0;
  for (jj = 1; jj <= m_NC; jj++)
    m_DS[jj] = 0.01*m_W[jj]; // set extent of reaction for all components to 1/100*m_W[i]
  for (i = 1; i <= m_NR; i++)
    {        // check each reaction
    L = m_IR[i];
    if (m_W[L] <= 0.0)
      {                          // negative moles
      if (m_DG[i] > 0.0)
        {                      // but posetive delta G ??
        m_IC[i] = NMINOR;
        #if dbgVCS
        if (dbgSolveSeq())
          dbgp("\nChanged %3d  to nminor in 'ChkOptBasis'",i);
        #endif
        m_Minor++;
        }
      else
        {             // chemical potential for new major is negative
        m_IC[i] = MAJOR;
        #if dbgVCS
        if (dbgSolveSeq())
          dbgp("\nChanged %3d  to major in 'ChkOptBasis'",i);
        #endif
        }
      }
    else
      {                   // positive moles
      if (m_SI[L] != SinglePhase)
        {
        for (jj = 1; jj <= m_NC; jj++)
          if ((m_SI[jj] != SinglePhase && m_SC[jj][i] != 0.0 && m_W[L] >= m_DS[jj]) && (jj <= m_NC))
            goto jump;
        if  (m_W[L] < 0.1*m_TMole[m_SI[L]])
          {
          m_IC[i] = MINOR;
          #if dbgVCS
          if (dbgSolveSeq())
            dbgp("\nChanged %3d  to m_Minor in 'ChkOptBasis'",i);
          #endif
          m_Minor++;
          }
        else
          {
jump:     m_IC[i] = MAJOR;
          #if dbgVCS
          if (dbgSolveSeq())
            dbgp("\nChanged %3d  to major in 'ChkOptBasis'",i);
          #endif
          }
        }
      else
        {                     // for single - specie phases
        m_IC[i] = MAJOR;
        #if dbgVCS
        if (dbgSolveSeq())
          dbgp("\nChanged %3d  to major in 'ChkOptBasis'",i);
        #endif
        }
      }
    }
  m_IM = (m_Minor == m_NR);    // Set m_IM true if all the reactions are m_Minor
  return (m_Route = L212); // equilibrium check for majors
  }

// THE FOLLOWING FUNCTIONS CAN STILL BE OPTIMISED
// ---------------- CHECK FOR CONVERGENCE -----------------------
// -----------equilibrium check for major species ---------------
CVCSSolver::VCSPath CVCSSolver::EquiChkMaj(void)
  {
  int i;              // loop counter

  if (m_Iter >= m_nMaxIt)
    {
    m_ICONV = TRUE;
    return (m_Route = L857);  // print results
    }
  for (i = 1; i <= m_NR; i++)
    {
    if ((m_IC[i] > MINOR && fabs(m_DG[i]) > 1e-6))
      {
      m_ILT = FALSE;
      return (m_Route = L093);  // pre-set and calculate again
      }
    }
  // ----- equilibrium among majors => no check for minors --------
  if (m_Minor == 0)
    return (m_Route = L096); // check element abundances and maybe exit
  else
    return (m_Route = L227);   // equilibrium check for m_Minor species
  }

// -----------equilibrium check for m_Minor species ---------------
CVCSSolver::VCSPath CVCSSolver::EquiChkMin(void)
  {
  int i;              // loop counter

  if (m_Iti == 0)
    DeltaG(1,1,m_NR);   // minors only
  else
    {
    DFE(m_W,0,1); // for m_TMole and components + m_Minor reactions only
    DeltaG(1,1,m_NR);         // minors only
    }
  if (m_Iter >= m_nMaxIt)
    {
    m_ICONV = TRUE;
    return (m_Route = L857);  // print results
    }
  for (i = 1; i <= m_NR; i++)
    {
    if ((m_IC[i] == MINOR && fabs(m_DG[i]) > 1e-5))
      {
      m_ILT = TRUE;         // convergence among majors
                            // but not among minors
      m_Iti = 0;
      return (m_Route = L090);  // calculate again
      }
    }
  // ------ equilibrium among minors as well ---------
  return (m_Route = L096);  // check element abundances
  }

// ---------- evaluate final/intermediate element abundances ---------------
CVCSSolver::VCSPath CVCSSolver::ElemAbund(void)
  {
  int i;          //loop counter

  ELAB();
  #if dbgVCS
  if (dbgSolveSeq() || dbgElemAbund())
    {
    dbgp("\nElemental abundances            Correct    From estimate    Diff\n");
    for (i = 1; i <= m_NC; i++)
      {
      if (m_GAI[i] == 0.0)
        dbgp("  %2d %-14s    %15.1f  %+.7e  %+.7e\n",i, EDB[m_L2ElId[i]].m_Name,m_GAI[i],m_GA[i],fabs(m_GA[i] - m_GAI[i]));
      else
        dbgp("  %2d %-14s    %+15.7e  %+.7e  %+.7e\n",i, EDB[m_L2ElId[i]].m_Name,m_GAI[i],m_GA[i],fabs(m_GA[i] - m_GAI[i]));
      }
    }
  #endif

  if (m_LEC)
    {      // if the bottom loop was already done
    for (i = 1; i <= m_NC; i++)
      {
//      if (m_GAI[i] < 1.0e-20) // CNM == 0.0)
      if (m_GAI[i] == 0.0)
        {               // zero elements inputed
        if (fabs(m_GA[i]) > 1e-10)
          {
          #if dbgVCS
          if (dbgElemAbund())
            dbgp("--------> GOTO L1955 @ 2  %.7e > %.7e", fabs(m_GA[i]), 1e-10);
          #endif
          if (ElCorr()==0)                  // correct element abundances
            return (m_Route = L1955);    // go to beginning of iteration cycle
          // par too small - abort
          // temp
#if WITHFORCERESTART
          ForceRestart=1;
#endif
          goto LoopDone;
          }
        }
      else
        {
        //---- changed 0.5e-8 to 0.5e-6 ----
        // -- C floats are 7 digit precision
        if (fabs(m_GA[i] - m_GAI[i]) > 0.5e-8*m_GAI[i])
          {
          #if dbgVCS
          if (dbgElemAbund())
            dbgp("--------> GOTO L1955 @ 3  %.7e > %.7e", fabs(m_GA[i] - m_GAI[i]), 0.5e-8*m_GAI[i]);
          #endif
          if (ElCorr()==0)                  // correct element abundances
            return (m_Route = L1955);    // go to beginning of iteration cycle
          // par too small - abort
          // temp
#if WITHFORCERESTART
          ForceRestart=1;
#endif
          goto LoopDone;
          }
        }
      }

LoopDone:
    if(m_MR == m_M)
      {
      #if dbgVCS
      if (dbgElemAbund())
        dbgp("--------> GOTO L857 no deleted species -> print results");
      #endif
      return (m_Route = L857);  // no deleted species -> print results
// THIS IS THE END OF THE EQUILIBRIUM CALCULATIONS
      }
    else
      {
      #if dbgVCS
      if (dbgElemAbund())
        dbgp("--------> GOTO L1366 re-check deleted species");
      #endif
      return (m_Route = L1366);     // re-check deleted species
      }
    }
  else
    {
    #if dbgVCS
    if (dbgElemAbund())
      dbgp("--------> GOTO L1955 @ 3 LEC False");
    #endif
    m_LEC = TRUE;           // must go through this loop before the above loop
    ElCorr();           // correct element abundances
    return (m_Route = L1955);    // go to beginning of iteration cycle
    }
  }

// ---------- re-check deleted species -------------------------
CVCSSolver::VCSPath CVCSSolver::ChkDelSpecie(void)
  {
  int i,k,        // loop counter
      mr1,nr1,npb,
      L;          // index

  double xt[MAXPHASE];

  mr1 = m_MR + 1;
  nr1 = m_NR + 1;
  for (i = mr1; i <= m_M; i++) // from the last reaction to the last defined specie
    m_FE[i] = m_FF[i];
  DeltaG(0,nr1,m_N);        // all reactions
  npb = 0;
  for (k = 1; k <= m_NoPhases; k++)
    if (m_TMole[k] > 0.0)
      xt[k] = log((double)1e32 * m_TMole[k]);
  for (i = nr1; i <= m_N; i++)
    {
    L = m_IR[i];
    k = m_SI[i];
    if ((m_TMole[k] > 0.0 && m_DG[i] < xt[k]) || (m_TMole[k] == 0.0 && m_DG[i] < 0.0))
      {
      m_W[L] = 1e-20;
      m_TMole[k] += m_W[m_MR];
      m_IC[i] = MINOR;
      m_Minor++;
      m_NR++;
      m_MR++;
      npb = 1;
      if (!DeleteIt(L,1))
        {

        #if dbgVCS
        if (dbgSolveSeq())
          dbgp("\nChkDelSpecie --- %i Not Deleted", L);
        #endif
        return (m_Route = L1366); // possibillity of an unlimited loop
        }
      }
    }
  if (npb == 0)
    {
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\nChkDelSpecie --- npb == 0");
    #endif
    return (m_Route = L857);    // print results
    }
  else
    {
    m_ILT = FALSE;
    DFE(m_W,0,1); // for m_TMole and components + m_Minor reactions only
    DeltaG(0,nr1,m_NR);   // all reactions
    m_Iti = 0;
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\nChkDelSpecie --- npb != 0");
    #endif
    return (m_Route = L090);
    }
  }

int CVCSSolver::DeleteIt(int L, int mm)
  {
  int    nnl,k;
  double temp,t1;

  nnl = L - m_NC;
  if (L != m_MR)
    {    // re-arrange data when specie  added or removed
    DSW(m_Wt,m_MR,L);
    DSW(m_W,m_MR,L);
    DSW(m_FF,m_MR,L);
    DSW(m_InitialChemPot,m_MR,L);
    if (m_SI[m_MR] == SinglePhase)
      DSW(m_FE,m_MR,L);
    Swap2(m_SI,m_MR,L);
    DSW(m_FEL,m_MR,L);
    Swap2(m_IC,m_NR,nnl);
    DSW(m_DA,m_MR,L);       // this seems unnecessary
    Swap2(m_Ind,m_MR,L);
    Rotate(m_Index2, m_MR, L);
    for (k = 1; k <= m_NC; k++)
      {
      t1 = m_SC[k][m_NR];
      m_SC[k][m_NR] = m_SC[k][nnl];
      m_SC[k][nnl] = t1;
      }
    for (k = 1; k <= m_NoPhases; k++)
      {
      temp = m_DMole[k][m_NR];
      m_DMole[k][m_NR] = m_DMole[k][nnl];
      m_DMole[k][nnl] = temp;
      }
    for (k = 1; k <= m_NE; k++)
      {
      temp =m_BM[m_MR][k];
      m_BM[m_MR][k] = m_BM[L][k];
      m_BM[L][k] = temp;
      }
    }
  if (mm != 0)
    {
    //m_nNotDeleteItCnt=0;
    #if dbgVCS
    if (dbgSolveSeq())
      dbgp("\nDeleteIt --- %i %i %i %i (%i)", L, mm, m_NR, m_MR, m_nNotDeleteItCnt);
    #endif
    return (TRUE);
    }
  // ------- extra procedures when removing a species ---------------
  DSW(m_DG,m_NR,nnl);
  DSW(m_DS,m_MR,L);
  // -------- special procedures for removing a Phase ---------------
  m_TMole[m_SI[m_MR]] -= m_W[m_MR];
  m_NR--;                           // one less reaction
  m_MR--;                           // one less reaction
  #if dbgVCS
  if (dbgSolveSeq())
    dbgp("\nDeleteIt Phase --- %i %i %i %i %s (%i)", L, mm, m_NR, m_MR, (m_NR == 0)?"FALSE":"TRUE",m_nNotDeleteItCnt);
  #endif
  if (m_NR == 0)
    {
    m_nNotDeleteItCnt++;
    return (FALSE);
    }
  else
    {
    m_nNotDeleteItCnt=0;
    return (TRUE);
    }
}

// ----------- convergence forcer -----------------
void CVCSSolver::Force(void)
  {
  int   i;
  double  s1,s2,al;

  // ------- calculate slope at end of step -----------
  s2 = 0.0;
  for (i = 1; i <= m_MR; i++)
    s2 += m_FE[i]*m_DS[i];
  if (s2 <= 0.0)
    return;
    // -------- calculate original slope ----------------
  s1 = 0.0;
  for (i = 1; i <= m_MR; i++)
    s1 += m_FEL[i]*m_DS[i];
  if (s1 >= 0.0)
    return;
  // -------- fit parabola --------------
  al = s1 / (s1 - s2);
  if (al >= 0.95)
    return;
  // ------- adust mole numbers, chemical potential ------------
  for (i = 1; i <= m_MR; i++)
    if (m_DS[i] != 0.0)
      {
      m_DS[i] *= al;
      m_W[i] = GEZ(m_W[i] + m_DS[i]);
      }
  for (i = 1; i <= m_NoPhases; i++)
    m_TMole[i] = GEZ(m_TMole[i] + al*m_DeltaMole[i]);
  DFE(m_W,0,m_Iti);  // for m_TMole and components + all reactions/majors only
  m_FORCED = TRUE;
  return;
  }


// ---------- calculates reaction adjustments (delta sigma) --
// ------ m_DG[i] must be calulated before using this routine --
int CVCSSolver::ST2(void)
  {
  int i,j,L,k,SOLDEL;
  double s,dss,xx;

  SOLDEL = FALSE;
  for (i = 1; i <= m_NR; i++)
    {
    L = m_IR[i];
    if (m_W[L] == 0.0 && m_SI[L] != SinglePhase)
      {
      if (m_DG[i] >= -1e-4)
        m_DS[L] = 0.0;
      else
        {
        m_DS[L] = 1e-10;
        m_IC[i] = MAJOR;
        }
      }
    else if ((fabs(m_DG[i]) <= 1e-6) || (m_IC[i] <= MINOR && m_DG[i] >= 0.0))
      {
      }
    else
      {
      // ------- this is formula 6.4-16 in VCS manual --------
      if (m_SI[L] == SinglePhase)
        s = 0.0;
      else
        s = 1.0/m_W[L];
      for (j = 1; j <= m_NC; j++)
        if (m_SI[j] != SinglePhase)
          s += m_SC[j][i]*m_SC[j][i]/m_W[j];
      for (j = 1; j <= m_NoPhases; j++)
        if (m_TMole[j] > 0.0)
          s -= m_DMole[j][i]*m_DMole[j][i]/m_TMole[j];
      if (s != 0.0)
        m_DS[L] = -m_DG[i]/s;
      // --------- reaction entirely among condensed phases -----
      // --------- delete one solid and recompute basis ---------
      else
        {
        #if dbgVCS
        if (dbgSolveSeq())
          dbgp("\n Reaction entirely among condensed phases in 'ST2()'");
        #endif
        if (m_DG[i] <= 0.0)
          {
          dss =1e10;
          for (j = 1; j <= m_NC; j++)
            {
            if (m_SC[j][i] < 0.0)
              {
              xx = -m_W[j]/m_SC[j][i];
              if (xx < dss)
                {
                dss = xx;
                k = j;
                }
              }
            }
          }
        else
          {
          dss = m_W[L];
          k = L;
          for (j = 1; j <= m_NC; j++)
            {
            if (m_SC[j][i] > 0.0)
              {
              xx = m_W[j]/m_SC[j][i];
              if (xx < dss)
                {
                dss = xx;
                k = j;
                }
              }
            }
          dss = -dss;
          }
        if (dss != 0.0)
          {
          m_W[L] += dss;
          for (j = 1; j <= m_NC; j++)
            m_W[j] += dss*m_SC[j][i];
          m_W[k] = 0.0;
          if (k != L)
            SOLDEL = TRUE;
          }
        }
      }
    }
  if (SOLDEL)
    return (FALSE);
  else
    return (TRUE);
  }

// ------------ correct elemental abundances -----------------
int CVCSSolver::ElCorr(void)
  {
  int i,j,k,L,ip1;
  double par,xx,r;

  #if dbgVCS
  if (dbgMatrix())
    dbgp("\nEntered into 'ElCorr()'");
  #endif
  for (i = 1; i <= m_NC; i++)
    {
    m_SA[i] = m_GA[i] - m_GAI[i];
    for(j = 1; j <= m_NC; j++)
      m_SM[j][i] = m_BM[i][j];  // get a temporary transpose of m_BM(i,j)
    }
  #if dbgVCS
  if (dbgMatrix())
    {
    dbgp("\nData for m_BM[i,j] to use in MLEQU() in 'BasOpt(FALSE)'\n");
    for (i = 1; i <= m_M; i++)
      {
      for (j = 1; j <= m_NE; j++)
        dbgp("%4.2g ",m_BM[i][j]);
      dbgp("\n");
      }
    dbgp("\nData for m_SM[i,j] (Transformed m_BM[i,j])'\n");
    for (i = 1; i <= m_NE; i++)
      {
      for (j = 1; j <= m_M; j++)
        dbgp("%4.2g ",m_SM[i][j]);
      dbgp("\n");
      }
    }
  #endif
  // ------- MLEQU() ---------
  for (i = 1; i <= m_NC; i++)
    {
    if (m_SM[i][i] == 0.0)
      {
      ip1 = i+1;
      for (k = ip1; k <= m_NC; k++)
        if (m_SM[k][i] != 0.0)
          goto JUMP1;

      throw CVCSException("No unique solution - Number of components < Number of elements");

JUMP1:
      for (j = i; j <= m_NC; j++)
        m_SM[i][j] += m_SM[k][j];
      m_SA[i] += m_SA[k];
      }
    for (L = 1; L <= m_NC; L++)
      {
      if (L == i || m_SM[L][i] == 0.0)
        {
        }
      else
        {
        r = m_SM[L][i]/m_SM[i][i];
        for (j = i; j <= m_NC; j++) m_SM[L][j] -= m_SM[i][j]*r;
        m_SA[L] -= m_SA[i]*r;
        }
      }
    }
  for (i = 1; i <= m_NC; i++)
    m_SA[i] = -m_SA[i]/m_SM[i][i];
    // --------- end MLEQU() ------------
  par = 0.5;
  #if dbgVCS
  if (dbgElCorr())
    dbgp("\nElCorr");
  #endif
  for (i = 1; i <= m_NC; i++)
    {
    #if dbgVCS
    if (dbgElCorr())
      dbgp("\n %2i %-14s  %+.7e  %+.7e  %+.7e", i, EDB[m_L2ElId[i]].m_Name, m_SA[i], m_W[i], -m_SA[i]/Max(1.0e-70, m_W[i]));
    #endif
    xx = -m_SA[i]/Max(1.0e-70, m_W[i]);
    if (par < xx) par = xx;
    }
  par = 1.0/par;
  if (par <= 1.01 && par > 0.0)
    par = .99*par;
  else
    par = 1.0;

  if (par<1.0e-20)
    {
    #if dbgVCS
    if (dbgElCorr())
      {
      //SetMyDebug(0);
      dbgp("\n Par Too Small = %+.7e", par);
      }
    #endif
    return 1;
    }
  #if dbgVCS
  if (dbgElCorr())
    dbgp("\n Par = %+.7e", par);
  #endif

  for (i = 1; i <= m_NC; i++)
    {
    #if dbgVCS
    if (dbgElCorr())
      dbgp("\n %2i %-14s  W : %+.7e > %+.7e [%+.7e]", i, EDB[m_L2ElId[i]].m_Name, m_W[i], m_W[i] + par*m_SA[i], par*m_SA[i]);
    #endif
    m_W[i] += par*m_SA[i];
    if (m_W[i]<1.0e-200) // catch rounding
      {
      m_W[i] = 1.0e-200;
      #if dbgVCS
      if (dbgElCorr())
        dbgp(" << Set to Small");
      #endif
      }
    }
  #if dbgVCS
  if (dbgMatrix())
    {
    ELAB();
    dbgp("\nElemental abundances            Correct       From estimate\n");
    for (i = 1; i <= m_NE; i++)
      dbgp("  %2d %-14s    %+.7e     %+.7e\n",i, EDB[m_L2ElId[i]].m_Name,m_GAI[i],m_GA[i]);
    }
  #endif
  return 0;
  }

// -------- calculates free energy changes for the reactions ---------
void CVCSSolver::DeltaG(int L, int j, int kp)
  {
  int i,k,LL;
  double sdel[MAXPHASE];

  switch (L)
    {
    // -------- majors only ---------------
    case -3 :
    case -2 :
    case -1 :
      for (k = 1; k <= m_NR; k++)
        {
        if (m_IC[k] == MAJOR)
          {
          LL = m_IR[k];
          m_DG[k] = m_FE[LL];
          for (LL = 1; LL <= m_NC; LL++)
            m_DG[k] += m_SC[LL][k]*m_FE[LL];
          }
        }
      break;
    // ------------- all reactions from j to kp -----------
    case 0  :
      for (k = j; k <= kp; k++)
        {
        LL = m_IR[k];
        m_DG[k] = m_FE[LL];
        for (LL = 1; LL <= m_NC; LL++)
          m_DG[k] += m_SC[LL][k]*m_FE[LL];
        }
      break;
    // ------------ minors only (NMINOR + MINOR) ---------------
    case 1  :
      for (k =1; k <= m_NR; k++)
        {
        if (m_IC[k] <= MINOR)
          {
          LL = m_IR[k];
          m_DG[k] = m_FE[LL];
          for (LL = 1; LL <= m_NC; LL++)
            m_DG[k] += m_SC[LL][k]*m_FE[LL];
          }
        }
      break;
    }
  // ------ multispecies Phase with nt = 0 -------------
  for(i = 1; i <= m_NoPhases; i++)
    sdel[i] = 0.0;
  for (i = 1; i <= m_NR; i++)
    {
    LL = m_IR[i];
    if (m_W[LL] == 0.0)
      {
      if (m_DG[i] > 50.0)
        m_DG[i] = 50.0;
      if (m_DG[i] < -50.0)
        m_DG[i] = -50.0;
      for (k = 1; k <= m_NoPhases; k++)
        if (m_SI[LL] == k)
          sdel[k] += exp(-m_DG[i]);
      }
    }
  for (i = 1; i <= m_NR; i++)
    {
    LL = m_IR[i];
    if (m_W[LL] == 0.0)
      {
      for (k = 1; k <= m_NoPhases; k++)
        if (m_SI[LL] == k) m_DG[i] = 1 - sdel[k];
      }
    }
  }

// -------- Chooses optimum basis, calculates stoichiometry ----------
void CVCSSolver::BasOpt(int FIRST)
{
  int k,jr,jl,i,j,L,ip1;


  double temp,r;

  m_CONV = FALSE;
  m_NOpt++;
  for (i = 1; i <= m_MR; i++) m_AW[i] = m_W[i];
  for (jr = 1; jr <= m_NC; jr++)
    {
    do
      {
      k = AMax(m_AW,jr,m_MR);
      if (m_AW[k] == 0.0)
        {
        m_CONV = TRUE;  // the biggest specie is zero
        return;
        }
      if (m_AW[k] == m_TEST)
        {
        m_NC = jr - 1;
        m_N = m_M - m_NC;
        m_NR = m_N;
        for (i = 1; i <= m_M; i++)
          m_IR[i] = m_NC + i;
        }
      m_AW[k] = m_TEST;
      // ----- check linear independence with previous species ---
      // ----- logical function lindep(m_BM,jr,k,m_NC) ------
      jl = jr - 1;
      m_SA[jr] = 0.0;
      for (j = 1; j <= m_NC; j++)
        m_SM[j][jr] = m_BM[k][j];
      if (jl != 0)
        {
        for (j = 1; j <= jl; j++)
          {
          m_SS[j] = 0.0;
          for (i = 1; i <= m_NC; i++) m_SS[j] += m_SM[i][jr]*m_SM[i][j];
          m_SS[j] = m_SS[j]/m_SA[j];
          }
        for (j = 1; j <= jl; j++)
          for (L = 1; L <= m_NC; L++)
            m_SM[L][jr] -= m_SS[j]*m_SM[L][j];
        }
      for (j = 1; j <= m_NC; j++)
        if (fabs(m_SM[j][jr]) > 1e-17)
          m_SA[jr] += m_SM[j][jr]*m_SM[j][jr];
      }
    while (m_SA[jr] < 1e-6);
    // ----- rearrange data ----------
    DSW(m_W,jr,k);
    DSW(m_Wt,jr,k);
    DSW(m_FF,jr,k);
    DSW(m_InitialChemPot,jr,k);
    DSW(m_FE,jr,k);
    DSW(m_AW,jr,k);
    Swap2(m_SI,jr,k);
    DSW(m_DA,jr,k);
    DSW(m_FEL,jr,k);
    Swap2(m_Ind,jr,k);
    Rotate(m_Index2, jr, k);
    for(j = 1; j <= m_NE; j++)
      {
      temp = m_BM[jr][j];
      m_BM[jr][j] = m_BM[k][j];
      m_BM[k][j] = temp;
      }
    }
  if (FIRST)
    return;
  // ----- evaluate stoichiometric matrix, when FIRST = FALSE -------
  // Transform m_BM[] into m_SM[]
  for (j = 1; j <= m_NC; j++)
    {
    for (i = 1; i <= m_NC; i++)
      m_SM[i][j] = m_BM[j][i];
    }
  #if dbgVCS
  if (dbgMatrix())
    {
    dbgp("\nData for m_BM[i,j] to use in MLEQU() in 'BasOpt(FALSE)'\n");
    for (i = 1; i <= m_M; i++)
      {
      for (j = 1; j <= m_NE; j++)
        dbgp("%4.2g ",m_BM[i][j]);
      dbgp("\n");
      }
    dbgp("\nData for m_SM[i,j] (Transformed m_BM[i,j])'\n");
    for (i = 1; i <= m_NE; i++)
      {
      for (j = 1; j <= m_M; j++)
        dbgp("%4.2g ",m_SM[i][j]);
      dbgp("\n");
      }
    }
  #endif
  for (i = 1; i <= m_N; i++)
    {            // Transform m_BM[] into stoich.coeff.matr m_SC[]
    for(j = 1; j <= m_NC; j++) m_SC[j][i] = m_BM[m_IR[i]][j];
    }
  #if dbgVCS
  if (dbgMatrix())
    {
    dbgp("\nData for m_SC[i,j] to use in MLEQU() in 'BasOpt(FALSE)'\n");
    for (i = 1; i <= m_NC; i++)
      {
      for (j = 1; j <= m_N; j++)
        dbgp("%4.2g ",m_SC[i][j]);
      dbgp("\n");
      }
    }
  #endif
  // ------- MLEQU() ---------
  // ------ calculates stoichiometric matrix (reactions) --------
  for (i = 1; i <= m_NC; i++)
    {
    if (m_SM[i][i] == 0.0)
      {
      ip1 = i+1;
      for (k = ip1; k <= m_NC; k++)
        if (m_SM[k][i] != 0.0) goto JUMP;
      #if dbgVCS
      if (dbgMatrix())
        {
        dbgp("\nData for m_SM[i,j] of MLEQU() in 'BasOpt(FALSE)'\n");
        for (jr = 1; jr <= m_NE; jr++)
          {
          for (j = 1; j <= m_M; j++)
            dbgp("%4.2g ",m_SM[jr][j]);
          dbgp("\n newline \n");
          }
        }
      #endif
      throw CVCSException("No unique solution - Number of components < Number of elements");

  JUMP:
      for (j = i; j <= m_NC; j++)
        m_SM[i][j] += m_SM[k][j];
      for (j = 1; j <= m_N; j++)
        m_SC[i][j] += m_SC[k][j];
      }
    for (L = 1; L <= m_NC; L++)
      {
      if (L == i || m_SM[L][i] == 0.0)
        {
        }
      else
        {
        r = m_SM[L][i]/m_SM[i][i];
        for (j = i; j <= m_NC; j++)
          m_SM[L][j] -= m_SM[i][j]*r;
        for (j = 1; j <= m_N; j++)
          m_SC[L][j] -= m_SC[i][j]*r;
        }
      }
    }
  for (i = 1; i <= m_NC; i++)
    for (j = 1; j <= m_N; j++)
      m_SC[i][j] = -m_SC[i][j]/m_SM[i][i];
  #if dbgVCS
  if (dbgMatrix())
    {
    dbgp("\nStoichiometric matrix m_SC[i][j]\n\n");
    for (i = 1; i <= m_NC; i++)
      {
      for (j = 1; j <= m_N; j++)
        dbgp("%4.2g  ",m_SC[i][j]);
      dbgp("\n");
      }
    }
  #endif
  // ------ end of MLEQU --------
  for (i = 1; i <= m_N; i++)
    {
    k = m_IR[i];
    for (j = 0; j <= m_NoPhases; j++) m_DMole[j][i] = 0.0;
    if (m_SI[k] != SinglePhase) m_DMole[m_SI[k]][i] = 1.0;
    for (j = 1; j <= m_NC; j++)
      {
      if (fabs(m_SC[j][i]) <= 1e-6)
        m_SC[j][i] = 0.0;
      if (m_SI[j] != SinglePhase)
        m_DMole[m_SI[j]][i] += m_SC[j][i];
      }
    }
  }

// ------ computes elemental abundances ---------
void CVCSSolver::ELAB(void)
  {
  int i,j;
  #if dbgVCS
  if (dbgELAB())
    dbgp("\nELAB");
  #endif

  for (i = 1; i <= m_NE; i++)
    m_GA[i] = 0.0;
  for (j = 1; j <= m_NE; j++)
    {
    for (i = 1; i <= m_M; i++)
      m_GA[j] += m_BM[i][j]*m_W[i];
    #if dbgVCS
    if (dbgELAB())
      dbgp("\n %2i %-14s  GA:%+.7e ", j, EDB[m_L2ElId[j]].m_Name, m_GA[j]);
    #endif
    }
  }

// ------ evaluate chemical potential -----------
void CVCSSolver::DFE(CDVector & z, int kk, int LL)
// kk: >0 -> do it for m_TMole1
// kk:=<0 -> do it for m_TMole

// LL: =0 -> all components + all reactions
// LL: >0 -> all components + m_Minor reactions only
// LL: <0 -> all components + major reactions only

  {
  int L1,i,L;
  double x[MAXPHASE];

  if (kk > 0)
    {
    for (i = 1; i <= m_NoPhases; i++)
      {
      x[i] = m_TMole1[i];
      if (x[i] > 0.0)
        x[i] = log((double)x[i]);
      }
    }
  else
    {
    for (i = 1; i <= m_NoPhases; i++)
      {
      x[i] = m_TMole[i];
      if (x[i] > 0.0)
        x[i] = log((double)x[i]);
      }
    }
  if (LL == 0)
    L1 = m_MR;
  else
    L1 = m_NC;
  for (i = 1; i <= L1; i++)
    {      // all reaction species, or components
    if (z[i] != 0.0)
      {
      if(m_SI[i] != SinglePhase)
        m_FE[i] = m_FF[i] -x[m_SI[i]] + log((double)z[i]);
      }
    else
      m_FE[i] = m_FF[i];
    }
  if (LL < 0)
    {
    for (i = 1; i <= m_NR; i++)  // majors only
      {
      if (m_IC[i] == MAJOR)
        {
        L = m_IR[i];
        if (z[L] == 0.0)
          m_FE[L] = m_FF[L];
        else
          {
          if (m_SI[L] != SinglePhase)
            m_FE[L] = m_FF[L] -x[m_SI[L]] + log((double)z[L]);
          }
        }
      }
    }
  else if (LL > 0)
    {
    for (i = 1; i <= m_NR; i++)     // minors only
      {
      if (m_IC[i] <= MINOR)
        {
        L = m_IR[i];
        if (z[L] == 0.0)
          m_FE[L] = m_FF[L];
        else
          {
          if (m_SI[L] != SinglePhase)
            m_FE[L] = m_FF[L] -x[m_SI[L]] + log((double)z[L]);
          }
        }
      }
    }
  #if dbgVCS
  if (dbgIterDFE())
    {
    dbgp("\nCalled DFE()");
    for (i = 1; i <= m_M; i++)
      dbgp("\n     m_FE[%1d] = %g  m_FF[%1d] = %g",i,m_FE[i],i,m_FF[i]);
    }
  #endif
  }

// ----- the following functions re-arrange the vector elements ------
// --- swap integers -------
void CVCSSolver::Swap2(CLVector & x,int j1,int j2)
  {
  int t;
  t = x[j1];
  x[j1] = x[j2];
  x[j2] = t;
  }
void CVCSSolver::Rotate(CLVector & x,int j1,int j2)
  {
  x[m_Ind[j1]] = j1;
  x[m_Ind[j2]] = j2;
  }

int CVCSSolver::AMax(CDVector & x,int j,int m_N)
  {
  int i,k;
  double big;

  k = j;
  big = x[j];
  for (i = j; i <= m_N; i++)
    {
    if (x[i] > big)
      {
      k = i;
      big = x[i];
      }
    }
  return (k);
  }

// ----- swap floating points -----
void CVCSSolver::DSW(CDVector & x, int j1, int j2)
  {
  double t;
  t = x[j1];
  x[j1] = x[j2];
  x[j2] = t;
  }


// ------------ estimates equilibrium compositions ------------
void CVCSSolver::InEst(void)
  {

  double  xt1[MAXPHASE],
          xt2[MAXPHASE],
          par,xl,ikl,s,s1;

  int i,j,k,lt,L;

  ikl = 0.0;
  lt = 0;
  // -- setup data for linear programming routine ---
  // -- we will use m_SC[][] temporaly as the linear programing matrix --
  for (i = 1; i <= m_NE; i++)
    {
    //dbgpln("111:%3i %3i %14.8g", i,m_M+m_NE+1, m_GAI[i]);
    m_SC[i][m_M+m_NE+1] = m_GAI[i];
    }
  for (j = 1; j <= m_M ; j++)
    for (i = 1; i <= m_NE; i++)
      {
      //dbgpln("222:%3i %3i %14.8g", i, j, m_BM[j][i]);
      m_SC[i][j] = m_BM[j][i];  // transpose matrix
      };
  for (j = 1; j <= m_M; j++)
    {
    //dbgpln("333:%3i %3i %14.8g", m_NE+2, j, m_FF[j]);
    m_SC[m_NE+2][j] = m_FF[j];       // changed -m_FF[j] to m_FF[j] and do minimising
    }
  // ------ insert linear prog here ----------
  LinProg(m_AW,m_SC);
  for (i = 1; i <= m_M; i++)
    {
    m_DS[i] = 0.0;
    m_W[i] = m_AW[i];
    if (m_AW[i] == 0.0)
      m_W[i] = 1e-2;
    }
  for (i = 1; i <= m_M; i++)
    m_Wt[i] = m_W[i];
  BasOpt(FALSE);
  // ----- calculate total gaseous and liquid moles ----
  // ----- chemical potentials of basis ----------------
  for (i = 1; i <= m_NoPhases; i++)
    m_TMole1[i] = m_TInert[i];
  for (i = 1; i <= m_NC; i++)
    {
    if (m_SI[i] != SinglePhase)
      m_TMole1[m_SI[i]] += m_Wt[i];
    }
  for (i = 1; i <= m_NC; i++)
    {
    if (m_SI[i] != SinglePhase)
      m_FE[i] = m_FF[i] + log((double)m_Wt[i]/m_TMole1[m_SI[i]]); // check if m_TMole1[] could be zero
    }
  for (i = m_NC+1; i <= m_M; i++)
    m_FE[i] = m_FF[i];
  DeltaG(0,1,m_N);
  // ------ estimate reaction adjustments ----------
  for (i = 1; i <= m_NoPhases; i++)
    {
    m_DeltaMole[i] = 0.0;
    if (m_TMole1[i] != 0.0)
      {
      xt1[i] = log((double)1e32 * m_TMole1[i]);
      xt2[i] = log((double)1e-32 * m_TMole1[i]);
      }
    }
  // ++++++ equation 3.4 in Canadian Jour of Chemical Engineer Vol 46 August 1968 +++++
  for (i = 1; i <= m_NR; i++)
    {
    L = m_IR[i];
    if (m_SI[L] != SinglePhase)
      {
      if (m_DG[i] < xt1[m_SI[L]] && m_DG[i] > xt2[m_SI[L]])
        {
        m_DS[L] = m_TMole1[m_SI[L]] * exp(-m_DG[i]);
        for (k = 1; k <= m_NC; k++)
          m_DS[k] += m_SC[k][i] * m_DS[L];
        m_DeltaMole[m_SI[L]] += m_DMole[m_SI[L]][i] * m_DS[L];
        }
      }
    }
  // ------- keep basis specie positive -------------
  par = 0.5;
  for (i = 1; i <= m_NC; i++)
    if (par < (-m_DS[i]/Max(1.0e-70, m_Wt[i])))
      par = -m_DS[i]/Max(1.0e-70, m_Wt[i]);
  par = 1.0/par;
  if (par <= 1.0 && par > 0.0)
    par = 0.8*par;
  else
    par = 1.0;
  // ------- calculate new mole numbers ------------
  while(TRUE)
    {
    for (i = 1; i <= m_NC; i++)
      m_W[i] = m_Wt[i] + par*m_DS[i];
    for (i = m_NC+1; i <= m_M; i++)
      if (m_DS[i] != 0.0)
        m_W[i] = m_DS[i]*par;
    for (i = 1; i <= m_NoPhases; i++)
      m_TMole[i] = m_TMole1[i] + m_DeltaMole[i]*par;
    if(lt > 0)
      return;
    // ------- convergence - forcing section ----------
    DFE(m_W,0,0); // for m_TMole and components + all reactions
    s = 0.0;
    for (i = 1; i <= m_MR; i++)
      s += m_DS[i]*m_FE[i];
    if (s < 0.0 && ikl <= 0.0)
      return;
    else if (s == 0.0)
      return;
    if (ikl <= 0.0)
      {
      // -- try half step size ---
      s1 = s;
      par *= 0.5;
      ikl = 1.0;
      }
    else
      {
      // ---- fit parabola thro half and full steps -----
      xl = 0.5 * (1.0 - s/(s1-s));
      if (xl >= 0.0)
        {
        if (xl <= 1.0)
          par *= 2.0*xl;
        else                // too big a step, take orginal full step
          par *= 2.0;
        }
      else                   // poor direction reduce step size
        par *= 0.2;
      lt = 1;
      }
    }
  }

// ---------- linear programing ---------------------
// minimize (cc*psol) such that ax[m_NE+2,j]*psol = bb
// --------------------------------------------------
// this routine uses the Two-VCSPhase technique from the book
// "Some common basic programs-p.103". See also "Operations Research -
// H.A.Taha - p.70"

void CVCSSolver::LinProg(CDVector & psol, CDMatrix & ax)
  {
  int b[MAXELEMENT],
      leaving,enter,
      m1,m2,i,j,i1,j1,
      c1,c2,x;


  double  pivot, temp;

  // -- initialise variables -----
  m1 = m_NE + 1;
  m2 = m_NE + 2;
  c1 = m_M + m_NE + 1;
  c2 = m_M;
  // Check this dynamically
  if(m2 >= m_SC_Max_I || c1 >= m_SC_Max_J)
    {
    throw CVCSException("Exceeded allocated scratch pad memory");
    }

  for (i = 1; i <= c1; i++)
    ax[m1][i] = 0.0;
  // Zero the part which gave me endless problems as Floating point errors
  for (i = m_M+1; i <= c1; i++)
    ax[m2][i] = 0.0;
  for (i = 1; i <= m_NE; i++)
    {
    for (j = 1; j <= m_M; j++) ax[m1][j] -= ax[i][j];
    for (j = m_M+1; j <= (m_M+m_NE); j++) ax[i][j]=0.0;
    ax[i][m_M+i] = 1.0;             // artificial variables
    b[i] = m_M + i;
    }

  //for (i = 1; i < ax.GetRows(); i++)
  //  for (j = 1; j < ax.GetCols(); j++)
  //    dbgpln("YYY:%3i,%3i %14.8g", i, j, ax[i][j]);

  x = m1;
  while (x <= m2)
    {
    do
      {
      // ---- price out columns (find entering variable) -----
      temp = -1e-5;
      for (j = 1; j <= m_M; j++)
        {
        if (ax[x][j] < temp)
          {
          enter = j;              // enter = biggest negative value
          temp = ax[x][j];
          }
        }
      if (temp != -1e-5)
        {
        // --- find leaving variable ----
        temp = 1e38;
        // ----- choose smallest positive ratio --------
        for (i = 1; i <= m_NE; i++)
          {
          if (ax[i][enter] > 1e-5 && ax[i][c1]/ax[i][enter] < temp)
            {
            leaving = i;
            temp = ax[i][c1]/ax[i][enter];
            }
          }
        if (temp == 1e38)
          {
          throw CVCSException("The LP-solution is unbounded !!!!");
          }
        else
          {
          // ----- preform pivoting -----
          pivot = ax[leaving][enter];
          for (j = 1; j <= c1; j++)
            ax[leaving][j] /= pivot;
          for (i = 1; i <= m2; i++)
            {
            if (i != leaving) {
              for (j = 1;  j <= c1; j++)
                {
                if (j != enter)
                  {
                  //dbgpln("XXX:%3i,%3i,%3i,%3i %14.8g %14.8g %14.8g",
                  //       i, j, enter, leaving, ax[i][j], ax[i][enter], ax[leaving][j]);
                  ax[i][j] -= ax[i][enter]*ax[leaving][j];
                  if (fabs(ax[i][j]) < 1e-5)
                      ax[i][j] = 0.0;
                  }
                }
              }
            }
          for (i = 1; i <= m2; i++)
            ax[i][enter] = 0.0;
          ax[leaving][enter] = 1.0;
          b[leaving] = enter;
          }
        }
      }
    while (temp != -1e-5);
    if (x == m1)
      {
      for (i1 = 1; i1 <= m_NE; i1++)
        {
        if (b[i1] > c2) {
          if (ax[i1][c1] > 1e-5)
            {
            throw CVCSException("The LP-problem has no feasible solution.");
            }
          else
            {
            for (j1 =1 ; j1 <= c2 ; j1++)
              {
              if (fabs(ax[i1][j1]) > 1e-5)
                {
                leaving = i1;
                enter = j1;
                // ----- preform pivoting -----
                pivot = ax[leaving][enter];
                for (j = 1; j <= c1; j++) ax[leaving][j] /= pivot;
                for (i = 1; i <= m2; i++)
                  {
                  if (i != leaving)
                    {
                    for (j = 1;  j <= c1; j++)
                      {
                      if (j != enter)
                        {
                        ax[i][j] -= ax[i][enter]*ax[leaving][j];
                        if (fabs(ax[i][j]) < 1e-5)
                           ax[i][j] = 0.0;
                        }
                      }
                    }
                  }
                for (i = 1; i <= m2; i++)
                  ax[i][enter] = 0.0;
                ax[leaving][enter] = 1.0;
                b[leaving] = enter;
                j1 = c2;
                }
              }
            }
          }
        }
      }
    x++;
    }
  for (i = 1; i <= m_M; i++)
    psol[i] = 0.0;
  for (i = 1; i <= m_NE; i++)
    psol[b[i]] = ax[i][c1];
  }

// make sure that the phase token allocation is in sequentual order
void CVCSSolver::CheckPhaseDist(void)
  {
  int   i, j, k;
  for(i=1; i<=m_NoPhases; i++)
    {
    j=0;
    while(m_NoSpeciePhase[i+j] == 0)
      j++;
    for (k=1; k<=m_M; k++)
      {
      if(m_SI[k] >= i+j)
        m_SI[k] -= j;
      }
    for(k=i; k<m_nMaxPhase; k++)
      m_NoSpeciePhase[k] = m_NoSpeciePhase[k+j];
    }
  }

// If the mode was set to non-ideal calculations the solver calls this
// routine (0 < m_Alpha <=1). It should calculate the activity in terms of
// MU/RT and this should be added to the current chemical potential
void CVCSSolver::GetActivityCoef(void)
{
//  int i, L;
//  double coef;

// First calculate ln(solvent) if it is implemented
   // ln(solvent) implementation
// Now calculate the activity coefficient
DoBreak();
//  for (i = 1; i <= m_MR; i++)
//  {
//    L = m_Ind[i];
//    if (VCS_act[L] != MARKER)
//    {
//      coef = parse_opcode(VCS_act[L]);
//      #ifdef DEBUG2
//      dbgp("\nln gamma[%2d] = %+4e mu = %+4e sigma = %+4e",i,coef,m_InitialChemPot[i],m_DS[i]);
//      #endif
//      m_FF[i] += m_Alpha*(m_InitialChemPot[i] + coef - m_FF[i]);
//    }
//  }
}


#if 0
/*
//  calculate specie i's activity coeficient
double CVCSSolver::parse_opcode(INDEX i)
{
  double stack[10];
  int    k=-1;
  double temp;

  #ifdef DEBUG3
  if(opcode[i].mnem != CLR)
  {
    strcpy(m_sError, "OPCODE array index m_sError. Aborting....");
    longjmp(e_buf, 1);
  }
  #endif
  while (opcode[i].mnem != RTN)
  {
     #ifdef DEBUG3
     if(k>=9)
     {
     strcpy(m_sError, "Activity stack overflow. Aborting....");
     longjmp(e_buf, 1);
     }
     #endif
     switch (opcode[i].mnem)
     {
      case  CLR : k=-1;
          break;
      case  PUSH: stack[++k] = opcode[i].x.op_value;
          break;
      case  GET :
            // first check if the total moles for its phase is zero
            temp = m_TMole[m_SI[m_Index2[opcode[i].x.entry]]];
            if (temp == 0.0)
            stack[++k] = 0.0;
            else
            stack[++k] =  m_W[m_Index2[opcode[i].x.entry]] / temp;
          break;
      case  ADD : stack[--k] = stack[k] + stack[k+1];
          break;
      case  SUB : stack[--k] = stack[k] - stack[k+1];
          break;
      case  MUL : stack[--k] = stack[k] * stack[k+1];
          break;
      case  DIV : if(stack[k] != 0.0)
             stack[--k] = stack[k] / stack[k+1];
            else
            {
             strcpy(m_sError, "ERROR - divide by zero in parse_opcode. Aborting....");
             longjmp(e_buf, 1);
            }
          break;
      case  POW : stack[--k] = pow(stack[k], stack[k+1]);
          break;
      case  UNA : stack[k] = - stack[k];
          break;
      case  FUNC:
            switch ((int)opcode[i].x.entry)
            {
               case 0:  stack[k] = fabs(stack[k]);
                   break;
               case 1:  stack[k] = acos(stack[k]);
                   break;
               case 2:  stack[k] = asin(stack[k]);
                   break;
               case 3:  stack[k] = atan(stack[k]);
                   break;
               case 4:  stack[k] = cosh(stack[k]);
                   break;
               case 5:  stack[k] = cos(stack[k]);
                   break;
               case 6:  stack[k] = exp(stack[k]);
                   break;
               case 7:  stack[k] = log10(stack[k]);
                   break;
               case 8:  stack[k] = log(stack[k]);
                   break;
               case 9:  stack[k] = pow10(stack[k]);
                   break;
               case 10: stack[k] = (int)(stack[k] + 0.5);
                   break;
               case 11: stack[k] = sinh(stack[k]);
                   break;
               case 12: stack[k] = sin(stack[k]);
                   break;
               case 13: stack[k] = sqrt(stack[k]);
                   break;
               case 14: stack[k] *= stack[k];
                   break;
               case 15: stack[k] = tanh(stack[k]);
                   break;
               case 16: stack[k] = tan(stack[k]);
                   break;
               case 17: stack[k] = (int)stack[k];
                   break;
            }
          break;
     }
     i++;
  }
  if(k!=0)
  {
    strcpy(m_sError, "Internal stack m_sError. Aborting....");
    longjmp(e_buf, 1);
  }
  return stack[0];
}
*/
#endif
/// When the solver has completed its job it calls this function which should
// transfer the result to the program's data arrays

long CVCSSolver::Initialise(CDMatrix & SpElDef, CLVector & SpPhDef,
                            CIndexXlate & SpId2L, CIndexXlate & L2SpId,
                            CIndexXlate & ElId2L, CIndexXlate & L2ElId)
  {
  //return 0;
  m_nMaxSpecie=SpElDef.GetRows();
  m_nMaxElement=SpElDef.GetCols();
  int PhUsed[MAXPHASE];
  memset(PhUsed, 0, sizeof(PhUsed));
  for (int s=0; s<SpPhDef.GetLen(); s++)
    PhUsed[SpPhDef[s]]=1;
  m_nMaxPhase=0;
  for (int i=0; i<MAXPHASE; i++)
    if (PhUsed[i])
      m_nMaxPhase=i+1;

  m_SC_Max_I = m_nMaxElement+2,    // maxinum i dimension of array m_SC
  m_SC_Max_J = m_nMaxSpecie+m_nMaxElement;// maxinum j dimension of array m_SC

  // Now Set Array Sizes
  m_TMole.SetSize(m_nMaxPhase);
  m_TInert.SetSize(m_nMaxPhase);
  m_DeltaMole.SetSize(m_nMaxPhase);
  m_TMole1.SetSize(m_nMaxPhase);
  m_W.SetSize(m_nMaxSpecie);
  m_Wt.SetSize(m_nMaxSpecie);
  m_FEL.SetSize(m_nMaxSpecie);
  m_FE.SetSize(m_nMaxSpecie);
  m_InitialChemPot.SetSize(m_nMaxSpecie);
  m_FF.SetSize(m_nMaxSpecie);
  m_DS.SetSize(m_nMaxSpecie);
  m_DG.SetSize(m_nMaxSpecie);
  m_DA.SetSize(m_nMaxSpecie);
  m_AW.SetSize(m_nMaxSpecie);
  m_GA.SetSize(m_nMaxElement);
  m_GAI.SetSize(m_nMaxElement);
  m_SA.SetSize(m_nMaxElement);
  m_SS.SetSize(m_nMaxElement);
  m_SC.SetSize(m_SC_Max_I, m_SC_Max_J);
  m_BM.SetSize(m_nMaxSpecie, m_nMaxElement);
  m_DMole.SetSize(m_nMaxPhase, m_nMaxSpecie);
  m_SM.SetSize(m_nMaxElement, m_nMaxSpecie);

  m_NoSpeciePhase.SetSize(m_nMaxPhase);
  m_Index2.SetSize(m_nMaxSpecie);
  m_Ind.SetSize(m_nMaxSpecie);
  m_IR.SetSize(m_nMaxSpecie);
  m_IC.SetSize(m_nMaxSpecie);
  m_SI.SetSize(m_nMaxSpecie);

  m_SpElDef = SpElDef;
  m_SpPhDef = SpPhDef;

  m_SpId2L = SpId2L;
  m_L2SpId = L2SpId;
  m_ElId2L = ElId2L;
  m_L2ElId = L2ElId;

  m_pModel=NULL;
  //m_SpMass.SetSize(m_nMaxSpecie);
  //m_SpMoles.SetSize(m_nMaxSpecie);

  return VCS_NO_ERR;
  }

void CVCSSolver::InitialiseDataPhase(void)
  {
  for (int s=1; s<=m_M; s++)
    {
    m_SI[s]=m_SpPhDef[s];
    for (int e=1; e<=m_NE; e++)
      m_BM[s][e]=m_SpElDef[s][e];
    }

  for (int p = 0; p < m_nMaxPhase; p++)
    {   // Remember that phase 0 -> 'SINGLE SPECIE PHASE' or 'PURE CONDENSED PHASE'
    m_TInert[p] = 0.0;              /* Zero the inerts for each phase */
    m_NoSpeciePhase[p] = 0;         /* Zero the phase count for each phase */
    }
  for (s = 1; s <= m_M; s++)
    {
    m_NoSpeciePhase[m_SI[s]]++;   // count the no of species in each phase
    }
  };

void CVCSSolver::InitialiseDataMoles(bool Complete)
  {
  for (int e=1; e<=m_NE; e++)
    m_GAI[e]=0.0;
  double Tot=0.0;
  m_ExistsInSrc.SetSize(m_M+1);
  for (int s=1; s<=m_M; s++)
    {
    long si=m_L2SpId[s];
    //if (Complete)
      m_W[s]=m_SpMoles[si];
    Tot+=m_W[s];
    }
  Tot=Max(1.0e-70, Tot);

  for (s=1; s<=m_M; s++)
    {
    long si=m_L2SpId[s];
    m_ExistsInSrc[si]=(m_W[s]/Tot>1.0e-6);
    if (!m_ExistsInSrc[si])
      m_W[s]=0;
    }

  for (s=1; s<=m_M; s++)
    {
    // get the chemical potential
    long si=m_L2SpId[s];
    m_FF[s] = SDB[si].mlGf(Fidelity(), m_TempKFinal, Std_P, NULL, NULL);
    for (int e=1; e<=m_NE; e++)
      m_GAI[e]+=m_W[s]*m_BM[s][e];

    m_DA[s] = m_FF[s];            // store the starting chemical potential in m_DA[]
    }
  };

void CVCSSolver::InitialiseDataInternal()
  {
  int i,j;
  switch (m_Type)
    {
    case -1: m_GasConst = 0.0019872;    break; // kCal/mole
    case  0: m_GasConst = 1.0/m_TempKFinal;  break; // MU/RT
    case  1: m_GasConst = 0.0083143;    break; // kJ/mole
    case  2: m_GasConst = 8.3143;       break; // kJ/kmole or J/mole
    }

  //m_TLoop = 0;
  m_Iter = 0;
  m_NOpt = 0;
  m_NC = m_NE;
  m_IM = FALSE;
  m_ICONV = FALSE;
  m_ICONV1 = FALSE;
  m_nNotDeleteItCnt=0;
  for (i = 1; i <= m_M; i++)
    m_Index2[i] = m_Ind[i] = i;
  //++++++++ Watch out for this +++++++
  for (j = 1; j <= m_NoPhases; j++)
    {
    if (m_NoSpeciePhase[j] == 1 && m_TInert[j] == 0.0)
      { // only one specie in the phase - make it SinglePhase
      m_NoSpeciePhase[0]++;   // increase SinglePhase specie phase count
      m_NoSpeciePhase[j] = 0;
      for (i = 1; i <= m_M; i++)
        {
        if (j == GasPhase && m_SI[i] == GasPhase)
          {
          m_FF[i] += m_GasConst * m_TempKFinal * log((double)m_Press);
          m_SI[i] = SinglePhase;

          }
        else if (j == m_SI[i])
          m_SI[i] = SinglePhase;
        }
      }
    }
  m_N = m_M - m_NC;
  m_NR = m_N;
  m_MR = m_M;
  m_TEST = -1e-10;
  if (m_IEst != 0)
    {
    for (i = 1; i <= m_MR; i++) m_W[i] = -m_FF[i];
    m_TEST = -1e20;
    }
  for (i = 1; i <= m_M; i++)
    {
    m_IR[i] = m_NC + i;
    m_FF[i] /= (m_GasConst * m_TempKFinal);
    if (m_SI[i] == GasPhase)
      m_FF[i] += log((double)m_Press);
    if (m_SI[i] == SinglePhase)
      m_FE[i] = m_FF[i];
    m_InitialChemPot[i] = m_FF[i];  // initialise orginal chemical pot (activity)
    }
  if (m_IEst != 0)
    {    // do machine starting estimate
    InEst();
    ELAB();
    ElCorr();
    }
  else
    {
    //#ifdef DEBUG
    //dbgp("\nCalling 'BasOpt(TRUE)' from initial_data()");
    //#endif
    BasOpt(TRUE); // only calculate optimum basis but not the stioch. matrix
    ELAB();
    }
  }

//long CVCSSolver::InitialiseFeed(SpModel * pModel, SpVector & Moles, double TempStart, double TempFinal, double Press, flag FindTemp)
//  {
//
//  m_pModel=pModel;
//  m_bFindTemp=FindTemp;
//  // Initialise VCS Algorithm
//  m_M=m_nMaxSpecie-1;
//  m_NE=m_nMaxElement-1;
//  // Copy Info
//
//  // Get data & set options
//  m_nMaxIt = 200*(m_bFindTemp?10:1);
//  m_Alpha = 0.0;
//  m_IEst = 1; // 0=User Estimate / 1=algorithm to do initial estimate
//  m_Type = 2; // kJ/kmol
//
//  m_TempKStart=TempStart;
//  m_TempKFinal=TempFinal;
//  m_Press=kPa2Atm(Press);
//
//  for (int s=0; s<SDB.No(); s++)
//    {
//    m_SpMoles[s]=Moles[s];
//    m_SpMass[s]=Moles[s]*SDB[s].MoleWt();
//    }
//
//  // now we must enter the activity formalism
//  if (m_Alpha != 0.0)   // only do this for non_ideal cases
//    {
//    //VCS_act[k] = current_opcode = ptr->act_c[i]; // assign the index into the opcode_array
//    //if (current_opcode != MARKER)
//    //  { // this compound must have a activity formalism
//    //  // now we must go through our local copy of the opcodes and adjust the references
//    //  while (opcode[current_opcode++].mnem != RTN)
//    //    {
//    //    if (opcode[current_opcode].mnem == GET)
//    //       opcode[current_opcode].x.entry += (INDEX)(k - i); // now the opcode will point to the correct specie in VCS_specie
//    //    }
//    //  }
//    }
//
//    //memcpy(opcode, opcode_array, sizeof(opcode)); /* make a local copy of the opcode array */
//
//  InitialiseDataPhase();
//
//  m_NoPhases = 0;
//  for (int p = 1; p < m_nMaxPhase; p++)
//    if (m_NoSpeciePhase[p] != 0)
//      m_NoPhases++; // Count the number of phases
//  if (m_NoPhases == 0)
//    return VCS_PHASE_ERR;
//
//
//  return VCS_NO_ERR;
//  }


void CVCSSolver::InitialiseData(void)
  {
  InitialiseDataMoles(true);

  CheckPhaseDist();

  InitialiseDataInternal();
  #if dbgVCS
  if (dbgSolve())
    {
    dbgp("\n\nVCS - algorithm version 1.4");
    dbgp("\n  %3d Species %3d Elements %3d Components",m_M,m_NE,m_NC);
    for (int i = 0; i <= m_NoPhases; i++)
      dbgp("\n  %3d Phase %1d Species ",m_NoSpeciePhase[i],i);
    dbgp("\nPressure     = %8.3f kPa",Atm2kPa(m_Press));
    dbgp("\nTemp (Start) = %8.3f K  %8.3f C",m_TempKStart,K_2_C(m_TempKStart));
    dbgp("\nTemp (Final) = %8.3f K  %8.3f C",m_TempKFinal,K_2_C(m_TempKFinal));
    dbgp("\nAlpha        = %8.3f\n",m_Alpha);
    for (i = 1; i <= m_NoPhases; i++)
      dbgp("Phase %1d inerts  %5.2f\n",i,m_TInert[i]);

    dbgp("\nElemental abundances            Correct       From estimate\n");
    for (i = 1; i <= m_NE; i++)
      dbgp("  %2d %-14s    %+.7e     %+.7e\n",i, EDB[m_L2ElId[i]].m_Name,m_GAI[i],m_GA[i]);
    if (m_IEst == 0)
      dbgp("\nUser estimate of equilibrium");
    else
      dbgp("\nModified linear programming estimate of equilibrium");
    switch (m_Type)
      {
      case -1 :   dbgp("\nStandard chemical potential in kcal/mole\n"); break;
      case  0 :   dbgp("\nStandard chemical potential in MU/RT\n");     break;
      case  1 :   dbgp("\nStandard chemical potential in kJ/mole\n");   break;
      case  2 :   dbgp("\nStandard chemical potential in kJ/kmole\n");  break;
      }
    dbgp("\nSpecies         Stan. Chem. Pot.    Equilibrium est.   Formula vector");
    dbgp("\n                                                 ");
    for(int j = 1; j <= m_NE; j++)
      dbgp("  %3s", EDB[m_L2ElId[j]].m_Name);
    dbgp(" Phase\n");

    for (i = 1; i <= m_M; i++)
      {
      dbgp("%-12.12s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
      dbgp("");
      dbgp("%+18.7f     %+14.7f    ",m_DA[i],m_W[i]);
      for(j = 1; j <= m_NE; j++)
        dbgp("%4.1f ",m_BM[i][j]);
      dbgp("  %1d",(int)m_SI[i]);
      if (dbgIterThermo())
        {
        dbgp("  %18.7f", SDB[m_L2SpId[m_Ind[i]]].msCp(Fidelity(), m_TempKFinal, Atm2kPa(m_Press), NULL, NULL));
        dbgp("  %18.7f", SDB[m_L2SpId[m_Ind[i]]].msHf(Fidelity(), m_TempKFinal, Atm2kPa(m_Press), NULL, NULL));
        dbgp("  %18.7f", SDB[m_L2SpId[m_Ind[i]]].msHz(Fidelity(), m_TempKFinal, Atm2kPa(m_Press), NULL, NULL));
        }
      dbgp("\n");
      }
    }
  #endif
  }

void CVCSSolver::ReInitialiseData(void)
  {
  InitialiseDataPhase();

  InitialiseDataMoles(false);

  CheckPhaseDist();

  InitialiseDataInternal();

  #if dbgVCS
  if (dbgIterMole())
    {
    dbgp("\nPressure       %8.3f kPa\nTemperature    %8.3f Kelvin\n",Atm2kPa(m_Press),m_TempKFinal);

    }
  #endif
  }

void CVCSSolver::TransferResult(bool WithDebug)
  {
  int i;
  // Set the compound moles
  #if dbgVCS
  if (WithDebug)
    dbgp("\n Tranfer Result  %+14.14s, %+14.14s", "Feed", "Moles");
  #endif
  for (i = 1; i <= m_MR; i++)
    {
    int si=m_L2SpId[m_Ind[i]];
    if (m_ExistsInSrc[si])
      m_pModel->VValue[si] = m_W[i]*SDB[si].MoleWt();
    else
      m_pModel->VValue[si] = m_SpMass[si]+m_W[i]*SDB[si].MoleWt();
    #if dbgVCS
    if (WithDebug)
      dbgp("\n %2i %12.12s %s %14.7f, %14.7f", si, SDB[si].SymOrTag(), m_ExistsInSrc[si]?"Src":"   ", m_SpMoles[m_L2SpId[m_Ind[i]]], m_W[i]);
    #endif
    }
  if (m_MR != m_M)
    {
    // deleted species
    for (i = m_MR+1; i <= m_M; i++)
      {
      int si=m_L2SpId[m_Ind[i]];
      m_pModel->VValue[si] = 0.0;
      #if dbgVCS
      if (WithDebug)
        dbgp("\n %2i %12.12s %s %14.7f, %14.7f ", si, SDB[si].SymOrTag(), "   ", 0.0, 0.0);
      #endif
      }
    }
  }

long CVCSSolver::AdjustTemp()
  {
  double H=m_pModel->totHf();
  double OldTemp=m_TempKFinal;
  TransferResult(false);//dbgIterTemp()!=0);
  m_pModel->Set_totHf(H);
  //m_TempKFinal=0.5*(m_pModel->Temp()+OldTemp);
  m_TempKFinal=0.25*m_pModel->Temp()+0.75*OldTemp;
  #if dbgVCS
  if (dbgIterTemp())
    {
    dbgp("\n  ===================================== Adjust Temp:%10.4f > %10.4f  Hf:%.6g",
         K_2_C(OldTemp), K_2_C(m_TempKFinal), H);

    for (int i = 1; i <= m_M; i++)
      {
      int si=m_L2SpId[m_Ind[i]];
      dbgp("\n %2i %12.12s %14.7f, %14.7f", si, SDB[si].SymOrTag(), m_SpMoles[m_L2SpId[m_Ind[i]]], m_W[i]);
      if (dbgIterThermo())
        {
        dbgp("  %18.7f", SDB[si].msCp(Fidelity(), m_TempKFinal, Atm2kPa(m_Press), NULL, NULL));
        dbgp("  %18.7f", SDB[si].msHf(Fidelity(), m_TempKFinal, Atm2kPa(m_Press), NULL, NULL));
        dbgp("  %18.7f", SDB[si].msHz(Fidelity(), m_TempKFinal, Atm2kPa(m_Press), NULL, NULL));
        }
      }
    dbgp("\n");
    }

  #endif
  return 1;
  }

// -----------  final delta G/RT values ---------------------------
// using m_FEL[], m_TMole1[] & m_Wt[] as temporary values
CVCSSolver::VCSPath CVCSSolver::DumpChanges(void)
  {
  #if dbgVCS
  int i;
  int j;
  int k;                      // loop counters , temp values
  int   mr1;
  double  g;
  int   L;
  double  lnK;
  #endif


//  #if dbgVCS
//  if (dbgSolveSeq())
//    dbgp("\nTransfering data to spreadsheet");
//  #endif
  #if dbgVCS
  if (dbgSolve())
    {
    j = m_MR + 1;
    k = m_NR + 1;
    for (i = 1; i <= m_NR; i++)
      {
      j--;k--;
      m_DG[j] = m_DG[k];
      }
    for (i = 1; i < m_nMaxPhase; i++)
      m_TMole1[i] = 0.0;
    // first calculate total mass
    // we will use m_FEL as the temp variable for the mass%
    // and m_TMole1 as the total mass for the phase
    for (i = 1; i <= m_MR; i++)
      {
    //  DoBreak();
      m_FEL[i] = SDB[m_L2SpId[m_Ind[i]]].MoleWt()*m_W[i];
      m_TMole1[m_SI[i]] += m_FEL[i];
      }
    // calculate mass% and mole fractions
    for (i = 1; i <= m_MR; i++)
      {
      if (m_SI[i] == SinglePhase)
        {
        if (m_W[i] == 0.0)
          {
          m_Wt[i] = 0.0;          // single-species phase
          m_FEL[i] = 0.0;     // m_Wt%
          }
        else if (m_W[i] > 0.0)
          {
          m_Wt[i] = 1.0;          // mole fraction
          m_FEL[i] = 100.0;       // m_Wt%
          }
        }
      else
        {
        if (m_TMole1[m_SI[i]] != 0.0)
          {
          m_Wt[i] = m_W[i]/m_TMole[m_SI[i]];    // all other phases
          m_FEL[i] = m_FEL[i]/m_TMole1[m_SI[i]]*100.0;
          }
        else
          m_FEL[i] = 0.0;
        }
      }

    dbgp("\n------------------------------------ Results -----------------------------------");

    // --------------- print out the results --------------------
    // --------- to be included ------------
    if (m_ICONV)
      dbgp("\n CONVERGENCE CRITERION NOT SATISFIED.");
    dbgp("\nTemp iterations                  = %3d",m_TLoop);
    dbgp("\nMole iterations                  = %3d",m_Iter);
    dbgp("\nStoichiometry matrix evaluations = %3d\n",m_NOpt);
    dbgp("\nPressure     = %8.3f kPa",Atm2kPa(m_Press));
    dbgp("\nTemp (Final) = %8.3f K  %8.3f C",m_TempKFinal,K_2_C(m_TempKFinal));
    dbgp("\nAlpha        = %8.3f",m_Alpha);

    g = 0.0;
    for (i = 1; i <= m_NoPhases; i++)
      if (m_TMole[i] > 0.0 && m_TInert[i] > 0.0)
        g += m_TInert[i]*log((double)m_TInert[i]/m_TMole[i]);
    for (i =1; i <= m_MR; i++)
      g += m_W[i]*m_FE[i];
    dbgp("\nG/RT         = %+.7e\n",g);

    for (i = 1; i <= m_NoPhases; i++)
      dbgp("\nTotal phase %1d moles = %+14.7f mass = %+14.7f kg",i,m_TMole[i], m_TMole1[i]/1000.0);

    dbgp("\nSpecies            Feed Mass     Feed Moles      Equil moles    Mole Percent  DG/RT Reaction    Mass Perc  ln gamma\n");
    // All the components
    for (i = 1; i <= m_NC; i++)
      {
      dbgp("%-12.12s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
      dbgp("  ");
      dbgp("%+14.7f %+14.7f > %+14.7f  %+14.7f  %+14.5f                     %8.4f",
             m_SpMass[m_L2SpId[m_Ind[i]]],
             m_SpMoles[m_L2SpId[m_Ind[i]]],
             m_W[i], m_Wt[i]*100, m_FEL[i]);
      //if (VCS_act[m_Ind[i]] != MARKER)
      //  dbgp("   %8.4f", parse_opcode(VCS_act[m_Ind[i]]));
      dbgp("\n");
      }
    // Now all the reactions
    for (i = m_NC+1;i <= m_MR; i++)
      {
      dbgp("%-12.12s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
      dbgp("  ");
      dbgp("%+14.7f %+14.7f > %+14.7f  %+14.5f  %+14.7f     %8.4f",
        m_SpMass[m_L2SpId[m_Ind[i]]],
        m_SpMoles[m_L2SpId[m_Ind[i]]],
        m_W[i], m_Wt[i]*100, m_DG[i], m_FEL[i]);
      //if (VCS_act[m_Ind[i]] != MARKER)
      //  dbgp("   %8.4f", parse_opcode(VCS_act[m_Ind[i]]));
      dbgp("\n");
      }
    // print the deleted species
    if (m_MR != m_M)
      {
      mr1 = m_MR + 1;
      for (i = mr1; i <= m_M; i++)
        {
        dbgp("%-12.12s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
        dbgp("  ");
        dbgp("%+14.7f %+14.7f > %+14.14s",
          m_SpMass[m_L2SpId[m_Ind[i]]],
          m_SpMoles[m_L2SpId[m_Ind[i]]],
          "< 1.0e-32");
        dbgp("\n");
        }
      }
    dbgp("\nElemental abundances            Correct       From estimate\n");
    for (i = 1; i <= m_NE; i++)
      dbgp("  %2d %-14s    %+.7e     %+.7e\n",i, EDB[m_L2ElId[i]].m_Name,m_GAI[i],m_GA[i]);
    }
  #endif
  #if dbgVCS
  if (dbgIterMole())
    {
    dbgp("\n\nStoichiometric matrix m_SC[i][j]\n\n            ");
    for (i = 1; i <= m_NC; i++)
      {
      dbgp("%12.12s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
      dbgp("");

      }
    // now lets calculate ln K for the non-ideal case
    dbgp(" Delta Gø  ln K(non-ideal)\n");
    for (i = 1; i <= m_NR; i++)
      {
      L = m_IR[i];
      lnK = m_FF[L];    // - ln K
      g = m_DA[L];    // delta G
      for (j = 1; j <= m_NC; j++)
        {
        g += m_SC[j][i]*m_DA[j];
        lnK += m_SC[j][i]*m_FF[j];
        }
      dbgp(" 0 = ");
      dbgp("%-7.7s",SDB[m_L2SpId[m_Ind[L]]].SymOrTag());
      dbgp(" ");
      for (j = 1; j <= m_NC; j++)
        dbgp("%7.2g     ",m_SC[j][i]);
      dbgp("%+8.6f  %+8.6f\n", g, -lnK);
      }
    }
  #endif
  return (m_Route = LXferResult);
  }


void CVCSSolver::prin_immed(void)
  {
  int i;//,j;
  double g;

  dbgp("\n\nImmediate results, after forcer........");
  dbgp("\n\nIteration %3d : %3d Iterations since last evaluation of STOICH",m_Iter,m_It1-1);
  dbgp("\nNumber of basis optimisations = %3d  Immediate counter m_Iti = %3d",m_NOpt,m_Iti);
  dbgp("\nSpecies          Feed Moles    Initial moles     Final moles           MU/RT      delta G/RT\n");
  for (i = 1; i <= m_NC; i++)
    {
    dbgp("%-7.7s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
    dbgp("      ");
    dbgp("%+14.7f > %+14.7f  %+14.7f  %+14.7f\n",m_SpMoles[m_L2SpId[m_Ind[i]]],m_Wt[i],m_W[i],m_FE[i]);
    }
  for (i = m_NC+1; i <= m_MR; i++)
    {
    dbgp("%-7.7s",SDB[m_L2SpId[m_Ind[i]]].SymOrTag());
    dbgp("      ");
    dbgp("%+14.7f > %+14.7f  %+14.7f  %+14.7f  %+14.7f\n",m_SpMoles[m_L2SpId[m_Ind[i]]],m_Wt[i],m_W[i],m_FE[i],m_DG[i-m_NC]);
    }
  g = 0.0;
  for (i = 1; i <= m_NoPhases; i++)
    if (m_TMole[i] > 0.0 && m_TInert[i] > 0.0) g += m_TInert[i]*log((double)m_TInert[i]/m_TMole[i]);
  for (i =1; i <= m_MR; i++) g += m_W[i] * m_FE[i];
  dbgp("\nG/RT              = %+.7e",g);
  ELAB();
  dbgp("\nElemental abundances            Correct       From estimate\n");
  for (i = 1; i <= m_NE; i++)
    dbgp("  %2d %-14s    %+.7e     %+.7e\n",i, EDB[m_L2ElId[i]].m_Name,m_GAI[i],m_GA[i]);
  }

void CVCSSolver::CalcFinalInfo(void)
  {
  m_Hf_FdT=m_pModel->totHf(som_ALL, m_TempKStart, m_Press)-
           m_pModel->totHf(som_ALL, m_TempKStart, m_Press, &m_SpMass/*.pMArray()*/);
  m_Hf_PrT=m_pModel->totHf(som_ALL, m_TempKFinal, m_Press)-
           m_pModel->totHf(som_ALL, m_TempKFinal, m_Press, &m_SpMass/*.pMArray()*/);

  //m_Hf_RefT=m_pModel->totHf(som_ALL, m_TempKStart, m_Press);

  if (0)
    {
    dbgpln(" ==========================");
    dbgpln(" %12.3f  %12.3f", m_pModel->totHf(som_ALL, m_TempKStart, m_Press),
                              m_pModel->totHf(som_ALL, m_TempKFinal, m_Press));
    dbgpln(" %12.3f  %12.3f", m_pModel->totHf(som_ALL, m_TempKStart, m_Press, &m_SpMass/*.pMArray()*/),
                              m_pModel->totHf(som_ALL, m_TempKFinal, m_Press, &m_SpMass/*.pMArray()*/));

    dbgpln(" %12.12s  %12.12s", "---------------", "---------------");
    dbgpln(" %12.3f  %12.3f", m_Hf_FdT, m_Hf_PrT);
    dbgpln(" ==========================");
    }
  };

// ==========================================================================
//
//
//
// ==========================================================================

