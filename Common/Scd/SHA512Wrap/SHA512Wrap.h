////////////////////////////////////////////////////////////////
// MSDN Magazine -- June 2006
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 2005 (VC8) on Windows XP. Tab size=3.
//
// This file declares all the wrappers for RegexWrap. The implementation is
// in RegexWrap.cpp
//
#pragma once
#include "ManWrap.h"
#ifdef _MANAGED
using namespace System::Runtime::InteropServices;
using namespace System::Security::Cryptography;
#endif

//////////////////
// Wrapper for SHA512.
//
class CMSHA512 : public CMObject
{
	DECLARE_WRAPPER(SHA512, Object);
public:
	WREXPORT CMSHA512 Create();
  WREXPORT byte* ComputeHash(byte* buffer, int len);
};