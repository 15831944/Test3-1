////////////////////////////////////////////////////////////////
// MSDN Magazine -- June 2006
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 2005 (VC8) on Windows XP. Tab size=3.
//
#include "stdafx.h"
#include "SHA512Wrap.h"

#define S(s) (gcnew String(s))

//////////////// SHA512 ////////////////

IMPLEMENT_WRAPPER(SHA512, HashCollection)

CMSHA512 CMSHA512::Create()
{
	return (*this)->Create();
}

byte* CMSHA512::ComputeHash(byte* buffer, int len)
{ 
  // Requires some manipulation to get between c++ and .net constructs.
  array<Byte> ^byteArray = gcnew array<Byte>(len + 2);
  Marshal::Copy( (IntPtr)buffer, byteArray, 0, len );
  pin_ptr<Byte> p =  &(*this)->ComputeHash(byteArray)[0];
  return p;
}