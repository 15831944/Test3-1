// MEncryptDatConfig.h

#pragma once

#include "EncryptDatConfig.h"

using namespace System;

namespace MEncryptDatConfigNS {

	public ref class MEncryptDatConfig
	{
  public:
    EncryptDatConfig *encryptDatConfig;

    int FlagPGMEncrypted()      { return encryptDatConfig->FlagPGMEncrypted;      }
    int FlagRCTEncrypted()      { return encryptDatConfig->FlagPGMEncrypted;      }
    int FlagDXFEncrypted()      { return encryptDatConfig->FlagPGMEncrypted;      }
    int FlagAllowSave()         { return encryptDatConfig->FlagAllowSave;         }
    int FlagAllowExcelReports() { return encryptDatConfig->FlagAllowExcelReports; }

    MEncryptDatConfig()
    {
      encryptDatConfig = new EncryptDatConfig();
    }

    int EncryptionVersion()
    {
      return encryptDatConfig->EncryptionVersion;
    }

    int BuildSysCADVersion()
    {
      return encryptDatConfig->BuildSysCADVersion;
    }

    int BuildSysCADBuild()
    {
      return encryptDatConfig->BuildSysCADBuild;
    }

    int LoSysCADVersion()
    {
      return encryptDatConfig->LoSysCADVersion;
    }

    int LoSysCADBuild()
    {
      return encryptDatConfig->LoSysCADBuild;
    }

    int HiSysCADVersion()
    {
      return encryptDatConfig->HiSysCADVersion;
    }

    int HiSysCADBuild()
    {
      return encryptDatConfig->HiSysCADBuild;
    }

    int FlagCount()
    {
      return FLAGCOUNT;
    }

    int* Flags()
    {
      return encryptDatConfig->Flags;
    }

    int Flag(int i)
    {
      return encryptDatConfig->Flags[i];
    }

    void SetFlag(int i, int value)
    {
      encryptDatConfig->Flags[i] = value;
    }

    void Load(String^ filename)
    {
      char* s = new char[filename->Length+1];
      int i;

      for (i=0; i<filename->Length; i++)
      {
        s[i] = (char)filename[i];
      }

      s[i] = 0;

      encryptDatConfig->Load(s);
    }

    void Save(String^ filename)
    {
      char* s = new char[filename->Length+1];
      int i;

      for (i=0; i<filename->Length; i++)
      {
        s[i] = (char)filename[i];
      }

      s[i] = 0;

      encryptDatConfig->Save(s);

      delete s;
    }

  };
}
