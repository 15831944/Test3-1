//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Styles.cpp
//  $Author    :  Jim Hansen
//
//  Description:  property pages
//                for scaling and alarming
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "Styles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScalingPage property page

//IMPLEMENT_DYNCREATE(ScalingPage, CPropertyPage)

ScalingPage::ScalingPage() : CPropertyPage(ScalingPage::IDD)
{
   //{{AFX_DATA_INIT(ScalingPage)
   m_units = _T("");
   m_example = _T("");
   m_deadband = 0.0;
   m_logDeadband = 0.0;
   m_maxEngRange = 0.0;
   m_maxRawRange = 0.0;
   m_minEngRange = 0.0;
   m_minRawRange = 0.0;
   //}}AFX_DATA_INIT
}

ScalingPage::~ScalingPage()
{
}

BOOL ScalingPage::UpdateScaling(OAnalogData* pScaling, BOOL save )
{
   CSLock wait( &tagCS );   // protect tags
   ASSERT_VALID( pScaling );
   if( save )  // that is, save state from the UI controls
      {
      pScaling->EngUnits    = m_units;
      pScaling->Deadband    = m_deadband;
      pScaling->LogDeadband = m_logDeadband;
      pScaling->MinEngRange = m_minEngRange;
      pScaling->MaxEngRange = m_maxEngRange;
      pScaling->MinRawRange = m_minRawRange;
      pScaling->MaxRawRange = m_maxRawRange;
      }
   else
      {
      m_units       = pScaling->EngUnits;
      m_deadband    = pScaling->Deadband;
      m_logDeadband = pScaling->LogDeadband;
      m_minEngRange = pScaling->MinEngRange;
      m_maxEngRange = pScaling->MaxEngRange;
      m_minRawRange = pScaling->MinRawRange;
      m_maxRawRange = pScaling->MaxRawRange;
      }
   return TRUE;
}

void ScalingPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ScalingPage)
   DDX_Text(pDX, IDC_ENGUNITS, m_units);
   DDX_Text(pDX, IDC_EXAMPLE, m_example);
   DDX_Text(pDX, IDC_DEBAND, m_deadband);
   DDX_Text(pDX, IDC_LOGDEADBAND, m_logDeadband);
   DDX_Text(pDX, IDC_MAXEU, m_maxEngRange);
   DDX_Text(pDX, IDC_MAXRAW, m_maxRawRange);
   DDX_Text(pDX, IDC_MINEU, m_minEngRange);
   DDX_Text(pDX, IDC_MINRAW, m_minRawRange);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ScalingPage, CPropertyPage)
   //{{AFX_MSG_MAP(ScalingPage)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScalingPage message handlers
/////////////////////////////////////////////////////////////////////////////
// AlarmingPage property page

//IMPLEMENT_DYNCREATE(AlarmingPage, CPropertyPage)

AlarmingPage::AlarmingPage() : CPropertyPage(AlarmingPage::IDD)
{
   //{{AFX_DATA_INIT(AlarmingPage)
   m_devDeadband = 0.0;
   m_high = FALSE;
   m_hihi = FALSE;
   m_lolo = FALSE;
   m_low = FALSE;
   m_major = FALSE;
   m_minor = FALSE;
   m_rate = FALSE;
   m_valueHigh = 0.0;
   m_priorHigh = 0;
   m_valueHiHi = 0.0;
   m_priorHiHi = 0;
   m_valueLoLo = 0.0;
   m_priorLoLo = 0;
   m_valueLow = 0.0;
   m_priorLow = 0;
   m_valueMajor = 0.0;
   m_priorMajor = 0;
   m_valueMinor = 0.0;
   m_priorMinor = 0;
   m_percentRate = 0.0;
   m_priorRate = 0;
   //}}AFX_DATA_INIT
}

AlarmingPage::~AlarmingPage()
{
}

void AlarmingPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AlarmingPage)
   DDX_Text(pDX, IDC_DEVDEADBAND, m_devDeadband);
   DDX_Check(pDX, IDC_ENABLEHIGH, m_high);
   DDX_Check(pDX, IDC_ENABLEHIHI, m_hihi);
   DDX_Check(pDX, IDC_ENABLELOLOALM, m_lolo);
   DDX_Check(pDX, IDC_ENABLELOW, m_low);
   DDX_Check(pDX, IDC_ENABLEMAJOR, m_major);
   DDX_Check(pDX, IDC_ENABLEMINOR, m_minor);
   DDX_Check(pDX, IDC_ENABLERATE, m_rate);
   DDX_Text(pDX, IDC_HIGH, m_valueHigh);
   DDX_Text(pDX, IDC_HIGHPRIOR, m_priorHigh);
   DDX_Text(pDX, IDC_HIHI, m_valueHiHi);
   DDX_Text(pDX, IDC_HIHIPRIOR, m_priorHiHi);
   DDX_Text(pDX, IDC_LOLO, m_valueLoLo);
   DDX_Text(pDX, IDC_LOLOPRIOR, m_priorLoLo);
   DDX_Text(pDX, IDC_LOW, m_valueLow);
   DDX_Text(pDX, IDC_LOWPRIOR, m_priorLow);
   DDX_Text(pDX, IDC_MAJOR, m_valueMajor);
   DDX_Text(pDX, IDC_MAJORPRIOR, m_priorMajor);
   DDX_Text(pDX, IDC_MINOR, m_valueMinor);
   DDX_Text(pDX, IDC_MINORPRIOR, m_priorMinor);
   DDX_Text(pDX, IDC_RATE, m_percentRate);
   DDX_Text(pDX, IDC_RATEPRIOR, m_priorRate);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AlarmingPage, CPropertyPage)
   //{{AFX_MSG_MAP(AlarmingPage)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AlarmingPage message handlers

