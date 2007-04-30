#pragma once

struct DemoDatConfig
{
  static const char EncryptionVersion = 0;

  char SysCADVersion;
  char SysCADBuild;

  static const char FlagCount = 64;
  char Flags[FlagCount];

  DWORD dwCrc32;
};


DemoDatConfig InitialiseDemoDatConfig()
{
  DemoDatConfig config;

  config.SysCADVersion = 0;
  config.SysCADBuild = 0;

  for (int i=0; i<config.FlagCount; i++)
    config.Flags[i] = 0;

  config.dwCrc32 = 0;

  return config;
}


#define DemoDatConfigFlag_ = 0;
