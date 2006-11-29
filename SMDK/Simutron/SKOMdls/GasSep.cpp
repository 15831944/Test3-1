//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __GASSEP_CPP
#include "gassep.h"
//#pragma optimize("", off)
#include "iso646.h"

//---------------------------------------------------------------------------

class CEqnData
  {
  public:
    CEqnData()
      {
      m_MMFeed       = 6;
      m_PFeed        = 350;
      m_PRich        = 101.35;
      m_PStrip       = 101.35;
      m_PRecycle     = 101.35;
      m_PRatio       = 3.5;
      m_PMiddle      = 150;
      m_QmFeedEst    = 3.0e-3;
      m_QmRichEst    = 1.0e-3;
      m_QmStripEst   = 1.0e-3;
      m_QmRecycleEst = 1.0e-3;             
      m_QmFeedRqd    = 3.0e-3;
      m_QmRichRqd    = 1.0e-3;
      m_QmStripRqd   = 1.0e-3;
      m_QmRecycleRqd = 1.0e-3;             
      //m_MlFracFeed[4];
      };

  public:
    double    m_MMFeed;
    double    m_PFeed;
    double    m_PRich;
    double    m_PStrip;
    double    m_PRecycle;
    double    m_PRatio;
    double    m_PMiddle;
    double    m_QmFeedEst;
    double    m_QmRichEst;
    double    m_QmStripEst;
    double    m_QmRecycleEst;             
    double    m_QmFeedRqd;
    double    m_QmRichRqd;
    double    m_QmStripRqd;
    double    m_QmRecycleRqd;             
    double    m_MlFracFeed[4];
  };


class CMassFlwEqn   // MassFlw
  {
  public:
    CMassFlwEqn();
    void BuildDataFields(LPCTSTR Tag, MDataDefn & DD);
    double Evaluate(int i, CEqnData & ED);
  public:
    enum          eEqns { Name0, Name1 }; 
    eEqns         m_eEqn;
    double        m_A;
    double        m_B;
    double        m_C;
    double        m_d;
    //double        m_PRatio;
  };

//---------------------------------------------------------------------------

class CBetaEqn   // Beta
  {
  public:
    CBetaEqn();
    void BuildDataFields(LPCTSTR Tag, MDataDefn & DD);
    double Evaluate(int i, CEqnData & ED);

  protected:
    enum          eEqns { Name0, Name1, Emp2 }; 
    eEqns         m_eEqn;
    double        m_A;
    double        m_B;
    double		    m_C;	
    double        m_BetaTemp[3];
  };

//---------------------------------------------------------------------------

class CLambdaEqn // Lambda
  {
  public:
    CLambdaEqn();
    void BuildDataFields(LPCTSTR Tag, MDataDefn & DD);
    double Evaluate(int i, CEqnData & ED);

  public:
    enum        eEqns { Name0, Emp1 }; 
    eEqns       m_eEqn;
    double      m_A;
    double		  m_B;
    double		  m_C;
    double		  m_LambdaTemp[3];
  };

//---------------------------------------------------------------------------

class CThetaEqn   // Theta
  {
  public:
    CThetaEqn();
    void BuildDataFields(LPCTSTR Tag, MDataDefn & DD);
    double Evaluate(int i, CEqnData & ED);
  public:
    enum          eEqns { Name0, Name1 }; 
    eEqns         m_eEqn;
    double        m_A;
    double        m_B;
    double        m_C;
    double		  m_d;

    //double        m_PRatio;
  };

//---------------------------------------------------------------------------

class CGasSep : public MBaseMethod
  {
  public:
    CGasSep(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void Init();
    virtual void BuildDataFields();
    virtual bool ExchangeDataFields();   // virtual method for manipulating data
    virtual bool ValidateDataFields();  // virtual method for validating data
    virtual void StartStep();
    virtual bool EvalFlowEquations(eScdFlwEqnTasks Task, long IONo, long FE, long LnkNo); // iterations with each Dyn Iteration - Converge Pressure-Flow relationships
    virtual void EvalProducts();

    bool           m_bDataOK;
    bool		   m_KFeedIterate, m_KRichIterate, m_KRecycleIterate, m_KStripIterate, m_Iterate;

    static CArray<long, long&>     sm_GasId;
    static CArray<double, double&> sm_S;
    static CArray<double, double&> sm_Wt;

    double         m_dAbundFeed;
    double         m_dAbundRich;
    double         m_dAbundStrip;

    double         m_KFeed,     m_KFeedPrev;
    double         m_KRich,     m_KRichPrev;
    double         m_KRecycle,  m_KRecyclePrev;
    double         m_KStrip,    m_KStripPrev;
    double         m_Damping;

    CMassFlwEqn    m_MassFlwEqn;
    CBetaEqn       m_BetaEqn;
    CLambdaEqn     m_LambdaEqn;
    CThetaEqn      m_ThetaLightEqn;
    CThetaEqn      m_ThetaHeavyEqn;

    double         m_dMassFlw;
    double         m_dLambda;
    double         m_dTheta;
    double         m_dPhi;
    double         m_dAlpha;
    //double         m_dBeta;

    double         m_ThetaLight;
    double         m_ThetaHeavy;

    long           m_lEval;
    CEqnData       m_ED;

    CArray<double, double&> m_Beta;
    CArray<double, double&> m_Lambda;
    //CArray<double, double&> m_MassFlw;
    CArray<double, double&> m_Bterm;        // Intermediate in Beta calculation
    CArray<double, double&> m_Lterm;        // Intermediate in Lambda calculaton
    CArray<double, double&> m_Z;        // Calcd Mole Fraction In Feed         
    CArray<double, double&> m_Y;        // Calcd Mole Fraction In Lite Stream
    CArray<double, double&> m_X;        // Calcd Mole Fraction In Heavy Stream
    CArray<double, double&> m_R;        // Calcd Mole Fraction In Recycle Stream
    CArray<double, double&> m_RichFrac; // Measured Mass Fraction In Lite Stream
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

CMassFlwEqn::CMassFlwEqn()
  {
  m_eEqn=Name0;
  m_A=1;
  m_B=0;
  m_C=1.5;
  m_d = 1;
  }

void CMassFlwEqn::BuildDataFields(LPCTSTR Tag, MDataDefn & DD)
  {
  DD.StructBegin(Tag);
  static MDDValueLst DDBetaCalcs[]=
    {
      {Name0, "Const"},
      {Name1, "Surface"},
      {0}
    };

  DD.Long   ("",   "Equation",    (long*)&m_eEqn,   MF_Parameter|MF_SetOnChange, DDBetaCalcs);
  switch (m_eEqn)
    {
    case Name0:
      DD.Double ("",   "Const",       &m_C,             MF_Parameter);
      break;
    case Name1:
      DD.Double ("",   "A",           &m_A,             MF_Parameter);
      DD.Double ("",   "B",           &m_B,             MF_Parameter);
      DD.Double ("",   "C",           &m_C,             MF_Parameter);
      DD.Double ("",   "d",           &m_d,             MF_Parameter);
      break;
    }
  DD.StructEnd();
  }

double CMassFlwEqn::Evaluate(int i, CEqnData & ED)
  {
  switch (m_eEqn)
    {
    case Name0:
      {
      return m_C;
      }
    case Name1:
      {
      return Range(0.000001,(m_A*ED.m_PFeed/100 + m_B*ED.m_PRatio + m_C*ED.m_MMFeed + m_d)/1000,100.0); 
      }
    }
  return dNAN;
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

CBetaEqn::CBetaEqn()
  {
  m_eEqn=Name0;
  m_A=0.85;
  m_B=0.3;
  m_C=1;
  }

void CBetaEqn::BuildDataFields(LPCTSTR Tag, MDataDefn & DD)
  {
  DD.StructBegin(Tag);
  static MDDValueLst DDBetaCalcs[]=
    {
      {CBetaEqn::Name0, "Constant"},
      {CBetaEqn::Name1, "Pi_Po_Ratio"},
      {CBetaEqn::Emp2,  "Empirical"},
      {0}
    };

  DD.Long   ("",   "Equation",    (long*)&m_eEqn,    MF_Parameter|MF_SetOnChange, DDBetaCalcs);
  switch (m_eEqn)
    {
    case Name0:
      {
      DD.Double ("",   "Beta_gas",       &m_A,     MF_Parameter);
      DD.Double ("",   "Beta_iso_1",     &m_B,     MF_Parameter);
      DD.Double ("",   "Beta_iso_2",     &m_C,     MF_Parameter);
      break;
      }
    case Name1:
      {
      DD.Double ("",   "Exponent",    &m_A,       MF_Parameter);
      DD.Double ("",   "Multiplier",  &m_B,       MF_Parameter);
      break;
      }
    case Emp2:
      break;
    }
  DD.StructEnd();
  }

double CBetaEqn::Evaluate(int i, CEqnData & ED)
  {
  switch (m_eEqn)
    {
    case Name0: 
      {
      switch (i)
        {
        case 0:
          {
          return m_A;
          }
        case 1:
          {
          return m_B;
          }
        case 2:
          {
          return m_C;
          }
        }
      }
    case Name1:
      {
      double A=Pow((CGasSep::sm_Wt[i+1]/CGasSep::sm_S[i+1]) - (CGasSep::sm_Wt[i]/CGasSep::sm_S[i]), m_A);
      double B= Pow((CGasSep::sm_S[i+1]+CGasSep::sm_S[i])/2.0, m_A);
      return Pow(ED.m_PRatio,(( A * B * m_B) / ED.m_MMFeed));
      }
    case Emp2:
      {
      switch (i)
        {
        case 0:
          {
          double BgasExp1 = 3.425815;
          double BgasExp = BgasExp1/ED.m_MMFeed;
          m_BetaTemp[0]=Pow(ED.m_PRatio, BgasExp);
          return m_BetaTemp[i];
          }
        case 1:
          {
          double BisoExp = log(m_BetaTemp[0])*0.109098;
          m_BetaTemp[1]= exp(BisoExp);
          return m_BetaTemp[1];
          }
        case 2:
          {
          m_BetaTemp[2] = m_BetaTemp[1];
          return m_BetaTemp[2];
          }
        }
      }
    }
  return dNAN;
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

CLambdaEqn::CLambdaEqn()
  {
  m_eEqn=Name0;
  m_A=1;
  m_B=1;
  m_C=1;

  }

void CLambdaEqn::BuildDataFields(LPCTSTR Tag, MDataDefn & DD)
  {
  DD.StructBegin(Tag);
  static MDDValueLst DDBetaCalcs[]=
    {
      {Name0, "Const"},
      {Emp1, "Empirical"},
      {0}
    };

  DD.Long   ("",   "Equation",    (long*)&m_eEqn,   MF_Parameter|MF_SetOnChange, DDBetaCalcs);
  switch (m_eEqn)
    {
    case Name0:
      {
      DD.Double ("",   "Lamda_gas",       &m_A,             MF_Parameter);
      DD.Double ("",   "Lamda_iso_1",     &m_B,             MF_Parameter);
      DD.Double ("",   "Lamda_iso_2",     &m_C,             MF_Parameter);
      break;
      }
    case Emp1:
      break;
    }
  DD.StructEnd();
  }

double CLambdaEqn::Evaluate(int i, CEqnData & ED)
  {
  switch (m_eEqn)
    {
    case Name0:
      {
      switch (i)
        {
        case 0:
          {
          return m_A;
          }
        case 1:
          {
          return m_B;
          }
        case 2:
          {
          return m_C;
          }
        }
      }

    case Emp1:
      {
      switch (i)
        {
        case 0:
          {
          double LgasExp1 = -.733835243;
          double LgasExp = LgasExp1/ED.m_MMFeed;
          m_LambdaTemp[0]=pow(ED.m_PRatio,LgasExp);
          return m_LambdaTemp[0];
          }
        case 1:
          {
          double LisoExp = log(m_LambdaTemp[0])*0.109098;
          m_LambdaTemp[1]= exp(LisoExp);
          return m_LambdaTemp[1];
          }
        case 2:
          {
          m_LambdaTemp[2]=m_LambdaTemp[1];
          return m_LambdaTemp[2];
          }
        }
      }
    }
  return dNAN;
  } 

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

CThetaEqn::CThetaEqn()
  {
  m_eEqn=Name0;
  m_A=1;
  m_B=1;
  m_C=0.1;
  m_d =1;
  }

void CThetaEqn::BuildDataFields(LPCTSTR Tag, MDataDefn & DD)
  {
  DD.StructBegin(Tag);
  static MDDValueLst DDBetaCalcs[]=
    {
      {Name0, "Const"},
      {Name1, "Surface"},
      {0}
    };

  DD.Long   ("",   "Equation",    (long*)&m_eEqn,   MF_Parameter|MF_SetOnChange, DDBetaCalcs);
  switch (m_eEqn)
    {
    case Name0:
      DD.Double ("",   "Const",       &m_C,             MF_Parameter);
      break;
    case Name1:
      DD.Double ("",   "A",           &m_A,             MF_Parameter);
      DD.Double ("",   "B",           &m_B,             MF_Parameter);
      DD.Double ("",   "C",           &m_C,             MF_Parameter);
      DD.Double ("",   "d",           &m_d,             MF_Parameter);
      break;
    }
  DD.StructEnd();
  }

double CThetaEqn::Evaluate(int i, CEqnData & ED)
  {
  switch (m_eEqn) 
    {
    case Name0:
      {
      return m_C;
      }
    case Name1: 
      {
      return Range(0.005,(m_A*ED.m_PFeed/100+m_B*ED.m_PRatio+m_C*ED.m_MMFeed + m_d)/100,0.5);
      }
    }
  return dNAN;
  }

//====================================================================================
//====================================================================================
//====================================================================================
//====================================================================================
//====================================================================================

const byte idFeed     = 0; 
const byte idRich     = 1;
const byte idStrip    = 2;
const byte idRecycle  = 3;

static MInOutDefStruct s_IODefs[] =
  {                                               
  //  Desc;           Name; Id;       Rqd; Max; CnId, FracHgt;  Options;
    {"Feed Gas",      "Z" , idFeed,     1,   1,    0,    1.0f,    MIO_In |MIO_Material},
    {"Enriched Gas",  "Y" , idRich,     1,   1,    0,    1.0f,    MIO_Out|MIO_Material},
    {"Stripped Gas",  "X" , idStrip,    1,   1,    0,    1.0f,    MIO_Out|MIO_Material},
    {"Recycle Gas",   "R" , idRecycle,  1,   1,    0,    1.0f,    MIO_Out|MIO_Material}, 
    {NULL}
  };

static double Drw_GasSep[] = { MDrw_Poly, -6,10, 10,0, -6,-10, -6,10, MDrw_End };

//---------------------------------------------------------------------------

CArray<long, long&> CGasSep::sm_GasId;
CArray<double, double&> CGasSep::sm_S;
CArray<double, double&> CGasSep::sm_Wt;

DEFINE_TRANSFER_UNIT(CGasSep, "GasSep", DLL_GroupName)
void CGasSep_UnitDef::GetOptions()
  {
  SetDefaultTag("GS");
  SetTreeDescription("SKO:GasSep");
  SetDrawing("SizeSeparation", Drw_GasSep);
  SetDescription("SKO Gas Seperator");
  SetModelSolveMode(MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_Separation);
  //SetModelLicense(MLicense_User);
  };

//---------------------------------------------------------------------------

CGasSep::CGasSep(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  m_bDataOK=false;

  //m_lSpCarrier=-1;
  sm_GasId.SetSize(1);
  sm_GasId[0]=-1;

  m_KFeed=50000;
  m_KRich=50000;
  m_KStrip=50000;
  m_KRecycle=50000;

  m_KFeedPrev=0;
  m_KRichPrev=0;
  m_KRecyclePrev=0;
  m_KStripPrev=0;
  m_Damping=0.2;

  m_dAbundFeed=0;
  m_dAbundRich=0;
  m_dAbundStrip=0;

  m_dTheta=0.5;
  m_dPhi=0.5;
  m_dAlpha=0;
  //m_dBeta=1;
  //m_dMassFlw=0;
  m_dLambda=0;


  m_ThetaLight=0;
  m_ThetaHeavy=0;

  m_lEval=0;
  }

//---------------------------------------------------------------------------

void CGasSep::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

const long idDX_GasCount = 1;
const long idDX_NdPress  = 2;

void CGasSep::BuildDataFields()
  {

  //DD.Visibility();

  DD.Text   ("Sep Gases");
  DD.Long   ("",             "NoGasses",  idDX_GasCount, MF_ParamStopped|MF_SetOnChange);
  //DD.Long   ("",             "SpCarrier",  DC_,     "",   &m_lSpCarrier,     this, MF_Parameter, SDB.DDSpListDash());
  CString S;
  for (int i=0; i<sm_GasId.GetSize(); i++)
    {
    S.Format("Gas%i",i);
    DD.Long ("",            S,   &sm_GasId[i],    MF_Parameter|MF_SetOnChange, gs_MVDefn.DDSpListDash());
    }

  //DD.Page   ("Eqns");
  DD.Text("");
  DD.StructBegin("Eqns");
  m_MassFlwEqn.BuildDataFields("MassFlw", DD);
  m_BetaEqn.BuildDataFields("Beta", DD);
  m_LambdaEqn.BuildDataFields("Lamda", DD);
  m_ThetaLightEqn.BuildDataFields("ThetaLight", DD);
  m_ThetaHeavyEqn.BuildDataFields("ThetaHeavy", DD);
  DD.StructEnd();
  DD.Page   ("Operating");
  DD.Text   ("Flow Configuration");
  DD.Double ("",            "KFeed",        &m_KFeed,         MF_Parameter);//MF_Parameter);
  DD.Double ("",            "KRich",        &m_KRich,         MF_Parameter);
  DD.Double ("",            "KStrip",       &m_KStrip,        MF_Parameter);
  DD.Double ("",            "KRecycle",     &m_KRecycle,      MF_Parameter);
  DD.Double ("",            "Damping",      &m_Damping,       MF_Parameter);
  DD.Bool   ("",            "Iterate",      &m_Iterate,       MF_Parameter);
  DD.Text   ("");

  DD.Double ("",            "PFeed",        &m_ED.m_PFeed   ,              0, MC_P("kPag"));
  DD.Double ("",            "PRich",        &m_ED.m_PRich   ,              0, MC_P("kPag"));
  DD.Double ("",            "PStrip",       &m_ED.m_PStrip  ,              0, MC_P("kPag"));
  DD.Double ("",            "PRecycle",     &m_ED.m_PRecycle,              0, MC_P("kPag"));

  DD.Double ("",            "NdPress",      idDX_NdPress,              0, MC_P("kPag"));
  DD.Text   ("");
  DD.Double ("",            "FeedQmRqd",    &m_ED.m_QmFeedRqd,         0, MC_Qm("kg/s"));
  DD.Double ("",            "FeedQmEst",    &m_ED.m_QmFeedEst,         0, MC_Qm("kg/s"));
  DD.Text   ("");
  DD.Double ("",            "RichQmRqd",    &m_ED.m_QmRichRqd,         0, MC_Qm("kg/s"));
  DD.Double ("",            "RichQmEst",    &m_ED.m_QmRichEst,         0, MC_Qm("kg/s"));
  DD.Text   ("");
  DD.Double ("",            "StripQmRqd",   &m_ED.m_QmStripRqd,        0, MC_Qm("kg/s"));
  DD.Double ("",            "StripQmEst",   &m_ED.m_QmStripEst,        0, MC_Qm("kg/s"));
  DD.Text   ("");
  DD.Double ("",            "RecycleQmRqd", &m_ED.m_QmRecycleRqd,      0, MC_Qm("kg/s"));
  DD.Double ("",            "RecycleQmEst", &m_ED.m_QmRecycleEst,      0, MC_Qm("kg/s"));
  DD.Text   ("");
  DD.Text   ("Separation");
  DD.StructBegin("Beta");
  for (int i=0; i<m_Beta.GetSize(); i++)
    {
    S.Format("%i_%i",i,i+1);
    DD.Double("",             S,           &m_Beta[i],         0);
    }
  DD.StructEnd();
  DD.StructBegin("Lambda");
  for (int i=0; i<m_Lambda.GetSize(); i++)
    {
    S.Format("%i_%i",i,i+1);
    DD.Double("",             S,           &m_Lambda[i],         0);
    }
  DD.StructEnd();


  DD.Double ("",            "ThetaLight",   &m_ThetaLight,    0, MC_Frac("%"));
  DD.Double ("",            "ThetaHeavy",   &m_ThetaHeavy,    0, MC_Frac("%"));

  //enums          { BE_Name0, BE_Name1 }; // KCG to change

  //  DD.Double ("IsoCut",       "Theta",   DC_Frac, "%",  &m_dTheta,      this, MF_Parameter);
  //  DD.Double ("",             "Beta",    DC_,     "",   &m_dBeta,       this, MF_Parameter);
  //  DD.Double ("",             "Alpha",   DC_,     "",   &m_dAlpha,      this, 0);
  //  DD.Double ("",             "MassFlw",   DC_,     "",   &m_dMassFlw,      this, 0);
  //  DD.Text   ("Carrier");
  //  DD.Double ("CarrierCut",   "Phi",     DC_Frac, "%",  &m_dPhi,        this, MF_Parameter);

  DD.Text   ("");
  //DD.Text   ("Enriched Fractions");
  DD.StructBegin("RichFrac");
  for (int i=1; i<m_RichFrac.GetSize(); i++)
    if (i<sm_GasId.GetCount() && sm_GasId[i]>=0)
      DD.Double ("", gs_MVDefn[sm_GasId[i]].Symbol(),   &m_RichFrac[i],    0, MC_Frac("%"));
  DD.StructEnd();

  //DD.Text   ("");
  //DD.Double ("FeedAbundance",  "",      DC_,     "",   &m_dAbundFeed,  this, 0);
  //DD.Double ("RichAbundance",  "",      DC_,     "",   &m_dAbundRich,  this, 0);
  //DD.Double ("StripAbundance", "",      DC_,     "",   &m_dAbundStrip, this, 0);

  DD.Text   ("");
  //BuildDataFieldsShowIOs(DD);
  DD.Page   ("Splits");

  DD.StructBegin("Feed Ml_frac");
  for (int i=0; i<m_Z.GetSize(); i++)
    {
    S.Format("%i_%i",i,i+1);
    DD.Double("",             S,           &m_Z[i],         0);
    }
  DD.StructEnd();

  DD.StructBegin("Rich_frac");
  for (int i=0; i<m_Y.GetSize(); i++)
    {
    S.Format("%i_%i",i,i+1);
    DD.Double("",             S,           &m_Y[i],         0);
    }
  DD.StructEnd();

  DD.StructBegin("Strip Ml_frac");
  for (int i=0; i<m_X.GetSize(); i++)
    {
    S.Format("%i_%i",i,i+1);
    DD.Double("",             S,           &m_X[i],         0);
    }
  DD.StructEnd();

  DD.StructBegin("Recyc Ml_frac");
  for (int i=0; i<m_R.GetSize(); i++)
    {
    S.Format("%i_%i",i,i+1);
    DD.Double("",             S,           &m_R[i],         0);
    }
  DD.StructEnd();




  }

//---------------------------------------------------------------------------

bool CGasSep::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_GasCount: 
      {
      if (DX.HasReqdValue)
        {
        int newn=Max(2L,DX.Long);
        int n=sm_GasId.GetSize();
        sm_GasId.SetSize(newn);
        sm_S.SetSize(newn);
        sm_Wt.SetSize(newn);
        m_Beta.SetSize(newn-1);
        m_Lambda.SetSize(newn-1);
        m_Z.SetSize(newn);
        m_Y.SetSize(newn);
        m_X.SetSize(newn);
        m_R.SetSize(newn);
        m_Bterm.SetSize(newn);
        m_Lterm.SetSize(newn);
        m_RichFrac.SetSize(newn);
        for (int i=n; i<sm_GasId.GetCount(); i++)
          {
          sm_GasId[i]   = -1;
          sm_S[i]       = 1;
          sm_Wt[i]      = 1;
          m_RichFrac[i] = 0;
          }

        for (int i=n; i<m_Beta.GetCount(); i++)
          {
          m_Beta[i]=1;
          m_Bterm[i]=1;
          m_Lterm[i]=1;
          m_Lambda[i]=1;
          }
        }
      DX.Long=sm_GasId.GetSize();
      return true;
      }
    case idDX_NdPress:
      if (Joins.Count>0)
        DX.Double=Joins[0].P;
      else
        DX.Double=NAN;
      return true;
    }
  return MBaseMethod::ExchangeDataFields();
  };

//---------------------------------------------------------------------------

bool CGasSep::ValidateDataFields()
  {
  bool OK=true;

  m_Damping=Range(0.0, m_Damping, 0.9);

  m_bDataOK = true;
  for (int i=0; i<sm_GasId.GetCount(); i++)
    m_bDataOK = m_bDataOK && (sm_GasId[i]>=0 && sm_GasId[i]<gs_MVDefn.Count() && gs_MVDefn[sm_GasId[i]].IsGas());

  if (m_bDataOK)
    {
    int i=1;
    while (i<sm_GasId.GetSize())
      {
      if (gs_MVDefn[sm_GasId[i-1]].MolecularWt() > gs_MVDefn[sm_GasId[i]].MolecularWt())
        {
        Exchange(sm_GasId[i-1], sm_GasId[i]);
        if (i>1)
          i--;
        }
      else
        i++;
      }

    for (int s=0; s<sm_GasId.GetSize(); s++)
      {
      sm_S[s]  = gs_MVDefn[sm_GasId[s]].MolecularDiam()*1.0e10;
      sm_Wt[s] = gs_MVDefn[sm_GasId[s]].MolecularWt();
      }
    }
  //m_dBeta   = DD.ValidateRange("Beta",  0.01, m_dBeta,  100.0);
  m_dTheta  = DV.ValidateRange("Theta", 0.01, m_dTheta, 0.99);
  m_dPhi    = DV.ValidateRange("Phi",   0.01, m_dPhi,   0.99);


  if (m_bDataOK)
    Log.ClearCondition(1);
  else
    Log.SetCondition(1, MMsg_Warning, "Incomplete Configuration");
  return OK;
  };

//---------------------------------------------------------------------------

void CGasSep::StartStep()
  {
  try 
    {
    m_KFeedPrev         = m_KFeed;
    m_KRichPrev         = m_KRich;
    m_KStripPrev        = m_KStrip;
    m_KRecyclePrev      = m_KRecycle;
    m_KFeedIterate	  = false;
    m_KRichIterate	  = false;
    m_KRecycleIterate	= false;
    m_KStripIterate	  = false;


    m_ED.m_PFeed        = FlwIOs[FlwIOs.First[idFeed ]].IOPFlange;  
    m_ED.m_PRich        = FlwIOs[FlwIOs.First[idRich ]].IOPFlange; 
    m_ED.m_PStrip       = FlwIOs[FlwIOs.First[idStrip]].IOPFlange; 
    m_ED.m_PRecycle     = FlwIOs[FlwIOs.First[idRecycle]].IOPFlange; 

    double POutMean     = GTZ(m_ED.m_PRich+m_ED.m_PStrip+m_ED.m_PRecycle)/3.0;
    m_ED.m_PMiddle      = 100*Sqrt(GTZ(m_ED.m_PFeed/POutMean));
    m_ED.m_PRatio       = Range(1.0,m_ED.m_PFeed/POutMean,6.0);

    m_ED.m_QmFeedRqd    = m_MassFlwEqn.Evaluate(0,m_ED);
    //m_ED.m_QmFeedRqd    = Range(3.0, m_ED.m_QmFeedRqd, 15.0);

    m_ThetaLight        = m_ThetaLightEqn.Evaluate(0, m_ED);
    m_ThetaHeavy        = m_ThetaHeavyEqn.Evaluate(1, m_ED);

    m_ED.m_QmRichRqd    = m_ThetaLight*m_ED.m_QmFeedRqd *(1-m_Z[0])/GTZ(1-m_Y[0]);
    //m_ED.m_QmRichRqd    = Range(0.1, m_ED.m_QmRichRqd, 5.0);

    m_ED.m_QmStripRqd   = m_ThetaHeavy*m_ED.m_QmFeedRqd *(1-m_Z[0])/GTZ(1-m_X[0]);
    //m_ED.m_QmStripRqd   = Range(0.1, m_ED.m_QmStripRqd, 5.0);

    m_ED.m_QmRecycleRqd = Range(0.0, (m_ED.m_QmFeedRqd-m_ED.m_QmRichRqd-m_ED.m_QmStripRqd), m_ED.m_QmFeedRqd);
    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }

  }

//---------------------------------------------------------------------------

bool CGasSep::EvalFlowEquations(eScdFlwEqnTasks Task, long IONo, long FE, long LnkNo)
  {
  try
    {
    switch (Task)
      {
      case FET_GetMode:
        {
        MFlowBlk FB = FlwIOs[IONo].FlowBlk;//(IONo, FE);
        FB.SetQmFree();
        return true;
        }
      case FET_CalcDP:
        {
        MFlowBlk FB = FlwIOs[IONo].FlowBlk;//(IONo, FE);
        FB.SetDerivsBad(false);
        FB.SetDPb(0.0, 0.0); // In is Positive

        const int HoldIterAll=false;

        switch (FlwIOs[IONo].Id)
          {
          case idFeed    : 
            if  (!HoldIterAll && (m_KFeedIterate or m_Iterate))
              {
              m_KFeed=m_Damping*m_KFeedPrev+(1.0-m_Damping)*fabs(m_ED.m_PFeed-m_ED.m_PMiddle)/GTZ(m_ED.m_QmFeedRqd);
              m_KFeed=Range(1e3, m_KFeed, 1e9);
              }
            m_KFeedIterate = false;
            break;

          case idRich    :
            if  (!HoldIterAll && (m_KRichIterate or m_Iterate))	
              {
              m_KRich=m_Damping*m_KRichPrev+(1.0-m_Damping)*fabs(m_ED.m_PRich-m_ED.m_PMiddle)/GTZ(m_ED.m_QmRichRqd);
              m_KRich=Range(1e3, m_KRich, 1e9);
              }
            m_KRichIterate = false;
            break;

          case idStrip   : 
            if  (!HoldIterAll && (m_KStripIterate or m_Iterate))	
              {
              m_KStrip=m_Damping*m_KStripPrev+(1.0-m_Damping)*fabs(m_ED.m_PStrip-m_ED.m_PMiddle)/GTZ(m_ED.m_QmStripRqd);
              m_KStrip=Range(1e3, m_KStrip, 1e9);
              }
            m_KStripIterate = false;
            break;

          case idRecycle :  
            if  (!HoldIterAll && (m_KRecycleIterate or m_Iterate))	
              {
              m_KRecycle=m_Damping*m_KRecyclePrev+(1.0-m_Damping)*fabs(m_ED.m_PRecycle-m_ED.m_PMiddle)/GTZ(m_ED.m_QmRecycleRqd);
              m_KRecycle=Range(1e3, m_KRecycle, 1e9);
              }
            m_KRecycleIterate = false;
            break;
          }

        double Q=GEZ(fabs(FB.QmMeas()));
        if (1)//Q>1.0e-12)
          {
          double DQ=Max(1.0e-8, 1.0e-2*Q);

          //Meth[iMethod]->Init(Q, QFeed);

          double Y[4]={m_KFeed, m_KRich,m_KStrip,m_KRecycle};


          double X=Y[FlwIOs[IONo].Id];


          double FlowLoss0=-FB.QmSign()*X*Q;//Meth[iMethod]->DP(Q, QFeed);
          double FlowLoss1=-FB.QmSign()*X*(Q+DQ);//Meth[iMethod]->DP(Q+DQ, QFeed);
          double DFlowLoss=(FlowLoss1-FlowLoss0)/DQ;

          FB.SetDPq(FlowLoss0, DFlowLoss);
          }
        else
          FB.SetDPq(0.0, 0.0);
        return true;
        }
      }
    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }

  return false;

  };

//---------------------------------------------------------------------------

void CGasSep::EvalProducts()
  {
  long xxx= JoinMask;
  try
    {
    if (0)
      {
      // Examples of Environment Variables
      double X1=StdP;
      double X2=StdT;
      double X3=AtmosPress();
      double X4=AmbientTemp();
      double X5=BaseElevation();
      double X6=WindSpeed(); 
      double X7=WindDirection();
      }

    const int ioFeed  = FlwIOs.First[idFeed ];
    const int ioRich  = FlwIOs.First[idRich ];
    const int ioStrip = FlwIOs.First[idStrip];
    const int ioRec   = FlwIOs.First[idRecycle];

    MFlow FlwFeed  = FlwIOs[ioFeed ];
    MFlow FlwRich  = FlwIOs[ioRich ];
    MFlow FlwStrip = FlwIOs[ioStrip];
    MFlow FlwRec   = FlwIOs[ioRec  ];

    if (m_bDataOK && FlwFeed.EstFlwIn>0 && 
      FlwStrip.EstFlwOut>0 && 
      FlwRich.EstFlwOut>0 && 
      FlwRec.EstFlwOut>0)
      {
      MStream QFeed;
      FlwIOs.AddMixtureIn_Id(QFeed, ioFeed);
      MStream & QRich  = FlwRich.Stream;
      MStream & QStrip = FlwStrip.Stream;
      MStream & QRecycle   = FlwRec.Stream;


      if (QFeed.MoleWt() > 1e-10)
        {
        m_ED.m_QmFeedEst  = FlwFeed.EstFlwIn;
        m_ED.m_QmRichEst  = FlwRich.EstFlwOut;
        m_ED.m_QmStripEst = FlwStrip.EstFlwOut;
        m_ED.m_QmRecycleEst   = FlwRec.EstFlwOut;             

        m_ED.m_PFeed      = FlwFeed.IOPFlange;
        m_ED.m_PRich      = FlwRich.IOPFlange;
        m_ED.m_PStrip     = FlwStrip.IOPFlange;
        m_ED.m_PRecycle   = FlwRec.IOPFlange;

        m_ED.m_MMFeed = GTZ(QFeed.MoleWt());

        m_ED.m_PRatio= Range(1.0,m_ED.m_PFeed/((m_ED.m_PRich+m_ED.m_PStrip+m_ED.m_PRecycle)/3),6.0);

        double TotMoles=QFeed.Moles();
        for (int i=0; i<m_Z.GetCount(); i++)
          {
          m_Z[i]=QFeed.Ml[sm_GasId[i]]/GTZ(TotMoles);
          m_ED.m_MlFracFeed[i]=m_Z[i];
          }

        //check for valid feed...

        const double QmFeed  = QFeed.MassFlow();
        const double QmRich  = QRich.MassFlow();
        const double QmStrip = QStrip.MassFlow();
        QRecycle.Copy(QFeed);
        QRich.ZeroMass();
        QStrip.ZeroMass();
        QRich.SelectModel(&QFeed, m_ED.m_QmRichEst);
        QStrip.SelectModel(&QFeed, m_ED.m_QmStripEst);


        int ij = m_Beta.GetCount();
        for (int i=0; i<ij; i++)
          {
          m_Beta[i]= m_BetaEqn.Evaluate(i, m_ED);
          m_Lambda[i]= m_LambdaEqn.Evaluate(i, m_ED);
          }
        m_ThetaLight=m_ThetaLightEqn.Evaluate(0, m_ED);
        m_ThetaHeavy=m_ThetaHeavyEqn.Evaluate(1, m_ED);

        //        double MMM = QFeed.VMole[sm_GasId[i]]/QFeed.Moles();      
        int Last=m_Bterm.GetCount()-1;
        for (int i=0; i<Last; i++)
          {
          m_Z[i]=QFeed.Ml[sm_GasId[i]]/GTZ(QFeed.Moles());
          m_Bterm[i]=QFeed.Ml[sm_GasId[i]]/GTZ(QFeed.Ml[sm_GasId[Last]]);
          m_Lterm[i]=QFeed.Ml[sm_GasId[i]]/GTZ(QFeed.Ml[sm_GasId[Last]]);
          for (int j=i; j<Last; j++)
            {
            m_Bterm[i]*=m_Beta[j];
            m_Lterm[i]*=m_Lambda[j];
            }
          }

        double SumBterm=0.0;
        double SumLterm=0.0;
        for (int i=0; i<Last; i++)
          {
          SumBterm+=m_Bterm[i];
          SumLterm+=m_Lterm[i];
          }

        m_Y[Last]=1.0/(1.0+SumBterm);
        m_X[Last]=1.0/(1.0+SumLterm);

        double yFrac = 0.0;
        double xFrac = 0.0;
        int ik = m_Y.GetCount()-1;
        for (int i=0; i<ik; i++)
          {
          m_Y[i]=m_Bterm[i]*m_Y[Last];
          m_X[i]=m_Lterm[i]*m_X[Last];
          yFrac += m_Y[i];
          xFrac += m_X[i];
          }
        m_Y[i]=m_Y[Last];
        yFrac = yFrac + m_Y[i];
        m_X[i]=m_X[Last];
        xFrac = xFrac + m_X[i];


        double Est_Rich_M[10];
        double Est_Strip_M[10];
        double Est_Recyc_M[10];
        double FeedM_Tot = 0.0;
        double Est_Crct = m_ED.m_QmFeedEst / GTZ(QmFeed); 
        for(int i=0; i< m_Y.GetCount(); i++)
          {
          double Feed_M = QFeed.M[sm_GasId[i]];
          double EstTotal = m_ED.m_QmStripEst + m_ED.m_QmRichEst + m_ED.m_QmRecycleEst;
          if (fabs(EstTotal)>0.0)
            {

            Est_Rich_M[i] = Feed_M * m_ED.m_QmRichEst / EstTotal;
            Est_Strip_M[i]= Feed_M * m_ED.m_QmStripEst / EstTotal;
            Est_Recyc_M[i]= Feed_M * m_ED.m_QmRecycleEst / EstTotal;
            FeedM_Tot += Feed_M - Est_Rich_M[i] - Est_Strip_M[i] - Est_Recyc_M[i];
            }
          else
            {
            Est_Rich_M[i] = 0.0;
            Est_Strip_M[i]= 0.0;
            Est_Recyc_M[i]= 0.0;
            }
          }



        //calculate mass flows
        //double Wlight, Wheavy, Wrec;
        //Wlight = m_ThetaLight*QmFeed*(1-m_Z[0])/(1-m_Y[0]);
        //Wheavy = m_ThetaHeavy*QmFeed*(1-m_Z[0])/(1-m_X[0]);
        //Wrec = QmFeed - Wlight - Wheavy;
        int il = m_R.GetCount();
        double rfrac = 0.0;
        for (int i=0; i<il; i++)
          {
//          m_R[i]=(m_ED.m_QmFeedRqd*m_Z[i]-m_ED.m_QmRichRqd*m_Y[i]-m_ED.m_QmStripRqd*m_Z[i])/GTZ(m_ED.m_QmRecycleRqd);
          m_R[i]=(m_ED.m_QmFeedEst*m_Z[i]-m_ED.m_QmRichEst*m_Y[i]-m_ED.m_QmStripEst*m_Z[i])/GTZ(m_ED.m_QmRecycleEst);
          rfrac += m_R[i];
          }

        double MYMoles = 0.0;
        double MXMoles = 0.0;
        double MRMoles = 0.0;
        for (int i=0; i<=ik; i++)
          {
          MYMoles += sm_Wt[i]* m_Y[i];
          MXMoles += sm_Wt[i]* m_X[i];
          MRMoles += sm_Wt[i]* m_R[i];
//          MYMass += sm_Wt[i]* m_Y[i] * sm_Wt[i];
//          MXMass += sm_Wt[i]* m_X[i] * sm_Wt[i];
//          MYMass += sm_Wt[i]* m_Y[i] * sm_Wt[i];
          }

        //m_ED.m_QmRecycleEst=Range(0.0, m_ED.m_QmRecycleEst, QFeed.MassFlow()); // ensure mass balance
        //QRecycle.SetM(QFeed,MP_All,m_ED.m_QmRecycleEst);

        //dbgpln("--------------------------------");
        double SilaneFeed=0.0;
        double SilaneRich=0.0;
        double SilaneStrip=0.0;
        double SpecUsed;
        double SpltDiff;
        for (int i=0; i<=ik; i++)
          {
          double SpecDiff=0.0;
          double EstTotal = m_ED.m_QmStripEst + m_ED.m_QmRichEst + m_ED.m_QmRecycleEst;
          double SpcInFeed   = QFeed.M[sm_GasId[i]];
//          double SpcInFeed   = QFeed.M[sm_GasId[i]]* m_ED.m_QmFeedEst / GTZ(QmFeed);
          double Spc2Strip   = m_ED.m_QmStripEst*m_X[i]*sm_Wt[i]/MXMoles;
          double Spc2Rich    = m_ED.m_QmRichEst*m_Y[i]*sm_Wt[i]/MYMoles;
          double Spc2Recycle = m_ED.m_QmRecycleEst*m_R[i]*sm_Wt[i]/MRMoles;//GEZ(SpcInFeed-Spc2Rich-Spc2Strip);
Spc2Recycle = SpcInFeed-Spc2Rich-Spc2Strip;
          if (Spc2Recycle < 0.0)
            {
            Spc2Recycle = 0.0;
            }
          SpltDiff = m_X[i] + m_Y[i] + m_R[i];

          SpecDiff = Spc2Strip + Spc2Rich + Spc2Recycle - SpcInFeed;

          Spc2Strip -= GTZ(SpecDiff * m_ED.m_QmStripEst / GTZ(EstTotal));

          Spc2Rich -= GTZ(SpecDiff * m_ED.m_QmRichEst / GTZ(EstTotal)); 

          Spc2Recycle -= GTZ(SpecDiff * m_ED.m_QmRecycleEst / GTZ(EstTotal)); 

          SpecDiff = Spc2Strip + Spc2Rich + Spc2Recycle - SpcInFeed;
          if (Spc2Recycle<0 || Spc2Rich<0 || Spc2Strip<0)
            { int xxx=0; };

          //dbgpln(" %i %20s %20s %20s %20s %20s", i, DbgFltString(SpcInFeed), DbgFltString(Spc2Rec), DbgFltString(Spc2Rich), DbgFltString(Spc2Strip), DbgFltString(Range(0.0, Spc2Rich, SpcInFeed)));
          Spc2Rich=Range(0.0, Spc2Rich, SpcInFeed);
           QRich.M[sm_GasId[i]]=Spc2Rich;
           QStrip.M[sm_GasId[i]]=Spc2Strip;
           QRecycle.M[sm_GasId[i]]=Spc2Recycle * Est_Crct;

//           QRich.M[sm_GasId[i]]    = Est_Rich_M[i] * Est_Crct;
//           QStrip.M[sm_GasId[i]]   = Est_Strip_M[i] * Est_Crct;
//           QRecycle.M[sm_GasId[i]] = Est_Recyc_M[i] * Est_Crct;



          if (i>0) // ? Skip the carrier
            {
            SilaneFeed  += SpcInFeed;
            SilaneRich  += Spc2Rich;
            SilaneStrip += Spc2Strip;
            }
          }

        QRich.SetTP(QFeed.T, QFeed.P);
        QStrip.SetTP(QFeed.T, QFeed.P);
        QRecycle.SetTP(QFeed.T, QFeed.P);

        double TotSepMass=0;
        for (int i=1; i<=ik; i++) // ? Skip the carrier
          TotSepMass+=QRich.M[sm_GasId[i]];

        m_RichFrac[0]=0.0;
        for (int i=1; i<=ik; i++) // ? Skip the carrier
          m_RichFrac[i]=QRich.M[sm_GasId[i]]/GTZ(TotSepMass);

        }
      else
        {
        MBaseMethod::EvalProducts();
        return;
        }
      }
    else
      {
      MBaseMethod::EvalProducts();
      }
    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//===========================================================================
//
//                                 
//
//==========================================================================
