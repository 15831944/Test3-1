// Scribble.h: interface for the CScribble class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIBBLE_H__1D6D99FD_3C2A_44A5_8B18_820B8229B9C9__INCLUDED_)
#define AFX_SCRIBBLE_H__1D6D99FD_3C2A_44A5_8B18_820B8229B9C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
// Scribble License Record that will be added or read from
// the target file or DLL after the EOF marker
// Put whatever we like in here as the scribble routine will write
// the whole data structure as a series of bytes. It knows the number
// of bytes to read or write using the sizeof function
#define OldMethod 0

#if OldMethod
struct ScribbleRecordV1_0
{
  // Scrible DevLic Version Number
  unsigned short major;
  unsigned short minor;
  char           minorchar;

  long          spare;
  unsigned char filename[15]; // for now, Up to 15 chars of DLL Filename
};
#else
struct ScribbleRecordV1_1
{
  // Scrible DevLic Version Number
  unsigned short major;
  unsigned short minor;
  char           minorchar;
  unsigned short build;
  unsigned short spare1;
  long           spare2;
  unsigned char filename[15]; // for now, Up to 15 chars of DLL Filename
};
#endif

class CScribble  
{
public:
	static unsigned short ver_major;
	static unsigned short ver_minor;
	static char           ver_minorchar;
  static unsigned short ver_build;

	static bool ReadScribble( CString &fname );
	static bool AddScribble( CString &fname , bool Replace );
	static const char* GetScribbleVer( CString &VerStr );

  CScribble();
	virtual ~CScribble();

};

#endif // !defined(AFX_SCRIBBLE_H__1D6D99FD_3C2A_44A5_8B18_820B8229B9C9__INCLUDED_)
