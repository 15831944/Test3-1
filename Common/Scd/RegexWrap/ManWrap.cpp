////////////////////////////////////////////////////////////////
// MSDN Magazine -- June 2006
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 2005 (VC8) on Windows XP. Tab size=3.
//
#include "stdafx.h"
#include "ManWrap.h"

//////////////// Wrap Object, mother of all .NET objects ////////////////

CMObject::CMObject()
{
	// default ctor must be here and not inline, so m_handle is appropriately
	// initialized to a NULL GCHandle. Otherwise it will have unpredictable
	// values since native code sees it as intptr_t
}

CMObject::CMObject(const CMObject& o) : m_handle(o.m_handle)
{
	// See remarks above
}

CMObject& CMObject::operator=(const CMObject& r)
{
	m_handle = r.m_handle;	// copies underlying GCHandle.Target
	return *this;
}

BOOL CMObject::operator==(const CMObject& r) const
{
	return (*this)->Equals(r.ThisObject());
}

CString CMObject::ToString() const
{
	return (*this)->ToString();
}

CString CMObject::TypeName() const
{
	return (*this)->GetType()->Name;
}

BOOL CMObject::IsNull() const
{
	return ThisObject()==nullptr;
}

//////////////// Exception ////////////////

IMPLEMENT_WRAPPER(Exception, Object)

CString CMException::HelpLink() const
{
	return (*this)->HelpLink;
}

CString CMException::Message() const
{
	return (*this)->Message;
}

CString CMException::Source() const
{
	return (*this)->Source;
}

//////////////// ArgumentException ////////////////

IMPLEMENT_WRAPPER(ArgumentException, Exception)

CString CMArgumentException::Message() const
{
	return (*this)->Message;
}

CString CMArgumentException::ParamName() const
{
	return (*this)->ParamName;
}
