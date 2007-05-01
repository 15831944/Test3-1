#pragma once

#include <fstream>

#include "..\..\..\Common\Scd\ScdLib\Scdver.h"
#include "..\..\..\Common\Scd\ScdLib\RevHist.h"

class EncryptDatConfig
{
public:
  static const short FlagCount = 64;
  static const short PasswordLength = 65; // maximum password length 64 chars and ending null.
  static const short MACAddressLength = 13; // mac address is specified by 12 characters and ending null -- no ':' or '.'
  static const short MACAddressCount = 10; // up to 10 mac addresses allowed.
  static const short BufferSize = FlagCount + 7 * 2 + PasswordLength + MACAddressLength * MACAddressCount;
    // Must be FlagCount + (number of int parameters) + (size of password in char / 2) + (mac address length * number of mac addresses / 2).

  static const short FlagPGMEncrypted      = 0;
  static const short FlagRCTEncrypted      = 1;
  static const short FlagDXFEncrypted      = 2;
  static const short FlagAllowSave         = 3;
  static const short FlagAllowExcelReports = 4;

  unsigned short EncryptionVersion;

  unsigned short BuildSysCADVersion;
  unsigned short LoSysCADVersion;
  unsigned short HiSysCADVersion;

  unsigned short BuildSysCADBuild;
  unsigned short LoSysCADBuild;
  unsigned short HiSysCADBuild;

  char MACAddress[MACAddressCount][MACAddressLength];

  char Password[PasswordLength];

  char Flag[FlagCount];

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

  HiSysCADVersion = SHRT_MAX;
  HiSysCADBuild = SHRT_MAX;

  for (int i=0; i<FlagCount; i++)
    Flag[i] = 0; // Clear all flags.

  // Explicitly set specific default flags.
  Flag[FlagPGMEncrypted] = 1;
  Flag[FlagRCTEncrypted] = 1;
  Flag[FlagDXFEncrypted] = 1;
  Flag[FlagAllowSave] = 0;
  Flag[FlagAllowExcelReports] = 0;

  for (int i=0; i<PasswordLength; i++)
    Password[i] = 0;

  for (int i=0; i<MACAddressCount; i++)
    for (int j=0; j<MACAddressLength; j++)
      MACAddress[i][j] = 0;
}

void EncryptDatConfig::Load(char* filename)
{
  unsigned char *buffera;
  unsigned char *bufferb;
  int length;

  std::fstream fin;
  fin.open(filename, std::ios::in|std::ios::binary);
  if (fin.is_open())
  {    
    fin.seekg(0, std::ios_base::end);
    length = fin.tellg()/2;
    fin.seekg(0, std::ios_base::beg);

    buffera = new unsigned char[length+1];
    bufferb = new unsigned char[length+1];
    fin.read((char*)buffera, length);
    fin.close();

    buffera[length] = length;

    int seed = length; 

    for (int i=length-1; i>=0; i--) // decrypt the buffer.
    {
      seed *= 1103515245;
      seed += 12345; // add some deterministic noise to the system.
      bufferb[i] = buffera[i] ^ buffera[i+1] ^ (unsigned char)i ^ (unsigned char)seed;
    }

    if (length = BufferSize) // only do this if there is the right amount of data in the file.
    {
      int bufferIdx = 0;

      EncryptionVersion =  bufferb[bufferIdx++]*256;
      EncryptionVersion += bufferb[bufferIdx++];


      BuildSysCADVersion =  bufferb[bufferIdx++]*256;
      BuildSysCADVersion += bufferb[bufferIdx++];

      BuildSysCADBuild =  bufferb[bufferIdx++]*256;
      BuildSysCADBuild += bufferb[bufferIdx++];


      LoSysCADVersion =  bufferb[bufferIdx++]*256;
      LoSysCADVersion += bufferb[bufferIdx++];

      LoSysCADBuild =  bufferb[bufferIdx++]*256;
      LoSysCADBuild += bufferb[bufferIdx++];


      HiSysCADVersion =  bufferb[bufferIdx++]*256;
      HiSysCADVersion += bufferb[bufferIdx++];

      HiSysCADBuild =  bufferb[bufferIdx++]*256;
      HiSysCADBuild += bufferb[bufferIdx++];

      for (int i=0; i<FlagCount; i++)
        Flag[i] = bufferb[bufferIdx++];

      for (int i=0; i<PasswordLength; i++)
        Password[i] = bufferb[bufferIdx++];;

      for (int i=0; i<MACAddressCount; i++)
        for (int j=0; j<MACAddressLength; j++)
          MACAddress[i][j] = bufferb[bufferIdx++];;
    }

    delete buffera;
    delete bufferb;
  }
}

void EncryptDatConfig::Save(char* filename)
{
  unsigned char *buffer = new unsigned char[BufferSize+1];

  int bufferIdx = 0;

  buffer[bufferIdx++] = EncryptionVersion / 256;
  buffer[bufferIdx++] = EncryptionVersion % 256;


  buffer[bufferIdx++] = BuildSysCADVersion / 256;
  buffer[bufferIdx++] = BuildSysCADVersion % 256;
   
  buffer[bufferIdx++] = BuildSysCADBuild / 256;
  buffer[bufferIdx++] = BuildSysCADBuild % 256;


  buffer[bufferIdx++] = LoSysCADVersion / 256;
  buffer[bufferIdx++] = LoSysCADVersion % 256;
  
  buffer[bufferIdx++] = LoSysCADBuild / 256;
  buffer[bufferIdx++] = LoSysCADBuild % 256;


  buffer[bufferIdx++] = HiSysCADVersion / 256;
  buffer[bufferIdx++] = HiSysCADVersion % 256;
  
  buffer[bufferIdx++] = HiSysCADBuild / 256;
  buffer[bufferIdx++] = HiSysCADBuild % 256;

  for (int i=0; i<FlagCount; i++)
    buffer[bufferIdx++] = Flag[i];

  for (int i=0; i<PasswordLength; i++)
    buffer[bufferIdx++] = Password[i];

  for (int i=0; i<MACAddressCount; i++)
    for (int j=0; j<MACAddressLength; j++)
      buffer[bufferIdx++] = MACAddress[i][j];

  buffer[BufferSize] = (unsigned char)BufferSize;

  int seed = BufferSize; 

  for (int i=BufferSize-1; i>=0; i--) // decrypt the buffer.
  {
    seed *= 1103515245;
    seed += 12345; // add some deterministic noise to the system.
    buffer[i] = buffer[i] ^ buffer[i+1] ^ (unsigned char)i ^ (unsigned char)seed;
  }

  std::fstream fout;
  fout.open(filename, std::ios::out|std::ios::binary);
  if (fout.is_open())
  {    
    fout.write((char*)buffer, BufferSize*2);
    fout.close();
  }

  delete buffer;
}