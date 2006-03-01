#pragma once

#import "c:\program files\common files\system\ado\msado15.dll" rename("EOF", "adEOF")

class CElectricalImport
  {
  public:
    CElectricalImport(void);
    ~CElectricalImport(void);

    bool Import();
    bool ReadLoadList();
    bool ReadEquipList();
    bool ReadCableList();

  protected:
    _bstr_t  m_bsConnect;
    _bstr_t  m_bsSource;

    ADODB::_RecordsetPtr m_RS1;

  };
