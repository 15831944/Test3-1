// MEncryptDatConfig.h

#pragma once

#include "EncryptDatConfig.h"

using namespace System;

namespace MEncryptDatConfigNS {

	public ref class MEncryptDatConfig
	{
  public:
    EncryptDatConfig *encryptDatConfig;

    unsigned short FlagPGMEncrypted()      { return encryptDatConfig->FlagPGMEncrypted;      }
    unsigned short FlagRCTEncrypted()      { return encryptDatConfig->FlagRCTEncrypted;      }
    unsigned short FlagDXFEncrypted()      { return encryptDatConfig->FlagDXFEncrypted;      }
    unsigned short FlagAllowSave()         { return encryptDatConfig->FlagAllowSave;         }
    unsigned short FlagAllowExcelReports() { return encryptDatConfig->FlagAllowExcelReports; }

    MEncryptDatConfig() { encryptDatConfig = new EncryptDatConfig(); }

    unsigned short EncryptionVersion() { return encryptDatConfig->EncryptionVersion; }

    unsigned short BuildSysCADVersion() { return encryptDatConfig->BuildSysCADVersion; }
    unsigned short BuildSysCADBuild() { return encryptDatConfig->BuildSysCADBuild; }

    unsigned short LoSysCADVersion() { return encryptDatConfig->LoSysCADVersion; }
    unsigned short LoSysCADBuild() { return encryptDatConfig->LoSysCADBuild; }

    unsigned short HiSysCADVersion() { return encryptDatConfig->HiSysCADVersion; }
    unsigned short HiSysCADBuild() { return encryptDatConfig->HiSysCADBuild; }

    String^ MACAddress(int idx)
    {
      String^ macAddress;
      for (size_t i=0; i<strlen(encryptDatConfig->MACAddress[idx]); i++)
      {
        macAddress += encryptDatConfig->MACAddress[idx][i];
      }
      return macAddress;
    }

    void SetMACAddress(int idx, String^ macAddress)
    {
      char* s = new char[macAddress->Length+1];
      int i;

      for (i=0; i<macAddress->Length; i++)
      {
        s[i] = (char)macAddress[i];
      }

      s[i] = 0;

      strcpy_s(encryptDatConfig->MACAddress[idx], encryptDatConfig->MACAddressLength-1, s);

      delete s;
    }


    String^ Password()
    {
      String^ password;
      for (size_t i=0; i<strlen(encryptDatConfig->Password); i++)
      {
        password += encryptDatConfig->Password[i];
      }
      return password;
    }

    void SetPassword(String^ password)
    {
      char* s = new char[password->Length+1];
      int i;

      for (i=0; i<password->Length; i++)
      {
        s[i] = (char)password[i];
      }

      s[i] = 0;

      strcpy_s(encryptDatConfig->Password, encryptDatConfig->PasswordLength-1, s);

      delete s;
    }

    unsigned short FlagCount() { return encryptDatConfig->FlagCount; }
    unsigned short Flag(int i) { return encryptDatConfig->Flag[i]; }
    void SetFlag(int i, int value) { encryptDatConfig->Flag[i] = value; }

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

      delete s;
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
