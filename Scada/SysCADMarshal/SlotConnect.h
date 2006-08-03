// SlotConnect.h: interface for the CSlotConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLOTCONNECT_H__9255AA2C_F19E_4EA5_A5D8_D4ADEEF318E2__INCLUDED_)
#define AFX_SLOTCONNECT_H__9255AA2C_F19E_4EA5_A5D8_D4ADEEF318E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "slotcommon.h"
#include "codeblock.h"

// =======================================================================
//
//
//
// =======================================================================

class CSlot;
class CLink;
class CDevice;

// =======================================================================
//
//
//
// =======================================================================

struct CSlotConnPrfPt
  {
  float X,Y;
  };

class CSlotConnPrf 
  {
  protected:
    CString      m_sTag; //FileName etc
    enum { eNULL, eABS, eSCL, eSCLPERC, eCONTRONIC } m_Mode;
    bool m_bYReversed;
    CArray <CSlotConnPrfPt, CSlotConnPrfPt&> m_Points;

  public:
    CSlotConnPrf ();
    long Parse(LPCSTR File);

    double X2Y(double X);
    double Y2X(double Y);
     
    LPCSTR ErrorString(long RetCode);
  };

class CSlotConnOp_Base
  {
  protected:

  public:
    virtual double Exec(double d, double DeltaTime)=0;
    virtual double DoConditioning(double d, double DeltaTime) { return d; };
    virtual bool   IsConditioning() { return false; };
  };

class CSlotConnOp_Rev : public CSlotConnOp_Base
  {
  protected:
    double m_dOffVal;  
  public:
    CSlotConnOp_Rev(double i_dOffVal)
      {
      m_dOffVal=i_dOffVal;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return m_dOffVal-d;
      };
  };

class CSlotConnOp_Not : public CSlotConnOp_Base
  {
  public:
    CSlotConnOp_Not()
      {
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return d == 0.0 ? 1.0 : 0.0;
      };
  };

class CSlotConnOp_Abs : public CSlotConnOp_Base
  {
  protected:
  
  public:
    CSlotConnOp_Abs()
      {
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return fabs(d);
      };
  };

class CSlotConnOp_Cmp : public CSlotConnOp_Base
  {
  protected:
    int m_iCmpTest;
    double m_dCmpVal;  
  public:
    CSlotConnOp_Cmp(int iOp, double i_dCmpVal)
      {
      m_iCmpTest=iOp;
      m_dCmpVal=i_dCmpVal;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      switch (m_iCmpTest)
        {
        case -2:return d<m_dCmpVal;
        case -1:return d<=m_dCmpVal;
        case  0:return d==m_dCmpVal;
        case  1:return d>=m_dCmpVal;
        case  2:return d>m_dCmpVal;
        default:return false;
        }
      };
  };

class CSlotConnOp_Range : public CSlotConnOp_Base
  {
  protected:
    double m_dMin;    
    double m_dMax;  

  public:
    CSlotConnOp_Range(double i_dMin, double i_dMax)
      {
      m_dMin=i_dMin;
      m_dMax=i_dMax;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      d=Range(m_dMin, d, m_dMax);
      return d;
      };
  };

class CSlotConnOp_Sqr : public CSlotConnOp_Base
  {
  protected:
    double m_dMult;   
    double m_dMin1;  
    double m_dMin2;  
    double m_dMax1;  
    double m_dMax2;  

  public:
    CSlotConnOp_Sqr(double i_dMult, double i_dMin1, double i_dMax1, double i_dMin2, double i_dMax2)
      {
      m_dMult=i_dMult;
      m_dMin1=i_dMin1;
      m_dMax1=i_dMax1;
      m_dMin2=i_dMin2;
      m_dMax2=i_dMax2;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      if (m_dMax1!=m_dMin1)
        d=(d-m_dMin1)/(m_dMax1-m_dMin1);
      d = Sign(d)*Sqr(d*m_dMult);
      if (m_dMax2!=m_dMin2)
        d=m_dMin2 + d*(m_dMax2-m_dMin2);
      return d;
      };
  };

class CSlotConnOp_Sqrt : public CSlotConnOp_Base
  {
  protected:
    double m_dMult;   
    double m_dMin1;  
    double m_dMin2;  
    double m_dMax1;  
    double m_dMax2;  

  public:
    CSlotConnOp_Sqrt(double i_dMult, double i_dMin1, double i_dMax1, double i_dMin2, double i_dMax2)
      {
      m_dMult=i_dMult;
      m_dMin1=i_dMin1;
      m_dMax1=i_dMax1;
      m_dMin2=i_dMin2;
      m_dMax2=i_dMax2;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      if (m_dMax1!=m_dMin1)
        d=(d-m_dMin1)/(m_dMax1-m_dMin1);
      d = Sign(d)*Sqrt(fabs(d))*m_dMult;
      if (m_dMax2!=m_dMin2)
        d=m_dMin2 + d*(m_dMax2-m_dMin2);
      return d;
      };
  };

class CSlotConnOp_Profile : public CSlotConnOp_Base
  {
  protected:
    bool m_bReverse;
    CSlotConnPrf * m_pXY;

  public:
    CSlotConnOp_Profile(bool i_bReverse, CSlotConnPrf * i_pXY)
      {
      m_bReverse=i_bReverse;
      m_pXY=i_pXY;
      };
    virtual ~CSlotConnOp_Profile()
      {
      delete m_pXY;
      };
    virtual double Exec(double d, double DeltaTime) { return d; };
    virtual bool   IsConditioning() { return true; };
    virtual double DoConditioning(double d, double DeltaTime)
      {
      //if (m_bReverse)
      //  d = m_pXY->Y2X(d); 
      //else
      //  d = m_pXY->X2Y(d); 
      return d;
      };
  };

class CSlotConnOp_Filter : public CSlotConnOp_Base
  {
  protected:
    byte m_iType;
    double m_dTau;         
    double m_dPrev;         

  public:
    CSlotConnOp_Filter(byte i_iType, double i_dTau)      
      {
      m_iType=i_iType;
      m_dTau=Max(0.001, i_dTau);
      m_dPrev=dNAN;
      };
    virtual double Exec(double d, double DeltaTime) { return d; };
    virtual bool   IsConditioning() { return true; };
    virtual double DoConditioning(double d, double DeltaTime)
      {
      double Alpha = 1.0-exp(-DeltaTime/m_dTau);
      if (Valid(m_dPrev))
        d = d * Alpha + (1-Alpha)*m_dPrev; 
      m_dPrev=d; 
      return d;
      };
  };

class CSlotConnOp_DeadBand : public CSlotConnOp_Base
  {
  protected:
    byte m_iType;
    double m_dBand;         
    double m_dPrev;         

  public:
    CSlotConnOp_DeadBand(byte i_iType, double i_dBand)      
      {
      m_iType=i_iType;
      m_dBand=Max(0.0, i_dBand);
      m_dPrev=dNAN;
      };
    virtual double Exec(double d, double DeltaTime) { return d; };
    virtual bool   IsConditioning() { return true; };
    virtual double DoConditioning(double d, double DeltaTime)
      {
      //double Alpha = 1.0-exp(-DeltaTime/m_dTau);
      if (Valid(m_dPrev))
        {
        double Half=0.5*m_dBand;
        if (d>m_dPrev+Half)
          d=d-Half;
        else if (d<m_dPrev-Half)
          d=d+Half;
        else
          d=m_dPrev;
        }
      m_dPrev=d; 
      return d;
      };
  };

class CSlotConnOp_Noise : public CSlotConnOp_Base , protected CNoise
  {
  protected:
    //CNoise   * m_pFn;                       
    bool m_Relative;

  public:
    CSlotConnOp_Noise(NoiseTypes iType, bool Relative, double dStdDev, double dMean=0.0)
      {
      SetType(NoiseTypes(iType));
      SetGlobal(false);
      SetSeed(rand());
      Init(dMean, dStdDev); 
      m_Relative=Relative;
      };
    virtual double Exec(double d, double DeltaTime) { return d; };
    virtual bool   IsConditioning() { return true; };
    virtual double DoConditioning(double d, double DeltaTime)
      {
      double N=GetVal();
      d += (m_Relative ? d * N : N);
      return d;
      };
  };

class CSlotConnOp_Oscillator : public CSlotConnOp_Base
  {
  protected:
    bool    m_Relative;
    int     m_Type;
    float   m_Period;
    float   m_Lo;
    float   m_Hi;
    double  m_TimeInCycle;

  public:
    CSlotConnOp_Oscillator(int Type, float Period, bool Relative, float Lo, float Hi)
      {
      m_Type        =  Type;     
      m_Period      =  Period;   
      m_Relative    =  Relative; 
      m_Lo          =  Lo;       
      m_Hi          =  Hi; 
      m_TimeInCycle = 0;
      };
    virtual double Exec(double d, double DeltaTime) { return d; };
    virtual bool   IsConditioning() { return true; };
    virtual double DoConditioning(double d, double DeltaTime)
      {
      double Disturb=0;
      while (m_TimeInCycle>m_Period)
        m_TimeInCycle -= m_Period;
      double HalfPeriod=0.5*m_Period;
      switch (m_Type)
        {
        case 0:
          if (m_TimeInCycle<HalfPeriod)
            Disturb = (m_Relative ? d:1.0) * m_Lo;
          else
            Disturb = (m_Relative ? d:1.0) * m_Hi;
          break;
        case 1:
          Disturb = (m_Relative ? d:1.0) * (m_Lo+(m_Hi-m_Lo) * (m_TimeInCycle/m_Period));
          break;
        case 2:
          if (m_TimeInCycle<HalfPeriod)
            Disturb = (m_Relative ? d:1.0) * (m_Lo+(m_Hi-m_Lo) * (m_TimeInCycle/HalfPeriod));
          else
            Disturb = (m_Relative ? d:1.0) * (m_Lo+(m_Hi-m_Lo) * ((m_TimeInCycle/HalfPeriod)-1.0));
            break;
        }
      m_TimeInCycle += DeltaTime;
      return d+Disturb;
      };
  };

class CSlotConnOp_Scale : public CSlotConnOp_Base
  {
  protected:
    double     m_dMult;            
    double     m_dOffset;            

  public:
    CSlotConnOp_Scale(double i_dMult, double i_dOffset)            
      {
      m_dMult=i_dMult;
      m_dOffset=i_dOffset;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return d * m_dMult + m_dOffset;
      };
  };

class CSlotConnOp_Mask: public CSlotConnOp_Base
  {
  protected:
    long m_lMask;
  public:

    CSlotConnOp_Mask(bool AsBitNo, long Val)
      {
      m_lMask=AsBitNo ? (1<<Val) : Val;
      };
    virtual double Exec(double d, double DeltaTime)
      {
      return ((long)Range((double)LONG_MAX, d, (double)LONG_MIN) & m_lMask);
      };
  };

// -----------------------------------------------------------------------

enum eConnWhere { eCW_Null, eCW_Slot, eCW_Link, eCW_CdBlk };

class CSlotConnOpArray : public CArray <CSlotConnOp_Base *, CSlotConnOp_Base *> {};

class CSlotConnect
  {
  public:
    CString      m_sTag;       //tag for set or get
    //CString      m_sCnvTxt;    //conversion text of Src/Dest
    //CString      m_sVariable;
    //CString      m_sCdTag;

    CSlotConnOpArray m_Ops;

    bool         m_bValid;     // Address OK
    bool         m_bIsSet;     //is this a Set
    bool         m_bIsGet;     //is this a Get
    bool         m_bInvert;    //perform a logical not before set or get

    CDelayBlock  m_Delay;
    
    eConnSrcDst  m_eSrc;
    eConnSrcDst  m_eDst;

    long         m_lSrcIndex;   // Index of slot 
    long         m_lDstIndex;   // Index of slot 
    //long         m_lLink;      // Index of Subscription

    long         m_lCdBlk;     // Index of CodeBlock 
    CCdBlk      *m_pCdBlk;      //pointer to CodeBlock (either this or m_pSlot)
    GCVar       *m_pCdBlkVar;   //pointer to CodeBlock variable
    bool         m_bCdBlkVarFlt;

    CSlotConnect(LPCSTR pTag, /*LPCSTR pCnvTxt,*/ bool IsSet, bool Inv);
    ~CSlotConnect(); 
    void          SetDelayTimes(DWORD DelayTimeRise, DWORD DelayTimeFall);
    bool          AddRiseValue(COleVariant v1, DWORD t1);
    bool          AddFallValue(COleVariant v1, DWORD t1);
    void          AddOp(CSlotConnOp_Base* pOp) { m_Ops.Add(pOp); };

    void          Initialise();
    bool          Connect(CSlot * pSlot);
    void          CorrectConnects(CLongArray & OldNos);
    void          ApplyRangeLink2Slot(CSlot & S, COleVariant &V);
    void          ApplyRangeSlot2Link(CSlot & S, COleVariant &V);
    void          Process(eConnSrcDst eSrc, long SrcI, eConnSrcDst eDst, long SrcDstI, CFullValue & SrcValue, int Direction);
    //void          GetValue(COleVariant &V);
    void          ProcessOps(VARIANT &);
    void          Dump(LPCSTR Hd);
    //bool PutSubsValue2Slot(CDriverManagerBase * pMngr, CPkDataItem *pPItem, bool ForcesOn, double DeltaTime, bool AllowNoise);

  protected:
    long        getSlotCount() const;
    CSlot     * getSlot(long Index) const;
    void        putSlot(long Index, CSlot * pSlot);
    long        getLinkCount() const;
    CLink     * getLink(long Index) const;
    void        putLink(long Index, CLink * pLink);
    long        getDeviceCount() const;
    CDevice   * getDevice(long Index) const;
    void        putDevice(long Index, CDevice * pDevice);

    __declspec(property(get=getSlotCount))             long NSlots;
    __declspec(property(get=getSlot,put=putSlot))      CSlot* Slots[];
    __declspec(property(get=getLinkCount))             long NLinks;
    __declspec(property(get=getLink,put=putLink))      CLink* Links[];
    __declspec(property(get=getDeviceCount))           long NDevices;
    __declspec(property(get=getDevice,put=putDevice))  CDevice* Devices[];
  };

class CSlotConnArray : public CArray <CSlotConnect*, CSlotConnect*> {};

// =======================================================================
//
//
//
// =======================================================================

#endif // !defined(AFX_SLOTCONNECT_H__9255AA2C_F19E_4EA5_A5D8_D4ADEEF318E2__INCLUDED_)
