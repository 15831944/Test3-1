#pragma once

class CalculateDemoKey
{
public:
  CalculateDemoKey(void);
  ~CalculateDemoKey(void);

  static void CRCFiles(char* folder, char* extension, DWORD &dwCrc32);
  static void CalculateDemoKey::CalculateKey(char* folder, char* extension, DWORD multiplier, DWORD& key);
  static DWORD DoCalculateDemoKey(char* folder);
};
