////////////////////////////////////////////////////////////////
// MSDN Magazine -- June 2006
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 2005 (VC8) on Windows XP. Tab size=3.
//
#include "stdafx.h"
#include "RegexWrap.h"

#define S(s) (gcnew String(s))

//////////////// Capture ////////////////

IMPLEMENT_WRAPPER(Capture, Object)

int CMCapture::Index() const
{
	return (*this)->Index;
}
int CMCapture::Length() const
{
	return (*this)->Length;
}
CString CMCapture::Value() const
{
	return (*this)->Value;
}

//////////////// CaptureCollection ////////////////

IMPLEMENT_COLLECTION(CaptureCollection, Object)

CMCapture CMCaptureCollection::operator[](int i)
{
	try {
		return (*this)->default[i];
	} catch (ArgumentException^ e) {
		throw CMArgumentException(e);
	} catch (Exception^ e) {
		throw CMException(e);
	}
}

//////////////// Group ////////////////

IMPLEMENT_WRAPPER(Group, Capture)

bool CMGroup::Success() const
{
	return (*this)->Success;
}
CMCaptureCollection CMGroup::Captures() const
{
	return (*this)->Captures;
}

//////////////// GroupCollection ////////////////

IMPLEMENT_COLLECTION(GroupCollection, Object)

CMGroup CMGroupCollection::operator[](int i)
{
	return (*this)->default[i];
}
CMGroup CMGroupCollection::operator[](LPCTSTR name)
{
	return (*this)->default[S(name)];
}

//////////////// Match ////////////////

IMPLEMENT_WRAPPER(Match, Group)

#pragma warning( push )
#pragma warning( disable : 4835 )
// one-and-only Empty object
WREXPORT const CMMatch CMMatch::Empty(Match::Empty);
#pragma warning( pop ) 

CMMatch CMMatch::NextMatch() const
{
	return (*this)->NextMatch();
}

CString CMMatch::Result(CString sReplace) const
{
	return (*this)->Result(gcnew String(sReplace));
}

CMGroupCollection CMMatch::Groups() const
{
	return (*this)->Groups;
}

//////////////////
// This class implements a MatchEvaluator delegate that calls an ordinary C++
// callback. The static Create function returns a new instance initialized
// with a given C++ callback fn and void* param.
//
ref class WrapMatchEvaluator
{
protected:
	CMMatch::evaluator _pfn;				 // native callback fn
	void* _param;								 // native param

public:
	WrapMatchEvaluator(CMMatch::evaluator pfn, void* param)
		: _pfn(pfn),_param(param)
	{
	}

	String^ callback(Match^ m)
	{
		return _pfn ? gcnew String((_pfn)(CMMatch(m),_param)) : m->Value;
	}

	static MatchEvaluator^ Create(CMMatch::evaluator pfn, void* param)
	{
		WrapMatchEvaluator^ me = gcnew WrapMatchEvaluator(pfn, param);
		return gcnew MatchEvaluator(me, &WrapMatchEvaluator::callback);
	}
};

//////////////// MatchCollection ////////////////

IMPLEMENT_COLLECTION(MatchCollection, Object)

CMMatch CMMatchCollection::operator[](int i)
{
	try {
		return (*this)->default[i];
	} catch (Exception^ e) {
		throw CMException(e);
	}
}

//////////////// Regex ////////////////

IMPLEMENT_WRAPPER(Regex, Object)

//////////////////
// Wrap construction of Regex object in try/catch to wrap any exception before
// re-throwing it. Regex throws an ArgumentException if the regex is
// ill-formed.
//
Regex^ NewRegex(LPCTSTR s)
{
	try {
		return gcnew Regex(gcnew String(s));
	} catch (Exception^ e) {
		throw CMException(e);
	}
}

CMRegex::CMRegex(LPCTSTR s) : CMObject(NewRegex(s))
{
}

Regex^ NewRegex(LPCTSTR s, CMRegex::Options opt)
{
	try {
		return gcnew Regex(gcnew String(s), static_cast<RegexOptions>(opt));
	} catch (Exception^ e) {
		throw CMException(e);
	}
}

CMRegex::CMRegex(LPCTSTR s, Options opt) : CMObject(NewRegex(s, opt))
{
}

////////////////
// Below are all the wrapper fns. They're so brainless you could write them
// in your sleep.
//
CMMatch CMRegex::Match(LPCTSTR input)
{
	return (*this)->Match(S(input));
}

CMMatch CMRegex::Match(LPCTSTR input, int beg)
{
	return (*this)->Match(S(input), beg);
}

CMMatch CMRegex::Match(LPCTSTR input, int beg, int len)
{
	return (*this)->Match(S(input), beg, len);
}

CMMatch CMRegex::Match(LPCTSTR input, LPCTSTR pattern)
{
	return Regex::Match(S(input), S(pattern));
}

CMMatch CMRegex::Match(LPCTSTR input, LPCTSTR pattern, Options opt)
{
	return Regex::Match(S(input), S(pattern), static_cast<RegexOptions>(opt));
}

bool CMRegex::IsMatch(LPCTSTR input)
{
	return (*this)->IsMatch(S(input));
}

bool CMRegex::IsMatch(LPCTSTR input, int beg)
{
	return (*this)->IsMatch(S(input), beg);
}

bool CMRegex::IsMatch(LPCTSTR input, LPCTSTR pattern)
{
	return Regex::IsMatch(S(input), S(pattern));
}

CMMatchCollection CMRegex::Matches(LPCTSTR input)
{
	return (*this)->Matches(S(input));
}

CMMatchCollection CMRegex::Matches(LPCTSTR input, int beg)
{
	return (*this)->Matches(S(input), beg);
}

CMMatchCollection CMRegex::Matches(LPCTSTR input, LPCTSTR pattern)
{
	return Regex::Matches(S(input), S(pattern));
}

vector<CString> CMRegex::Split(LPCTSTR input)
{
	return wrap_array<CString,String>((*this)->Split(S(input)));
}

vector<CString> CMRegex::Split(LPCTSTR input, int n)
{
	return wrap_array<CString,String>((*this)->Split(S(input), n));
}

vector<CString> CMRegex::Split(LPCTSTR input, LPCTSTR pattern)
{
	return wrap_array<CString,String>(Regex::Split(S(input), S(pattern)));
}

vector<CString> CMRegex::Split(LPCTSTR input, LPCTSTR pattern, Options opt)
{
	return wrap_array<CString,String>(Regex::Split(S(input), S(pattern),
		static_cast<RegexOptions>(opt)));
}

CString CMRegex::Escape(CString s)
{
	return Regex::Escape(S(s));
}

CString CMRegex::Replace(LPCTSTR input, LPCTSTR replace)
{
	return (*this)->Replace(S(input), S(replace));
}

CString CMRegex::Replace(LPCTSTR input, LPCTSTR replace, int n)
{
	return (*this)->Replace(S(input), S(replace), n);
}

CString CMRegex::Replace(LPCTSTR input, LPCTSTR pattern, LPCTSTR replace)
{
	return Regex::Replace(S(input), S(pattern), S(replace));
}

CString CMRegex::Replace(LPCTSTR input, LPCTSTR pattern, LPCTSTR replace, Options opt)
{
	return Regex::Replace(S(input), S(pattern), S(replace),
		static_cast<RegexOptions>(opt));
}

CString CMRegex::Replace(LPCTSTR input, CMMatch::evaluator me, void* lp)
{
	return (*this)->Replace(S(input), WrapMatchEvaluator::Create(me, lp));
}

CString CMRegex::Replace(LPCTSTR input, CMMatch::evaluator me, void* lp, int n)
{
	return (*this)->Replace(S(input), WrapMatchEvaluator::Create(me, lp), n);
}

CString CMRegex::Replace(LPCTSTR input, LPCTSTR pattern, CMMatch::evaluator me, void* lp)
{
	return Regex::Replace(S(input), S(pattern), WrapMatchEvaluator::Create(me, lp));
}

CString CMRegex::Replace(LPCTSTR input, LPCTSTR pattern, CMMatch::evaluator me, void* lp,
	Options opt)
{
	return Regex::Replace(S(input), S(pattern), WrapMatchEvaluator::Create(me, lp),
		static_cast<RegexOptions>(opt));
}

vector<CString> CMRegex::GetGroupNames()
{
	return wrap_array<CString,String>((*this)->GetGroupNames());
}

vector<int> CMRegex::GetGroupNumbers()
{
	return wrap_array<int>((*this)->GetGroupNumbers());
}

CString CMRegex::GroupNameFromNumber(int i)
{
	return (*this)->GroupNameFromNumber(i);
}

int CMRegex::GroupNumberFromName(CString s)
{
	return (*this)->GroupNumberFromName(S(s));
}
