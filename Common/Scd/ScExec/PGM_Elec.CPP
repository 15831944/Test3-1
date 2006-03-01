//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*#M:Wire, used by PGM.*/

#include "stdafx.h"
#define  __PGM_ELEC_CPP
#include "sc_defs.h"
#include "pgm_elec.h"

//const short Idf_Index        = 1;
const short Idf_S            = 1;
const short Idf_V            = 2;
const short Idf_VPh          = 3;
const short Idf_I            = 4;
const short Idf_IPh          = 5;
const short Idf_Phase        = 6;

const short Idf_SetImpedance      = 10;
const short Idf_SetImpedanceLoad  = 11;
const short Idf_AddSeries         = 12;
const short Idf_AddParallel       = 13;
const short Idf_SetLoad           = 14;
const short Idf_SetSource         = 15;
const short Idf_ClosedCct         = 16;

long MakeFullIndex(long iTStrip, long iTerm) { return iTStrip*1000+iTerm; };
long ExtractTermIndex(long i) { return i%1000; };
long ExtractTermStripIndex(long i) { return i/1000; };

//===========================================================================
//
//
//
//===========================================================================

GCETerminalBase::GCETerminalBase(GCClassVar * pClassVar):
  m_lId(((GCLongVar*)pClassVar->GetVarByName("Id"))->m_var)
  //m_dVolts(((GCDoubleVar*)pClassVar->GetVarByName("Volts"))->m_var),  
  //m_dAmps(((GCDoubleVar*)pClassVar->GetVarByName("Amps"))->m_var),  
  //m_dPhase(((GCDoubleVar*)pClassVar->GetVarByName("Phase"))->m_var)  
  {
  m_lId=0/*NULL*/;
  //m_dVolts=0;
  //m_dAmps=0;
  //m_dPhase=0;
  //m_iTStrip=-1;
  //m_iTerm=-1;
  }

//---------------------------------------------------------------------------

GCETerminalBase::~GCETerminalBase()
  {
  }

//==========================================================================

GCETerminal::GCETerminal(GCInsMngr & IB) : GCClassVar(IB.m_pVarList, "Terminal", VarClassDefn)
  {
  AddVar  (IB, "Id",     &GCLong, VarConst);
  //AddVar  (IB, "Volts",     &GCDouble);
  //AddVar  (IB, "Amps",      &GCDouble);
  //AddVar  (IB, "Phase",     &GCDouble);
  AddFunct(IB, "Volts",        0, False, Idf_V      );
  AddFunct(IB, "Amps",         0, False, Idf_I      );
  AddFunct(IB, "Phase",        0, False, Idf_Phase  );
  }

//---------------------------------------------------------------------------

void GCETerminal::Init(GCClassVar * pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new GCETerminalBase(pClassVar);
  }
  
//---------------------------------------------------------------------------

void GCETerminal::Done(GCClassVar * pClassVar)
  {
  delete (GCETerminalBase*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCETerminal::CallFunct(GCInsMngr &IB, pvoid pSubClass, short FunctId, GCClassVar *ClassVar)
  {
  GCETerminalBase* pETB = (GCETerminalBase*)pSubClass;
  switch (FunctId)
    {
    case Idf_V: 
      return pETB->m_V.Mag();
    //case Idf_I: 
    //  return pETB->m_I.Mag();
    //case Idf_Phase: 
    //  return pETB->m_I.Ang()-pETB->m_V.Ang();
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCETerminal::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCETerminal::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  GCETerminalBase* pProf = (GCETerminalBase*)pSubClass;
  return True;
  }
  
//===========================================================================
//
//
//
//===========================================================================

GCTermStripBase::GCTermStripBase(GCClassVar * pClassVar) :
  m_lId(((GCLongVar*)pClassVar->GetVarByName("Id"))->m_var)
  {
  m_lId=0;
  }

//---------------------------------------------------------------------------

GCTermStripBase::~GCTermStripBase()
  {
  }

//==========================================================================

GCTermStrip::GCTermStrip(GCInsMngr & IB) : GCClassVar(IB.m_pVarList, "TerminalStrip", VarClassDefn)
  {
  AddVar  (IB, "Id",       &GCLong, VarConst);
  //AddFunct(IB, "Index",        0, False, Idf_Index     );
  AddFunct(IB, "SetImpedance",      4, False, Idf_SetImpedance    );  // (iL,iR,R,X) : Line, Return, Resistance, Reactance
  AddFunct(IB, "SetImpedanceLoad",  4, False, Idf_SetImpedanceLoad);  // (iL,iR,R,X) : Line, Return, Resistance, Reactance
  //AddFunct(IB, "AddSeries",         4, False, Idf_AddSeries   );  // (iL,iR,R,X) : Line, Return, Resistance, Reactance
  //AddFunct(IB, "AddParallel",       4, False, Idf_AddParallel );  // (iL,iR,R,X) : Line, Return, Resistance, Reactance
  AddFunct(IB, "SetLoad",           4, False, Idf_SetLoad         );  // (iL,iR,kVa,PFactor) : Line, Return, Resistance, Reactance
  AddFunct(IB, "SetSource",         6, False, Idf_SetSource       );  // (iL,iR,V,Phi,R,X) : Line, Return, Volts, Phase, Resistance, Reactance
  AddFunct(IB, "ClosedCct",    2, False, Idf_ClosedCct  );  // (iL,iR) : Line, Return
  AddFunct(IB, "Amps",              2, False, Idf_I               );  // (iL,iR) : Line, Return
  }

//---------------------------------------------------------------------------

void GCTermStrip::Init(GCClassVar * pClassVar)
  {
  pClassVar->m_pSubClass = (pvoid)new GCTermStripBase(pClassVar);
  }
  
//---------------------------------------------------------------------------

void GCTermStrip::Done(GCClassVar * pClassVar)
  {
  delete (GCTermStripBase*)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCTermStrip::CallFunct(GCInsMngr &IB, pvoid pSubClass, short FunctId, GCClassVar *ClassVar)
  {
  GCTermStripBase* pTSB = (GCTermStripBase*)pSubClass;
  switch (FunctId)
    {
//    case Idf_Index: 
//      return MakeFullIndex(pTSB->m_iTStrip,0);
    case Idf_SetImpedanceLoad:
    case Idf_SetImpedance:
    case Idf_AddSeries:
    case Idf_AddParallel:
      {
      double  X       = IB.GetDParm();
      double  R       = IB.GetDParm();
      CETerminal * Ret  = (CETerminal *)IB.GetLParm();
      CETerminal * Line = (CETerminal *)IB.GetLParm();
      //
      //_asm int 3;

      //dbgpln("PGM:CallFunct SetIm:%10.2g %10.2gi [%08x]%-25s --> [%08x]%s", R, X, Line, Line->Name(), Ret, Ret->Name());

      //long Index=pTSB->TSIndex();
      //if (Index>=0)
        {
        //CETermStrip &S=(*(*IB.m_pTermStripPtrs)[Index]);
        switch (FunctId)
          {
          case Idf_SetImpedance:     IB.m_pTermStripPtrs->SetImpedance(0, Line, Ret, R, X, false); break;
          case Idf_SetImpedanceLoad: IB.m_pTermStripPtrs->SetImpedance(0, Line, Ret, R, X, true); break;
          case Idf_AddSeries:    
            INCOMPLETECODE();
            //S.AddSeries   (iLine, iRet, R, X, (*IB.m_pTermStripPtrs)); 
            break;
          case Idf_AddParallel:  
            INCOMPLETECODE();
            //S.AddParallel (iLine, iRet, R, X, (*IB.]m_pTermStripPtrs)); 
            break;
          }
        }
      }
      return 0;
    case Idf_SetLoad:
      {
      }
      break;
    case Idf_SetSource:
      {
      double  X     = IB.GetDParm();
      double  R     = IB.GetDParm();
      double  Phi   = IB.GetDParm();
      double  V     = IB.GetDParm();
      CETerminal * Ret  = (CETerminal *)IB.GetLParm();
      CETerminal * Line = (CETerminal *)IB.GetLParm();

      //long Index=pTSB->TSIndex();
      //if (Index>=0)
      //  {
        //CETermStrip &S=(*(*IB.m_pTermStripPtrs)[Index]);
      IB.m_pTermStripPtrs->SetSource(0, Line, Ret, V, Phi, R, X); 
      //  }
      return 0;
      }
    case Idf_ClosedCct:
      {
      CETerminal * Ret  = (CETerminal *)IB.GetLParm();
      CETerminal * Line = (CETerminal *)IB.GetLParm();
      CECompData * pC=IB.m_pTermStripPtrs->GetCompData(0, Line, Ret, false); 
      if (pC)
        return pC->ClosedCct();
      return 0;
      }
    case Idf_I:
      {
      CETerminal * Ret  = (CETerminal *)IB.GetLParm();
      CETerminal * Line = (CETerminal *)IB.GetLParm();
      CECompData * pC=IB.m_pTermStripPtrs->GetCompData(0, Line, Ret, false); 
      if (pC)
        return pC->Current().Mag();
      return 0;
      }
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCTermStrip::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  }

//---------------------------------------------------------------------------
    
flag GCTermStrip::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  GCTermStripBase* pProf = (GCTermStripBase*)pSubClass;
  return True;
  }
  
//===========================================================================
//
//
//
//===========================================================================
