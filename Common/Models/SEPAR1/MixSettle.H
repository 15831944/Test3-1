// SysCAD Copyright Kenwalt (Pty) Ltd 1995

#ifndef  MIXSETTLE_H
#define  MIXSETTLE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif  
		
#ifdef MIXSETTLE_CPP
  #define DllImportExport DllExport
#elif !defined(Separ1)
  #define DllImportExport DllImport
#else
  #define DllImportExport DllImport
#endif

enum SVMethods { SVM_Isotherms, SVM_Reactions };

// ==========================================================================

class LMS
  {
  public:
    double Beta;   
    double Weight; 
    double Err;    
    double Out;   
    double RqdOut;   
    double Input;   
    double NewOut;

    LMS()
      { 
      Beta=0.0;
      Weight=0.0;
      Err=0.0;
      Out=0.0;
      RqdOut=0.0;
      Input=0.0;
      NewOut=0.0;
      };
    ~LMS() {};
    void DoCalc();
  };

inline void LMS::DoCalc()
  {
  // estimate y-variable - using the Least Mean Squared rule 
  Err=RqdOut-Out; /* prev */
  Weight /* new */ = Weight /* prev */ + ((Beta * Err * Input)/(Input*Input));
  NewOut=Input*Weight /* new */ ;
  };

// ==========================================================================

DEFINE_TAGOBJ(MixerSettler);
//A mixer and settler model for solvent extraction. 

class MixerSettler : public MdlNode
  {
  private:
    CReactionBase  RB;//CReactionBlock *pRB;
    // CHeatXBlock    *pHX;

  public:
    SpContainer    Mixer;       // The mixer container
    SpContainer    Settler;     // The settler container
    
    SpConduit      QMxInEst;    // The stream (conduit) representing the sum of inflows to the mixer 
    SpConduit      QMxOFlow;    // The stream overflowing from the mixer to the settler

	  SpMArray       Aqueous, Organic; // arrays defining the aqueous & organic species

   	double         MixOFLev;    //mixer overflow level in %
    double         QmAcc, QvAcc; 
    double         MxResT;      // Mixer residence time;
    double         StResT;      // Settler residence time;
    double         RhoSep;      // The phase (organic/aqueous) separation density
    Strng          PriMetal, SecMetal;
    double 		     AqsInConc, OrgInConc;
    double 		     AqsOutConc, OrgOutConc;
		double				 AqLoss, OgLoss;
    double		     OARatio;
    flag		       Extraction, ReactionsOn;
    double         MinOrgConc, MinAqsConc;
    double         IntersectPt1, IntersectPt2;
    double         MaxLoadedConc, MaxStripConc;
    double         dReactExt;
    double         SEff;
    byte           iSol, iMethod;
    bool           m_AllowOveride;

    C2DFn          PriIsotherm;    // Distribution isotherm curve
    //C2DFn          SecIsotherm;    // Distribution isotherm curve
    flag           DoLMS;
    LMS            LMS1;           // a neural net type Least Mean Squared algorithm that 
                                   // slowly converges the extraction/stripping to match the
                                   // equilibrium isotherm.


    MixerSettler(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~MixerSettler();
	  virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
	  virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   InitialiseSolution();
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual void   SetDatumsDone();
    virtual flag   Set_Sizes();
    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);
  	virtual void   EvalJoinFlows(int JoinNo);
    virtual void   EvalSteadyState();

  	/*virtual void   MeasRqdFlows(rSpContainer M, HgtOrdArray & Ord, int io0, int ioN, double & QOutMRqd, double & QOutVRqd); // hss 5/1/97
  	virtual double SetOutFlows(rSpContainer M, 
                        HgtOrdArray & Ord, int io0, int ioN, double SFrac, double VFrac);*/
#if CNMTOREMOVE
  	virtual void   MeasureRqdFlows(SpContainer &Contents, double SLEVDensity, double SLEVSLfrac,
                              HgtOrdInfo & Info, 
                              int io0, int ioN, 
                              double & IoLvl,
                              double & QOutMRqd, double & QOutVRqd, 
                              double & QOutMAvl, double & QOutVAvl);

  	virtual double SetOutFlows(rSpContainer M, HgtOrdInfo &Info, int io0, int ioN, double SFrac, double VFrac);
  	virtual void   EvalProducts_SurgeLevel(rSpContainer M, rSpConduit QInEst, dword IOIds);
#endif

	  //overrides EvalProducts_SurgeLevel found in M_Base.h
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);
    virtual void   EvalDiscrete(flag ControlEnabled, flag ControlDisabled);

    virtual double FindNewYConc(double* InX, double* InY, double* Yintcpt);
    virtual double MetalConc(SpContainer* WV, SpMArray &Phse, Strng Metal, double* MetMass);
    virtual double MetalConc(SpConduit* FlwIn, SpMArray &Phse, Strng Metal, double* MetMass);

    virtual void   OnAppActivate(BOOL bActive)
      { 
      RB.OnAppActivate(bActive);
      };
    virtual int    FilesUsed(CFilesUsedArray & Files) 
      {
      RB.FilesUsed(Files);
      MdlNode::FilesUsed(Files);
      return Files.GetCount();
      //return RB.FilesUsed(Files); 
      };
    virtual flag   GetOtherData(FilingControlBlock &FCB) { return RB.GetOtherData(FCB); };
    virtual flag   PutOtherData(FilingControlBlock &FCB) { return RB.PutOtherData(FCB); };

    double         MetalMass(SpConduit &QIn);
    double         MetalExtract(double StripMass, double StripVol, double VolAqOut, double ExtConc, double StripConc, double Ratio);
    double         GetDensity(int index, double T, double P, SpPropOveride *Ovr);

    virtual dword  ModelStatus();

    // ConditionBlk Override
    DEFINE_CI(MixerSettler, MdlNode, 4);

#if DOPBSMOOTHING  
    virtual void   EvalMsAGDatas();
#endif
    
  };   

// ==========================================================================

#undef DllImportExport

#endif


