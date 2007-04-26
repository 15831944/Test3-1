#include "StdAfx.h"
#include "CalculateDemoKey.h"
#include "crc32static.h"

CalculateDemoKey::CalculateDemoKey(void)
{
}

CalculateDemoKey::~CalculateDemoKey(void)
{
}

void CalculateDemoKey::CRCFiles(char* folder, char* extension, DWORD &dwCrc32)
{
  size_t len = strlen(folder)+4;
  char* Tmp= new char[len];
  strcpy(Tmp, folder);
  strcat(Tmp, "*.*");

  WIN32_FIND_DATA fd;
  HANDLE H = FindFirstFile(Tmp, &fd);

  delete Tmp;

  bool AllDone = (H==INVALID_HANDLE_VALUE); 
  while (!AllDone)
  {
    if ((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0) 
    {
      if ((_stricmp(fd.cFileName, ".")!=0)&&(_stricmp(fd.cFileName, "..")!=0))
      {
        size_t len = strlen(folder)+strlen(fd.cFileName)+2;
        char* temp = new char[len];
        strcpy(temp, folder);
        strcat(temp, fd.cFileName);
        strcat(temp, "\\");

        CRCFiles(temp, extension, dwCrc32);

        delete temp;
      }
    }
    else
    {
      size_t len = strlen(extension)+1;
      char* temp = new char[len];
      strcpy(temp, &fd.cFileName[strlen(fd.cFileName)-strlen(extension)]);
      _strlwr(temp);
      if (_stricmp(temp, extension)==0)
      {
        size_t len = strlen(folder)+strlen(fd.cFileName)+2;
        char* Tmp = new char[len];
        strcpy(Tmp, folder);
        strcat(Tmp, fd.cFileName);

        CCrc32Static::FileCrc32Streams(Tmp, dwCrc32);

        delete Tmp;
      }

      delete temp;
    }
    AllDone = !FindNextFile(H, &fd);
  }
  FindClose(H);
}

DWORD CalculateDemoKey::DoCalculateDemoKey(char* folder)
{    
  DWORD dwCrc32_dxf = 0xD5452A29;
  DWORD dwCrc32_rct = 0x33CAEEBF;
  DWORD dwCrc32_pgm = 0x6E5415D4;
  DWORD dwCrc32_mdb = 0x6A0696B1;
  DWORD dwCrc32_trn = 0x4F4AEE74;
  DWORD dwCrc32_dat = 0xDF3297AC;

  DWORD multiplier_dxf = 0x2BAE3564;
  DWORD multiplier_rct = 0x76777938;
  DWORD multiplier_pgm = 0x2DCD0605;
  DWORD multiplier_mdb = 0xEFA6BF10;
  DWORD multiplier_trn = 0xBD367A8F;
  DWORD multiplier_dat = 0xD4F8A26A;

  CRCFiles(folder, ".dxf", dwCrc32_dxf);
  CRCFiles(folder, ".rct", dwCrc32_rct);
  CRCFiles(folder, ".pgm", dwCrc32_pgm);
  CRCFiles(folder, ".mdb", dwCrc32_mdb);
  CRCFiles(folder, ".trn", dwCrc32_trn);
  CRCFiles(folder, ".dat", dwCrc32_dat);

  CRCFiles(folder, ".dxf.x", dwCrc32_dxf);
  CRCFiles(folder, ".rct.x", dwCrc32_rct);
  CRCFiles(folder, ".pgm.x", dwCrc32_pgm);
  CRCFiles(folder, ".mdb.x", dwCrc32_mdb);
  CRCFiles(folder, ".trn.x", dwCrc32_trn);
  CRCFiles(folder, ".dat.x", dwCrc32_dat);

  return   dwCrc32_dxf * multiplier_dxf
         ^ dwCrc32_rct * multiplier_rct
         ^ dwCrc32_pgm * multiplier_pgm
         ^ dwCrc32_mdb * multiplier_mdb
         ^ dwCrc32_trn * multiplier_trn
         ^ dwCrc32_dat * multiplier_dat;
}

