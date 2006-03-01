#include "stdafx.h"
#include ".\electricalimport.h"

#define dbgElecImport  01

#if dbgElecImport
#include "dbgmngr.h"
static CDbgMngr dbgLoads          ("ElectImport", "Loads");
#endif

//=========================================================================

CElectricalImport::CElectricalImport(void)
  {
  }

//=========================================================================

CElectricalImport::~CElectricalImport(void)
  {
  }

//=========================================================================

bool CElectricalImport::Import(void)
  {
  if (1)
    {
    LogNote("SysCAD", MB_OK, "Electrical Import Not currently implemented");
    return false;
    }
  else
    {
#if 0
    Strng s;
    CSCDFileDialog Dlg(true, NULL, "*.mdb", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "SnapShots (*.snp)|*.snp|All Files (*.*)|*.*||");
    Dlg.m_ofn.lpstrInitialDir = PrjFiles();
    Dlg.m_ofn.lpstrTitle = "ELMSE Database";

    if (Dlg.DoModal()==IDOK)
      {
      CString Fn, Connect;
      Fn=(LPCTSTR)Dlg.GetPathName();
#else
      {
      CString Fn, Connect;
      Fn="D:\\SysCAD90\\Projects\\Tests\\Power-OrdEval\\Electricals\\elmse.mdb";
#endif
      Connect.Format("Provider=%s;Data Source=%s",

        "Microsoft.Jet.OLEDB.4.0", (LPCTSTR)Fn);
      // OR Driver={Microsoft Access Driver (*.mdb)};DBQ=physical path to .mdb file
      m_bsConnect=Connect;
      try
        {
        m_RS1=ADODB::_RecordsetPtr(__uuidof(ADODB::Recordset));
        ReadLoadList();
        ReadEquipList();
        ReadCableList();

        return true;
        }
      catch (_com_error & e)
        {
        LogError("ELMSE", 0, "Exception Occurred:%s", e.Description());
        }
      }
    }
  return false;
  }

//=========================================================================

inline long GetLong(ADODB::_RecordsetPtr & RS, LPCTSTR FldName)
  {
  long L=-1;
  _variant_t v=RS->Fields->Item[ _variant_t(FldName)]->Value;
  if (v.vt!=VT_NULL)
    L=v;
  return L;
  }

inline double GetDouble(ADODB::_RecordsetPtr & RS, LPCTSTR FldName)
  {
  double D=0;
  _variant_t v=RS->Fields->Item[ _variant_t(FldName)]->Value;
  if (v.vt!=VT_NULL)
    D=v;
  return D;
  }

inline CString GetString(ADODB::_RecordsetPtr & RS, LPCTSTR FldName)
  {
  CString S("");
#if DotNet 
  _variant_t v=RS->Fields->Item[ _variant_t(FldName)]->Value;
  if (v.vt!=VT_NULL)
    S=v;
#else
//TODO S=v line above does not compile in V6
  INCOMPLETECODE()
#endif
  return S;
  }

//=========================================================================
 
bool CElectricalImport::ReadLoadList()
  {
  HRESULT         hr=0;
  //ADODB::_RecordsetPtr  m_RS1=NULL;
  ADODB::FieldPtr       Fld=0;
  try
    {
    _bstr_t Source("LoadList");
    m_RS1->Open(Source, m_bsConnect, ADODB::adOpenForwardOnly, ADODB::adLockOptimistic/*ReadOnly*/, -1);
    while ( m_RS1->adEOF == VARIANT_FALSE )
      {
      long    LoadKEY         = GetLong  (m_RS1, "LoadKEY");
      long    EquipKEY        = GetLong  (m_RS1, "EquipKEY");
      CString Tag             = GetString(m_RS1, "EquipmentNo");
      CString Desc1           = GetString(m_RS1, "Description1");
      CString Desc2           = GetString(m_RS1, "Description2");
      double  AbsorbedLoad    = GetDouble(m_RS1, "AbsorbedLoad");
      double  Volts           = GetDouble(m_RS1, "Volts");
      double  Power           = GetDouble(m_RS1, "Power");
      long    NoOfPhases      = GetLong  (m_RS1, "NoOfPhases");
      long    NoOfPoles       = GetLong  (m_RS1, "NoOfPoles");
      double  PFactor         = GetDouble(m_RS1, "PFactor");
      double  Efficiency      = GetDouble(m_RS1, "Efficiency");
      double  FLC             = GetDouble(m_RS1, "FLC");
      double  DiversityFactor = GetDouble(m_RS1, "DiversityFactor");
      CString LoadType        = GetString(m_RS1, "LoadType");
      CString StarterType     = GetString(m_RS1, "StarterType");

      #if dbgElecImport
      if (dbgLoads())
        {
        dbgpln("Load :%5i %5i %-20s %-10s %-10s %2i %2i %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %s %s", 
              LoadKEY, EquipKEY, (LPCTSTR)Tag,
              (LPCTSTR)LoadType, (LPCTSTR)StarterType, 
              NoOfPhases,NoOfPoles, AbsorbedLoad, Volts, Power, PFactor, Efficiency, FLC, DiversityFactor,
              (LPCTSTR)Desc1, (LPCTSTR)Desc2);
        }
      #endif
      m_RS1->MoveNext();
      }
    m_RS1->Close();
    }
  catch(...)
    {
    }
  return false;
  }

//=========================================================================

bool CElectricalImport::ReadEquipList()
  {
  HRESULT         hr=0;
  //ADODB::_RecordsetPtr  m_RS1=NULL;
  ADODB::FieldPtr       Fld=0;
  try
    {
    //_bstr_t Source("SELECT CableId,CableNo,FromId,ToId,CableCatId,MasterCableId,FLC,Volts,NoOfPhases,Length,MaxLength FROM CABLES");
    _bstr_t Source("SELECT * FROM EQUIPMENT");
    m_RS1->Open(Source, m_bsConnect, ADODB::adOpenForwardOnly, ADODB::adLockOptimistic/*ReadOnly*/, -1);
    while ( m_RS1->adEOF == VARIANT_FALSE )
      {
      long EquipId        = GetLong  (m_RS1, "EquipId");
      CString EquipmentNo = GetString(m_RS1, "EquipmentNo");
      CString Desc1       = GetString(m_RS1, "Description1");
      CString Desc2       = GetString(m_RS1, "Description2");
      CString EqType      = GetString(m_RS1, "EqType");

      #if dbgElecImport
      if (dbgLoads())
        {
        dbgpln("Equip :%5i %-20s %-20s %s %s", 
              EquipId,(LPCTSTR)EquipmentNo,(LPCTSTR)EqType,(LPCTSTR)Desc1,(LPCTSTR)Desc2);
        }
      #endif
      m_RS1->MoveNext();
      }
    m_RS1->Close();
    }
  catch(...)
    {
    }
  return false;
  }

//=========================================================================
 
bool CElectricalImport::ReadCableList()
  {
  HRESULT         hr=0;
  //ADODB::_RecordsetPtr  m_RS1=NULL;
  ADODB::FieldPtr       Fld=0;
  try
    {
    //_bstr_t Source("SELECT CableId,CableNo,FromId,ToId,CableCatId,MasterCableId,FLC,Volts,NoOfPhases,Length,MaxLength FROM CABLES");
    _bstr_t Source("SELECT * FROM CABLES");
    m_RS1->Open(Source, m_bsConnect, ADODB::adOpenForwardOnly, ADODB::adLockOptimistic/*ReadOnly*/, -1);
    while ( m_RS1->adEOF == VARIANT_FALSE )
      {
      long CableId       = GetLong  (m_RS1, "CableId");
      CString CableNo    = GetString(m_RS1, "CableNo");
      long FromId        = GetLong  (m_RS1, "FromId");
      long ToId          = GetLong  (m_RS1, "ToId");
      long CableCatId    = GetLong  (m_RS1, "CableCatId");
      long MasterCableId = GetLong  (m_RS1, "MasterCableId");
      double FLC         = GetDouble(m_RS1, "FLC");
      double Volts       = GetDouble(m_RS1, "Volts");
      long NoOfPhases    = GetLong  (m_RS1, "NoOfPhases");
      double Length      = GetDouble(m_RS1, "Length");
      double MaxLength   = GetDouble(m_RS1, "MaxLength");

      #if dbgElecImport
      if (dbgLoads())
        {
        dbgpln("Cable:%5i %-20s %4i %4i %4i %4i %10.4f %10.4f %4i %10.4f %10.4f", 
              CableId,(LPCTSTR)CableNo,FromId,ToId,CableCatId,MasterCableId,FLC,Volts,NoOfPhases,Length,MaxLength);
        }
      #endif
      m_RS1->MoveNext();
      }
    m_RS1->Close();
    }
  catch(...)
    {
    }
  return false;
  }

//=========================================================================
