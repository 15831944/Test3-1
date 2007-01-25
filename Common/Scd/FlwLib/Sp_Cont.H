//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SP_CONT_H
#define  __SP_CONT_H

#ifndef __SC_DEFS_H
  #include <sc_defs.h>
#endif
#ifndef __SP_MODEL_H
  #include "sp_model.h"
#endif
#ifndef __SCDTEMPL_H
  #include "scdtempl.h"
#endif

#ifdef __SP_CONT_CPP
  #define DllImportExport DllExport
#elif !defined(FLWLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

// ===========================================================================

DEFINE_TAGOBJ(SpConduit);           // Transfers Material and Temperature
//DEFINE_TAGOBJ(SpDirectCd);          // Transfers Material and Temperature to Target
DEFINE_TAGOBJ(SpContainer);         // Container
DEFINE_TAGOBJ(SpImage);             // Material Image
DEFINE_TAGOBJ(SpVectorObj);         // Fractions Only
class FlwNode;
class CDirectFlwIO;                 // Only defined in M_BASE

// ===========================================================================
//
//                            Spcontainer Address
//
// ===========================================================================

//_FWDDEF(SpTargetTag)
//class DllImportExport SpTargetTag
//  {
//  public:
//    SpTargetTag(int LclIOId, int RmtIOId);
//    ~SpTargetTag();
//
//    void           SetTag(char * Tag);
//    flag           Configured() { return sTargetTag.Length()>0; };
//    flag           Valid();
//    char *         Tag();
//    SpContainer &  Container();
//    void           Reset();
//
//    void           AddMass2Target(SpConduit & Q, double QMass, double DeltaTime);
//    void           AddMass2Target(SpModel &M, double QMass, double DeltaTime);
//
//  protected:
//    flag           bMustLook;
//    Strng          sTargetTag;
//    SpContainer *  pTarget;
//    int            m_LclIOId;
//    int            m_RmtIOId;
//  };
//
//typedef CSCDList <SpTargetTag*, SpTargetTag*> SpTargetTagList;
//typedef CSCDList <SpContainer*, SpContainer*> SpContainerList;
//typedef CSCDPtrListIter <SpTargetTagList, SpTargetTag*> SpTargetTagIter;

// ===========================================================================
//
//                       List af all SpContAddress
//
// ===========================================================================

//_FWDDEF(SpTargetTags);
//class DllImportExport SpTargetTags
//  {
//  protected:
//    SpContainerList &Targets;
//    SpTargetTagList &Tags;
//    Strng_List      &MissingTargets;
//
//    pTaggedObject  pOwningObj;
//    Strng          sDefaultAreaName;
//
//  public:
//
//    SpTargetTags();
//    ~SpTargetTags();
//
//    void           Init(pTaggedObject OwningObj);
//    void           Term();
//    void           AddAddress(SpTargetTag* pAdd);
//    void           DelAddress(SpTargetTag* pAdd);
//    SpContainer *  Find(char * AreaTag);
//    flag           InitialiseSolution();
//    flag           TerminateSolution();
//
//    char *         DefaultArea(char * DefaultAreaName=NULL);
//
//    int            NMissingTargets() { return MissingTargets.Length(); };
//    char *         MissingTarget(int i) { pStrng p=MissingTargets.AtIndex(i); return p ? p->Str() : NULL; };
//
//  };
//
//extern DllImportExport SpTargetTags TargetTags;

// ===========================================================================
//
//                   SpConduit - Used for Transporting Material
//
// ===========================================================================

class DllImportExport SpConduit : public SpModelOwner
  {
  DEFINE_SPARES(SpConduit);

  protected:
    static CSpView  GlblView;

  public :

    SpConduit(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    SpConduit(pchar pTag_, pTaggedObject pAttach=NULL, TagObjAttachment eAttach=TOA_Unknown);
    SpConduit();
    virtual ~SpConduit();

    virtual void    SetStateAction(IE_Enables E);
//    void           Enable(IE_Enables E);// {pModel->Enable();};
//    void           Disable();// {pModel->Disable();};

    // Model Members --------------------------------------------------------
    void            SelectModel(SpModel *pOther, flag ForceIt);
    void            SelectModel(SpConduit *pOther, flag ForceIt);
    void            SelectModel(SpContainer *pOther, flag ForceIt);
    void            SelectModel(CSpMdlSlct & Slct);

    void            CopyModel(SpModel *RqdModel);
    void            CopyQualities(SpModel *SrcModel, int q0=-1, int q1=-1);
    //void            ChangeModel(SpModel *pOther);
    //void            ChangeModel(SpConduit *pOther);
    //void            ChangeModel(SpContainer *pOther);
    void            ChangeModel(SpModelOwner *pOther);
    void            ChangeModel(pchar ReqdModelDesc, bool UseAsClassId);
    void            ChangeModel(TagObjClass *pRqdModelClass_);
    void            SetModel(pchar ReqdModelDesc, bool UseAsClassId=false);
    void            SetModel(TagObjClass *pRqdModelClass_);
    flag            ModelIs(TagObjClass *pMC);

    long            Fidelity() { return pModel->Fidelity(); };

    void            SetState(eScdMdlStateActs RqdState);

    void            SetDataSign(int Sgn)                        { pModel->SetDataSign(Sgn); };
    signed char     DataSign()                                  { return pModel->DataSign(); };
    void            SetNegDataOK(bool OK)                       { pModel->SetNegDataOK(OK); };
    bool            NegDataOK()                                 { return pModel->NegDataOK(); };
    bool            Modified()                                  { return pModel->Modified(); };
    void            ClrModified()                               { pModel->ClrModified(); };

    void            DumpQualities()                             { pModel->DumpQualities(); };

    void            SetHoldVentExpand()                         { pModel->SetHoldVentExpand(); };
    void            ClrHoldVentExpand()                         { pModel->ClrHoldVentExpand(); };
    void            SetVentExpandOK()                           { pModel->SetVentExpandOK(); };
    void            ClrVentExpandOK()                           { pModel->ClrVentExpandOK(); };

    void            SetSQFlags(byte Flags, flag On)             { pModel->SetSQFlags(Flags, On); }
    flag            SQFlagsSet(byte Flags)                      { return pModel->SQFlagsSet(Flags); }

    SpQuality *     FindQuality(int No)                         { return pModel->QualityPtr(No, False);};
    SpQuality *     CreateQuality(int No)                       { return pModel->QualityPtr(No, True);};
    void            KillQuality(int No)                         { pModel->QualityRemove(No);};
    void            SetQualityHoldCopy(int No, flag On)         { SpQuality * p=FindQuality(No); if (p) p->SetHoldCopy(On); }; 

    double          getVValue(int s)                            { return pModel->VValue[s]; };
    double          getVMass(int s)                             { return pModel->VMass[s]; };
    double          getVMole(int s)                             { ASSERT(s>=0 && s<SVSpcCount()); return pModel->VMass[s]/SDB[s].MoleWt(); };
    double          getVAttr(int s)                             { return pModel->VAttr[s]; };
    double          getVCalc(int s)                             { return pModel->VCalc[s]; };
    double        * getVPtr(int s)                              { return pModel->VPtr[s]; }

    void            SetVValue(int i, double V)                  { pModel->SetVValue(i, V); };
    void            SetVMass(int i, CSysVector &Vec, double V)  { pModel->SetVMass(i, Vec, V); };
    void            AddVMass(int i, CSysVector &Vec, double V)  { pModel->AddVMass(i, Vec, V); };
    void            SclVMass(int i, double V)                   { pModel->SclVMass(i, V); };
    void            ClrVMass(int i)                             { pModel->ClrVMass(i); };
    void            SetVAttr(int i, double V)                   { pModel->SetVAttr(i, V); };
    void            SetSpcScalar(double V)                      { pModel->SetSpcScalar(V); };

    __declspec(property(get=getVValue,put=SetVValue))           double VValue[];
    __declspec(property(get=getVMass))                          double VMass[];
    __declspec(property(get=getVMole))                          double VMole[];
    __declspec(property(get=getVAttr,put=SetVAttr))             double VAttr[];
    __declspec(property(get=getVCalc))                          double VCalc[];
    __declspec(property(get=getVPtr))                           double *VPtr[];

    void           SetUsage(SpMdlUsage What)                                                              { pModel->SetUsage(What); };
    SpMdlUsage     Usage()                                                                                { return pModel->Usage(); };

    void           SetView(SV_View View)                                                                  { pModel->SetView(View); };
    SV_View        GetView()                                                                              { return pModel->GetView(); };
    void           SetViewBasis(SV_ViewBasis ViewBasis)                                                   { pModel->SetViewBasis(ViewBasis); };
    SV_ViewBasis   GetViewBasis()                                                                         { return pModel->GetViewBasis(); };
    long           GenVarsAvail()                                                                         { return pModel->GenVarsAvail(); };
    long           SpVarsAvail()                                                                          { return pModel->SpVarsAvail(); };

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB)                                                  { pModel->BuildDataDefn_Vars(DDB);};
    virtual void   BuildDataDefn_Species(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, size_t BlkSize)  { pModel->BuildDataDefn_Species(DDB, Blks, BlkSize); };

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges()                                                                      { return pModel->CheckDataRanges(); };

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    void           QZero();
    void           QCopy(SpConduit & C);
    void           QCopy(SpConduit & C, double Press);

    //void           QSet(int No, double M);

    void           SetLoMassLimit(double LoLimit=0.0) { pModel->SetLoMassLimit(LoLimit); };

    void           QSetF(SpModel &M, eSetMass How, double SF, double LF, double VF, double Press);
    void           QSetF(SpModel &M, PhMask PhaseM, double Frac, double Press);
    void           QSetF(SpModel &M, CSysVector &Filter, double Qm__, double Press);

    void           QSetM(SpModel &M, PhMask PhaseM, double Qm__, double Press);
    void           QSetM(SpModel &M, CSysVector &Filter, double Qm__, double Press);

    void           QSetF(SpModel &M, PhMask PhaseM, double Frac);
    void           QSetF(SpModel &M, CSysVector &Filter, double Frac);

    void           QSetM(SpModel &M, PhMask PhaseM, double Qm__);
    void           QSetM(SpModel &M, CSysVector &Filter, double Qm__);

    void           QAddF(SpModel &M, PhMask PhaseM, double Frac);
    void           QAddF(SpModel &M, CSysVector &Filter, double Frac);

    void           QAddM(SpModel &M, PhMask PhaseM, double Qm__);
    void           QAddM(SpModel &M, CSysVector &Filter, double Qm__);

    void           QSubF(SpModel &M, PhMask PhaseM, double Frac);
    void           QSubF(SpModel &M, CSysVector &Filter, double Frac);

    void           QSubM(SpModel &M, PhMask PhaseM, double Qm__);
    void           QSubM(SpModel &M, CSysVector &Filter, double Qm__);

    void           QScaleMass(PhMask PhaseM, double Mult) { pModel->ScaleMass(PhaseM, Mult);};
    void           QAdjustQmTo(PhMask PhaseM, double RqdQm) { pModel->AdjustMassTo(PhaseM, RqdQm); };
    void           QSetTraceMass() { pModel->SetTraceMass(); };

    void           QSaveMass(SpVector & V)    { pModel->SaveMass(V);    };
    void           QSaveMass(CSysVector & A)    { pModel->SaveMass(A);    };
    void           QRestoreMass(SpVector & V) { pModel->RestoreMass(V); };
    void           QRestoreMass(CSysVector & A) { pModel->RestoreMass(A); };

    double         QMass(PhMask PhaseM=som_ALL)                                   { return pModel->Mass(PhaseM); };
    double         QMass(CIArray & SpIds)                                         { return pModel->Mass(SpIds); };
    double         QMass(CSysVector &Scale)                                       { return pModel->Mass(Scale); };
    double         QMole(PhMask PhaseM=som_ALL)                                   { return pModel->Moles(PhaseM); };
    double         QMole(CIArray & SpIds)                                         { return pModel->Moles(SpIds); };
    double         QMole(CSysVector &Scale)                                       { return pModel->Moles(Scale); };

    double         QVolume(PhMask PhaseM=som_ALL)                                 { return pModel->Volume(PhaseM); };
    double         QVolume(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL) { return pModel->Volume(PhaseM, T_, P_, S); };
    double         QVolume(PhMask PhaseM, CSysVector &S)                          { return pModel->Volume(PhaseM, Temp(), Press(), &S); };
    double         QVolume(CIArray & SpIds)                                       { return pModel->Volume(SpIds); };
    double         QVolume(CIArray & SpIds, double T_, double P_, CSysVector* S=NULL) { return pModel->Volume(SpIds, T_, P_, S); };
    double         QVolume(CIArray & SpIds, CSysVector &S)                        { return pModel->Volume(SpIds, Temp(), Press(), &S); };
    double         QVolume(CSysVector &Scale)                                     { return pModel->Volume(Scale); };

    double         QNVolume(PhMask PhaseM=som_ALL, CSysVector* S=NULL)            { return pModel->NVolume(PhaseM, S); };
    double         QNVolume(PhMask PhaseM, CSysVector &S)                         { return pModel->NVolume(PhaseM, &S); };
    double         QNVolume(CIArray & SpIds, CSysVector* S=NULL)                  { return pModel->NVolume(SpIds, S); };
    double         QNVolume(CIArray & SpIds, CSysVector &S)                       { return pModel->NVolume(SpIds, &S); };
    double         QNVolume(CSysVector &Scale)                                    { return pModel->NVolume(Scale); };

    double         QElementMass(CIArray & ElementIds, PhMask Phase=som_ALL)       { return pModel->ElementMass(ElementIds, Phase); };

    double         Rho(PhMask PhaseM=som_ALL)                                     { return pModel->Rho(PhaseM);};
    double         Rho(PhMask PhaseM, double T, double P, CSysVector* S=NULL)     { return pModel->Rho(PhaseM, T, P, S);};
    double         Rho(CIArray & SpIds)                                           { return pModel->Rho(SpIds);};
    double         Rho(CIArray & SpIds, double T, double P, CSysVector* S=NULL)   { return pModel->Rho(SpIds, T, P, S);};
    double         Rho(CSysVector &Scale, double T, double P, CSysVector* S=NULL) { return pModel->Rho(Scale, T, P, S);};
    double         Rho(CSysVector &Scale)                                         { return pModel->Rho(Scale);};
    double         NRho(PhMask PhaseM=som_ALL, CSysVector* S=NULL)                { return pModel->NRho(PhaseM, S);};
    double         NRho(CIArray & SpIds, CSysVector* S=NULL)                      { return pModel->NRho(SpIds, S);};
    double         NRho(CSysVector &S)                                            { return pModel->NRho(som_ALL, &S); };

    double         DynamicViscosity(PhMask PhaseM=som_ALL) { return pModel->DynamicViscosity(PhaseM);};
    double         DynamicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->DynamicViscosity(PhaseM, T, P, S);};
    double         KinematicViscosity(PhMask PhaseM=som_ALL) { return pModel->KinematicViscosity(PhaseM);};
    double         KinematicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->KinematicViscosity(PhaseM, T, P, S);};
    double         ThermalConductivity(PhMask PhaseM=som_ALL) { return pModel->ThermalConductivity(PhaseM);};
    double         ThermalConductivity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->ThermalConductivity(PhaseM, T, P, S);};
    double         PrandtlNo(PhMask PhaseM=som_ALL) { return pModel->PrandtlNo(PhaseM);};
    double         PrandtlNo(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->PrandtlNo(PhaseM, T, P, S);};
    double         SurfaceTension(PhMask PhaseM=som_ALL) { return pModel->SurfaceTension(PhaseM);};
    double         SurfaceTension(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->SurfaceTension(PhaseM, T, P, S);};

    void           SetBadTempOK(flag OK) { pModel->SetBadTempOK(OK); };
    flag           BadTempOK()           { return pModel->BadTempOK(); };

    void           SetTempPress(double T_, double P_) {pModel->SetTempPress(T_, P_);};

    double         Temp() { return pModel->Temp();};
    void           SetTemp(double T) { pModel->SetTemp(T); };

    double         Press() { return pModel->Press();};
    void           SetPress(double P) { pModel->SetPress(P);};

    double         PartialPress(SpecieIter & Ids, double T, double P=dNAN)         { return pModel->PartialPress(Ids, T, P);};
    double         PartialPress(int SpIncIds, int SpExcIds, double P=dNAN)         { return pModel->PartialPress(SpIncIds, SpExcIds, Temp(), P);};
    double         PartialPressC(int CmpIncIds, int CmpExcIds, double P=dNAN)      { return pModel->PartialPress(CmpIncIds, CmpExcIds, Temp(), P);};
    double         PartialPressFrac(SpecieIter & Ids, double T, double P=dNAN)     { return pModel->PartialPressFrac(Ids, T, P);};
    double         PartialPressFrac(int SpIncIds, int SpExcIds, double P=dNAN)     { return pModel->PartialPressFrac(SpIncIds, SpExcIds, Temp(), P);};
    double         PartialPressFracC(int CmpIncIds, int CmpExcIds, double P=dNAN)  { return pModel->PartialPressFrac(CmpIncIds, CmpExcIds, Temp(), P);};

    void           SetVLEBlk(CVLEBase * pVLE)                     { pModel->m_pVLE=pVLE;  };
    void           ClrVLEBlk(CVLEBase * pVLE)                     { if (pModel->m_pVLE==pVLE) pModel->m_pVLE=NULL; };
    int            FlashCmpIndex()                                { return pModel->FlashCmpIndex(); };
    int            FlashLiqIndex()                                { return pModel->FlashLiqIndex(); };
    int            FlashVapIndex()                                { return pModel->FlashVapIndex(); };
    LPTSTR         FlashDescription()                             { return pModel->FlashDescription(); };
    
    LPTSTR         SaturationDescription()                        { return pModel->SaturationDescription(); };
    int            SaturationMethod()                             { return pModel->SaturationMethod(); };
    LPTSTR         SaturationMethodUsed()                         { return pModel->SaturationMethodUsed(); };
    int            SaturationCmpIndex()                           { return pModel->SaturationCmpIndex(); };
    double         SaturationTotalP(double T, int iSatComp=-1)    { return pModel->SaturationTotalP(T, NULL, iSatComp); };
    double         SaturationP(double T, int iSatComp=-1)         { return pModel->SaturationP(T, NULL, iSatComp); };
    double         SaturationT(double P, int iSatComp=-1)         { return pModel->SaturationT(P, NULL, iSatComp); };
    double         PureSaturationP(double T, int iSatComp=-1)     { return pModel->PureSaturationP(T, NULL, iSatComp); };
    double         PureSaturationT(double P, int iSatComp=-1)     { return pModel->PureSaturationT(P, NULL, iSatComp); };
    double         BoilingPtElevation(double P_)                  { return pModel->BoilingPtElevation(P_); };

    double         MassFrac(PhMask PhaseM=som_ALL) {return pModel->MassFrac(PhaseM);};
    double         MoleFrac(PhMask PhaseM=som_ALL) {return pModel->MoleFrac(PhaseM);};
    double         VolFrac(PhMask PhaseM, double Temp/*=Std_T*/, double Press/*=Std_P*//*, SpPropOveride *Ovr*/) { return pModel->VolFrac(PhaseM, Temp, Press); };
    double         SpecieConc(double T_, int iSpNo, PhMask PhaseM=som_ALL) {return pModel->SpecieConc(T_, iSpNo, PhaseM);};
    double         PhaseConc(double T_, PhMask Ph, PhMask PhaseM=som_ALL) {return pModel->PhaseConc(T_, Ph, PhaseM);};

    double         MoleWt(PhMask PhaseM=som_ALL) { return pModel->MoleWt(PhaseM); };

    double         CpCv(PhMask PhaseM=som_ALL) { return pModel->CpCv(PhaseM); };

    double         msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)    { return pModel->msCp(Phase, T_, P_, pMA); };
    double         msCp(PhMask Phase, double T_)                                    { return pModel->msCp(Phase, T_, Press()); };
    double         msCp(PhMask Phase=som_ALL)                                       { return pModel->msCp(Phase, Temp(), Press()); };
    double         msHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHs(Phase, T_, P_, pMA);};
    double         msHs(PhMask Phase, double T_)                                  { return pModel->msHs(Phase, T_, Press());};
    double         msHs(PhMask Phase=som_ALL)                                     { return pModel->msHs(Phase, Temp(), Press());};
    double         msHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHz(Phase, T_, P_, pMA);};
    double         msHz(PhMask Phase, double T_)                                  { return pModel->msHz(Phase, T_, Press());};
    double         msHz(PhMask Phase=som_ALL)                                     { return pModel->msHz(Phase, Temp(), Press());};
    double         msHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHf(Phase, T_, P_, pMA);};
    double         msHf(PhMask Phase, double T_)                                  { return pModel->msHf(Phase, T_, Press());};
    double         msHf(PhMask Phase=som_ALL)                                     { return pModel->msHf(Phase, Temp(), Press());};
    double         msSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msSf(Phase, T_, P_, pMA);};
    double         msSf(PhMask Phase, double T_)                                  { return pModel->msSf(Phase, T_, Press());};
    double         msSf(PhMask Phase=som_ALL)                                     { return pModel->msSf(Phase, Temp(), Press());};
                                                                                    
    double         totCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)   { return pModel->totCp(Phase, T_, P_, pMA); };
    double         totCp(PhMask Phase=som_ALL)                                      { return pModel->totCp(Phase, Temp(), Press()); };
    double         totHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHs(Phase, T_, P_, pMA);};
    double         totHs(PhMask Phase=som_ALL)                                    { return pModel->totHs(Phase, Temp(), Press());};
    double         totHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHz(Phase, T_, P_, pMA);};
    double         totHz(PhMask Phase=som_ALL)                                    { return pModel->totHz(Phase, Temp(), Press());};
    double         totHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHf(Phase, T_, P_, pMA);};
    double         totHf(PhMask Phase=som_ALL)                                    { return pModel->totHf(Phase, Temp(), Press());};
    double         totSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totSf(Phase, T_, P_, pMA);};
    double         totSf(PhMask Phase=som_ALL)                                    { return pModel->totSf(Phase, Temp(), Press());};

    void           Set_totHs(double H, double P=dNAN, SpModel * pRefMdl=NULL)     { pModel->Set_totHs(H, P, pRefMdl); };
    void           Set_totHz(double H, double P=dNAN, SpModel * pRefMdl=NULL)     { pModel->Set_totHz(H, P, pRefMdl); };
    void           Set_totHf(double H, double P=dNAN, SpModel * pRefMdl=NULL)     { pModel->Set_totHf(H, P, pRefMdl); };

    double         msLatentHeatVap(double T, double P, CSysVector * pMA=NULL)         { return pModel->msLatentHeatVap(T, P, pMA);};

    void           dbgDump(char * Hdr, flag Vert=1);
 private:
    SpConduit &     operator=(const SpConduit & S) {ASSERT(0); return *this;} // assignment prevent its use

  };

// ===========================================================================
//
//                    DirectedConduit Used for Vent Overflow Etc.
//
// ===========================================================================

//class DllImportExport SpDirectCd //: public SpConduit
//  {
//  public:
//    SpDirectCd();//TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
//    //SpDirectCd(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
//    virtual ~SpDirectCd();
//
//    SpTargetTag   &Target()                     { return m_Trg; };
//    flag           ZeroReqd()                   { return m_fZeroReqd; };
//    void           ClrZeroReqd()                { m_fZeroReqd=False; };
//    void           SetZeroReqd(flag On=true)    { m_fZeroReqd=On; };
//    flag           Discard()                    { return m_fDiscard; };
//    void           ClrDiscard()                 { m_fDiscard=False; };
//    void           SetDiscard(flag On=true)     { m_fDiscard=On; };
//    double         Total()                      { return m_dTotal; };
//    void           SetTotal(double Total)       { m_dTotal=Total; };
//    void           SetCd(SpConduit * Cd)        { m_pConduit=Cd; };
//    bool           CdOK()                       { return m_pConduit!=NULL; };
//    SpConduit    & getCd()                      { return *m_pConduit; };
//
//    _declspec(property(get=getCd))              SpConduit    & Cd;
//
//    //DEFINE_SPARES(SpDirectCd);
//
//  protected:
//    SpTargetTag    m_Trg;
//    flag           m_fZeroReqd;
//    flag           m_fDiscard;
//    double         m_dTotal;
//    SpConduit    * m_pConduit;
//  };
//
// ===========================================================================
//
//                    SpContainer Used for Holding Material
//
// ===========================================================================

XID xidCnPress              = SpcXID(00);
XID xidCnTemp               = SpcXID(01);
XID xidCnALevel             = SpcXID(02);
XID xidCnRLevel             = SpcXID(03);
//XID xidCnInFlowRate         = SpcXID(17);
XID xidCnLevelSettle        = SpcXID(18);
XID xidCnOverFlowTarget     = SpcXID(20);
XID xidCnOverFlowTotal      = SpcXID(21);
XID xidCnOverFlowSettle     = SpcXID(22);
XID xidCnOverFlowRate       = SpcXID(23);
//XID xidCnOverFlowDiscard    = SpcXID(24);
XID xidCnGasVentTarget      = SpcXID(26);
XID xidCnGasVentTotal       = SpcXID(27);
XID xidCnGasVentRate        = SpcXID(28);
//XID xidCnGasVentDiscard     = SpcXID(29);

XID xidCnLSetTPL            = SpcXID(32);
XID xidCnLowestIOPt         = SpcXID(33);
XID xidCnVentLevel          = SpcXID(34);
XID xidCnVentPress          = SpcXID(35);
//XID xidCnVentAction         = SpcXID(36);

XID xidCnTopPRqd            = SpcXID(36);
XID xidCnIntTests           = SpcXID(37);
XID xidCnClosed             = SpcXID(38);
XID xidCnMixed              = SpcXID(39);
XID xidCnNetworked          = SpcXID(40);
XID xidCnStatsOn            = SpcXID(41);
XID xidCnStatsReset         = SpcXID(42);
XID xidCnStatsLvlAve        = SpcXID(43);
XID xidCnUseRhoH            = SpcXID(44);

// ---------------------------------------

const int MaxMixSetGrps     = 5;
const int MaxMixSetPrfPts   = 20;

XID xidCnMSEqn              = SpcXID(100);
XID xidCnMSSetPrfCnt        = SpcXID(101);

XID xidCnMSSetSpcCnt        = SpcXID(110);// Space for 5(MaxMixSetGrps)
XID xidCnMSAddSpc           = SpcXID(115);// Space for 5(MaxMixSetGrps)
XID xidCnMSRemSpc           = SpcXID(120);// Space for 5(MaxMixSetGrps)
XID xidCnMSDensity          = SpcXID(125);// Space for 5(MaxMixSetGrps)
XID xidCnMSIFLevel          = SpcXID(130);// Space for 5(MaxMixSetGrps)
XID xidCnMSMassFrac         = SpcXID(135);// Space for 5(MaxMixSetGrps)
XID xidCnMSMassTotal        = SpcXID(140);// Space for 5(MaxMixSetGrps)
XID xidCnMSVolumeFrac       = SpcXID(145);// Space for 5(MaxMixSetGrps)
XID xidCnMSVolumeTotal      = SpcXID(150);// Space for 5(MaxMixSetGrps)
XID xidCnMSLtFracLvlPt      = SpcXID(155);// Space for 5(MaxMixSetGrps) 

XID xidCnMSIOHgt            = SpcXID(200);// Space for 20(MaxMixSetPrfPts)*5(MaxMixSetGrps)
XID xidCnMSIOLtFrac         = SpcXID(300);// Space for 20(MaxMixSetPrfPts)*5(MaxMixSetGrps)
XID xidCnMSLtFracLvl        = SpcXID(400);// Space for 20(MaxMixSetPrfPts)*5(MaxMixSetGrps)

const dword SPDF_DynMode            = 0x00000001;
const dword SPDF_PBMode             = 0x00000002;
const dword SPDF_ShapeName          = 0x00000004;
const dword SPDF_ShapeData          = 0x00000008;
const dword SPDF_ShapeClosed        = 0x00000010;
const dword SPDF_ShapeNetworked     = 0x00000020;
const dword SPDF_ShapeMixed         = 0x00000040;
const dword SPDF_ShapeMnRes         = 0x00000080;
const dword SPDF_LevelSeek          = 0x00000100;
const dword SPDF_Inflow             = 0x00000200;
const dword SPDF_Overflow           = 0x00000400;
const dword SPDF_GasVent            = 0x00000800;
const dword SPDF_Safety             = 0x00001000;
const dword SPDF_Vapour             = 0x00002000;
const dword SPDF_MixSet             = 0x00004000;
const dword SPDF_NewPage1           = 0x00008000;
const dword SPDF_DynStats           = 0x00010000;
const dword SPDF_ShapeNameDisplay   = 0x01000000;
const dword SPDF_ShapeDataDisplay   = 0x02000000;

const dword SPDF_All                = (0xffffffff & ~SPDF_PBMode);

// ---------------------------------------------------------------------------

class DllImportExport CSpContPreset
  {
  protected:                    
    double      dTRqd;          
    double      dPRqd;          
    double      dLRqd;          
    flag        bTAdj;          
    flag        bDoVLE;         
    flag        bDoImg;         
    SpImage    *pImg;           
    static double dConvergeTol; 
                                
  public:
    CSpContPreset();
    virtual       ~CSpContPreset();
    virtual void  BuildDataDefn(DataDefnBlk &DDB, SpContainer & Cn);
    virtual flag  ValidateData(ValidateDataBlk & VDB, SpContainer & Cn);//double T, double P, double L);
    virtual flag  AdjustConditions(SpContainer & Cn);

    flag          TAdj() { return bTAdj; };
    void          SetTAdj(flag A) { bTAdj=A; };
    double        TRqd() { return dTRqd; };
    double        PRqd() { return dPRqd; };
    double        LRqd() { return dLRqd; };
    void          SetTRqd(double T) { dTRqd=T; };
    void          SetPRqd(double P) { dPRqd=P; };
    void          SetLRqd(double L) { dLRqd=L; };
    //short  SetupGas() { return iSetupGas; };
    //short  SetupLiq() { return iSetupLiq; };
    flag          DoVLE() { return bDoVLE; };
    flag          DoImg() { return bDoImg && (pImg!=NULL); };
    SpImage      *Img() { return pImg; };
    CSysVector *ImgM();
    void          SetImg(SpImage * Img) { pImg=Img; };

    static double ConvergeTol() { return dConvergeTol; };

  };

// ---------------------------------------------------------------------------

class DllImportExport CContStatInfo
  {
  public:
    word       iType;

    double     dTtlTime;
    double     dOverflowTime;
    double     dMaxLvl;
    double     dMinLvl;
    double     dTotalLvl;

    CContStatInfo();
    virtual ~CContStatInfo();
    void ResetStats();
    void BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj);
    flag DataXchg(DataChangeBlk & DCB, TaggedObject* pObj);
    void ExecIns(SpContainer* pContainer);
  };

// ---------------------------------------------------------------------------

class CCnMixSet;
class CCnMixSetEqn
  {
  public:

    CCnMixSet           * m_pMS;
    CArray <long, long>   m_GrpSpcs[2];

    CCnMixSetEqn(CCnMixSet * pMS)                               { m_pMS=pMS; };

    virtual LPCTSTR   Name()                                    = 0;
    virtual double    GetValue(double FracHgt, double IFLevel, double Level)  = 0;
    virtual void      BuildDataDefn(DataDefnBlk & DDB)          = 0;
    virtual flag      DataXchg(DataChangeBlk & DCB)             { return 0; };
    virtual flag      ValidateData(ValidateDataBlk & VDB)       { return 1; };

    double            SolveForPrfPt(double PrfPt, double IFLevel, double Level);

    inline SpContainer * Cn();
  };

class CCnMixSetEqnArray : public CArray<CCnMixSetEqn*,CCnMixSetEqn*> {};

class CCnMixSetPrfPt
  {
  public:
    enum ePtType {eNull, eSurface, eIO, ePt};

    ePtType           m_iType;
    double            m_dHeight;

    CCnMixSetPrfPt()                             { m_iType=eNull; m_dHeight=0; }
    CCnMixSetPrfPt(ePtType Type, double Height)  { m_iType=Type;  m_dHeight=Height; }
  };

class CCnMixSetPrfArray : public CArray<CCnMixSetPrfPt, CCnMixSetPrfPt&> {};


class DllImportExport CCnMixSet
  {
  public:
    enum eFlags
      { 
      PD_HgtPrf=0x01, 
      PD_FracPrf=0x02, 
      PD_LvlOfFrac=0x04,
      };

    CCnMixSet(SpContainer *pCn);
    ~CCnMixSet();

    static void       GlblInitialise();
    static void       GlblTerminate();
    void              Initialise();
    void              SetUpDDBGrps();
    void              SelectEqn(long i);
    void              StartStep();
    flag              On();
    flag              GetImage(double FracHgt, CSysVector &Img);

    void              BuildDataDefn(DataDefnBlk & DDB);
    flag              DataXchg(DataChangeBlk & DCB);
    flag              ValidateData(ValidateDataBlk & VDB);

    double            GrpMassFrac(long Index);
    double            GrpMassTotal(long Index);
    double            GrpVolumeFrac(long Index);
    double            GrpVolumeTotal(long Index);
    double            GrpDensity(long Index);
    double            GrpIFLevel(long Index);
    double            GrpPrfPtLevel(long Index);
    double            PrfLtFrac(long GrpIndex, long PrfIndex);
    double            GrpPrfPtLevel(long GrpIndex, long PrfIndex);
        
    void              SetPrfPtAt(CCnMixSetPrfPt::ePtType Type, double H);
    void              SetPrfPts(long N);

  public:
    SpContainer     * m_pCn;
    long              m_iEqn;
    long              m_nGrps;
    CCnMixSetEqnArray m_Eqns;
    flag              m_bVolBased;
    byte              m_bDisplay;
    long              m_nFracPrfPtsRqd;
    long              m_nHgtPrfPtsRqd;
    CCnMixSetPrfArray m_Prf;

    static CIArray    sm_AvailSpcs;
    bool              m_DDBGrpsValid;
    CIArray           m_GrpIndex[MaxMixSetGrps];
    DDBValueLstMem    m_DDBGrpsAdd[MaxMixSetGrps];
    DDBValueLstMem    m_DDBGrpsRemove[MaxMixSetGrps];
    double            m_dIFLevel[MaxMixSetGrps];
    double            m_dLtFracPoint[MaxMixSetGrps];

  };

inline SpContainer * CCnMixSetEqn::Cn() { return m_pMS->m_pCn; };

// ---------------------------------------------------------------------------

class DllImportExport SpContainer : public SpModelOwner
  {
  public:
    SpContainer(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    SpContainer(pchar pTag_, pTaggedObject pAttach=NULL, TagObjAttachment eAttach=TOA_Unknown);
    virtual ~SpContainer();

    double          getVValue(int s)                              { return pModel->VValue[s]; };
    double          getVMass(int s)                               { return pModel->VMass[s]; };
    double          getVMole(int s)                               { ASSERT(s>=0 && s<SVSpcCount()); return pModel->VMass[s]/SDB[s].MoleWt(); };
    double          getVAttr(int s)                               { return pModel->VAttr[s]; };
    double          getVCalc(int s)                               { return pModel->VCalc[s]; };
    double          getVValueDot(int s)                           { return pModel->m_dM.VValue[s]; };
    double          getVMassDot(int s)                            { return pModel->m_dM.VMass[s]; };
    double        * getVPtr(int s)                                { return pModel->VPtr[s]; }

    void            SetVValue(int i, double V)                    { pModel->SetVValue(i, V); };
    void            SetVMass(int i, CSysVector &Vec, double V)    { pModel->SetVMass(i, Vec, V); };
    void            AddVMass(int i, CSysVector &Vec, double V)    { pModel->AddVMass(i, Vec, V); };
    void            SclVMass(int i, double V)                     { pModel->SclVMass(i, V); };
    void            ClrVMass(int i)                               { pModel->ClrVMass(i); };
    void            SetVAttr(int i, double V)                     { pModel->SetVAttr(i, V); };
    void            SetVMassDot(int i, CSysVector &Vec, double V) { pModel->m_dM.SetVMass(i, Vec, V); };
    void            SetVValueDot(int i, double V)                 { pModel->m_dM.SetVValue(i, V); };
    void            SetSpcScalar(double V)                        { pModel->SetSpcScalar(V); };

    __declspec(property(get=getVValue,put=SetVValue))             double VValue[];
    __declspec(property(get=getVMass))                            double VMass[];
    __declspec(property(get=getVMole))                            double VMole[];
    __declspec(property(get=getVAttr,put=SetVAttr))               double VAttr[];
    __declspec(property(get=getVCalc))                            double VCalc[];
    __declspec(property(get=getVValueDot))                        double VValueDot[];
    __declspec(property(get=getVMassDot))                         double VMassDot[];
    __declspec(property(get=getVPtr))                             double *VPtr[];

    double         Duty()                                         { return pModel->Duty(); };
    void           SetDuty(double D)                              { pModel->SetDuty(D); };

    // Data Members ---------------------------------------------------------

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges() { return pModel->CheckDataRanges(); };
//    void           SetAllowPreset(flag On) { PresetInfo.bAllowAdj=On; };

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    // Shape Members --------------------------------------------------------
    virtual SpShape* Shape() { return pShape; };

    long           Fidelity() { return pModel->Fidelity(); };

    void           CopyShape(SpShape* RqdShape) { SpModelOwner::ChangeShape(pShape, RqdShape->Class()); };
    void           ChangeShape(pchar ReqdShapeDesc) { SpModelOwner::ChangeShape(pShape, ReqdShapeDesc); };
    void           ChangeShape(TagObjClass *pRqdShapeClass_) { SpModelOwner::ChangeShape(pShape, pRqdShapeClass_); };
    void           SetVolume(double V);
    void           SetVapVolScale(double S, bool DoAdjustments);
    void           SetHeight(double H)          { pShape->SetHeight(H);}
    double         ShapeVolume()                { return pShape->ApparentVolume();}
    double         ShapeHeight()                { return pShape->Height();}
    double         ShapeDatumPt()               { return pShape->DatumPt();}
    double         Level(bool WithEntrainedVap=true)
      {
      double V=pModel->Volume(som_SL);
      if (WithEntrainedVap && (dVapEntrained>0.0))
        V+=dVapEntrained/pModel->Mass(som_Gas)*pModel->Volume(som_Gas);
      return pShape->Level(V);
      }
    double         RLevel(bool WithEntrainedVap=true)
      {
      double V=pModel->Volume(som_SL);
      if (WithEntrainedVap && (dVapEntrained>0.0))
        V+=dVapEntrained/pModel->Mass(som_Gas)*pModel->Volume(som_Gas);
      return pShape->RLevel(V);
      }
    double         Level(CSysVector * MA, bool WithEntrainedVap=true)
      {
      double T=pModel->Temp();
      double P=pModel->Press();
      double V=pModel->Volume(som_SL, T, P, MA);
      if (WithEntrainedVap && (dVapEntrained>0.0))
        V+=dVapEntrained/pModel->Mass(som_Gas)*pModel->Volume(som_Gas, T, P, MA);
      return pShape->Level(V);
      }
    void           GetSLEVImage(CSysVector & M)
      {
      M=MArray();
      double VScl=0.0;
      if (dVapEntrained>0.0)
        VScl=dVapEntrained/pModel->Mass(som_Gas);
      M.Mult(som_Gas, VScl);
      }
    double         SLEVDensity()
      {
      SpMArray SLEVImg;
      GetSLEVImage(SLEVImg);
      return Rho(som_ALL, Temp(), Press(), &SLEVImg);
      }

    void           SetLowestIOPt(double LF)     { pShape->SetLowestIOPt(LF); };
    void           SetMaxLevel(double MaxLevel) { pModel->SetMaxLevel(MaxLevel); };          
    double         LowestIOPt()                 { return pShape->LowestIOPt(); };       
    double         MaxLevel()                   { return pModel->MaxLevel(); };       
    void           SetMaxPressure(double Press) { pModel->SetMaxPressure(Press); };
    double         MaxPressure()                { return pModel->MaxPressure(); };    
    void           SetMassScale(double Scl)     { pModel->SetMassScale(Scl); };             
    double         MassScale()                  { return pModel->MassScale(); };     
    void           SetDataSign(int Sgn)         { pModel->SetDataSign(Sgn); };                   
    signed char    DataSign()                   { return pModel->DataSign(); };        
    void           SetNegDataOK(bool OK)        { pModel->SetNegDataOK(OK); };
    bool           NegDataOK()                  { return pModel->NegDataOK(); };
    bool           Modified()                   { return pModel->Modified(); };
    void           ClrModified()                { pModel->ClrModified(); };


    void           SetUsage(SpMdlUsage What)                                                              { pModel->SetUsage(What); };
    SpMdlUsage     Usage()                                                                                { return pModel->Usage(); };

    void           SetSQFlags(byte Flags, flag On)                                                        { pModel->SetSQFlags(Flags, On); }
    flag           SQFlagsSet(byte Flags)                                                                 { return pModel->SQFlagsSet(Flags); }

    SpQuality *    FindQuality(int No)                                                                    { return pModel->QualityPtr(No, False);};
    SpQuality *    CreateQuality(int No)                                                                  { return pModel->QualityPtr(No, True);};
    void           KillQuality(int No)                                                                    { pModel->QualityRemove(No);};
    void           SetQualityHoldCopy(int No, flag On)                                                    { SpQuality * p=FindQuality(No); if (p) p->SetHoldCopy(On); }; 

    void           SetView(SV_View View)                                                                  { pModel->SetView(View); };
    SV_View        GetView()                                                                              { return pModel->GetView(); };
    void           SetViewBasis(SV_ViewBasis ViewBasis)                                                   { pModel->SetViewBasis(ViewBasis); };
    SV_ViewBasis   GetViewBasis()                                                                         { return pModel->GetViewBasis(); };
    long           GenVarsAvail()                                                                         { return pModel->GenVarsAvail(); };
    long           SpVarsAvail()                                                                          { return pModel->SpVarsAvail(); };

    virtual void   BuildDataDefn_Species(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, size_t BlkSize)  { pModel->BuildDataDefn_Species(DDB, Blks, BlkSize); };

    void           SetMixed(byte Mixed)             { pModel->SetMixed(Mixed); };
    byte           Mixed()                          { return pModel->Mixed(); };

    void           SetClosed(byte RqdClosed, byte AdjustContents=false);
    byte           Closed()                         { return pModel->Closed(); };

    void           SetPressAsSet(byte PressAsSet)   {pModel->SetPressAsSet(PressAsSet); } ;
    byte           PressAsSet()                     { return pModel->PressAsSet(); };

    flag           NetPressDamp()                   { return m_NetPressDamp; };
    void           SetNetPressDamp(flag On)         { m_NetPressDamp=On; };

    double         TopPressRqd()                    { return m_TopPRqd; };
    void           SetTopPressRqd(double P)         { m_TopPRqd=P; };

    flag           UseRhoH()                        { return m_UseRhoH; };
    void           SetUseRhoH(flag On)              { m_UseRhoH=On; };

    // Model Members --------------------------------------------------------
    void           SelectModel(SpModel *RqdModel, flag ForceIt);
    void           SelectModel(SpConduit *pOther, flag ForceIt);
    void           SelectModel(SpContainer *pOther, flag ForceIt);
    void           SelectModel(CSpMdlSlct & Slct);

    void           CopyModel(SpModel *RqdModel);
    void           CopyQualities(SpModel *SrcModel, int q0=-1, int q1=-1);
    void           ChangeModel(SpModel *pOther);
    void           ChangeModel(SpConduit *pOther);
    void           ChangeModel(SpContainer *pOther);
    void           ChangeModel(pchar ReqdModelDesc, bool UseAsClassId);
    void           ChangeModel(TagObjClass *pRqdModelClass_);
    void           SetModel(pchar ReqdModelDesc, bool UseAsClassId=false);
    void           SetModel(TagObjClass *pRqdModelClass_);
    flag           ModelIs(TagObjClass *pMC);

    void           SetState(eScdMdlStateActs RqdState);
//    void           EquilibrateQualities() { pModel->EquilibrateQualities(); };
    void           DumpQualities() { pModel->DumpQualities(); };

    // Initialise Members ---------------------------------------------------

    void           SetRequiredLevel(double TempRqd, double PressRqd, double LvlRqd, /*int Sp2Adjust,*/ CSysVector *pImg=NULL);
    void           SetRequiredPress(double TempRqd, double PressRqd, /*int Sp2Adjust,*/ CSysVector *pImg=NULL);
    void           SetRequiredConditions(CSpContPreset * Preset = NULL);
    void           SetPreset(CSpContPreset * Preset, SpImage * Img) { pPreset=Preset; if (pPreset) pPreset->SetImg(Img); };
    CSpContPreset & GetPreset() { return *pPreset; };

    // State Members --------------------------------------------------------
    void           SetHoldVentExpand() { pModel->SetHoldVentExpand(); };
    void           ClrHoldVentExpand() { pModel->ClrHoldVentExpand(); };
    void           SetVentExpandOK() { pModel->SetVentExpandOK(); };
    void           ClrVentExpandOK() { pModel->ClrVentExpandOK(); };

    virtual void   SetStateAction(IE_Enables E);
//    void           Enable(IE_Enables E);
//    void           Disable();

    void           SetMass(SpContainer &C, PhMask PhaseM, double Mass__);
    void           SetMass(SpConduit & C, PhMask PhaseM, double Mass__);

    void           ZeroMass();
    void           ZeroDeriv();
    void           Copy(SpContainer & C);

    void           AddDeriv(SpConduit & Flw, double Sgn_, double EntrainRate=0.0);
    void           AddInternalDerivs(CSysVector * pdMdt, double dHdt)                                                                         { pModel->AddInternalDerivs(pdMdt, dHdt); };
    void           AddDiscrete(SpConduit & Flw, double Sgn_);
    void           EvalMStates()                                                    { pModel->EvalMStates();};
    void           EvalHStates()                                                    { pModel->EvalHStates();};
    //void           EvalVStates()                                                  { pModel->EvalVStates();};
    void           ApplyDerivs(double dTime, flag DoDbg);
    void           ODEOperate(CODEDataBlock & ODB);
    void           EvalStatistics(eScdCtrlTasks Tasks=CO_All);

    double         Mass(PhMask PhaseM=som_ALL)                                      { return pModel->Mass(PhaseM);}
    double         Moles(PhMask PhaseM=som_ALL)                                     { return pModel->Moles(PhaseM);}
    double         MoleWt(PhMask PhaseM=som_ALL)                                    { return pModel->MoleWt(PhaseM); };

    double         Volume(PhMask PhaseM=som_ALL)                                    { return pModel->Volume(PhaseM); };
    double         Volume(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL)  { return pModel->Volume(PhaseM, T_, P_, S); };
    double         Volume(CSysVector &S)                                            { return pModel->Volume(som_ALL, Temp(), Press(), &S); };

    double         NVolume(PhMask PhaseM=som_ALL, CSysVector* S=NULL)               { return pModel->NVolume(PhaseM, S); };
    double         NVolume(CSysVector &S)                                           { return pModel->NVolume(som_ALL, &S); };

    double         ElementMass(CIArray & ElementIds, PhMask Phase=som_ALL)          { return pModel->ElementMass(ElementIds, Phase); };

    double         Rho(PhMask PhaseM=som_ALL) { return pModel->Rho(PhaseM);};
    double         Rho(PhMask PhaseM, double T, double P, CSysVector* S=NULL)       { return pModel->Rho(PhaseM, T, P, S);};
    double         NRho(PhMask PhaseM=som_ALL, CSysVector* S=NULL)                  { return pModel->NRho(PhaseM, S);};
    double         NRho(CSysVector &S)                                              { return pModel->NRho(som_ALL, &S); };

    double         DynamicViscosity(PhMask PhaseM=som_ALL)                          { return pModel->DynamicViscosity(PhaseM);};
    double         DynamicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL)   { return pModel->DynamicViscosity(PhaseM, T, P, S);};
    double         KinematicViscosity(PhMask PhaseM=som_ALL)                        { return pModel->KinematicViscosity(PhaseM);};
    double         KinematicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->KinematicViscosity(PhaseM, T, P, S);};
    double         ThermalConductivity(PhMask PhaseM=som_ALL)                       { return pModel->ThermalConductivity(PhaseM);};
    double         ThermalConductivity(PhMask PhaseM, double T, double P, CSysVector* S=NULL){ return pModel->ThermalConductivity(PhaseM, T, P, S);};
    double         PrandtlNo(PhMask PhaseM=som_ALL)                                 { return pModel->PrandtlNo(PhaseM);};
    double         PrandtlNo(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->PrandtlNo(PhaseM, T, P, S);};
    double         SurfaceTension(PhMask PhaseM=som_ALL)                            { return pModel->SurfaceTension(PhaseM);};
    double         SurfaceTension(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->SurfaceTension(PhaseM, T, P, S);};

    double         VapVolume()                                                      { return Max(1.0e-3, pShape->ActualVolume()-(pModel->Volume(som_SL)));};

    void           SetBadTempOK(flag OK)                                            { pModel->SetBadTempOK(OK); };
    flag           BadTempOK()                                                      { return pModel->BadTempOK(); };

    double         Press()                                                          { return pModel->Press();};
    double         Temp()                                                           {return pModel->Temp();}

    double         PartialPress(SpecieIter & Ids, double T, double P=dNAN)          { return pModel->PartialPress(Ids, T, P);};
    double         PartialPress(int SpIncIds, int SpExcIds, double P=dNAN)          { return pModel->PartialPress(SpIncIds, SpExcIds, Temp(), P);};
    double         PartialPressC(int CmpIncIds, int CmpExcIds, double P=dNAN)       { return pModel->PartialPress(CmpIncIds, CmpExcIds, Temp(), P);};
    double         PartialPressFrac(SpecieIter & Ids, double T, double P=dNAN)      { return pModel->PartialPressFrac(Ids, T, P);};
    double         PartialPressFrac(int SpIncIds, int SpExcIds, double P=dNAN)      { return pModel->PartialPressFrac(SpIncIds, SpExcIds, Temp(), P);};
    double         PartialPressFracC(int CmpIncIds, int CmpExcIds, double P=dNAN)   { return pModel->PartialPressFrac(CmpIncIds, CmpExcIds, Temp(), P);};

    void           SetVLEBlk(CVLEBase * pVLE)                    { pModel->m_pVLE=pVLE; };
    void           ClrVLEBlk(CVLEBase * pVLE)                    { if (pModel->m_pVLE==pVLE) pModel->m_pVLE=NULL; };
    int            FlashCmpIndex()                               { return pModel->FlashCmpIndex(); };
    int            FlashLiqIndex()                               { return pModel->FlashLiqIndex(); };
    int            FlashVapIndex()                               { return pModel->FlashVapIndex(); };
    LPTSTR         FlashDescription()                            { return pModel->FlashDescription(); };

    LPTSTR         SaturationDescription()                       { return pModel->SaturationDescription(); };
    int            SaturationMethod()                            { return pModel->SaturationMethod(); };
    LPTSTR         SaturationMethodUsed()                        { return pModel->SaturationMethodUsed(); };
    int            SaturationCmpIndex()                          { return pModel->SaturationCmpIndex(); };
    double         SaturationTotalP(double T, int iSatComp=-1)   { return pModel->SaturationTotalP(T, NULL, iSatComp); };
    double         SaturationP(double T, int iSatComp=-1)        { return pModel->SaturationP(T, NULL, iSatComp); };
    double         SaturationT(double P, int iSatComp=-1)        { return pModel->SaturationT(P, NULL, iSatComp); };
    double         PureSaturationP(double T, int iSatComp=-1)    { return pModel->PureSaturationP(T, NULL, iSatComp); };
    double         PureSaturationT(double P, int iSatComp=-1)    { return pModel->PureSaturationT(P, NULL, iSatComp); };
    double         BoilingPtElevation(double P_)                 { return pModel->BoilingPtElevation(P_); };

    void           Set_msHs(double H, double P=dNAN, SpModel * pRefMdl=NULL)   { pModel->Set_msHs(H, P, pRefMdl); };
    void           Set_msHz(double H, double P=dNAN, SpModel * pRefMdl=NULL)   { pModel->Set_msHz(H, P, pRefMdl); };
    void           Set_msHf(double H, double P=dNAN, SpModel * pRefMdl=NULL)   { pModel->Set_msHf(H, P, pRefMdl); };
    void           Set_totHs(double H, double P=dNAN, SpModel * pRefMdl=NULL)  { pModel->Set_totHs(H, P, pRefMdl); };
    void           Set_totHz(double H, double P=dNAN, SpModel * pRefMdl=NULL)  { pModel->Set_totHz(H, P, pRefMdl); };
    void           Set_totHf(double H, double P=dNAN, SpModel * pRefMdl=NULL)  { pModel->Set_totHf(H, P, pRefMdl); };

    void           SetTempPress(double T_, double P_) {pModel->SetTempPress(T_, P_);};
    void           SetTemp(double T_) {pModel->SetTemp(T_);};
    void           SetPress(double P_);

    void           SetLoMassLimit(double LoLimit=0.0) { pModel->SetLoMassLimit(LoLimit); };

    void           SetF(SpConduit & C, PhMask PhaseM, double Frac, double Press);
    void           SetF(SpConduit & C, eSetMass How, double SF, double LF, double VF, double Press);
    void           SetF(SpContainer &C, PhMask PhaseM, double Frac, double Press);
    void           SetF(SpContainer &C, CSysVector &Filter, double Frac, double Press);
    void           SetF(SpImage &C, PhMask PhaseM, double Frac, double Press);
    void           SetF(SpImage &C, CSysVector &Filter, double Qm__, double Press);

    void           SetM(SpConduit & C, PhMask PhaseM, double Qm__, double Press);
    void           SetM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ, double Press);
    void           SetM(SpContainer &C, PhMask PhaseM, double Qm__, double Press);
    void           SetM(SpContainer &C, CSysVector &Filter, double Qm__, double Press);
    void           SetM(SpImage &C, PhMask PhaseM, double Qm__, double Press);
    void           SetM(SpImage &C, CSysVector &Filter, double Qm__, double Press);

    void           AddF(SpConduit & C, eSetMass How, double SF, double LF, double VF);
    void           AddF(SpConduit & C, PhMask PhaseM, double Frac);
    void           AddF(SpContainer &C, PhMask PhaseM, double Frac);
    void           AddF(SpContainer &C, CSysVector &Filter, double Frac);
    void           AddF(SpImage &C, PhMask PhaseM, double Frac);
    void           AddF(SpImage &C, CSysVector &Filter, double Frac);

    void           AddM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ);
    void           AddM(SpConduit & C, PhMask PhaseM, double Qm__);
    void           AddM(SpContainer &C, PhMask PhaseM, double Qm__);
    void           AddM(SpContainer &C, CSysVector &Filter, double Qm__);
    void           AddM(SpImage &C, PhMask PhaseM, double Qm__);
    void           AddM(SpImage &C, CSysVector &Filter, double Qm__);

    void           SubF(SpConduit & C, eSetMass How, double SF, double LF, double VF);
    void           SubF(SpConduit & C, PhMask PhaseM, double Frac);
    void           SubF(SpContainer &C, PhMask PhaseM, double Frac);
    void           SubF(SpContainer &C, CSysVector &Filter, double Frac);
    void           SubF(SpImage &C, PhMask PhaseM, double Frac);
    void           SubF(SpImage &C, CSysVector &Filter, double Frac);

    void           SubM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ);
    void           SubM(SpConduit & C, PhMask PhaseM, double Qm__);
    void           SubM(SpContainer &C, PhMask PhaseM, double Qm__);
    void           SubM(SpContainer &C, CSysVector &Filter, double Qm__);
    void           SubM(SpImage &C, PhMask PhaseM, double Qm__);
    void           SubM(SpImage &C, CSysVector &Filter, double Qm__);

    double         MassFrac(PhMask PhaseM=som_ALL) {return pModel->MassFrac(PhaseM);};
    double         MoleFrac(PhMask PhaseM=som_ALL) {return pModel->MoleFrac(PhaseM);};
    double         VolFrac(PhMask PhaseM, double Temp/*=Std_T*/, double Press/*=Std_P*//*, SpPropOveride *Ovr*/) { return pModel->VolFrac(PhaseM, Temp, Press); };
    double         SpecieConc(double T_, int iSpNo, PhMask PhaseM=som_ALL) {return pModel->SpecieConc(T_, iSpNo, PhaseM);};
    double         PhaseConc(double T_, PhMask Ph, PhMask PhaseM=som_ALL) {return pModel->PhaseConc(T_, Ph, PhaseM);};

    void           SetDebug(flag DbgOn_) {pModel->SetDebug(DbgOn_);};
    void           ScaleMass(PhMask PhaseM, double Mult) { pModel->ScaleMass(PhaseM, Mult);};
    void           SetTraceMass() { pModel->SetTraceMass(); };

    void           SaveMass(SpVector & V)    { pModel->SaveMass(V);    };
    void           SaveMass(CSysVector & A)    { pModel->SaveMass(A);    };
    void           RestoreMass(SpVector & V) { pModel->RestoreMass(V); };
    void           RestoreMass(CSysVector & A) { pModel->RestoreMass(A); };

    double         CpCv(PhMask PhaseM=som_ALL) { return pModel->CpCv(PhaseM); };

    double         msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)    { return pModel->msCp(Phase, T_, P_, pMA); };
    double         msCp(PhMask Phase, double T_)                                    { return pModel->msCp(Phase, T_, Press()); };
    double         msCp(PhMask Phase=som_ALL)                                       { return pModel->msCp(Phase, Temp(), Press()); };
    double         msHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHs(Phase, T_, P_, pMA);};
    double         msHs(PhMask Phase, double T_)                                  { return pModel->msHs(Phase, T_, Press());};
    double         msHs(PhMask Phase=som_ALL)                                     { return pModel->msHs(Phase, Temp(), Press());};
    double         msHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHz(Phase, T_, P_, pMA);};
    double         msHz(PhMask Phase, double T_)                                  { return pModel->msHz(Phase, T_, Press());};
    double         msHz(PhMask Phase=som_ALL)                                     { return pModel->msHz(Phase, Temp(), Press());};
    double         msHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHf(Phase, T_, P_, pMA);};
    double         msHf(PhMask Phase, double T_)                                  { return pModel->msHf(Phase, T_, Press());};
    double         msHf(PhMask Phase=som_ALL)                                     { return pModel->msHf(Phase, Temp(), Press());};
    double         msSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msSf(Phase, T_, P_, pMA);};
    double         msSf(PhMask Phase, double T_)                                  { return pModel->msSf(Phase, T_, Press());};
    double         msSf(PhMask Phase=som_ALL)                                     { return pModel->msSf(Phase, Temp(), Press());};

    double         totCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)   { return pModel->totCp(Phase, T_, P_, pMA); };
    double         totCp(PhMask Phase=som_ALL)                                      { return pModel->totCp(Phase, Temp(), Press()); };
    double         totHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHs(Phase, T_, P_, pMA);};
    double         totHs(PhMask Phase=som_ALL)                                    { return pModel->totHs(Phase, Temp(), Press());};
    double         totHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHz(Phase, T_, P_, pMA);};
    double         totHz(PhMask Phase=som_ALL)                                    { return pModel->totHz(Phase, Temp(), Press());};
    double         totHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHf(Phase, T_, P_, pMA);};
    double         totHf(PhMask Phase=som_ALL)                                    { return pModel->totHf(Phase, Temp(), Press());};
    double         totSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totSf(Phase, T_, P_, pMA);};
    double         totSf(PhMask Phase=som_ALL)                                    { return pModel->totSf(Phase, Temp(), Press());};

    void           dbgDump(char * Hdr, flag Vert=1);

    SpConduit &     QOutEst() { if (pQOutEst==NULL) { pQOutEst=new SpConduit("QOut", this, TOA_Free); StructureChanged(this); }; return *pQOutEst; };

  public:
    void SetSpillEtc(CDirectFlwIO * pSpill, CDirectFlwIO * pVent) 
      {
      m_pSpill=pSpill; 
      m_pVent=pVent; 
      }
    bool           SpillExists()    { return m_pSpill!=NULL; };
    CDirectFlwIO & getSpill()       { return *m_pSpill; };

    bool           VentExists()     { return m_pVent!=NULL; };
    CDirectFlwIO & getVent()        { return *m_pVent; };

    _declspec(property(get=getSpill))    CDirectFlwIO & Spill;
    _declspec(property(get=getVent))     CDirectFlwIO & Vent;

    double         ResidenceTime(double SigmaQvIn, double SigmaQvOut, PhMask PhaseM);

    double         VapourEntrained() { return dVapEntrained; }; 
    double         VapourEscapeTau() { return dVapEscapeTau; };
    double         VapourLiberationTau() { return dVapLiberationTau; };

//    virtual double MCB_GetVolume();
//    virtual double MCB_SetVolume(double V);
    virtual void   MCB_GetOriginalTP(double &OrigT, double &OrigP);
    virtual void   MCB_OnSetVolume(flag KeepLPT, double OrigT, double OrigP, double OrigVol, double NewVol);
    virtual void   MCB_OnSetVapVolScl(double OrigScl, double NewScl);
//
    void           SetMeanResTimeCalcsReqd(flag On) { bMRTRqd=On;};
    flag           MeanResTimeCalcsReqd() { return bMRTRqd;};
    SpVector     * MeanResTimes() { return &SpMRT; };
    void           InitMeanResTimeCalcs();
    void           CompleteMeanResTimeCalcs(SpConduit &Fd, SpConduit &Pr);

    int            TestLevelLimits();

  private:
    SpContainer &  operator=(const SpContainer &S) {ASSERT(0); return *this;} // assignment prevent its use

  public:
    DEFINE_CI(SpContainer, SpModelOwner, 8)
    DEFINE_SPARES(SpContainer);
  public:
    SpShape*       pShape;
    //SpModel *      pModel;

  protected:
    static CSpView  GlblView;
    static long    TotalNo;
    long           MyNo;
    SpConduit *    pQOutEst;
    CDirectFlwIO * m_pSpill;
    CDirectFlwIO * m_pVent;

    double         dVapEntrained;    // Fraction of Vapour Entrained
    double         dVapEntrainRate;
    double         dVapEscapeTau;     // Time Constant of Vapour Escaping From Container to Atmosphere
    double         dVapLiberationTau; // Time Constant of Vapour Leaving the Entrained Phase

    CStateArray    m_States;          // Array of Integrators

    CSpContPreset *pPreset;

    CContStatInfo *pStats;       // dynamic stats info
    double         dInitMt;      // contents mass at start of DynStatsRun

    flag           bMRTRqd;      //
    SpVector       SpMRT;        // Mean Residence Time of each specie
    SpVector       SpMRTWrk;     // Workspace for SpMRT

    flag           m_NetPressDamp;
    double         m_TopPRqd;
    flag           m_UseRhoH;

  public:
    CCnMixSet      m_MixSet;

    double         dLevelSettle;
    double         dOverFlowSettle;
    dword          dwDisplay;

    flag           m_bKeepLPTOnVChange;

    double         m_HiLvlLimit;
    double         m_LoLvlLimit;
    flag           m_LvlHi;
    flag           m_LvlLo;
    flag           m_LmtAsStatus;


  };

// ===========================================================================
//
//                   SpImage - Used for Keeping Analyses of Material
//
// ===========================================================================

XID xidImgPRqd         = SpcXID(1000);
XID xidImgTRqd         = SpcXID(1001);
XID xidImgSLRqd        = SpcXID(1002);

enum SpImgMode { SPI_QModeNone, SPI_QModeQm, SPI_QModeQv, SPI_QModeNQv };

class DllImportExport SpImage : public SpModelOwner
  {
  public :
    SpImage(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    SpImage(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpImage();

    void            SetViewBlk(const BDDSpecies_Blk *ViewBlk, size_t ViewBlkSize)
                      { pViewBlk=ViewBlk; iViewBlkSize=ViewBlkSize; };

    // Model Members --------------------------------------------------------
    void            SelectModel(SpModel *RqdModel, flag ForceIt);
    void            SelectModel(CSpMdlSlct & Slct);

    void            CopyModel(SpModel *RqdModel);
    void            CopyQualities(SpModel *SrcModel, int q0=-1, int q1=-1);
    void            ChangeModel(SpModel *pOther);
    void            ChangeModel(SpConduit *pOther);
    void            ChangeModel(SpContainer *pOther);
    void            ChangeModel(pchar ReqdModelDesc, bool UseAsClassId);
    void            ChangeModel(TagObjClass *pRqdModelClass_);
    void            SetModel(pchar ReqdModelDesc, bool UseAsClassId=false);
    void            SetModel(TagObjClass *pRqdModelClass_);
    flag            ModelIs(TagObjClass *pMC);

    void            SetState(eScdMdlStateActs RqdState) { };

//    void            EquilibrateQualities() { pModel->EquilibrateQualities(); };
    void            DumpQualities() { pModel->DumpQualities(); };

    void            SetSQFlags(byte Flags, flag On) { pModel->SetSQFlags(Flags, On); }
    flag            SQFlagsSet(byte Flags) { return pModel->SQFlagsSet(Flags); }

    double          getVValue(int s)                            { return pModel->VValue[s]; };
    double          getVMass(int s)                             { return pModel->VMass[s]; };
    double          getVMole(int s)                             { ASSERT(s>=0 && s<SVSpcCount()); return pModel->VMass[s]/SDB[s].MoleWt(); };
    double          getVAttr(int s)                             { return pModel->VAttr[s]; };
    double          getVCalc(int s)                             { return pModel->VCalc[s]; };
    double        * getVPtr(int s)                              { return pModel->VPtr[s]; }

    void            SetVValue(int i, double V)                  { pModel->SetVValue(i, V); };
    void            SetVMass(int i, CSysVector &Vec, double V)  { pModel->SetVMass(i, Vec, V); };
    void            AddVMass(int i, CSysVector &Vec, double V)  { pModel->AddVMass(i, Vec, V); };
    void            SclVMass(int i, double V)                   { pModel->SclVMass(i, V); };
    void            ClrVMass(int i)                             { pModel->ClrVMass(i); };
    void            SetVAttr(int i, double V)                   { pModel->SetVAttr(i, V); };
    void            SetSpcScalar(double V)                      { pModel->SetSpcScalar(V); };

    __declspec(property(get=getVValue,put=SetVValue))           double VValue[];
    __declspec(property(get=getVMass))                          double VMass[];
    __declspec(property(get=getVMole))                          double VMole[];
    __declspec(property(get=getVAttr,put=SetVAttr))             double VAttr[];
    __declspec(property(get=getVCalc))                          double VCalc[];
    __declspec(property(get=getVPtr))                           double *VPtr[];

    void           SetUsage(SpMdlUsage What)                                                              { pModel->SetUsage(What); };
    SpMdlUsage     Usage()                                                                                { return pModel->Usage(); };

    void           SetView(SV_View View)                                                                  { pModel->SetView(View); };
    SV_View        GetView()                                                                              { return pModel->GetView(); };
    void           SetViewBasis(SV_ViewBasis ViewBasis)                                                   { pModel->SetViewBasis(ViewBasis); };
    SV_ViewBasis   GetViewBasis()                                                                         { return pModel->GetViewBasis(); };
    long           GenVarsAvail()                                                                         { return pModel->GenVarsAvail(); };
    long           SpVarsAvail()                                                                          { return pModel->SpVarsAvail(); };

    virtual void   BuildDataDefn_Vars(DataDefnBlk & DDB)                                                  { pModel->BuildDataDefn_Vars(DDB);};
    virtual void   BuildDataDefn_Species(DataDefnBlk & DDB, const BDDSpecies_Blk * Blks, size_t BlkSize)  { pModel->BuildDataDefn_Species(DDB, Blks, BlkSize); };

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual long   CheckDataRanges()              { return pModel->CheckDataRanges(); };
 
    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    void           Zero();
    void           Copy(SpConduit & C);
    void           Copy(SpConduit & C, double Press);
    void           Copy(SpContainer & C);
    void           Copy(SpContainer & C, double Press);

    bool           MassInUseOK()                     { return pModel->MassInUseOK();    };
    void           SetMassInUseOK(bool On)           { pModel->SetMassInUseOK(On);      };
    bool           MassInUse(int i)                  { return pModel->MassInUse(i);     };
    void           SetMassInUse(int i, bool InUse)   { pModel->SetMassInUse(i, InUse);  };
    void           SetMassInUse(bool InUse)          { pModel->SetMassInUse(InUse);     };
    void           ClrMassInUse()                    { pModel->ClrMassInUse();          };

    void           SetF(SpConduit & C, PhMask PhaseM, double Frac, double Press);
    void           SetF(SpConduit & C, eSetMass How, double SF, double LF, double VF, double Press);
    void           SetF(SpContainer &C, PhMask PhaseM, double Frac, double Press);
    void           SetF(SpContainer &C, CSysVector &Filter, double Frac, double Press);
    void           SetF(SpImage &C, PhMask PhaseM, double Frac, double Press);
    void           SetF(SpImage &C, CSysVector &Filter, double Qm__, double Press);

    void           SetM(SpConduit & C, PhMask PhaseM, double Qm__, double Press);
    void           SetM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ, double Press);
    void           SetM(SpContainer &C, PhMask PhaseM, double Qm__, double Press);
    void           SetM(SpContainer &C, CSysVector &Filter, double Qm__, double Press);
    void           SetM(SpImage &C, PhMask PhaseM, double Qm__, double Press);
    void           SetM(SpImage &C, CSysVector &Filter, double Qm__, double Press);

    void           AddF(SpConduit & C, eSetMass How, double SF, double LF, double VF);
    void           AddF(SpConduit & C, PhMask PhaseM, double Frac);
    void           AddF(SpContainer &C, PhMask PhaseM, double Frac);
    void           AddF(SpContainer &C, CSysVector &Filter, double Frac);
    void           AddF(SpImage &C, PhMask PhaseM, double Frac);
    void           AddF(SpImage &C, CSysVector &Filter, double Frac);

    void           AddM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ);
    void           AddM(SpConduit & C, PhMask PhaseM, double Qm__);
    void           AddM(SpContainer &C, PhMask PhaseM, double Qm__);
    void           AddM(SpContainer &C, CSysVector &Filter, double Qm__);
    void           AddM(SpImage &C, PhMask PhaseM, double Qm__);
    void           AddM(SpImage &C, CSysVector &Filter, double Qm__);

    void           SubF(SpConduit & C, eSetMass How, double SF, double LF, double VF);
    void           SubF(SpConduit & C, PhMask PhaseM, double Frac);
    void           SubF(SpContainer &C, PhMask PhaseM, double Frac);
    void           SubF(SpContainer &C, CSysVector &Filter, double Frac);
    void           SubF(SpImage &C, PhMask PhaseM, double Frac);
    void           SubF(SpImage &C, CSysVector &Filter, double Frac);

    void           SubM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ);
    void           SubM(SpConduit & C, PhMask PhaseM, double Qm__);
    void           SubM(SpContainer &C, PhMask PhaseM, double Qm__);
    void           SubM(SpContainer &C, CSysVector &Filter, double Qm__);
    void           SubM(SpImage &C, PhMask PhaseM, double Qm__);
    void           SubM(SpImage &C, CSysVector &Filter, double Qm__);

    double         MassFrac(PhMask PhaseM=som_ALL) {return pModel->MassFrac(PhaseM);};
    double         MoleFrac(PhMask PhaseM=som_ALL) {return pModel->MoleFrac(PhaseM);};
    double         VolFrac(PhMask PhaseM, double Temp/*=Std_T*/, double Press/*=Std_P*//*, SpPropOveride *Ovr*/) { return pModel->VolFrac(PhaseM, Temp, Press); };
    double         SpecieConc(double T_, int iSpNo, PhMask PhaseM=som_ALL) {return pModel->SpecieConc(T_, iSpNo, PhaseM);};
    double         PhaseConc(double T_, PhMask Ph, PhMask PhaseM=som_ALL) {return pModel->PhaseConc(T_, Ph, PhaseM);};

    double         Volume(PhMask PhaseM=som_ALL) { return pModel->Volume(PhaseM); };
    double         Volume(PhMask PhaseM, double T_, double P_, CSysVector* S=NULL) { return pModel->Volume(PhaseM, T_, P_, S); };
    double         Volume(CSysVector &S) { return pModel->Volume(som_ALL, Temp(), Press(), &S); };

    double         NVolume(PhMask PhaseM=som_ALL, CSysVector* S=NULL) { return pModel->NVolume(PhaseM, S); };
    double         NVolume(CSysVector &S) { return pModel->NVolume(som_ALL, &S); };

    double         Rho(PhMask PhaseM=som_ALL) { return pModel->Rho(PhaseM);};
    double         Rho(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->Rho(PhaseM, T, P, S);};
    double         NRho(PhMask PhaseM=som_ALL, CSysVector* S=NULL) { return pModel->NRho(PhaseM, S);};
    double         NRho(CSysVector &S) { return pModel->NRho(som_ALL, &S); };

    double         DynamicViscosity(PhMask PhaseM=som_ALL) { return pModel->DynamicViscosity(PhaseM);};
    double         DynamicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->DynamicViscosity(PhaseM, T, P, S);};
    double         KinematicViscosity(PhMask PhaseM=som_ALL) { return pModel->KinematicViscosity(PhaseM);};
    double         KinematicViscosity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->KinematicViscosity(PhaseM, T, P, S);};
    double         ThermalConductivity(PhMask PhaseM=som_ALL) { return pModel->ThermalConductivity(PhaseM);};
    double         ThermalConductivity(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->ThermalConductivity(PhaseM, T, P, S);};
    double         PrandtlNo(PhMask PhaseM=som_ALL) { return pModel->PrandtlNo(PhaseM);};
    double         PrandtlNo(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->PrandtlNo(PhaseM, T, P, S);};
    double         SurfaceTension(PhMask PhaseM=som_ALL) { return pModel->SurfaceTension(PhaseM);};
    double         SurfaceTension(PhMask PhaseM, double T, double P, CSysVector* S=NULL) { return pModel->SurfaceTension(PhaseM, T, P, S);};

    void           SetBadTempOK(flag OK) { pModel->SetBadTempOK(OK); };
    flag           BadTempOK()           { return pModel->BadTempOK(); };

    double         Temp() { return pModel->Temp();};
    double         Press() { return pModel->Press();};

    double         PartialPress(SpecieIter & Ids, double T, double P=dNAN)         { return pModel->PartialPress(Ids, T, P);};
    double         PartialPress(int SpIncIds, int SpExcIds, double P=dNAN)         { return pModel->PartialPress(SpIncIds, SpExcIds, Temp(), P);};
    double         PartialPressC(int CmpIncIds, int CmpExcIds, double P=dNAN)      { return pModel->PartialPress(CmpIncIds, CmpExcIds, Temp(), P);};
    double         PartialPressFrac(SpecieIter & Ids, double T, double P=dNAN)     { return pModel->PartialPressFrac(Ids, T, P);};
    double         PartialPressFrac(int SpIncIds, int SpExcIds, double P=dNAN)     { return pModel->PartialPressFrac(SpIncIds, SpExcIds, Temp(), P);};
    double         PartialPressFracC(int CmpIncIds, int CmpExcIds, double P=dNAN)  { return pModel->PartialPressFrac(CmpIncIds, CmpExcIds, Temp(), P);};

    void           SetTempPress(double T_, double P_)            { pModel->SetTempPress(T_, P_);};
    void           SetTemp(double T)                             { pModel->SetTemp(T); };
    void           SetPress(double P)                            { pModel->SetPress(P);};

    void           Set_totHs(double H, double P=dNAN, SpModel * pRefMdl=NULL) { pModel->Set_totHs(H, P, pRefMdl); };
    void           Set_totHz(double H, double P=dNAN, SpModel * pRefMdl=NULL) { pModel->Set_totHz(H, P, pRefMdl); };
    void           Set_totHf(double H, double P=dNAN, SpModel * pRefMdl=NULL) { pModel->Set_totHf(H, P, pRefMdl); };

    void           SetVLEBlk(CVLEBase * pVLE)                    { pModel->m_pVLE=pVLE; };
    void           ClrVLEBlk(CVLEBase * pVLE)                    { if (pModel->m_pVLE==pVLE) pModel->m_pVLE=NULL; };
    int            FlashCmpIndex()                               { return pModel->FlashCmpIndex(); };
    int            FlashLiqIndex()                               { return pModel->FlashLiqIndex(); };
    int            FlashVapIndex()                               { return pModel->FlashVapIndex(); };
    LPTSTR         FlashDescription()                            { return pModel->FlashDescription(); };

    LPTSTR         SaturationDescription()                       { return pModel->SaturationDescription(); };
    int            SaturationMethod()                            { return pModel->SaturationMethod(); };
    LPTSTR         SaturationMethodUsed()                        { return pModel->SaturationMethodUsed(); };
    int            SaturationCmpIndex()                          { return pModel->SaturationCmpIndex(); };
    double         SaturationTotalP(double T, int iSatComp=-1)   { return pModel->SaturationTotalP(T, NULL, iSatComp); };
    double         SaturationP(double T, int iSatComp=-1)        { return pModel->SaturationP(T, NULL, iSatComp); };
    double         SaturationT(double P, int iSatComp=-1)        { return pModel->SaturationT(P, NULL, iSatComp); };
    double         PureSaturationP(double T, int iSatComp=-1)    { return pModel->PureSaturationP(T, NULL, iSatComp); };
    double         PureSaturationT(double P, int iSatComp=-1)    { return pModel->PureSaturationT(P, NULL, iSatComp); };
    double         BoilingPtElevation(double P_)                 { return pModel->BoilingPtElevation(P_); };

    double         msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)    { return pModel->msCp(Phase, T_, P_, pMA); };
    double         msCp(PhMask Phase, double T_)                                    { return pModel->msCp(Phase, T_, Press()); };
    double         msCp(PhMask Phase=som_ALL)                                       { return pModel->msCp(Phase, Temp(), Press()); };
    double         msHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHs(Phase, T_, P_, pMA);};
    double         msHs(PhMask Phase, double T_)                                  { return pModel->msHs(Phase, T_, Press());};
    double         msHs(PhMask Phase=som_ALL)                                     { return pModel->msHs(Phase, Temp(), Press());};
    double         msHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHz(Phase, T_, P_, pMA);};
    double         msHz(PhMask Phase, double T_)                                  { return pModel->msHz(Phase, T_, Press());};
    double         msHz(PhMask Phase=som_ALL)                                     { return pModel->msHz(Phase, Temp(), Press());};
    double         msHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msHf(Phase, T_, P_, pMA);};
    double         msHf(PhMask Phase, double T_)                                  { return pModel->msHf(Phase, T_, Press());};
    double         msHf(PhMask Phase=som_ALL)                                     { return pModel->msHf(Phase, Temp(), Press());};
    double         msSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)  { return pModel->msSf(Phase, T_, P_, pMA);};
    double         msSf(PhMask Phase, double T_)                                  { return pModel->msSf(Phase, T_, Press());};
    double         msSf(PhMask Phase=som_ALL)                                     { return pModel->msSf(Phase, Temp(), Press());};

    double         totCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL)   { return pModel->totCp(Phase, T_, P_, pMA); };
    double         totCp(PhMask Phase=som_ALL)                                      { return pModel->totCp(Phase, Temp(), Press()); };
    double         totHs(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHs(Phase, T_, P_, pMA);};
    double         totHs(PhMask Phase=som_ALL)                                    { return pModel->totHs(Phase, Temp(), Press());};
    double         totHz(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHz(Phase, T_, P_, pMA);};
    double         totHz(PhMask Phase=som_ALL)                                    { return pModel->totHz(Phase, Temp(), Press());};
    double         totHf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totHf(Phase, T_, P_, pMA);};
    double         totHf(PhMask Phase=som_ALL)                                    { return pModel->totHf(Phase, Temp(), Press());};
    double         totSf(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL) { return pModel->totSf(Phase, T_, P_, pMA);};
    double         totSf(PhMask Phase=som_ALL)                                    { return pModel->totSf(Phase, Temp(), Press());};

    // Set State Varibles -----------
    double         m_Qm_Rqd;
    double         m_Qv_Rqd;
    double         m_NQv_Rqd;
    double         m_P_Rqd;
    double         m_T_Rqd;
    SpImgMode      m_iQRqdMode;
    SpImgMode      m_iQRqdSet;

    double         TRqd()                 { return m_T_Rqd; };
    double         PRqd()                 { return m_P_Rqd; };
    void           SetTRqdTo(double TRqd);
    void           SetPRqdTo(double PRqd);

    void           SetQMode(SpImgMode What);
    SpImgMode      QMode()                { return m_iQRqdMode; };
    double         QmRqd()                { return m_iQRqdMode==SPI_QModeNone ? dNAN : m_Qm_Rqd; };
    double         QvRqd()                { return m_iQRqdMode==SPI_QModeNone ? dNAN : m_Qv_Rqd; };
    double         NQvRqd()               { return m_iQRqdMode==SPI_QModeNone ? dNAN : m_NQv_Rqd; };
    void           SetQmRqdTo(double QmRqd, flag SetSet, flag SetMode); 
    void           SetQvRqdTo(double QvRqd, flag SetSet, flag SetMode); 
    void           SetNQvRqdTo(double QvRqd, flag SetSet, flag SetMode);
  private:
    SpImage &      operator=(const SpImage &S) {ASSERT(0); return *this;} // assignment prevent its use

  DEFINE_SPARES(SpImage);

  protected:
    static CSpView  GlblView;

  public :
    const BDDSpecies_Blk *pViewBlk;
    size_t         iViewBlkSize;
    flag           bSpeciesOnly;

  };

// ===========================================================================
//
//
//
// ===========================================================================

DllImportExport void SpContainerDumpTest();

// ===========================================================================

XID xidSvView         = SpcXID(2000);
XID xidSvViewBasis    = SpcXID(2001);

class DllImportExport SpVectorObj : public TaggedObject
  {
  //DEFINE_SPARES(SpVectorObj);

  protected:
    SV_View        iView;
    SV_ViewBasis   iViewBasis;
  public :
    SpVector       M;

    SpVectorObj(TagObjClass *pClass_, pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    SpVectorObj(pchar pTag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpVectorObj();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   GetOtherData(FilingControlBlock &FCB);
    virtual flag   PutOtherData(FilingControlBlock &FCB);

    void           SetView(SV_View View) { iView=View; };
    SV_View        GetView() { return iView; };
    void           SetViewBasis(SV_ViewBasis ViewBasis) { iViewBasis=ViewBasis; };
    SV_ViewBasis   GetViewBasis() { return iViewBasis; };

  };

// ===========================================================================
//
//
//
// ===========================================================================

XID xidAtmPress         = SpcXID(900000);
XID xidAtmDens          = SpcXID(900001);
XID xidAmbTemp          = SpcXID(900002);
XID xidBaseElevation    = SpcXID(900003);
XID xidWindSpeed        = SpcXID(900004);
XID xidWindDirn         = SpcXID(900005);
XID xidRelHumid         = SpcXID(900006);

class DllImportExport CEnvironment : public SpVector
  {
  protected:
    enum eStates { eNoGas, eHasInert, eHasGas } m_State;
    bool  m_bDoValidate;
  public:
    CEnvironment() 
      {
      m_State=eNoGas; 
      m_bDoValidate=true;
      };

    int Validate();
    int HasGas() 
      {
      if (m_bDoValidate)
        Validate();
      return m_State!=eNoGas; 
      };

    void BuildDataDefn(pTaggedObject pOwn, char * Tag, DataDefnBlk &DDB);
    flag DataXchg(pTaggedObject pOwn, DataChangeBlk & DCB);
    flag ValidateData(ValidateDataBlk & VDB);
    void DoTestState();
  };

extern DllImportExport CEnvironment gs_Environment;

typedef CArray <SpContainer*, SpContainer*> SpContainerArray;

// ===========================================================================
//
//                        inlines for CSpContPreset
//
// ===========================================================================

inline CSysVector * CSpContPreset::ImgM() { return pImg->pMArray(); };

// ===========================================================================
//
//                        inlines for SpContainer
//
// ===========================================================================

inline void SpContainer::SelectModel(SpModel *RqdModel, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, RqdModel, ForceIt); };
inline void SpContainer::SelectModel(SpConduit *pOther, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, pOther->Model(), ForceIt); };
inline void SpContainer::SelectModel(SpContainer *pOther, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, pOther->Model(), ForceIt); };
inline void SpContainer::SelectModel(CSpMdlSlct & Slct)
  { SpModelOwner::SelectModel(pModel, Slct); };
inline void SpContainer::ChangeModel(SpModel *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Class()); };
inline void SpContainer::ChangeModel(SpConduit *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
inline void SpContainer::ChangeModel(SpContainer *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
inline void SpContainer::CopyModel(SpModel *RqdModel)
  { SpModelOwner::ChangeModel(pModel, RqdModel->Class()); };
inline void SpContainer::CopyQualities(SpModel *SrcModel, int q0, int q1)
  { SpModelOwner::CopyQualities(pModel, SrcModel, q0, q1); };
inline void SpContainer::ChangeModel(pchar ReqdModelDesc, bool UseAsClassId)
  { SpModelOwner::ChangeModel(pModel, ReqdModelDesc, UseAsClassId); };
inline void SpContainer::ChangeModel(TagObjClass *pRqdModelClass_)
  { SpModelOwner::ChangeModel(pModel, pRqdModelClass_); };
inline void SpContainer::SetModel(pchar ReqdModelDesc, bool UseAsClassId)
  { SpModelOwner::SetModel(pModel, ReqdModelDesc, UseAsClassId); };
inline void SpContainer::SetModel(TagObjClass *pRqdModelClass_)
  { SpModelOwner::SetModel(pModel, pRqdModelClass_); };
inline flag SpContainer::ModelIs(TagObjClass *pMC)
  { return pModel->Class()==pMC; };

inline void SpContainer::SetMass(SpContainer &C, PhMask PhaseM, double Mass__)
  {
  CopyModel(C.Model());
  pModel->SetMassM(C.Model(), PhaseM, Mass__, C.Press());
  };
inline void SpContainer::SetMass(SpConduit & C, PhMask PhaseM, double Mass__)
  {
  CopyModel(C.Model());
  pModel->SetMassM(C.Model(), PhaseM, Mass__, C.Press());
  };
inline void SpContainer::SetPress(double P_)
  { if (!Closed()) pModel->SetPress(P_); };

// ===========================================================================
//
//                        inlines for SpConduit
//
// ===========================================================================

inline void SpConduit::SelectModel(SpModel *pOther, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, pOther, ForceIt); };
inline void SpConduit::SelectModel(SpConduit *pOther, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, pOther->Model(), ForceIt); };
inline void SpConduit::SelectModel(SpContainer *pOther, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, pOther->Model(), ForceIt); };
inline void SpConduit::SelectModel(CSpMdlSlct & Slct)
  { SpModelOwner::SelectModel(pModel, Slct); };
inline void SpConduit::CopyModel(SpModel *RqdModel)
  { SpModelOwner::ChangeModel(pModel, RqdModel->Class()); };
inline void SpConduit::CopyQualities(SpModel *SrcModel, int q0, int q1)
  { SpModelOwner::CopyQualities(pModel, SrcModel, q0, q1); };

inline void SpConduit::ChangeModel(SpModelOwner *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
inline void SpConduit::ChangeModel(pchar ReqdModelDesc, bool UseAsClassId)
  { SpModelOwner::ChangeModel(pModel, ReqdModelDesc, UseAsClassId); };
inline void SpConduit::ChangeModel(TagObjClass *pRqdModelClass_)
  { SpModelOwner::ChangeModel(pModel, pRqdModelClass_); };
inline void SpConduit::SetModel(pchar ReqdModelDesc, bool UseAsClassId)
  { SpModelOwner::SetModel(pModel, ReqdModelDesc, UseAsClassId); };
inline void SpConduit::SetModel(TagObjClass *pRqdModelClass_)
  { SpModelOwner::SetModel(pModel, pRqdModelClass_); };
inline flag SpConduit::ModelIs(TagObjClass *pMC)
  { return pModel->Class()==pMC; };


inline void SpConduit::QZero()
  { pModel->ZeroMass(); };
inline void SpConduit::QCopy(SpConduit & C)
  { CopyModel(C.pModel); pModel->Copy(C.pModel); }
inline void SpConduit::QCopy(SpConduit & C, double Press)
  { CopyModel(C.pModel); pModel->Copy(C.pModel); pModel->SetTempPress(C.Temp(), Press); }

//inline void SpConduit::QSet(int No, double M) {pModel->M[No]=M; pModel->ClrStatesOK(); };

inline void SpConduit::QSetF(SpModel &M, eSetMass How, double SF, double LF, double VF, double Press)
  { CopyModel(&M); pModel->SetMassF(&M, How, SF, LF, VF, Press); };
inline void SpConduit::QSetF(SpModel &M, PhMask PhaseM, double Frac, double Press)
  { CopyModel(&M); pModel->SetMassF(&M, PhaseM, Frac, Press); };
inline void SpConduit::QSetF(SpModel &M, CSysVector &Filter, double Frac, double Press)
  { CopyModel(&M); pModel->SetMassF(&M, Filter, Frac, Press); };

inline void SpConduit::QSetM(SpModel &M, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(&M); pModel->SetMassM(&M, PhaseM, Qm__, Press); };
inline void SpConduit::QSetM(SpModel &M, CSysVector &Filter, double Qm__, double Press)
  { CopyModel(&M); pModel->SetMassM(&M, Filter, Qm__, Press); };

inline void SpConduit::QSetF(SpModel &M, PhMask PhaseM, double Frac)
  { CopyModel(&M); pModel->SetMassF(&M, PhaseM, Frac, M.Press()); };
inline void SpConduit::QSetF(SpModel &M, CSysVector &Filter, double Frac)
  { CopyModel(&M); pModel->SetMassF(&M, Filter, Frac, M.Press()); };

inline void SpConduit::QSetM(SpModel &M, PhMask PhaseM, double Qm__)
  { CopyModel(&M); pModel->SetMassM(&M, PhaseM, Qm__, M.Press()); };
inline void SpConduit::QSetM(SpModel &M, CSysVector &Filter, double Qm__)
  { CopyModel(&M); pModel->SetMassM(&M, Filter, Qm__, M.Press()); };


inline void SpConduit::QAddF(SpModel &M, PhMask PhaseM, double Frac)
  { pModel->AddMassF(&M, PhaseM, Frac); };
inline void SpConduit::QAddF(SpModel &M, CSysVector &Filter, double Frac)
  { pModel->AddMassF(&M, Filter, Frac); };

inline void SpConduit::QAddM(SpModel &M, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(&M, PhaseM, Qm__); };
inline void SpConduit::QAddM(SpModel &M, CSysVector &Filter, double Qm__)
  { pModel->AddMassM(&M, Filter, Qm__); };

inline void SpConduit::QSubF(SpModel &M, PhMask PhaseM, double Frac)
  { pModel->SubMassF(&M, PhaseM, Frac); };
inline void SpConduit::QSubF(SpModel &M, CSysVector &Filter, double Frac)
  { pModel->SubMassF(&M, Filter, Frac); };

inline void SpConduit::QSubM(SpModel &M, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(&M, PhaseM, Qm__); };
inline void SpConduit::QSubM(SpModel &M, CSysVector &Filter, double Qm__)
  { pModel->SubMassM(&M, Filter, Qm__); };

// ===========================================================================
//
//                        inlines for SpImage
//
// ===========================================================================

inline void SpImage::SelectModel(SpModel *RqdModel, flag ForceIt)
  { SpModelOwner::SelectModel(pModel, RqdModel, ForceIt); };
inline void SpImage::SelectModel(CSpMdlSlct & Slct)
  { SpModelOwner::SelectModel(pModel, Slct); };

inline void SpImage::CopyModel(SpModel *RqdModel)
  { SpModelOwner::ChangeModel(pModel, RqdModel->Class()); };
inline void SpImage::CopyQualities(SpModel *SrcModel, int q0, int q1)
  { SpModelOwner::CopyQualities(pModel, SrcModel, q0, q1); };
inline void SpImage::ChangeModel(SpModel *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Class()); };
inline void SpImage::ChangeModel(SpConduit *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
inline void SpImage::ChangeModel(SpContainer *pOther)
  { SpModelOwner::ChangeModel(pModel, pOther->Model()->Class()); };
inline void SpImage::ChangeModel(pchar ReqdModelDesc, bool UseAsClassId)
  { SpModelOwner::ChangeModel(pModel, ReqdModelDesc, UseAsClassId); };
inline void SpImage::ChangeModel(TagObjClass *pRqdModelClass_)
  { SpModelOwner::ChangeModel(pModel, pRqdModelClass_); };
inline void SpImage::SetModel(pchar ReqdModelDesc, bool UseAsClassId)
  { SpModelOwner::SetModel(pModel, ReqdModelDesc, UseAsClassId); };
inline void SpImage::SetModel(TagObjClass *pRqdModelClass_)
  { SpModelOwner::SetModel(pModel, pRqdModelClass_); };
inline flag SpImage::ModelIs(TagObjClass *pMC)
  { return pModel->Class()==pMC; };

inline void SpImage::Zero()
  { pModel->ZeroMass(); };
inline void SpImage::Copy(SpConduit & C)
  { CopyModel(C.Model()); pModel->Copy(C.Model()); }
inline void SpImage::Copy(SpConduit & C, double Press)
  { CopyModel(C.Model()); pModel->Copy(C.Model()); pModel->SetTempPress(C.Temp(), Press); }
inline void SpImage::Copy(SpContainer & C)
  { CopyModel(C.Model()); pModel->Copy(C.Model()); }
inline void SpImage::Copy(SpContainer & C, double Press)
  { CopyModel(C.Model()); pModel->Copy(C.Model()); pModel->SetTempPress(C.Temp(), Press); }

//inline void SpImage::Set(int No, double M) {pModel->M[No]=M; pModel->ClrStatesOK(); };

inline void SpImage::SetF(SpConduit & C, PhMask PhaseM, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), PhaseM, Frac, Press); pModel->SetMassScale(1.0); };
inline void SpImage::SetF(SpConduit & C, eSetMass How, double SF, double LF, double VF, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), How, SF, LF, VF, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetF(SpContainer &C, PhMask PhaseM, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), PhaseM, Frac, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetF(SpContainer &C, CSysVector &Filter, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), Filter, Frac, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetF(SpImage &C, PhMask PhaseM, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), PhaseM, Frac, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetF(SpImage &C, CSysVector &Filter, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), Filter, Frac, Press); pModel->SetMassScale(1.0);};

inline void SpImage::SetM(SpConduit & C, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), PhaseM, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), How, SQ, LQ, VQ, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetM(SpContainer &C, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), PhaseM, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetM(SpContainer &C, CSysVector &Filter, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), Filter, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetM(SpImage &C, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), PhaseM, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpImage::SetM(SpImage &C, CSysVector &Filter, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), Filter, Qm__, Press); pModel->SetMassScale(1.0);};

inline void SpImage::AddF(SpConduit & C, eSetMass How, double SF, double LF, double VF)
  { pModel->AddMassF(C.Model(), How, SF, LF, VF); pModel->SetMassScale(1.0);};
inline void SpImage::AddF(SpConduit & C, PhMask PhaseM, double Frac)
  { pModel->AddMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::AddF(SpContainer &C, PhMask PhaseM, double Frac)
  { pModel->AddMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::AddF(SpContainer &C, CSysVector &Filter, double Frac)
  { pModel->AddMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::AddF(SpImage &C, PhMask PhaseM, double Frac)
  { pModel->AddMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::AddF(SpImage &C, CSysVector &Filter, double Frac)
  { pModel->AddMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};


inline void SpImage::AddM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ)
  { pModel->AddMassM(C.Model(), How, SQ, LQ, VQ); pModel->SetMassScale(1.0);};
inline void SpImage::AddM(SpConduit & C, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::AddM(SpContainer &C, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::AddM(SpContainer &C, CSysVector &Filter, double Qm__)
  { pModel->AddMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::AddM(SpImage &C, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::AddM(SpImage &C, CSysVector &Filter, double Qm__)
  { pModel->AddMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};

inline void SpImage::SubF(SpConduit & C, eSetMass How, double SF, double LF, double VF)
  { pModel->SubMassF(C.Model(), How, SF, LF, VF); pModel->SetMassScale(1.0);};
inline void SpImage::SubF(SpConduit & C, PhMask PhaseM, double Frac)
  { pModel->SubMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::SubF(SpContainer &C, PhMask PhaseM, double Frac)
  { pModel->SubMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::SubF(SpContainer &C, CSysVector &Filter, double Frac)
  { pModel->SubMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::SubF(SpImage &C, PhMask PhaseM, double Frac)
  { pModel->SubMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpImage::SubF(SpImage &C, CSysVector &Filter, double Frac)
  { pModel->SubMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};


inline void SpImage::SubM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ)
  { pModel->SubMassM(C.Model(), How, SQ, LQ, VQ); pModel->SetMassScale(1.0);};
inline void SpImage::SubM(SpConduit & C, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::SubM(SpContainer &C, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::SubM(SpContainer &C, CSysVector &Filter, double Qm__)
  { pModel->SubMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::SubM(SpImage &C, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpImage::SubM(SpImage &C, CSysVector &Filter, double Qm__)
  { pModel->SubMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};


// ===========================================================================
//
//
//
// ===========================================================================

inline void SpContainer::SetF(SpConduit & C, PhMask PhaseM, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), PhaseM, Frac, Press); pModel->SetMassScale(1.0); };
inline void SpContainer::SetF(SpConduit & C, eSetMass How, double SF, double LF, double VF, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), How, SF, LF, VF, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetF(SpContainer &C, PhMask PhaseM, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), PhaseM, Frac, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetF(SpContainer &C, CSysVector &Filter, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), Filter, Frac, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetF(SpImage &C, PhMask PhaseM, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), PhaseM, Frac, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetF(SpImage &C, CSysVector &Filter, double Frac, double Press)
  { CopyModel(C.Model()); pModel->SetMassF(C.Model(), Filter, Frac, Press); pModel->SetMassScale(1.0);};

inline void SpContainer::SetM(SpConduit & C, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), PhaseM, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), How, SQ, LQ, VQ, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetM(SpContainer &C, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), PhaseM, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetM(SpContainer &C, CSysVector &Filter, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), Filter, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetM(SpImage &C, PhMask PhaseM, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), PhaseM, Qm__, Press); pModel->SetMassScale(1.0);};
inline void SpContainer::SetM(SpImage &C, CSysVector &Filter, double Qm__, double Press)
  { CopyModel(C.Model()); pModel->SetMassM(C.Model(), Filter, Qm__, Press); pModel->SetMassScale(1.0);};

inline void SpContainer::AddF(SpConduit & C, eSetMass How, double SF, double LF, double VF)
  { pModel->AddMassF(C.Model(), How, SF, LF, VF); pModel->SetMassScale(1.0);};
inline void SpContainer::AddF(SpConduit & C, PhMask PhaseM, double Frac)
  { pModel->AddMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::AddF(SpContainer &C, PhMask PhaseM, double Frac)
  { pModel->AddMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::AddF(SpContainer &C, CSysVector &Filter, double Frac)
  { pModel->AddMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::AddF(SpImage &C, PhMask PhaseM, double Frac)
  { pModel->AddMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::AddF(SpImage &C, CSysVector &Filter, double Frac)
  { pModel->AddMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};


inline void SpContainer::AddM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ)
  { pModel->AddMassM(C.Model(), How, SQ, LQ, VQ); pModel->SetMassScale(1.0);};
inline void SpContainer::AddM(SpConduit & C, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::AddM(SpContainer &C, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::AddM(SpContainer &C, CSysVector &Filter, double Qm__)
  { pModel->AddMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::AddM(SpImage &C, PhMask PhaseM, double Qm__)
  { pModel->AddMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::AddM(SpImage &C, CSysVector &Filter, double Qm__)
  { pModel->AddMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};

inline void SpContainer::SubF(SpConduit & C, eSetMass How, double SF, double LF, double VF)
  { pModel->SubMassF(C.Model(), How, SF, LF, VF); pModel->SetMassScale(1.0);};
inline void SpContainer::SubF(SpConduit & C, PhMask PhaseM, double Frac)
  { pModel->SubMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::SubF(SpContainer &C, PhMask PhaseM, double Frac)
  { pModel->SubMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::SubF(SpContainer &C, CSysVector &Filter, double Frac)
  { pModel->SubMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::SubF(SpImage &C, PhMask PhaseM, double Frac)
  { pModel->SubMassF(C.Model(), PhaseM, Frac); pModel->SetMassScale(1.0);};
inline void SpContainer::SubF(SpImage &C, CSysVector &Filter, double Frac)
  { pModel->SubMassF(C.Model(), Filter, Frac); pModel->SetMassScale(1.0);};


inline void SpContainer::SubM(SpConduit & C, eSetMass How, double SQ, double LQ, double VQ)
  { pModel->SubMassM(C.Model(), How, SQ, LQ, VQ); pModel->SetMassScale(1.0);};
inline void SpContainer::SubM(SpConduit & C, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::SubM(SpContainer &C, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::SubM(SpContainer &C, CSysVector &Filter, double Qm__)
  { pModel->SubMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::SubM(SpImage &C, PhMask PhaseM, double Qm__)
  { pModel->SubMassM(C.Model(), PhaseM, Qm__); pModel->SetMassScale(1.0);};
inline void SpContainer::SubM(SpImage &C, CSysVector &Filter, double Qm__)
  { pModel->SubMassM(C.Model(), Filter, Qm__); pModel->SetMassScale(1.0);};

// ===========================================================================
//
//
//
// ===========================================================================

template < class C > class CSpCOptionPtr
  {
  public:
    C * p;
    LPTSTR   pTag;
    TaggedObject * pOwn;

    CSpCOptionPtr(LPTSTR Tag, TaggedObject * Own) { p=NULL; pTag=Tag; pOwn=Own; };
    ~CSpCOptionPtr() { delete p; };
    bool Exists() { return p!=NULL; };
    void Remove() { delete p; p=NULL; };
    C * operator->()
      {
      if (p==NULL) 
        p=new C(pTag, pOwn, TOA_Embedded); 
      return p ;
      };
    C & operator()()
      {
      if (p==NULL) 
        p=new C(pTag, pOwn, TOA_Embedded); 
      return *p ;
      };
    C * operator&()
      {
      if (p==NULL) 
        p=new C(pTag, pOwn, TOA_Embedded); 
      return p ;
      };
  };

// ===========================================================================
//
//
//
// ===========================================================================

#define dbgDumpSpcInfo      WITHDEBUG

#if (dbgDumpSpcInfo)
DllImportExport void dbgSpDeriv(char * t, char * Tag, SpContainer& Sp);
DllImportExport void dbgSpFlows(char * t, char * Tag, SpConduit& Sp, flag Tags);
DllImportExport void dbgSpMasses(char * t, char * Tag, SpContainer& Sp, flag Tags);
DllImportExport void dbgCnState(char * t, char * Tag, SpContainer& Sp, flag Tags);
#endif

// ===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport

#endif
