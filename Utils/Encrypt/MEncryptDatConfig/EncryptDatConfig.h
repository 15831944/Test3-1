#pragma once

#include <fstream>

#include "..\..\..\Common\Scd\ScdLib\Scdver.h"
#include "..\..\..\Common\Scd\ScdLib\RevHist.h"

#define FLAGCOUNT 64
#define BUFFERSIZE (FLAGCOUNT + 7) // Must be FLAGCOUNT + (number of int parameters).

class EncryptDatConfig
{
public:
  static const int FlagPGMEncrypted      = 0;
  static const int FlagRCTEncrypted      = 1;
  static const int FlagDXFEncrypted      = 2;
  static const int FlagAllowSave         = 3;
  static const int FlagAllowExcelReports = 4;

  int EncryptionVersion;

  int BuildSysCADVersion;
  int LoSysCADVersion;
  int HiSysCADVersion;

  int BuildSysCADBuild;
  int LoSysCADBuild;
  int HiSysCADBuild;

  int *Flags;

  EncryptDatConfig();

  void Load(char* filename);
  void Save(char* filename);
};


EncryptDatConfig::EncryptDatConfig()
{
  EncryptionVersion = 0;

  BuildSysCADVersion = SCD_PATCH_SVN;
  BuildSysCADBuild = SCD_BUILD_NUM;

  LoSysCADVersion = 0;
  LoSysCADBuild = 0;

  HiSysCADVersion = INT_MAX;
  HiSysCADBuild = INT_MAX;

  Flags = new int[FLAGCOUNT];

  for (int i=0; i<FLAGCOUNT; i++)
    Flags[i] = 0;
}

void EncryptDatConfig::Load(char* filename)
{
  int *buffera;
  int *bufferb;
  int length;

  std::fstream fin;
  fin.open(filename, std::ios::in|std::ios::binary);
  if (fin.is_open())
  {    
    fin.seekg(0, std::ios_base::end);
    length = fin.tellg()/2;
    fin.seekg(0, std::ios_base::beg);

    buffera = new int[length+1];
    bufferb = new int[length+1];
    fin.read(reinterpret_cast<char*>(buffera), length);
    fin.close();

    buffera[length] = length;

    int seed = length; 

    for (int i=length-1; i>=0; i--) // decrypt the buffer.
    {
      seed *= 1103515245;
      seed += 12345; // add some deterministic noise to the system.
      bufferb[i] = buffera[i] ^ buffera[i+1] ^ i ^ seed;
    }

    if (length >= BUFFERSIZE) // only do this if there is enough data from the file.
    {
      int bufferIdx = 0;

      EncryptionVersion = bufferb[bufferIdx++];

      BuildSysCADVersion = bufferb[bufferIdx++];
      BuildSysCADBuild = bufferb[bufferIdx++];

      LoSysCADVersion = bufferb[bufferIdx++];
      LoSysCADBuild = bufferb[bufferIdx++];

      HiSysCADVersion = bufferb[bufferIdx++];
      HiSysCADBuild = bufferb[bufferIdx++];

      delete Flags;
      Flags = new int[FLAGCOUNT];

      for (int i=0; i<FLAGCOUNT; i++)
        Flags[i] = bufferb[bufferIdx++];
    }

    delete buffera;
    delete bufferb;
  }
}

void EncryptDatConfig::Save(char* filename)
{
  int *buffer = new int[BUFFERSIZE+1];

  int bufferIdx = 0;

  buffer[bufferIdx++] = EncryptionVersion;

  buffer[bufferIdx++] = BuildSysCADVersion;
  buffer[bufferIdx++] = BuildSysCADBuild;

  buffer[bufferIdx++] = LoSysCADVersion;
  buffer[bufferIdx++] = LoSysCADBuild;

  buffer[bufferIdx++] = HiSysCADVersion;
  buffer[bufferIdx++] = HiSysCADBuild;

  for (int i=0; i<FLAGCOUNT; i++)
    buffer[bufferIdx++] = Flags[i];

  buffer[BUFFERSIZE] = BUFFERSIZE;

  int seed = BUFFERSIZE; 

  for (int i=BUFFERSIZE-1; i>=0; i--) // decrypt the buffer.
  {
    seed *= 1103515245;
    seed += 12345; // add some deterministic noise to the system.
    buffer[i] = buffer[i] ^ buffer[i+1] ^ i ^ seed;
  }

  std::fstream fout;
  fout.open(filename, std::ios::out|std::ios::binary);
  if (fout.is_open())
  {    
    fout.write((char*)buffer, BUFFERSIZE*2);
    fout.close();
  }

  delete buffer;
}