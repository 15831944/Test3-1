#pragma once
//-------------------------------------------------------------------------------------------------------
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LIMNDW

#define CONFIGURATION   gs_DWCfg
#define CONFIGURATIONS  gs_DWCfgs

const int xlerrRef  = -1;
const int xlerrNull = -2;

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CLimn_ModelData_ParmHdr
  {
  public:
    CLimn_ModelData_ParmHdr(LPCTSTR Text="", int nRows=0, int nCols=0)
      {
      m_Text=Text;
      m_nRows=nRows;
      m_nCols=nCols;
      }
    CString m_Text;
    int     m_nRows, m_nCols;
  };

class CLimn_ModelData_ParmDef
  {
  public:
    CLimn_ModelData_ParmDef();
    CLimn_ModelData_ParmDef(LPCSTR Class, LPCSTR Tag, LPCSTR Index, LPCTSTR CnvStr, bool IsBool, bool Hide, CArray<CLimn_ModelData_ParmHdr, CLimn_ModelData_ParmHdr&> & RqdHdrs) ;

  public:
    bool      m_IsBool;
    bool      m_Hide;
    bool      m_ArrayBegin;
    bool      m_ArrayEnd;
    int       m_ArrayLen;
    CString   m_Class;
    CString   m_Tag;
    CString   m_Index;
    CString   m_CnvStr;
    MCnv      m_Cnv;
    double    m_Scale;

    CArray<CLimn_ModelData_ParmHdr, CLimn_ModelData_ParmHdr&> m_Hdrs;    
  };

class CLimn_ModelData_Common
  {
  friend class CLimn_ModelData_Base;
  public:
    CLimn_ModelData_Common();
    ~CLimn_ModelData_Common();

  public:
    CArray<CLimn_ModelData_ParmDef*, CLimn_ModelData_ParmDef*>    m_Defs;
    CArray<CLimn_ModelData_ParmHdr, CLimn_ModelData_ParmHdr&>     m_RqdHdrs;    
  };

class CLimn_ModelData_Base
  {
  friend class CLimn_ModelData_Access;
  public:
    CLimn_ModelData_Base(CLimn_ModelData_Common * pCommon, double * pData=NULL);
    ~CLimn_ModelData_Base(void);

    enum eDataIndex {DI_None, DI_OSz, DI_DSz, DI_SG};

    class CRefBase
      {
      public:
        CRefBase();
        bool Initialise(CLimn_ModelData_Base * pBase, int Count);
        //bool Initialise(CLimn_ModelData_Base * pBase, int Rows, int Cols, const eDataIndex RDI, const eDataIndex CDI, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide=false);

      protected:
        CLimn_ModelData_Base * m_pBase;
        int    m_iParm;
      };

    class CBooleanRef : public CRefBase
      {
      public:
        CBooleanRef();
        void Initialise(CLimn_ModelData_Base * pBase, LPCSTR Tag, const bool DefaultValue, bool Hide=false);
        bool operator()()               { return m_pBase->m_pData[m_iParm] > 0.5; };
        void Set(bool value)            { m_pBase->m_pData[m_iParm] = value ? 1.0:0.0; };
      };

    class CDoubleRef : public CRefBase 
      {
      public:
        CDoubleRef();
        void Initialise(CLimn_ModelData_Base * pBase, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide=false);
        double& operator()()            { return m_pBase->m_pData[m_iParm]; };
      };

    class CVectorRef : public CRefBase
      {
      public:
        CVectorRef();
        void Initialise(CLimn_ModelData_Base * pBase, LPCSTR Class, int Len, const eDataIndex DI, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide=false);
        void Initialise(CLimn_ModelData_Base * pBase, LPCSTR Class, int Len, const eDataIndex DI, LPCSTR Tag, const double * DefaultValue, LPCTSTR CnvStr, bool Hide=false);
        double& operator()(int i)       { return m_pBase->m_pData[m_iParm+i]; };
        double * Ptr()                  { return &m_pBase->m_pData[m_iParm+0]; };;
      };
    class CMatrixRef : public CRefBase 
      {
      public:
        CMatrixRef();
        void Initialise(CLimn_ModelData_Base * pBase, LPCSTR Class, int Rows, int Cols, const eDataIndex RDI, const eDataIndex CDI, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide=false);
        void Initialise(CLimn_ModelData_Base * pBase, LPCSTR Class, int Rows, int Cols, const eDataIndex RDI, const eDataIndex CDI, LPCSTR Tag, const double * DefaultValue, LPCTSTR CnvStr, bool Hide=false);
        double& operator()(int r,int c) { return m_pBase->m_pData[m_iParm+m_nCols*r+c]; };
        double * Ptr()                  { return &m_pBase->m_pData[m_iParm+0]; };

      protected:
        int m_nCols;
      };

    void      Initialise();
    int       DataCount() { return m_Data.GetCount(); };
    void      BuildDataFields(MDataDefn & DD);
    bool      ExchangeDataFields(MDataChange & DX);

    void      MarkText(LPCTSTR Text);
    void      MarkMatrix(LPCTSTR Header, int Rows, int Cols);
    int       AddParms(int Count, bool FirstTime);

  protected:
    bool                            m_LocalData;
    long                            m_nDataCount;
    CArray<double, double>          m_Data;
    double                        * m_pData;   
    CLimn_ModelData_Common        & m_Common;

    CArray<CLimn_ModelData_ParmDef*, CLimn_ModelData_ParmDef*>  & m_Defs;
    CArray<CLimn_ModelData_ParmHdr, CLimn_ModelData_ParmHdr&>   & m_RqdHdrs;    

  };

class CLimn_ModelData_Access
  {
  public:
    CLimn_ModelData_Access(CLimn_ModelData_Base & Base) :
      m_Base(Base)
      {
      for (int i=0; i<m_Base.m_Data.GetCount(); i++)
        {
        m_Base.m_Data[i] *= m_Base.m_Defs[i]->m_Scale; 
        gs_Dbg.PrintLn("Scale   :%-25s %10.4f %10.4f", m_Base.m_Defs[i]->m_Tag, m_Base.m_Data[i], m_Base.m_Defs[i]->m_Scale );
        }
      };
    ~CLimn_ModelData_Access()
      {
      for (int i=0; i<m_Base.m_Data.GetCount(); i++)
        {
        m_Base.m_Data[i] /= m_Base.m_Defs[i]->m_Scale; 
        gs_Dbg.PrintLn("UnScale :%-25s %10.4f %10.4f", m_Base.m_Defs[i]->m_Tag, m_Base.m_Data[i], m_Base.m_Defs[i]->m_Scale );
        }
      };
    operator double*() { return m_Base.m_pData; }
  public:
    CLimn_ModelData_Base & m_Base;
  };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///			Calculation functions for Limn Diamond Wizard models
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					MeanEfficiencyPartitionFraction
///						Calculates a mean partition fraction for a size interval by linear interpolation over 29 sub intervals
double MeanEfficiencyPartitionFraction(double Rf, double Alpha, double D50c, double intervalTopSize, double intervalBottomSize) ;
//
//
///					MeanTrompPartitionFraction
///						Calculates a mean partition fraction for a density interval by linear interpolation over 29 sub intervals
double MeanTrompPartitionFraction(double EpD, double Rho50D, double intervalTopSG, double intervalBottomSG) ;
//
//-------------------------------------------------------------------------------------------------------
//
///     Function to calculate the transformation matrix X ( p = X.f )
///     according to the Whiten Crusher Model described in
///     the JKMRC monograph:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p138ff
///
void WhitenCrusherTransformationMatrix(const LowerTriangularMatrix& BreakageDistributionFunction, 
                                       const DiagonalMatrix& ClassificationFunction, 
                                       Matrix& X) ;
//
///
///     Function to calculate the Classification Function
///     for the Whiten Crusher Model.
///     Refer:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p141
///
void WhitenCrusherClassificationFunction(int nSizes,
                                         double* Size, 
                                         double K1, 
                                         double K2, 
                                         double K3,
                                         DiagonalMatrix& C) ;
///
///     Function to calculate the value of the breakage distribution
///     function given T10 and a table ot T10 versus Tn
///     Refer:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p143
///
///     Uses spline interpolation
///
void WhitenCrusherBreakageFunction(int nSizes, 
                                   double* Size,
                                   double requiredT10,
                                   const Matrix& T10_V_Tn,
                                   LowerTriangularMatrix& B) ;



///
///     Function to perform the redistribution of densimetrics to match a feed stream
///     (this is a kludge to account for the fact that densimetrics are measured at
//       feed to the DMS. It also simulates liberation)
///
void RedistributeOreDensimetricToFeedDistribution(CDiamondWizardConfiguration* thisConfig,
                                                  double oreFlow,
                                                  Matrix& product,
                                                  ColumnVector& meanSizeDistribution,
                                                  ColumnVector& meanSGDistribution) ;

//-------------------------------------------------------------------------------------------------------
//
