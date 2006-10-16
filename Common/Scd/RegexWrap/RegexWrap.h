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
using namespace System::Text::RegularExpressions;
#endif

//////////////////
// Wrapper for .NET Capture class
//
class CMCapture : public CMObject
{
	DECLARE_WRAPPER(Capture, Object);
public:
	// wrapped properties/methods
	WREXPORT int Index() const;
	WREXPORT int Length() const;
	WREXPORT CString Value() const;
};

//////////////////
// Wrapper for CaptureCollection.
//
class CMCaptureCollection : public CMObject
{
	DECLARE_COLLECTION(CaptureCollection, Object);
public:
	WREXPORT CMCapture operator[](int i);
};

//////////////////
// Wrapper for Group.
//
class CMGroup : public CMCapture
{
	DECLARE_WRAPPER(Group, Capture);
public:
	// wrapped properties/methods
	WREXPORT bool Success() const;
	WREXPORT CMCaptureCollection Captures() const;
};

//////////////////
// Wrapper for GroupCollection.
//
class CMGroupCollection : public CMObject
{
	DECLARE_COLLECTION(GroupCollection, Object);
public:
	WREXPORT CMGroup operator[](int i);
	WREXPORT CMGroup operator[](LPCTSTR name);
};

//////////////////
// Wrapper for Match.
//
class CMMatch : public CMGroup
{
	DECLARE_WRAPPER(Match, Group);
public:
	// wrapped properties/methods
	WREXPORT CMMatch NextMatch() const;
	WREXPORT CString Result(CString sReplace) const;
	WREXPORT CMGroupCollection Groups() const;
	WREXPORT static const CMMatch Empty; // constant empty match
	typedef CString (CALLBACK* evaluator)(const CMMatch&, void* param);
};

//////////////////
// Wrapper for MatchCollection
//
class CMMatchCollection : public CMObject
{
	DECLARE_COLLECTION(MatchCollection, Object);
public:
	WREXPORT CMMatch operator[](int i);
};

//////////////////
// Wrapper for Regex. This one has all the good stuff.
//
class CMRegex : public CMObject {
	DECLARE_WRAPPER(Regex, Object);
public:
	enum Options {
		None = 0,
		IgnoreCase = 1,
		Multiline = 2,
		ExplicitCapture = 4,
		Compiled = 8,
		Singleline = 16,
		IgnorePatternWhitespace = 32,
		RightToLeft = 64,
		ECMAScript = 256,
		CultureInvariant = 512,
	};

	WREXPORT CMRegex(LPCTSTR s);
	WREXPORT CMRegex(LPCTSTR s, Options opt);
	WREXPORT CMMatch Match(LPCTSTR input);
	WREXPORT CMMatch Match(LPCTSTR input, int beg);
	WREXPORT CMMatch Match(LPCTSTR input, int beg, int len);
	WREXPORT static CMMatch Match(LPCTSTR input, LPCTSTR pattern);
	WREXPORT static CMMatch Match(LPCTSTR input, LPCTSTR pattern, Options opt);

	WREXPORT bool IsMatch(LPCTSTR input);
	WREXPORT bool IsMatch(LPCTSTR input, int beg);
	WREXPORT static bool IsMatch(LPCTSTR input, LPCTSTR pattern);

	WREXPORT CMMatchCollection Matches(LPCTSTR input);
	WREXPORT CMMatchCollection Matches(LPCTSTR input, int beg);
	WREXPORT static CMMatchCollection Matches(LPCTSTR input, LPCTSTR pattern);

	WREXPORT std::vector<CString> GetGroupNames();
	WREXPORT std::vector<int> GetGroupNumbers();

	WREXPORT CString GroupNameFromNumber(int i);
	WREXPORT int GroupNumberFromName(CString s);

	WREXPORT std::vector<CString> Split(LPCTSTR input);
	WREXPORT std::vector<CString> Split(LPCTSTR input, int n);
	WREXPORT static std::vector<CString> Split(LPCTSTR input, LPCTSTR pattern);
	WREXPORT static std::vector<CString> Split(LPCTSTR input, LPCTSTR pattern, Options opt);

	WREXPORT static CString Escape(CString s);

	WREXPORT CString Replace(LPCTSTR input, LPCTSTR replace);
	WREXPORT CString Replace(LPCTSTR input, LPCTSTR replace, int n);
	WREXPORT static CString Replace(LPCTSTR input, LPCTSTR pattern, LPCTSTR replace);
	WREXPORT static CString Replace(LPCTSTR input, LPCTSTR pattern, LPCTSTR replace, Options opt);
	WREXPORT CString Replace(LPCTSTR input, CMMatch::evaluator me, void* param);
	WREXPORT CString Replace(LPCTSTR input, CMMatch::evaluator me, void* param, int n);
	WREXPORT static CString Replace(LPCTSTR input, LPCTSTR pattern, CMMatch::evaluator me,
		void* param);
	WREXPORT static CString Replace(LPCTSTR input, LPCTSTR pattern, CMMatch::evaluator me,
		void* param, Options opt);
};
