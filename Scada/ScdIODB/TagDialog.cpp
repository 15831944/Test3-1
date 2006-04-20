//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  TagDialog.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Tag properties dialog
//
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "TagDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TagDialog dialog


TagDialog::TagDialog(CWnd* pParent /*=NULL*/)
   : CDialog(TagDialog::IDD, pParent)
{
   pTag = NULL;
   pDoc = NULL;
   //{{AFX_DATA_INIT(TagDialog)
   m_name = _T("");
   m_description = _T("");
   m_dataType = _T("");
   m_simulationType = -1;
	m_Value = _T("");
	//}}AFX_DATA_INIT
}

TagDialog::TagDialog(OModbusTag* tag, CDoc* doc, CWnd* pParent /*=NULL*/)
   : CDialog(TagDialog::IDD, pParent), pTag(tag), pDoc(doc)
{
   CSLock wait( &tagCS );   // protect tags
   ASSERT_VALID( pTag );
   ASSERT_VALID( pDoc );
   sheet = new CPropertySheet( _T("Tag Process Settings") );
   scaling = new ScalingPage ;
   sheet->AddPage( scaling );

   m_name            = pTag->name;
   m_description     = pTag->description;
   //m_enableScaling   = pTag->enableProcessing;
   //m_offset          = pTag->offset+1;    // user sees 1 based
   // m_range           = pTag->range;
   switch( pTag->nativeType )
      {
      default: m_dataType = _T("???"); break;
      case VT_EMPTY:m_dataType = _T("");     m_Value.Format("0");                               break;
      case VT_BOOL: m_dataType = _T("BOOL"); m_Value.Format("%s", pTag->value.boolVal?"1":"0"); break;
      case VT_I2:   m_dataType = _T("I2");   m_Value.Format("%i", pTag->value.bVal);            break;
      case VT_I4:   m_dataType = _T("I4");   m_Value.Format("%i", pTag->value.iVal);            break;
      case VT_R4:   m_dataType = _T("R4");   m_Value.Format("%i", pTag->value.lVal);            break;
      case VT_R8:   m_dataType = _T("R8");   m_Value.Format("%g", pTag->value.fltVal);          break;
      case VT_BSTR: m_dataType = _T("BSTR"); m_Value.Format("%g", pTag->value.dblVal);          break;
      }
  
   m_simulationType  = pTag->simType;
   }

TagDialog::~TagDialog()
{
   delete scaling;
   delete sheet;
}

BOOL TagDialog::OnInitDialog()
{
   CDialog::OnInitDialog();
   //scaling->UpdateScaling( pTag->pProcess, FALSE );
   //
   //if( m_enableScaling )
   //   {
   //   GetDlgItem(IDC_SCALING_SETTINGS)->EnableWindow();
   //   CreateDescription();
   //   }
   //else
   //   {
   //   GetDlgItem(IDC_SCALING_SETTINGS)->EnableWindow(FALSE);
   //   m_scalingDescription.Empty();
   //   }

   UpdateData( FALSE );
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void TagDialog::CreateDescription()
{
   //CString scale;
   //scale.Format( _T("Scaling: input range %g to %g scales to %g to %g %s\n"),
   //            pTag->pProcess->MinRawRange, pTag->pProcess->MaxRawRange,
   //            pTag->pProcess->MinEngRange, pTag->pProcess->MaxEngRange,
   //            pTag->pProcess->EngUnits );
   //CString alarm;
   //m_scalingDescription = scale + alarm;
}

int TagDialog::UpdateTag(OModbusTag* pTag)
{
   ASSERT_VALID( pTag );
   pTag->name = m_name;
   pTag->description = m_description;
   //pTag->enableProcessing = m_enableScaling;
   //pTag->offset = m_offset-1;
   //_asm int 3; //?????????
   ////pTag->m_IsOutput= m_IsOutput;
   //CSLock wait( &tagCS );   // protect tags
   //_asm int 3; //?????????
   //if( 0)//m_range == OModbusTag::INPUT_BOOL || m_range == OModbusTag::OUTPUT_BOOL )
   //   pTag->nativeType = pTag->value.vt = VT_BOOL;
   //else
   //{
   //   //switch( m_dataType )
   //   //   {
   //   //   case 0: pTag->nativeType = VT_I2; break;
   //   //   case 1: pTag->nativeType = VT_I4; break;
   //   //   default:
   //   //   case 2: pTag->nativeType = VT_R4; break;
   //   //   case 3: pTag->nativeType = VT_BSTR; break;
   //   //   }
   //   //pTag->value.ChangeType( pTag->nativeType );
   //}
   pTag->simType = m_simulationType;

   return TRUE;
}


void TagDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TagDialog)
   DDX_Text(pDX, IDC_NAME, m_name);
   DDX_Text(pDX, IDC_DESCRIPTION, m_description);
   DDX_Text(pDX, IDC_DATATYPE, m_dataType);
   DDX_CBIndex(pDX, IDC_SIMULATION_TYPE, m_simulationType);
   DDX_Text(pDX, IDC_VALUE, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TagDialog, CDialog)
   //{{AFX_MSG_MAP(TagDialog)
   ON_BN_CLICKED(IDC_SCALING_SETTINGS, OnScalingSettings)
   ON_BN_CLICKED(IDC_ENABLE_SCALING, OnEnableScaling)
	ON_BN_CLICKED(ID_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TagDialog message handlers

void TagDialog::OnScalingSettings()
{
   scaling->UpdateScaling( pTag->pProcess, FALSE );   // in case style changed it
   if( sheet->DoModal() == IDOK )
      {
      CSLock wait( &tagCS );   // protect tags
      scaling->UpdateScaling( pTag->pProcess, TRUE );
      CreateDescription();
      UpdateData(FALSE);
      }
}

void TagDialog::OnEnableScaling()
{
//   UpdateData( TRUE );
//   if( m_enableScaling )
//      {
//      GetDlgItem(IDC_SCALING_SETTINGS)->EnableWindow();
//      CreateDescription();
//      }
//   else
//      {
//      GetDlgItem(IDC_SCALING_SETTINGS)->EnableWindow(FALSE);
//      m_scalingDescription.Empty();
//      }
//   UpdateData(FALSE);
}


void TagDialog::OnOK() 
{
    UpdateData( );
	POSITION pos = pDoc->pCurrent->tags.GetHeadPosition();
    while( pos )
    {
        OModbusTag* pNextTag = pDoc->pCurrent->tags.GetNext(pos);
        if( pNextTag->name.CompareNoCase(m_name)==0
         && pNextTag != pTag)
        {
            AfxMessageBox( _T("Duplicate Tag name") );
            return;
        }
    }
	
	CDialog::OnOK();
}

void TagDialog::OnSet() 
  {
  UpdateData( TRUE );
	
  pTag->quality = OPC_QUALITY_GOOD;
  //if( !pTag->active || pTag->m_IsOutput)
  //  continue;

  double f=atof(m_Value);

  switch( pTag->value.vt )
    {
    case VT_EMPTY:
    case VT_NULL:
      pTag->value.vt = VT_R8;
      pTag->value.dblVal = 0.0;
      m_Value.Format("0");
      break;
    case VT_BOOL:
      pTag->value.boolVal = f!=0.0;
      m_Value.Format("%s", pTag->value.boolVal?"1":"0");
      break;
    case VT_UI1: // uchar
      pTag->value.bVal = (unsigned char)f;
      m_Value.Format("%i", pTag->value.bVal);
      break;
    case VT_I2 :   // short
      pTag->value.iVal = (short)f;
      m_Value.Format("%i", pTag->value.iVal);
      break;
    case VT_I4 :  // long
      pTag->value.lVal = (long)f; 
      m_Value.Format("%i", pTag->value.lVal);
      break;
    case VT_R4 :
      pTag->value.fltVal = (float)f;
      m_Value.Format("%g", pTag->value.fltVal);
      break;
    case VT_R8 :
      pTag->value.dblVal = f; 
      m_Value.Format("%g", pTag->value.dblVal);
      break;
    default: ASSERT( FALSE );
    }
  CoFileTimeNow( &pTag->timestamp );

  UpdateData( FALSE );
  }
