//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Styles.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  property pages
//                for scaling and alarming.
//
//**************************************************************************

#ifndef __STYLES_H__             // Only Include Once
#define __STYLES_H__

class ScalingPage : public CPropertyPage
{

// Construction
public:
   ScalingPage();
   ~ScalingPage();
   BOOL UpdateScaling(OAnalogData* pScaling, BOOL save = TRUE );

// Dialog Data
   //{{AFX_DATA(ScalingPage)
   enum { IDD = IDD_TAGANLGDETAILS };
   CString  m_units;
   CString  m_example;
   double   m_deadband;
   double   m_logDeadband;
   double   m_maxEngRange;
   double   m_maxRawRange;
   double   m_minEngRange;
   double   m_minRawRange;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(ScalingPage)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(ScalingPage)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// AlarmingPage dialog

class AlarmingPage : public CPropertyPage
{

// Construction
public:
   AlarmingPage();
   ~AlarmingPage();

// Dialog Data
   //{{AFX_DATA(AlarmingPage)
   enum { IDD = IDD_TAGANLGALARM };
   double   m_devDeadband;
   BOOL     m_high;
   BOOL     m_hihi;
   BOOL     m_lolo;
   BOOL     m_low;
   BOOL     m_major;
   BOOL     m_minor;
   BOOL     m_rate;
   double   m_valueHigh;
   UINT     m_priorHigh;
   double   m_valueHiHi;
   UINT     m_priorHiHi;
   double   m_valueLoLo;
   UINT     m_priorLoLo;
   double   m_valueLow;
   UINT     m_priorLow;
   double   m_valueMajor;
   UINT     m_priorMajor;
   double   m_valueMinor;
   UINT     m_priorMinor;
   double   m_percentRate;
   UINT     m_priorRate;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(AlarmingPage)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(AlarmingPage)
      // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

#endif