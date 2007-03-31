//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __PGM_N_CPP
#include "sc_defs.h"
#include "pgm_e.h"
#include "vectors.h"
#include "pgm_n.h"
#include "noise.h"
 
//===========================================================================


//===========================================================================

const short Idf_NoiseInit = 1;
const short Idf_NoiseSetup = 2;
const short Idf_NoiseSetType = 3;
const short Idf_NoiseSetGlobal = 4;
const short Idf_NoiseSetSeed = 5;
const short Idf_NoiseGetVal = 6;

//---------------------------------------------------------------------------

GCNoise::GCNoise(rGCInsMngr IB) : GCClassVar(IB.m_pVarList, &IB.m_VarMap, "NOISE", VarClassDefn)
  {
  AddVar(IB, "Output",    &GCDouble, VarConst);
  AddVar(IB, "Mean",      &GCDouble);
  AddVar(IB, "StdDev",    &GCDouble);
  //AddVar(IB, "NPts",       &GCLong);
  AddFunct(IB, "Init", 5, False, Idf_NoiseInit); //Init(mean, stddev, typ, global, seed)
  AddFunct(IB, "Setup", 4, False, Idf_NoiseSetup); //Setup(mean, stddev, typ, global)
/*  AddFunct(IB, "InitGaussian1", 3, False, Idf_NoiseInitGaussian1); //Init(mean, stddev, global)
  AddFunct(IB, "InitGaussian2", 3, False, Idf_NoiseInitGaussian2); //Init(mean, stddev, global)
  AddFunct(IB, "InitFlat1", 3, False, Idf_NoiseInitFlat1); //Init(mean, stddev, global)
  AddFunct(IB, "InitFlat2", 3, False, Idf_NoiseInitFlat2); //Init(mean, stddev, global)
  AddFunct(IB, "InitPoisson1", 2, False, Idf_NoiseInitPoisson1); //Init(lambda, global)
  AddFunct(IB, "InitPoisson2", 2, False, Idf_NoiseInitPoisson2); //Init(lambda, global)
  AddFunct(IB, "InitGamma1", 2, False, Idf_NoiseInitGamma1); //Init(mean, global)
  AddFunct(IB, "InitGamma2", 2, False, Idf_NoiseInitGamma2); //Init(mean, global)
  AddFunct(IB, "InitWeibull1", 3, False, Idf_NoiseInitWeibull1); //Init(a, b, global)
  AddFunct(IB, "InitWeibull2", 3, False, Idf_NoiseInitWeibull2); //Init(a, b, global)*/
  AddFunct(IB, "SetType", 1, False, Idf_NoiseSetType); //SetType(typ)
  AddFunct(IB, "SetGlobal", 1, False, Idf_NoiseSetGlobal); //SetGlobal(bit global)
  AddFunct(IB, "SetSeed", 1, False, Idf_NoiseSetSeed); //SetSeed(int seed)
  AddFunct(IB, "GetVal", 0, False, Idf_NoiseGetVal); //GetVal()
  }

//---------------------------------------------------------------------------

void GCNoise::Init(pGCClassVar pClassVar)
  {
  //pClassVar->m_pSubClass = (pvoid)new CBaseNoise(pClassVar);
  pClassVar->m_pSubClass = (pvoid)new CBaseNoise(((pGCDoubleVar)pClassVar->GetVarByName("Mean"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("StdDev"))->m_var, ((pGCDoubleVar)pClassVar->GetVarByName("Output"))->m_var);
  }
  
//---------------------------------------------------------------------------

void GCNoise::Done(pGCClassVar pClassVar)
  {
  delete (pCBaseNoise)(pClassVar->m_pSubClass);
  }

//---------------------------------------------------------------------------

double GCNoise::CallFunct(rGCInsMngr IB, pvoid pSubClass, short FunctId, pGCClassVar pClassVar)
  {
  pCBaseNoise pNoise = (pCBaseNoise)pSubClass;
  switch (FunctId)
    {
    case Idf_NoiseInit:
      {
      long Seed = IB.GetLParm();
      BOOL UseGlobal = (IB.GetCParm()!=0);
      byte NoiseTyp = Range(0, (int)IB.GetCParm(), (int)NS_Last-1);
      pNoise->SetType((NoiseTypes)NoiseTyp);
      pNoise->SetGlobal(UseGlobal);
      pNoise->SetSeed(Seed);
      double StdDev = IB.GetDParm();
      pNoise->Init(IB.GetDParm(), StdDev);
      break;
      }
    case Idf_NoiseSetup:
      {
      BOOL UseGlobal = (IB.GetCParm()!=0);
      byte NoiseTyp = Range(0, (int)IB.GetCParm(), (int)NS_Last-1);
      pNoise->SetType((NoiseTypes)NoiseTyp);
      pNoise->SetGlobal(UseGlobal);
      double StdDev = IB.GetDParm();
      pNoise->Init(IB.GetDParm(), StdDev);
      break;
      }
    case Idf_NoiseSetType:
      {
      byte NoiseTyp = Range(0, (int)IB.GetCParm(), (int)NS_Last-1);
      pNoise->SetType((NoiseTypes)NoiseTyp);
      break;
      }
    case Idf_NoiseSetGlobal:
      pNoise->SetGlobal(IB.GetCParm()!=0);
      break;
    case Idf_NoiseSetSeed:
      pNoise->SetSeed(IB.GetLParm());
      break;
    case Idf_NoiseGetVal:
      return pNoise->GetVal();
      break;
    default:
      ASSERT(FALSE); //function not defined
    }
  return 0.0;
  }

//---------------------------------------------------------------------------

void GCNoise::OnSave(FilingControlBlock &FCB, pvoid pSubClass)
  {
  pCBaseNoise pNoise = (pCBaseNoise)pSubClass;
  GCFCBAppendRec(FCB, '{', "NOISE", NULL, 0); // Start Internal Vars
  SaveVal(FCB, "Type", (byte)pNoise->NoiseType());
  SaveVal(FCB, "UseGlobal", (byte)pNoise->IsGlobal());
  GCFCBAppendRec(FCB, '}', "NOISE", NULL, 0); // End Internal Vars
  }

//---------------------------------------------------------------------------
    
flag GCNoise::OnLoad(FilingControlBlock &FCB, pvoid pSubClass)
  {
  pCBaseNoise pNoise = (pCBaseNoise)pSubClass;
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
        {
        byte by;
        if (Buff.Try("Type", by)) 
          {
          pNoise->SetType((NoiseTypes)by);
          break;
          }
        if (Buff.Try("UseGlobal", by)) 
          {
          pNoise->SetGlobal(by!=0);
          break;
          }
        break;
        }
      }
    }
  return True;
  }
  
//===========================================================================

/*
Member Functions

Init (double Mean, double StdDev, byte NoiseType, bit Global, long Seed)

Parameters

Mean:Double field. Specifies the mean.
StdDev:Double field. Specifies the standard deviation for Gaussian distribution.
NoiseType:Byte field. Specifies the noise generator type required See SetType function for more details.
Global:Bit field. Must the random numbers be calculated from a global generator. See SetGlobal function for more details.
Seed:Long field. A seed number for the random generator.
 
Return Value :

Zero.

Description :

This initialises all the parameters for the random number generator.


SetGlobal (bit Global)

Parameters

Global:Bit field. Must the random numbers be calculated from a global generator.

Return Value :

Zero.

Description :

This must be called after the SetType or Init functions. It negates the affect of the SetSeed function, so it must be called before the SetSeed function. This is only meaningful for 

*/

