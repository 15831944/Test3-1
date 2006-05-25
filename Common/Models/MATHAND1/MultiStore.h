#pragma once

//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MULTISTORE_H
#define  __MULTISTORE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_BASE_H
  #include "m_base.h"
#endif
#ifndef __MTRLOGIC_H
  #include "mtrlogic.h"
#endif
#ifndef __SP_QUEUE_H
  #include "sp_queue.h"
#endif

#ifdef __MULTISTORE_CPP
  #define DllImportExport DllExport
#elif !defined(MatHand1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class CMultiStorage;

#define WITHMSSRCSNK 0

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CMultiStore);                 
class DllImportExport CMultiStore : public SpModelOwner
  {
  public:
    CMultiStore(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    CMultiStore(pchar pTag_, pTaggedObject pAttach=NULL, TagObjAttachment eAttach=TOA_Unknown);
    virtual ~CMultiStore();

    double          getVValue(int s)                            { return pModel->m_M.VValue[s]; };
    double          getVMass(int s)                             { return pModel->m_M.VMass[s]; };
    double          getVMole(int s)                             { ASSERT(s>=0 && s<SVSpcCount()); return pModel->m_M.VMass[s]/SDB[s].MoleWt(); };
    double          getVAttr(int s)                             { return pModel->m_M.VAttr[s]; };
    double          getVCalc(int s)                             { return pModel->m_M.VCalc[s]; };
    double          getDVValue(int s)                           { return pModel->m_dM.VValue[s]; };
    double          getDVMass(int s)                            { return pModel->m_dM.VMass[s]; };

    void            SetVValue(int i, double V)                  { pModel->m_M.SetVValue(i, V); };
    void            SetVMass(int i, CSysVector &Vec, double V)  { pModel->m_M.SetVMass(i, Vec, V); };
    void            AddVMass(int i, CSysVector &Vec, double V)  { pModel->m_M.AddVMass(i, Vec, V); };
    void            SclVMass(int i, double V)                   { pModel->m_M.SclVMass(i, V); };
    void            ClrVMass(int i)                             { pModel->m_M.ClrVMass(i); };
    void            SetVAttr(int i, double V)                   { pModel->m_M.SetVAttr(i, V); };
    void            SetDVMass(int i, CSysVector &Vec, double V) { pModel->m_dM.SetVMass(i, Vec, V); };
    void            SetDVValue(int i, double V)                 { pModel->m_dM.SetVValue(i, V); };

    __declspec(property(get=getVValue,put=SetVValue))           double VValue[];
    __declspec(property(get=getVMass))                          double VMass[];
    __declspec(property(get=getVMole))                          double VMole[];
    __declspec(property(get=getVAttr,put=SetVAttr))             double VAttr[];
    __declspec(property(get=getVCalc))                          double VCalc[];
    __declspec(property(get=getDVValue))                        double DVValue[];
    __declspec(property(get=getDVMass))                         double DVMass[];

    // Data Members ---------------------------------------------------------

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges(bool CheckIO) { return pModel->CheckDataRanges(); };

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);


    void           SetUsage(SpMdlUsage What)                                                              { pModel->SetUsage(What); };
    SpMdlUsage     Usage()                                                                                { return pModel->Usage(); };

    void           SetSQFlags(byte Flags, flag On)                                                        { pModel->SetSQFlags(Flags, On); }
    flag           SQFlagsSet(byte Flags)                                                                 { return pModel->SQFlagsSet(Flags); }

    SpQuality *    FindQuality(int No)                                                                    { return pModel->QualityPtr(No, False);};
    SpQuality *    CreateQuality(int No)                                                                  { return pModel->QualityPtr(No, True);};
    void           KillQuality(int No)                                                                    { pModel->QualityRemove(No);};

    void           SetView(SV_View View)                                                                  { pModel->SetView(View); };
    SV_View        GetView()                                                                              { return pModel->GetView(); };
    void           SetViewBasis(SV_ViewBasis ViewBasis)                                                   { pModel->SetViewBasis(ViewBasis); };
    SV_ViewBasis   GetViewBasis()                                                                         { return pModel->GetViewBasis(); };

    virtual void   BuildDataDefn_Species(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, size_t BlkSize)  { pModel->BuildDataDefn_Species(DDB, Blks, BlkSize); };

    // Model Members --------------------------------------------------------
    void SelectModel(pSpModel RqdModel, flag ForceIt)         { SpModelOwner::SelectModel(pModel, RqdModel, ForceIt); };
    void SelectModel(pSpConduit pOther, flag ForceIt)         { SpModelOwner::SelectModel(pModel, pOther->Model(), ForceIt); };
    void SelectModel(SpContainer *pOther, flag ForceIt)       { SpModelOwner::SelectModel(pModel, pOther->Model(), ForceIt); };
    void SelectModel(CSpMdlSlct & Slct)                       { SpModelOwner::SelectModel(pModel, Slct); };
    void ChangeModel(pSpModel pOther)                         { SpModelOwner::ChangeModel(pModel, pOther->Class()); };
    void ChangeModel(pSpConduit pOther)                       { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
    void ChangeModel(SpContainer *pOther)                     { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
    void CopyModel(pSpModel RqdModel)                         { SpModelOwner::ChangeModel(pModel, RqdModel->Class()); };
    void CopyQualities(pSpModel SrcModel, int q0, int q1)     { SpModelOwner::CopyQualities(pModel, SrcModel, q0, q1); };
    void ChangeModel(pchar ReqdModelDesc, bool UseAsClassId)  { SpModelOwner::ChangeModel(pModel, ReqdModelDesc, UseAsClassId); };
    void ChangeModel(TagObjClass *pRqdModelClass_)            { SpModelOwner::ChangeModel(pModel, pRqdModelClass_); };
    void SetModel(pchar ReqdModelDesc, bool UseAsClassId)     { SpModelOwner::SetModel(pModel, ReqdModelDesc, UseAsClassId); };
    void SetModel(TagObjClass *pRqdModelClass_)               { SpModelOwner::SetModel(pModel, pRqdModelClass_); };
    flag ModelIs(TagObjClass *pMC)                            { return pModel->Class()==pMC; };

    void           SetState(eScdMdlStateActs RqdState);
//    void           EquilibrateQualities() { pModel->EquilibrateQualities(); };
    void           DumpQualities() { pModel->DumpQualities(); };


    virtual void   SetStateAction(IE_Enables E);

    void           ZeroMass()                                                               { pModel->ZeroMass(); };
    void           ZeroDeriv()                                                              { pModel->ZeroDeriv(); };
    void           Copy(SpContainer& C)                                                     { CopyModel(C.pModel); pModel->Copy(C.pModel); }

    double         Press()                                                                  { return pModel->Press();};
    double         Temp()                                                                   { return pModel->Temp();}

    double         Mass(PhMask PhaseM=som_ALL)                                              {return pModel->Mass(PhaseM);}
    double         Moles(PhMask PhaseM=som_ALL)                                             {return pModel->Moles(PhaseM);}
    double         MoleWt(PhMask PhaseM=som_ALL)                                            { return pModel->MoleWt(PhaseM); };

    double         Volume(PhMask PhaseM=som_ALL)                                            { return pModel->Volume(PhaseM); };
    double         Volume(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL)          { return pModel->Volume(PhaseM, T_, P_, S); };
    //double         Volume(CIArray & SpIds)                                                { return pModel->Volume(SpIds);};
    double         Volume(CSysVector &S)                                                    { return pModel->Volume(som_ALL, Temp(), Press(), &S); };

//    double         NVolume(PhMask PhaseM=som_ALL)                                         { return pModel->NVolume(PhaseM); };
    double         NVolume(PhMask PhaseM=som_ALL, CSysVector* S=NULL)                       { return pModel->NVolume(PhaseM, S); };
    //double         NVolume(CIArray & SpIds)                                               { return pModel->NVolume(SpIds); };
    double         NVolume(CSysVector &S)                                                   { return pModel->NVolume(som_ALL, &S); };

    double         Rho(PhMask PhaseM=som_ALL)                                               { return pModel->Rho(PhaseM);};
    double         Rho(PhMask PhaseM, double T, double P, CSysVector* S=NULL)               { return pModel->Rho(PhaseM, T, P, S);};
    double         NRho(PhMask PhaseM=som_ALL, CSysVector* S=NULL)                          { return pModel->NRho(PhaseM, S);};
    double         NRho(CSysVector &S)                                                      { return pModel->NRho(som_ALL, &S); };

    double         DynamicViscosity(PhMask PhaseM=som_ALL)                                      { return pModel->DynamicViscosity(PhaseM);};
    double         DynamicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL)      { return pModel->DynamicViscosity(PhaseM, T, P, S);};
    double         KinematicViscosity(PhMask PhaseM=som_ALL)                                      { return pModel->KinematicViscosity(PhaseM);};
    double         KinematicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL)      { return pModel->KinematicViscosity(PhaseM, T, P, S);};
    double         ThermalConductivity(PhMask PhaseM=som_ALL)                               { return pModel->ThermalConductivity(PhaseM);};
    double         ThermalConductivity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->ThermalConductivity(PhaseM, T, P, S);};
    double         PrandtlNo(PhMask PhaseM=som_ALL)                                         { return pModel->PrandtlNo(PhaseM);};
    double         PrandtlNo(PhMask PhaseM, double T, double P, CSysVector* S=NULL)         { return pModel->PrandtlNo(PhaseM, T, P, S);};
    double         SurfaceTension(PhMask PhaseM=som_ALL)                                    { return pModel->SurfaceTension(PhaseM);};
    double         SurfaceTension(PhMask PhaseM, double T, double P, CSysVector* S=NULL)    { return pModel->SurfaceTension(PhaseM, T, P, S);};

    //double         VapVolume()                                                              { return Max(1.0e-3, pShape->ActualVolume()-(pModel->Volume(som_SL)));};

    void           SetDebug(flag DbgOn_)                                                    { pModel->SetDebug(DbgOn_);};
    void           ScaleMass(PhMask PhaseM, double Mult)                                    { pModel->ScaleMass(PhaseM, Mult);};
    void           SetTraceMass()                                                           { pModel->SetTraceMass(); };

    void           SaveMass(SpVector & V)                                                   { pModel->SaveMass(V);    };
    void           SaveMass(CSysVector & A)                                                 { pModel->SaveMass(A);    };
    void           RestoreMass(SpVector & V)                                                { pModel->RestoreMass(V); };
    void           RestoreMass(CSysVector & A)                                              { pModel->RestoreMass(A); };

    double         CpCv(PhMask PhaseM=som_ALL)                                              { return pModel->CpCv(PhaseM); };

    double         msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)          { return pModel->msCp(Phase, T_, P_, pMA); };
    double         msCp(PhMask Phase, double T_)                                            { return pModel->msCp(Phase, T_, Press()); };
    double         msCp(PhMask Phase=som_ALL)                                               { return pModel->msCp(Phase, Temp(), Press()); };
    double         msHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)          { return pModel->msHs(Phase, T_, P_, pMA);};
    double         msHs(PhMask Phase, double T_)                                            { return pModel->msHs(Phase, T_, Press());};
    double         msHs(PhMask Phase=som_ALL)                                               { return pModel->msHs(Phase, Temp(), Press());};
    double         msHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)          { return pModel->msHz(Phase, T_, P_, pMA);};
    double         msHz(PhMask Phase, double T_)                                            { return pModel->msHz(Phase, T_, Press());};
    double         msHz(PhMask Phase=som_ALL)                                               { return pModel->msHz(Phase, Temp(), Press());};
    double         msHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)          { return pModel->msHf(Phase, T_, P_, pMA);};
    double         msHf(PhMask Phase, double T_)                                            { return pModel->msHf(Phase, T_, Press());};
    double         msHf(PhMask Phase=som_ALL)                                               { return pModel->msHf(Phase, Temp(), Press());};
    double         msSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)          { return pModel->msSf(Phase, T_, P_, pMA);};
    double         msSf(PhMask Phase, double T_)                                            { return pModel->msSf(Phase, T_, Press());};
    double         msSf(PhMask Phase=som_ALL)                                               { return pModel->msSf(Phase, Temp(), Press());};

    double         totCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)         { return pModel->totCp(Phase, T_, P_, pMA); };
    double         totCp(PhMask Phase=som_ALL)                                              { return pModel->totCp(Phase, Temp(), Press()); };
    double         totHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)         { return pModel->totHs(Phase, T_, P_, pMA);};
    double         totHs(PhMask Phase=som_ALL)                                              { return pModel->totHs(Phase, Temp(), Press());};
    double         totHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)         { return pModel->totHz(Phase, T_, P_, pMA);};
    double         totHz(PhMask Phase=som_ALL)                                              { return pModel->totHz(Phase, Temp(), Press());};
    double         totHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)         { return pModel->totHf(Phase, T_, P_, pMA);};
    double         totHf(PhMask Phase=som_ALL)                                              { return pModel->totHf(Phase, Temp(), Press());};
    double         totSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)         { return pModel->totSf(Phase, T_, P_, pMA);};
    double         totSf(PhMask Phase=som_ALL)                                              { return pModel->totSf(Phase, Temp(), Press());};

    void           Validate();

    //flag           GetOtherData(FilingControlBlock &FCB);
    //flag           PutOtherData(FilingControlBlock &FCB);

    double         GetProduct(int ProdNo, PhMask Pass, double QmRqd, SpConduit & Cd);
    void           SetFeed(int FeedNo, double QmRqd, SpConduit & Cd);

  private:
    CMultiStore       & operator=(const CMultiStore &S) {ASSERT(0); return *this;} // assignment prevent its use

  public:
    DEFINE_CI(CMultiStore, SpModelOwner, 8)
    DEFINE_SPARES(CMultiStore);

  public:
#if WITHMSSRCSNK
    double          m_SrcQm;
    double          m_SnkQm;
#endif
    double          m_TotProdQmRqd;
    double          m_ProdQmTotal;
    double          m_FeedQmTotal;
    Strng           m_Name;
    SpConduit       m_Feed;

  protected:
    static CSpView  GlblView;

  };

//--------------------------------------------------------------------------

class DllImportExport CStoreArray : public CArray<CMultiStore*, CMultiStore*>
  {
  public:
    CMultiStore & operator[](int i) { return *GetAt(i); };
  };

//--------------------------------------------------------------------------

//DEFINE_TAGOBJ(CMultiStore);                 
class DllImportExport CMultiStoreRB : public CReactionBase
  {
  public:
    CMultiStoreRB(FlwNode * pAttach, bool IsSurgeType, LPTSTR Tag) : CReactionBase(pAttach, IsSurgeType, Tag) {};

  private:
    CReactionBase & operator=(const CReactionBase &S) {ASSERT(0); return *this;} // assignment prevent its use

  public:
    //DEFINE_CI(CMultiStore, SpModelOwner, 8)
    //DEFINE_SPARES(CMultiStoreRB);

  public:;

  protected:;

  };

//--------------------------------------------------------------------------

class DllImportExport CStoreRBArray : public CArray<CMultiStoreRB*, CMultiStoreRB*>
  {
  public:
    CReactionBase & operator[](int i) { return *GetAt(i); };
  };

//--------------------------------------------------------------------------

class DllImportExport CMultiStoreIO
  {
  public:
    CMultiStoreIO();
    ~CMultiStoreIO();
    CMultiStoreIO(const CMultiStoreIO & I);
    CMultiStoreIO & operator = (const CMultiStoreIO & I);

    void      Validate(bool FeedStream, bool StoreCentric);
    void      ValidateSplit();

  public:
    byte            m_iSplitMethod;
    short           m_iSplitStoreIndex;
    CArray<double, double> m_Qm; //actual flowrate to (or from) each multistore
    CArray<double, double> m_QmRqd;
    CArray<double, double> m_Split;
    CArray<PhMask, PhMask> m_QmPass;

    // phases - temp workspace
    CArray<double, double> m_PhFrac; 

  };

class DllImportExport CStoreIOArray : public CArray<CMultiStoreIO, CMultiStoreIO&> {};

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CMultiStorage);
class DllImportExport CMultiStorage : public MdlNode
  {
  friend class CBlendControlHelper;
  public:
    CMultiStorage(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CMultiStorage();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    void           SetPileCount(short n);
    long           GetPileCount() { return m_Store.GetCount(); };
    void           SetRBsOn(flag On);
    void           AddIO(int n);
    void           RemoveIO(int n);
    bool           IOOutSplitOK(int i);
    int            GetOutputStreamIndex(char* pStreamTag);
    bool           IsFracForStream(int StreamIndex);
    void           SetStoreFracForStream(int StreamIndex, int StoreIndex, double Value);

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    virtual void   SetState(eScdMdlStateActs RqdState);
    virtual flag   InitialiseSolution();
    virtual void   StartSolution();
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   ConfigureJoins();
    virtual void   StartStep();
    virtual void   EvalJoinPressures(long JoinMask);
    virtual flag   EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo);

    virtual void   EvalProducts(long JoinMask);
    virtual void   ODEOperate(CODEDataBlock & ODB);

    virtual void   EvalPowerRequired();
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);

    virtual void   EvalDiscrete();

    virtual void   OnAppActivate(BOOL bActive);
    virtual int    FilesUsed(CFilesUsedArray & Files);
    virtual void   ClosureInfo();

    virtual dword  ModelStatus();

    // ConditionBlk Override
    DEFINE_CI(CMultiStorage, MdlNode, 4);

  protected:
    flag              m_bTrackFlwReq;
    flag              m_bRBsOn;
    CStoreArray       m_Store;
    CStoreIOArray     m_StoreIO;
    CStoreRBArray     m_StoreRB;
    short             m_Store2View;
    Strng             m_sCommonRct;
    flag              m_bReloadReqd;
    flag              m_bStoreCentric;

    CDirectFlwIO      m_Src;
    CDirectFlwIO      m_Snk;
    CDirectFlwIO      m_AccIn;
    CDirectFlwIO      m_AccOut;
    CDirectAccCalc    m_AccCalc;
  };

//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport

#endif
