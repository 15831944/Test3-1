// ToleranceItem.h: interface for the CToleranceItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOLERANCES_H__29F0A217_5E19_4DE9_A008_16347C55B8E2__INCLUDED_)
#define AFX_TOLERANCES_H__29F0A217_5E19_4DE9_A008_16347C55B8E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sc_defs.h"
#include <afxtempl.h> // "xafxtempl.h"

class CToleranceMngr;
class CToleranceItem;

class CToleranceItem  
  {
  protected:
    double m_dAbs;
    double m_dRel;

    CList <CToleranceItem*, CToleranceItem*> m_Related;
//    double m_
  public:
	  CToleranceItem();
	  virtual ~CToleranceItem();

  inline bool ConvergedDV(double D, double V, double & Err)
    {
    const double Vt = Max(fabs(V), fabs(V + D));
    Err = fabs(D) / (m_dAbs + Vt * m_dRel);
    return (Err < 1.0);
    };

  inline bool ConvergedVV(double V1, double V2, double & Err)
    {
    const double Vt = Max(fabs(V1), fabs(V2));
    Err = fabs(V2-V1) / (m_dAbs + Vt * m_dRel);
    return (Err < 1.0);
    };
  };

class CToleranceMngr : public CToleranceItem
  {
  public:
	  CToleranceMngr();
	  virtual ~CToleranceMngr();

  };

#endif // !defined(AFX_TOLERANCES_H__29F0A217_5E19_4DE9_A008_16347C55B8E2__INCLUDED_)
