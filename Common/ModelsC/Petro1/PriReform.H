//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PRIREFORM_H
#define  __PRIREFORM_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __2D_FN_H
  #include "2d_fn.h"
#endif

#define BASIC1
#define HEATXCHG1
#include "models.h"

#ifdef __PRIREFORM_CPP
  #define DllImportExport DllExport
#elif !defined(Petro1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

const byte HX1_Inoperative       = 0;
const byte HX1_LiquorLiquor      = 1;
const byte HX1_LiquorGas         = 2;
const byte HX1_GasGas            = 3;
const byte HX1_FullyCondensing   = 4;
//const byte HX1_PartialCondensing = 5;


class CStructure
  {
  public:

    double         Ci;
    double         Cp;
    double         dTh, dTc;
    double         Th, Tc;
    double         Hi, Ho;
    double         Duty;
    double         Mass;

    double         ThF, TcF;

    CStructure();
  };


// ==========================================================================
//
//  
//
// ==========================================================================

DEFINE_TAGOBJ(PriReform);
class DllImportExport PriReform : public MdlNode
  {
  public:
    CReactionBase  FuelRB;
    CReactionBase  GasRB;

    CHgtOrdInfo    m_HotBoxHgtOrd;
    SpContainer    m_HotBox;
    SpConduit      m_CombFeed;
    SpImage        m_HotBoxImg;
    CSpContPreset  m_HotBoxPreset;
    CCnStartCnds   m_HBStart;
    CEnvironHXBase m_EHX;

//  flag           m_bIgnition[7];
    struct 
      {
      flag         m_bOn;
      byte         m_iRow;
      byte         m_iCol;
      }            m_Detect[7];

    float          m_dBurnerOpen[84];
    //DualDbl        m_BurnCount;
    double         m_BurnCount;
    
    double         m_dCatMass;
    double         m_dCatTemp;
    double         m_dCatTempDeriv;
    double         m_dCatCp;
    double         m_dCatUA;
    double         m_dCatHeatIn;
    double         m_dCatHeat2Gas;

    CStateArray    m_States;                // Array of Integrators


    PriReform(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PriReform();

    void           BuildDataDefn(DataDefnBlk & DDB);
    flag           DataXchg(DataChangeBlk & DCB);
    flag           ValidateData(ValidateDataBlk & VDB);

    flag           InitialiseSolution();
    void           SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    void           SetDatumsDone();
    flag           Set_Sizes();
    void           SetState(eScdMdlStateActs RqdState);
    //void           StartSolution();
    void           PostConnect(int IONo);
    void           ConfigureJoins();
    void           StartStep();
    void           EvalJoinPressures(long JoinMask);
    CSpPropInfo*   IOGetNetProps(int i, double Qm);
    flag           EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);
    void           EvalProductsSurge(CNodeEvalIndex & NEI);
    void           EvalProducts(CNodeEvalIndex & NEI);
    void           EvalDerivs(CNodeEvalIndex & NEI);
    void           ODEOperate(CODEDataBlock & ODB);
    void           ApplyDerivs(double dTime, flag DoDbg);
    void           EvalDiscrete();
    void           ConvergeStates(CConvergeStateBlk &CSB);

    virtual flag   EvalPressureSens();

    void           ClosureInfo();

    virtual int    FilesUsed(CFilesUsedArray & Files) { return FuelRB.FilesUsed(Files)+GasRB.FilesUsed(Files); };

    dword          ModelStatus();

  private:
  };

// ===========================================================================

#undef DllImportExport


#endif


