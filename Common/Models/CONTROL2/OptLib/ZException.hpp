#ifndef Z_EXCEPTION_H
#define Z_EXCEPTION_H

#include "ZString.h"

//-----------------------------------------------------------------------------
#ifdef	__BORLANDC__
#include <vcl.h>

#include <stdio.h>
#include <string.h>
#pragma hdrstop

//-----------------------------------------------------------------------------
class ZException : public Exception
{
	public:
		int		ErrorCode;
      ZString  Source;

		//------------------------------------------------------------------------
      ZException(): Exception(AnsiString(""))
      {
      	ErrorCode = -1;
      };

		//------------------------------------------------------------------------
      ZException(const ZString & scMsg, int nCode=-1, const ZString & scSource=""): Exception(AnsiString(scMsg.c_str()))
      {
      	ErrorCode = nCode;
         Source = scSource;
      };

		//------------------------------------------------------------------------
      ZException(const ZString & scMsg, const ZString & scSource): Exception(AnsiString(scMsg.c_str()))
      {
      	ErrorCode = 0;
         Source = scSource;
      };

		//------------------------------------------------------------------------
      ZException(const char* msg, int nCode=-1, const char* szSource=""): Exception(AnsiString(msg))
      {
      	ErrorCode = nCode;
         Source = szSource;
      };

		//------------------------------------------------------------------------
		ZException & operator=(const char* msg)
		{
			Message = msg;
			return *this;
		};
		//------------------------------------------------------------------------
		ZException & operator=(const int code)
		{
			ErrorCode = code;

			return *this;
		};
};

//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
#pragma warning( disable : 4290 )

class ZException
{
	public:
		int		ErrorCode;
		ZString	Message;
      ZString  Source;

		//------------------------------------------------------------------------
      ZException()
      {
			ErrorCode = 0;
			Message = "ZException: Unassigned msg";
      };

		//------------------------------------------------------------------------
      ZException(const char* szMsg, int nCode=-1, const char* szSource="")
      {
			Message = szMsg;
      	ErrorCode = nCode;
         Source = szSource;
      };

		//------------------------------------------------------------------------
      ZException(const char* szMsg, const char* szSource)
      {
			Message = szMsg;
      	ErrorCode = 0;
         Source = szSource;
      };

		//------------------------------------------------------------------------
      ZException(const ZString & scMsg, int nCode=-1, const ZString & scSource="")
      {
			Message = scMsg;
      	ErrorCode = nCode;
         Source = scSource;
      };

		//------------------------------------------------------------------------
      ZException(const ZString & scMsg, const ZString & scSource)
      {
			Message = scMsg;
      	ErrorCode = 0;
         Source = scSource;
      };

		//------------------------------------------------------------------------
		ZException & operator=(const char* szMsg)
		{
			Message = szMsg;

			return *this;
		};
		//------------------------------------------------------------------------
		ZException & operator=(const int nCode)
		{
			ErrorCode = nCode;

			return *this;
		};
};
#endif __BORLANDC__

//-----------------------------------------------------------------------------
#endif Z_EXCEPTION_H