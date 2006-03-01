#include <stdafx.h>
#include <stdio.h>
#include <string.h>

#include "ZString.h"
#include "ZException.hpp"

//#include "TAsciiTypes.cpp"

#ifdef DEBUG_MEMORY
#include "DebugMemory.h"
#endif

//-----------------------------------------------------------------------------
const int MAX_ERROR_CODES = 5;

const int ZSTRING_NO_ERROR                = 0;
const int ZSTRING_INDEX_OUT_OF_RANGE_LOW  = 1;
const int ZSTRING_INDEX_OUT_OF_RANGE_HIGH = 2;
const int ZSTRING_NEW_FAILED              = 3;
const int ZSTRING_TYPE_CONVERT_ERROR      = 4;
const int ZSTRING_FAILED_TO_FIND_INTEGER  = 5;

const char *ZSTRING_ERROR_CODES[MAX_ERROR_CODES+1] = {
         "Internal Error: ZString: unknown error code",
         "Index < 0 in ZString",
         "Index >= Length() in ZString",
         "New failed in ZString",
         "String format invalid for type conversion"};


//#############################################################################
// Constructors

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Default constructor.
//=============================================================================
ZString::ZString(void)
{
   m_szStr = new char [16];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString()");
#endif
   m_szStr[0] = '\0';
   m_nStrSize = 0;
   m_nMemSize = 16;

   m_cDummyChar = '\0';
   m_nTokenIndex = -1;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor with initialisation facility.
//=============================================================================
ZString::ZString(int nInitSize, char cInit)
{
   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   if (nInitSize < 0)
   {
      m_szStr = new char [16];
      if (m_szStr == NULL)
      {
         ReportError(ZSTRING_NEW_FAILED);
         return;
      }
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(int,char)");
#endif
      m_szStr[0] = '\0';
      m_nStrSize = 0;
      m_nMemSize = 16;
      return;
   }

   m_szStr = new char[nInitSize+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(int,char)");
#endif
   m_nMemSize = nInitSize+1;
   m_nStrSize = nInitSize;

   if (cInit != '\0')
      memset(m_szStr, int(cInit), nInitSize);
   m_szStr[m_nStrSize] = '\0';
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Copy constructor.
//=============================================================================
ZString::ZString(const ZString & scSrc)
{
   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   m_szStr = new char[scSrc.m_nStrSize+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const ZString&)");
#endif
   m_nMemSize = scSrc.m_nStrSize+1;
   strcpy(m_szStr, scSrc.m_szStr);
   m_nStrSize = scSrc.m_nStrSize;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a null terminated character string.
//=============================================================================
ZString::ZString(const char *szSrc)
{
   int   nLen = int(strlen(szSrc));

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const char*)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szSrc);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a character.
//=============================================================================
ZString::ZString(char cSrc)
{
   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   m_szStr = new char[16];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const char)");
#endif
   m_nMemSize = 16;
   m_nStrSize = 1;
   m_szStr[0] = cSrc;
   m_szStr[1] = '\0';
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a short.
//=============================================================================
ZString::ZString(short nSrc)
{
   char  szTemp[30];
   int   nLen;

   printf("ZString constructor (const short) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%hd",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const short)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a unsigned short.
//=============================================================================
ZString::ZString(unsigned short nSrc)
{
   char  szTemp[30];
   int   nLen;

   printf("ZString constructor (const unsigned short) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%hu",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(unsigned short)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a int.
//=============================================================================
ZString::ZString(int nSrc)
{
   char  szTemp[30];
   int   nLen;

// printf("ZString constructor (const int) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%d",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(int)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a unsigned int.
//=============================================================================
ZString::ZString(unsigned int nSrc)
{
   char  szTemp[30];
   int   nLen;

// printf("ZString constructor (const unsigned int) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%u",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(unsigned int)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a long.
//=============================================================================
ZString::ZString(long nSrc)
{
   char  szTemp[30];
   int   nLen;

   printf("ZString constructor (const long) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%ld",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(long)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor that accepts a unsigned long.
//=============================================================================
ZString::ZString(unsigned long nSrc)
{
   char  szTemp[30];
   int   nLen;

// printf("ZString constructor (const unsigned long) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%lu",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(unsigned long)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Copy constructor for __int64 data type.
//=============================================================================
ZString::ZString(const __int64 nSrc)
{
   char  szTemp[30];
   int   nLen;

   printf("ZString constructor (const __int64) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%I64d",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const __int64)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Copy constructor for unsigned __int64 data type.
//=============================================================================
ZString::ZString(const unsigned __int64 nSrc)
{
   char  szTemp[30];
   int   nLen;

   printf("ZString constructor (const unsigned __int64) untested\n");

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%I64u",nSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const unsigned __int64)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Constructor for type double.
//=============================================================================
ZString::ZString(double dSrc)
{
   char  szTemp[30];
   int   nLen;

   m_nTokenIndex = -1;
   m_cDummyChar = '\0';
   sprintf(szTemp,"%g",dSrc);
   nLen = int(strlen(szTemp));

   m_szStr = new char[nLen+1];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(double)");
#endif
   m_nMemSize = nLen+1;
   strcpy(m_szStr, szTemp);
   m_nStrSize = nLen;
}

//=============================================================================
// PROCEDURE:
//    ZString::~ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Destructor.
//=============================================================================
ZString::~ZString()
{
   if (m_szStr)
   {
#ifdef DEBUG_MEMORY
      DebugMemDestroy((pvoid)m_szStr, "ZString::~ZString()");
#endif
      delete [] m_szStr;
   }
}

//#############################################################################
// Assignments

//=============================================================================
// PROCEDURE:
//    ZString::operator=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Assignment operator for ZString.
//=============================================================================
ZString & ZString::operator=(const ZString & rhs)
{

   if (!m_szStr || (m_nMemSize < rhs.m_nMemSize))
   {
      if (m_szStr)
      {
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)m_szStr, "ZString::operator=(ZString&)");
#endif
         delete [] m_szStr;
      }
      m_szStr = new char[rhs.m_nStrSize+1];
      if (m_szStr == NULL)
      {
         ReportError(ZSTRING_NEW_FAILED);
         return *this;
      }
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)m_szStr, "ZString::operator=(const ZString &)");
#endif
      m_nMemSize = rhs.m_nStrSize+1;
   }
   if (rhs.m_nStrSize)
      strcpy(m_szStr, rhs.m_szStr);
   else
      m_szStr[0] = '\0';

   m_nStrSize = rhs.m_nStrSize;
   m_nTokenIndex = -1;

   return *this;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Assignment operator for null terminated string.
//=============================================================================
ZString & ZString::operator=(const char * rhs)
{
   int   nLen = int(strlen(rhs));

   if (!m_szStr || (m_nMemSize < (nLen+1)))
   {
      if (m_szStr)
      {
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)m_szStr, "ZString::operator=(char*)");
#endif
         delete [] m_szStr;
      }
      m_szStr = new char[nLen+1];
      if (m_szStr == NULL)
      {
         ReportError(ZSTRING_NEW_FAILED);
         return *this;
      }
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)m_szStr, "ZString::operator=(const char *)");
#endif
      m_nMemSize = nLen+1;
   }
   strcpy(m_szStr, rhs);
   m_nStrSize = nLen;
   m_nTokenIndex = -1;

   return *this;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator+=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    += operator. Adds a ZString to the end of this string and returns this
// string by reference.
//=============================================================================
ZString & ZString::operator+=(const ZString & rhs)
{
   char  *pStr;

   if (m_nMemSize < (m_nStrSize + rhs.m_nStrSize + 1))
   {
      int   nNewSize;

      nNewSize = m_nStrSize + rhs.m_nStrSize + 1;
      pStr = new char[nNewSize];
      if (pStr == NULL)
      {
         ReportError(ZSTRING_NEW_FAILED);
         return *this;
      }
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)pStr, "ZString::operator+=(ZString &)");
#endif
      if (m_szStr)
      {
         strncpy(pStr, m_szStr, m_nMemSize);
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)m_szStr, "ZString::operator+=(ZString&)");
#endif
         delete [] m_szStr;
      }  // IF
      m_szStr = pStr;
      m_nMemSize = nNewSize;
   }
   strcpy(&m_szStr[m_nStrSize], rhs.m_szStr);
   m_nStrSize += rhs.m_nStrSize;

   return *this;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator+=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    += operator. Adds a null terminated string to the end of this string and
// returns this string by reference.
//=============================================================================
ZString & ZString::operator+=(const char * rhs)
{
   char  *pStr;
   int   nLen = int(strlen(rhs));

   if (m_nMemSize < (m_nStrSize + nLen + 1))
   {
      int   nNewSize;

      nNewSize = m_nStrSize + nLen + 1;
      pStr = new char[nNewSize];
      if (pStr == NULL)
      {
         ReportError(ZSTRING_NEW_FAILED);
         return *this;
      }
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)pStr, "ZString::operator+=(char *)");
#endif
      if (m_szStr)
      {
         strncpy(pStr, m_szStr, m_nMemSize);
#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)m_szStr, "ZString::operator+=(char*)");
#endif
         delete [] m_szStr;
      }  // IF
      m_szStr = pStr;
      m_nMemSize = nNewSize;
   }
   strcpy(&m_szStr[m_nStrSize], rhs);
   m_nStrSize += nLen;

   return *this;
}

//#############################################################################
// Comparisons

//=============================================================================
// PROCEDURE:
//    ZString::operator==
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Comparison operator accepting a ZString.
//=============================================================================
bool ZString::operator==(const ZString & rhs) const
{
   if (rhs.m_nStrSize != m_nStrSize)
      return false;

   for (int i = 0; i < m_nStrSize; i++)
      if (m_szStr[i] != rhs.m_szStr[i])
         return false;

   return true;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator==
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Comparison operator accepting a null terminated string.
//=============================================================================
bool ZString::operator==(const char *rhs) const 
{
   int   nLen = int(strlen(rhs));

   if (nLen != m_nStrSize)
      return false;

   int   i = 0;
   while (i < m_nStrSize)
   {
      if (m_szStr[i] != rhs[i])
         return false;
      i++;
   }
   return true;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator!=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Not comparison operator accepting a ZString arguament.
//=============================================================================
bool ZString::operator!=(const ZString & rhs) const
{
   if ((*this) == rhs)
      return false;
   return true;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator!=
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Not comparison operator accepting a null terminated string arguament.
//=============================================================================
bool ZString::operator!=(const char *rhs) const
{
   if ((*this) == rhs)
      return false;
   return true;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator>
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    > operator accepting a ZString arguament.
//=============================================================================
bool ZString::operator>(const ZString & rhs) const
{
   return (strcmp(this->m_szStr, rhs.m_szStr) > 0);
}

//=============================================================================
// PROCEDURE:
//    ZString::operator<
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    < operator accepting a ZString arguament.
//=============================================================================
bool ZString:: operator<(const ZString & rhs) const
{
   return (strcmp(this->m_szStr, rhs.m_szStr) < 0);
}

//=============================================================================
// PROCEDURE:
//    ZString::operator[]
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    [] operator allowing access to individual characters. Starts at index 0.
// This version is for read only acess.
//=============================================================================
const char & ZString::operator[](const int nIndex) const
{
   if (nIndex < 0)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_LOW);
      return m_cDummyChar; // eventually I might generate an exception
   }
   if (nIndex >= m_nStrSize)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_HIGH);
      return m_cDummyChar; // eventually I might generate an exception
   }

   return m_szStr[nIndex];
}

//=============================================================================
// PROCEDURE:
//    ZString::operator[]
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    [] operator allowing access to individual characters. Starts at index 0.
// This version is for modification access.
//=============================================================================
char & ZString::operator[](const int nIndex)
{
   char  *pszStr;

   if (nIndex < 0)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_LOW);
      return m_cDummyChar;
   }
   else if (nIndex >= m_nMemSize-1)
   {
      pszStr = m_szStr;

      m_szStr = new char[nIndex+64];   // add a few extra to avoid continuous new calls.
      if (m_szStr == NULL)
      {
         ReportError(ZSTRING_NEW_FAILED);
         return m_cDummyChar;
      }
#ifdef DEBUG_MEMORY
      DebugMemAllocate((pvoid)m_szStr, "ZString::operator[](int)");
#endif

      if (pszStr)
      {
         strcpy(m_szStr, pszStr);

#ifdef DEBUG_MEMORY
         DebugMemDestroy((pvoid)pszStr, "ZString::operator[](int)");
#endif
         delete [] pszStr;
      }
      m_nMemSize = nIndex+64;

      for (int i = m_nStrSize; i < nIndex; i++)
         m_szStr[i] = char(' ');
      m_szStr[nIndex+1] = '\0';
      m_nStrSize = nIndex+1;
   }

   return m_szStr[nIndex];
}

//#############################################################################
// Concatenation

//=============================================================================
// PROCEDURE:
//    ZString::operator+
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Allows string concatenation. This version accepts a ZString arguament.
//=============================================================================
ZString ZString::operator+(const ZString & rhs) const
{
   ZString  scTemp; 

   scTemp = (*this);
   scTemp += rhs;

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::operator+
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Allows string concatenation. This version accepts a null terminated
// string arguament.
//=============================================================================
ZString ZString::operator+(const char * rhs) const
{
   ZString  scTemp;

   scTemp = (*this);
   scTemp += rhs;

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::c_str
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a const null terminated string. The string pointed to is the
// actual string as stored in the class.
//=============================================================================
const char * ZString::c_str() const
{
   if (m_szStr)
      return m_szStr;
   else
      return NULL;
}

//=============================================================================
// PROCEDURE:
//    ZString::Length
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns the string length.
//=============================================================================
int   ZString::Length() const
{
   return m_nStrSize;
}

//=============================================================================
// PROCEDURE:
//    ZString::Count
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns the string length.
//=============================================================================
int   ZString::Count() const
{
   return m_nStrSize;
}

//=============================================================================
// PROCEDURE:
//    ZString::IsEmpty
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns whether or not the string has zero length.
//=============================================================================
bool  ZString::IsEmpty() const
{
   return (m_nStrSize == 0);
}

//=============================================================================
// PROCEDURE:
//    ZString::ContainsChars
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns whether or not the string contains any of the characters in the
// passed in string scTestStr. Case sensitive.
//    NOT CURRENTLY IMPLEMENTED.
//=============================================================================
bool  ZString::ContainsChars(const ZString &scTestStr) const
{
   throw ZException("ZString::ContainsChars not implemented");
}

//=============================================================================
// PROCEDURE:
//    ZString::NotContainsChars
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns true if the string doesn't contain any of the characters in the
// string passed in. Case sensitive.
//    NOT CURRENTLY IMPLEMENTED.
//=============================================================================
bool  ZString::NotContainsChars(const ZString &scTestStr) const
{
   throw ZException("ZString::NotContainsChars not implemented");
}

//=============================================================================
// PROCEDURE:
//    ZString::OnlyContainsChars
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns whether or not the string only contains one or more characters in
// the passed in string. Case sensitive.
//    NOT CURRENTLY IMPLEMENTED.
//=============================================================================
bool  ZString::OnlyContainsChars(const ZString &scTestStr) const
{
   throw ZException("ZString::OnlyContainsChars not implemented");
}

//=============================================================================
// PROCEDURE:
//    ZString::ContainsStr
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns whether or not the string contains the passed in string. Case 
// sensitive.
//    NOT CURRENTLY IMPLEMENTED.
//=============================================================================
bool  ZString::ContainsStr(const ZString &scTestStr) const
{
   throw ZException("ZString::ContainsStr not implemented");
}

//=============================================================================
// PROCEDURE:
//    ZString::NotContainsStr
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns false if this string contains the passed in string. Case
// Sensitive.
//    NOT CURRENTLY IMPLEMENTED.
//=============================================================================
bool  ZString::NotContainsStr(const ZString &scTestStr) const
{
   throw ZException("ZString::NotContainsStr not implemented");
}

//=============================================================================
// PROCEDURE:
//    ZString::ContainsType
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Indicates the this string contains any of the provided ascii types.
// For example, the following code snipet will indicate whether the
// string contains white space or digits.
//    if (testStr.Contains(atWhiteSpace | atDigits))
//       printf("\"%s\" contains white space and/or digits\n", testStr.c_str());
//=============================================================================
bool  ZString::ContainsType(TAsciiTypes AsciiTypes) const
{
   unsigned int   nMaskedTypes = ((unsigned int)AsciiTypes) & ASCII_TYPES_MASK;
   TAsciiTypes asciiTypes = TAsciiTypes(nMaskedTypes);

   for (int i = 0; i < m_nStrSize; i++)
   {
      int   nCharInt = (int) m_szStr[i];

      if (nCharInt >= 127)
      {
         if (asciiTypes & atNonAscii)
            return true;
      }
      else
      {
         if (asciiTypes & CHAR_TO_ASCII_TYPES[nCharInt])
            return true;
      }
   }  // FOR
   return false;
}

//=============================================================================
// PROCEDURE:
//    ZString::NotContainsType
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Indicates the this string doesn't contain any of the provided ascii types.
//=============================================================================
bool  ZString::NotContainsType(TAsciiTypes AsciiTypes) const
{
   unsigned int   nMaskedTypes = ((unsigned int)AsciiTypes) & ASCII_TYPES_MASK;
   TAsciiTypes asciiTypes = TAsciiTypes(nMaskedTypes);

   for (int i = 0; i < m_nStrSize; i++)
   {
      int   nCharInt = (int) m_szStr[i];

      if (nCharInt >= 127)
      {
         if (asciiTypes & atNonAscii)
            return false;
      }
      else
      {
         if (asciiTypes & CHAR_TO_ASCII_TYPES[nCharInt])
            return false;
      }
   }  // FOR
   return true;
}

//=============================================================================
// PROCEDURE:
//    ZString::OnlyContainsType
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns true if the string only contains the indicated character types.
//=============================================================================
bool ZString::OnlyContainsType(TAsciiTypes AsciiTypes) const
{
   for (int i = 0; i < m_nStrSize; i++)
   {
      int   nCharInt = (int) m_szStr[i];

      if (nCharInt >= 0x7f)
      {
         if (!(AsciiTypes & atNonAscii))
            return false;
      }
      else
      {
         if (!(AsciiTypes & CHAR_TO_ASCII_TYPES[nCharInt]))
            return false;
      }
   }  // FOR
   return true;
}


//#############################################################################
// Misc Utilities

//=============================================================================
// PROCEDURE:
//    ZString::LowerCase
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a lowercase copy of this.
//=============================================================================
ZString ZString::LowerCase(void) const
{
   ZString  scTemp(*this);   // copy constructor

   scTemp.DoLowerCase();

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::DoLowerCase
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts this string to all lowercase.
//=============================================================================
ZString & ZString::DoLowerCase(void)
{
   char  addChar = 'a' - 'A';

   for (int i = 0; i < m_nStrSize; i++)
      if (  (m_szStr[i] >= 'A') &&
            (m_szStr[i] <= 'Z')     )
         m_szStr[i] += addChar;

   return *this;
}

//=============================================================================
// PROCEDURE:
//    ZString::UpperCase
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns an uppercase copy of this.
//=============================================================================
ZString ZString::UpperCase(void) const
{
   ZString  scTemp(*this);   // copy constructor

   scTemp.DoUpperCase();

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::DoUpperCase
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Converts this string to all uppercase.
//=============================================================================
ZString & ZString::DoUpperCase(void)
{
   char  subChar = 'a' - 'A';

   for (int i = 0; i < m_nStrSize; i++)
      if (  (m_szStr[i] >= 'a') &&
            (m_szStr[i] <= 'z')     )
         m_szStr[i] -= subChar;

   return *this;
}

//=============================================================================
// PROCEDURE:
//    ZString::SubString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a substring of the original string. This version accepts a start
// index and a character count. If count passes the end of the string, the
// string from nIndex to the end of the string is returned.
//    If the index is out of range, either a ZException is thrown or if
// ZSTRING_DONT_THROW is defined, an empty string is returned.
//=============================================================================
ZString ZString::SubString(int nIndex, int nCount) const
{
   ZString  scTemp(nCount,' ');  // initial size constructor
   ZString  scNullStr;

   if (nIndex < 0)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_LOW);
      return scNullStr; // empty string
   }  // IF

   //------------
   // If we index passed the end of the string, generate an error.
   //------------
   if (nIndex >= m_nStrSize)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_HIGH);
      return scNullStr; // empty string
   }  // IF

   //------------
   // If the requested substring goes passed the end of the string,
   // we limit the count to being within the string.
   //------------
   if ((nIndex + nCount) > m_nStrSize)
      nCount = m_nStrSize - nIndex;

   if (nCount < 0)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_LOW);
      return scNullStr; // empty string
   }  // IF

   memcpy(scTemp.m_szStr, &(m_szStr[nIndex]), nCount);
   scTemp.m_szStr[nCount] = '\0';
   scTemp.m_nStrSize = nCount;

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::SubString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a substring of the original string. This version accepts a start
// index and returns from the start index to the end of the string.
//    If the index is out of range, either a ZException is thrown or if
// ZSTRING_DONT_THROW is defined, an empty string is returned.
//=============================================================================
ZString ZString::SubString(int nIndex) const
{
   ZString  scTemp(m_nStrSize, '\0'); // initial size constructor

   if (nIndex < 0)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_LOW);
      return (scTemp=""); // empty string
   }  // IF

   if (nIndex > m_nStrSize)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_HIGH);
      return (scTemp=""); // empty string
   }  // IF

   if (nIndex == m_nStrSize)
      return (scTemp=""); // empty string

   scTemp = &(m_szStr[nIndex]);

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::InitToken
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Used to initialise the Token index for the NextToken procedure to the
// start.
//=============================================================================
void ZString::InitToken(void)
{
   m_nTokenIndex = -1;
}

//=============================================================================
// PROCEDURE:
//    ZString::NextToken
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure is meant to reproduce the functionality of the strtok
// system procedure. Each call returns the next token separated by the current
// delimiter. After each call, the token Index points to the first character
// after the current token. This has implications if the delimiter string
// is altered between calls.
//=============================================================================
ZString ZString::NextToken(const ZString &scDelimiters)
{
   int      nDelimLen = scDelimiters.m_nStrSize;
   int      nIndex = m_nTokenIndex+1;
   ZString  scTemp;
   int      nDelim;

   //--------------
   // Are we currently passed the end of the string?
   //--------------
   if (m_nTokenIndex >= m_nStrSize)
   {
      scTemp = "";
      m_nTokenIndex = -1;
      return scTemp;
   }

   // first we keep incrementing the m_nTokenIndex index until the character
   // pointed to is not a in the delimiter string.
   while (nIndex < m_nStrSize)
   {
      nDelim = 0;
      bool  delimFound = false;
      while (nDelim < nDelimLen)
      {
         if (scDelimiters.m_szStr[nDelim] == m_szStr[nIndex])
         {
            delimFound = true;
            break;
         }
         nDelim++;
      }  // WHILE
      if (!delimFound)
         break;
      nIndex++;
   }  // WHILE

   //--------------
   // Have we run off the end of the string?
   //--------------
   if (nIndex >= m_nStrSize)
   {  // then we have reached the end of the string
      scTemp = "";
      m_nTokenIndex = -1;  // Reset to start again.
      return scTemp;
   }

   //--------------
   // Where still here so we must have a valid token
   //--------------
   int nStartIndex = nIndex;
   while (nIndex < m_nStrSize)
   {
      nDelim = 0;
      while (nDelim < nDelimLen)
      {
         if (scDelimiters.m_szStr[nDelim] == m_szStr[nIndex])
         {
            scTemp = SubString(nStartIndex, nIndex - nStartIndex);
            m_nTokenIndex = nIndex;
            return scTemp;
         }
         nDelim++;
      }  // WHILE
      nIndex++;
   }  // WHILE

   // If we got here we ran out of string. Thus from nStartIndex to the end
   // of the string is the last token in the string
   scTemp = SubString(nStartIndex, nIndex - nStartIndex);
   m_nTokenIndex = m_nStrSize;
   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::FirstDelimiter
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Searches for any of the characters in the passed in string, returning
// the index of the first character found, or -1 if none found.
//=============================================================================
int ZString::FirstDelimiter(const ZString &scDelimiters) const
{
   int   nDelimLen;
   int   nIndex = 0;
   int   nDelim;

   nDelimLen = scDelimiters.m_nStrSize;

   while (nIndex < m_nStrSize)
   {
      nDelim = 0;
      while (nDelim < nDelimLen)
      {
         if (scDelimiters.m_szStr[nDelim] == m_szStr[nIndex])
            return nIndex;
         nDelim++;
      }  // WHILE
      nIndex++;
   }  // WHILE

   return -1;
}

//=============================================================================
// PROCEDURE:
//    ZString::LastDelimiter
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Searches backwards for any of the characters in the passed in string,
// returning the index of the first character found (i.e the last occurrence
// in the string), or -1 if none found.
//=============================================================================
int ZString::LastDelimiter(const ZString &scDelimiters) const
{
   int   nDelimLen;
   int   nIndex = m_nStrSize-1;
   int   nDelim;

   nDelimLen = scDelimiters.m_nStrSize;

   while (nIndex > 0)
   {
      nDelim = 0;
      while (nDelim < nDelimLen)
      {
         if (scDelimiters.m_szStr[nDelim] == m_szStr[nIndex])
            return nIndex;
         nDelim++;
      }  // WHILE
      nIndex--;
   }  // WHILE

   return -1;
}

//=============================================================================
// PROCEDURE:
//    ZString::IsPathDelimiter
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns true if the character at index nIndex is either "\\" or "/".
// On error either throws a ZException or if ZSTRING_DONT_THROW is defined,
// returns false.
//=============================================================================
bool ZString::IsPathDelimiter(int nIndex) const
{
   if (nIndex < 0)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_LOW);
      return false;
   }  // IF

   if (nIndex >= m_nStrSize)
   {
      ReportError(ZSTRING_INDEX_OUT_OF_RANGE_HIGH);
      return false;
   }  // IF

   return ( (m_szStr[nIndex] == '\\') || (m_szStr[nIndex] == '/') );
}

//=============================================================================
// PROCEDURE:
//    ZString::ToInt
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Attempts to convert the this string to an integer. 
// On error, throws a ZException or if ZSTRING_DONT_THROW is defined, returns
// 0.
//=============================================================================
int ZString::ToInt(void) const
{
   int               nValue = 0;
   ZString           scNum = *this;
   ZString::TIntType intBaseType;
   bool              bSignChar = false;
   bool              bNegative = false;

   scNum.DoTrim();

   //-----------
   // Handle sign character.
   //-----------
   if (scNum[0] == '+')
   {
      scNum[0] = ' ';
      scNum.DoTrim();
      bSignChar = true;
   }
   else if (scNum[0] == '-')
   {
      scNum[0] = ' ';
      scNum.DoTrim();
      bNegative = true;
      bSignChar = true;
   }

   //-----------
   // Work out the type of the number.
   //-----------
   intBaseType = scNum.IntBaseType();

   if ((intBaseType == itOct) && bSignChar)
   {
      //------------
      // Then the first characters of the string were something like
      // +0456 which after stripping the + looks like an octal number.
      // However clearly it must be interpreted as a decimal integer.
      //------------
      intBaseType = itDec;
   }

   switch (intBaseType)
   {
      case itHex:
         if (sscanf(scNum.c_str(), "%x", &nValue) != 1)
         {
            ReportError("ToInt(): Incorrect Hex format");
            return 0;
         }
         break;

      case itOct:
         if (sscanf(scNum.c_str(), "%o", &nValue) != 1)
         {
            ReportError("ToInt(): Incorrect Octal format");
            return 0;
         }
         break;

      case itBin:
         try
         {
            nValue = ToBinary();
         }
         catch(...)
         {
            ReportError("ToInt(): Incorrect Binary format");
            return 0;
         }
         break;

      case itUnknown:
      case itNotAnInt:
         ReportError("ToInt(): String is not an integer");
         return 0;

      case itDec:
      default:
         if (sscanf(scNum.c_str(), "%d", &nValue) != 1)
         {
            ReportError("ToInt(): Incorrect Decimal format");
            return 0;
         }
         if (bNegative)
            nValue*=-1;
         break;
   }  // SWITCH

   if (bNegative)
   {
      switch (intBaseType)
      {
         case itHex:
            ReportError("ToInt(): Negative sign found with hexadecimal number");
            return 0;

         case itOct:
            ReportError("ToInt(): Negative sign found with octal number");
            return 0;

         case itBin:
            ReportError("ToInt(): Negative sign found with binary number");
            return 0;

         default:
            break;
      }  // SWITCH
   }  // IF

   return nValue;
}

//=============================================================================
// PROCEDURE:
//    ZString::IntBaseType;
//-----------------------------------------------------------------------------
// DECRIPTION:
//    Tries to determine the base type of the integer stored in *this.
//=============================================================================
ZString::TIntType ZString::IntBaseType(void) const
{
   int   nStart;
   int   nEnd;

   if (m_nStrSize < 1)
      return itNotAnInt;

   ZString  scTemp = (*this).UpperCase().Trim();

   bool bCharABCDEFFound = false;
   bool bChar89Found     = false;
   bool bChar234567Found = false;
   bool bChar01Found     = false;

   bool bHexSyntax       = false;
   bool bOctalSyntax     = false;
   bool bBinSyntax       = false;

   nStart = 0;
   nEnd = scTemp.Count();
   if (scTemp.SubString(0,2) == "0X")
   {
      bHexSyntax = true;
      nStart = 2;
   }
   else if (scTemp.SubString(0,1) == "0")
   {
      bOctalSyntax = true;
      nStart = 1;
   }

   if (scTemp[scTemp.Count()-1] == 'B')
   {
      bBinSyntax = true;
      bOctalSyntax = false;
      nStart = 0;
      nEnd--;
   }

   if (bBinSyntax && bHexSyntax)
      return itNotAnInt;

   if ((nEnd-nStart) == 0)
   {
      if (bOctalSyntax)
      // Then string is "0". i.e. any base type
         return itAny;
      else
         return itNotAnInt;
   }

   //-----------
   // Now we search through the string checking for any inconsistencies
   //-----------
   for (int i = nStart; i < nEnd; i++)
   {
      //--------------
      // Part of the ASCII character set sequence is:
      //    -./0123456789:;<=>?@ABCDEFGHIJK
      //--------------
      char  achar = scTemp[i];

      if (  (achar < '0'       ) ||
            (achar > 'F'       ) ||
            (  (achar > '9') &&
               (achar < 'A')   )    )
      {
         return itNotAnInt;
      }
      if (achar < '2')
         bChar01Found = true;
      else if (achar < '8')
         bChar234567Found = true;
      else if (achar < 'A')
         bChar89Found = true;
      else
         bCharABCDEFFound = true;
   }

   //------------
   // Now for the logic based on the combinations of Syntax and characters found.
   //------------

   //------------
   // Is it Hex?
   //------------
   if (bHexSyntax)
      return itHex;
   else if (bCharABCDEFFound)
   {
      if (!bBinSyntax)
         return itHex;
      else
         return itNotAnInt;
   }
   else if (bChar89Found)
   {
      if (!bBinSyntax)
         return itHexOrDec;
      else
         return itNotAnInt;
   }
   else if (bChar234567Found)
   {
      if (bBinSyntax)
         return itNotAnInt;
      else if (bOctalSyntax)
         return itOct;
      else
         return itHexOrDecOrOct;
   }
   else if (bChar01Found)
   {
      if (bBinSyntax)
         return itBin;
      else
         return itAny;
   }
   else
      return itNotAnInt;
}

//=============================================================================
// PROCEDURE:
//    ZString::ToDouble
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Attempts to convert the this string to a double. 
// On error, throws a ZException or if ZSTRING_DONT_THROW is defined, returns
// 0.
//=============================================================================
double ZString::ToDouble(void) const
{
   double   dValue;

   if (sscanf(m_szStr, "%lg", &dValue) != 1)
   {
      ReportError(ZSTRING_TYPE_CONVERT_ERROR);
      return 0;
   }
   return dValue;
}

//=============================================================================
// PROCEDURE:
//    ZString::Search
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Searches this for a passed in sub-string. Returns the success or
// otherwise of the search. The search can be case sensitive or not.
//=============================================================================
bool ZString::Search(const ZString &scStr,
                     int &nStartIndex,
                     const bool bMatchCase)
{
   int   nInCount = scStr.m_nStrSize;

   if (nInCount > m_nStrSize)
      return false;

   if (bMatchCase)
   {
      for (nStartIndex = 0; nStartIndex <= (m_nStrSize-nInCount); nStartIndex++)
      {
         if (this->SubString(nStartIndex, nInCount) == scStr)
            return true;
      }
      return false;
   }
   else
   {
      ZString  scUpperCaseStr = scStr.UpperCase();
      ZString  scUpperCaseThis = this->UpperCase();


      for (nStartIndex = 0; nStartIndex <= (m_nStrSize-nInCount); nStartIndex++)
      {
         if (scUpperCaseThis.SubString(nStartIndex,nInCount) == scUpperCaseStr)
            return true;
      }
      return false;
   }
}

//=============================================================================
// PROCEDURE:
//    ZString::Split
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a vector of strings representing the original string broken up
// according to the separators. There are three modes of operation:
// * smAny - New string for each occurrence of any character in the separator 
//       string. Two adjacent separator characters result in a null string.
// * smAnyNoNull - New string for each group of characters in the separator
//       string. Adjacent separator characters do not result in null strings.
// * smWhole - The separator string as a whole is treated as the separator 
//       element. Adjacent complete separator strings result in null strings.
//=============================================================================
void ZString::Split(const ZString &scSeparators, 
                    ZStringVect & vect, 
                    ZStringSplitMode enumMode) const
{
   int   i;
   bool  bMatchCase = false;
   int   nStartIndex = 0;

   vect.clear();

   if (scSeparators.Length() == 0)
   {
      vect.insert(vect.end(), *this);
      return;
   }

   ZString  scLine;

   switch (enumMode)
   {
   case smAny:
   case smAnyNoNull:
      for (i = 0; i < m_nStrSize; i++)
      {
         bool  bSeparatorFound = false;
         for (int j = 0; j < scSeparators.m_nStrSize; j++)
         {
            if (this->m_szStr[i] == scSeparators.m_szStr[j])
            {
               bSeparatorFound = true;
               break;
            }
         }  // FOR
         if (bSeparatorFound)
         {
            if (enumMode == smAnyNoNull)
            {
               if (i == nStartIndex)
               {
                  nStartIndex = i + 1;
                  continue;
               }
            }

            scLine = this->SubString(nStartIndex, i - nStartIndex);
            vect.insert(vect.end(), scLine);
            nStartIndex = i + 1;
         }
      }  // FOR
      if (nStartIndex < i)
      {
         scLine = this->SubString(nStartIndex, i - nStartIndex);
         vect.insert(vect.end(), scLine);
      }
      break;

   case smWhole:
      bMatchCase = true;
   case smWholeNoCase:
      {
         int   nStartIndex = 0;
         ZString  scWorking = *this;
         while (1)
         {
            if (!scWorking.Search(scSeparators, nStartIndex, bMatchCase))
            {
               vect.insert(vect.end(), scWorking);
               return;
            }
            // If we get to here we have found our string inside the scWorking
            // string.
            // First we add the segment of the string before the search string...
            scLine = scWorking.SubString(0, nStartIndex);
            vect.insert(vect.end(), scLine);
            // Then we remove the pre-search string segment plus the search
            // string itself from the working string and continue searching.
            scWorking = scWorking.SubString(nStartIndex + scSeparators.Length());
         }  // WHILE
      }
   }  // SWITCH
}

//=============================================================================
// PROCEDURE:
//    ZString::Trim
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Returns a trimmed copy of this. That is with white space removed from the
// start and the end.
//=============================================================================
ZString ZString::Trim()
{
   ZString  scTemp = *this;

   scTemp.DoTrim();

   return scTemp;
}

//=============================================================================
// PROCEDURE:
//    ZString::DoTrim
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Trim this. That is remove white space at start and end.
//=============================================================================
ZString &ZString::DoTrim()
{
   int   i;

   if (m_nStrSize == 0)
      return *this;

   //------------
   // Find the last non white-space character.
   //------------
   i  = m_nStrSize-1;
   while (i >= 0)
   {
      if (  (m_szStr[i] != ' ' ) && 
            (m_szStr[i] != '\t')   )
         break;
      i--;
   }
   m_nStrSize = i+1;
   m_szStr[i+1]='\0';

   //------------
   // Find the first non white-space character.
   //------------
   i = 0;
   while (i < m_nStrSize)
   {
      if (  (m_szStr[i] != ' ' ) &&
            (m_szStr[i] != '\t')   )
         break;
      i++;
   }
   if (i != 0)
   {
      int   n = i;

      m_nStrSize = m_nStrSize - i;
      for (i = 0; i < m_nStrSize; i++)
         m_szStr[i] = m_szStr[i+n];

      m_szStr[m_nStrSize] = '\0';
   }

   return *this;
}

//=============================================================================
//=============================================================================
// Integer base variation utility functions.
// i.e. allows conversion to and from Hex, Octal and Binary.
//=============================================================================
//=============================================================================

//=============================================================================
// PROCEDURE:
//    ZString::ToHex
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure interprets the 'this' string as a hex number,
// returning its value (as an unsigned int).
// Generates an error (usually an exception) if it fails.
//=============================================================================
unsigned int ZString::ToHex(void) const
{
   unsigned int nValue;

   if (sscanf(m_szStr, "%x", &nValue) != 1)
   {
      ReportError("ToHex:String is not a valid hexadecimal number");
      return -1;
   }
   return nValue;
}

//=============================================================================
// PROCEDURE:
//    ZString::ToOctal
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure interprets the 'this' string as a octal number,
// returning its value (as an unsigned int).
// Generates an error (usually an exception) if it fails.
//=============================================================================
unsigned int ZString::ToOctal(void) const
{
   unsigned int nValue;

   if (sscanf(m_szStr, "%o", &nValue) != 1)
   {
      ReportError("ToOctal:String is not a valid octal number");
      return -1;
   }
   return nValue;
}

//=============================================================================
// PROCEDURE:
//    ZString::ToBinary
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This procedure interprets the 'this' string as a binary number,
// returning its value (as an unsigned int).
// It can handle the string whether or not it includes the "b" suffix.
// Generates an error (usually an exception) if it comes across an unexpected
// characters.
//=============================================================================
unsigned int ZString::ToBinary(void) const
{
   int      nBit;
   int      nValue = 0;
   int      nMultiplier = 1;
   int      nStrLength;
   ZString  scLocal = *this;

   scLocal.DoTrim();

   //----------
   // Remove the trailing 'b' character if it is there.
   //----------
   if (  (scLocal[scLocal.Count()-1] == 'b') ||
         (scLocal[scLocal.Count()-1] == 'B')   )
      scLocal = scLocal.SubString(0, scLocal.Count()-1);

   nStrLength = scLocal.Length();

   for (nBit = 0; nBit < nStrLength; nBit++)
   {
      if (scLocal[nStrLength-nBit-1] == '1')
         nValue += nMultiplier;
      else if (scLocal[nStrLength-nBit-1] != '0')
      {
         ReportError("ToBinary(void):Rogue character found in string");
         return -1;
      }
      nMultiplier <<= 1;
   }

   return nValue;
}

//=============================================================================
// PROCEDURE:
//    static ZString::Hex
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This static method accepts an unsigned int and returns a hexadecimal
// string.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0x".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string. 
//=============================================================================
ZString ZString::Hex(unsigned int Value, bool Syntax, bool LeadingZeros, int Width)
{
   ZString  scTemp;

   return scTemp.DoHex(Value, Syntax, LeadingZeros, Width);
}

//=============================================================================
// PROCEDURE:
//    static ZString::Hex64
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This static method accepts an unsigned __int64 and returns a hexadecimal
// string.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0x".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string. 
//=============================================================================
ZString ZString::Hex64(unsigned __int64 Value, bool Syntax, bool LeadingZeros, int Width)
{
   ZString  scTemp;

   return scTemp.DoHex64(Value, Syntax, LeadingZeros, Width);
}

//=============================================================================
// PROCEDURE:
//    static ZString::Octal
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This static method accepts an unsigned int and returns an octal
// string.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0x".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string.
//=============================================================================
ZString ZString::Octal(unsigned int Value, bool Syntax, int Width)
{
   ZString  scTemp;

   return scTemp.DoOctal(Value, Syntax, Width);
}

//=============================================================================
// PROCEDURE:
//    static ZString::Binary
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This static method accepts an unsigned int and returns a binary
// string.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0x".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string.
//=============================================================================
ZString ZString::Binary(unsigned int Value, bool Syntax, bool LeadingZeros, int Width)
{
   ZString  scTemp;

   return scTemp.DoBinary(Value, Syntax,LeadingZeros, Width);
}

//=============================================================================
// PROCEDURE:
//    ZString::DoHex
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This method accepts an unsigned int and overwrites *this with a
// hexadecimal string.
// It returns a reference to *this.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0x".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string.
//=============================================================================
ZString & ZString::DoHex(  unsigned int Value, bool Syntax,
                           bool LeadingZeros, int Width)
{
   char  szFormat[20];
   char  szTemp[100];
   char  szPrefix[5];

   if (Syntax)
      sprintf(szPrefix, "0x");
   else
      szPrefix[0] = '\0';

   if (LeadingZeros)
      sprintf(szFormat, "%s%c0%d%c", szPrefix, '%', Width, 'x');
   else
      sprintf(szFormat, "%s%c%d%c", szPrefix, '%', Width, 'x');

   sprintf(szTemp, szFormat, Value);

   (*this) = szTemp;

   return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZString::DoHex64
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This method accepts an unsigned __int64 and overwrites *this with a
// hexadecimal string.
// It returns a reference to *this.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0x".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string.
//=============================================================================
ZString & ZString::DoHex64(unsigned __int64 Value, bool Syntax,
                           bool LeadingZeros, int Width)
{
   char  szFormat[20];
   char  szTemp[100];
   char  szPrefix[5];

   if (Syntax)
      sprintf(szPrefix, "0x");
   else
      szPrefix[0] = '\0';

   if (LeadingZeros)
      sprintf(szFormat, "%s%c0%d%c", szPrefix, '%', Width, 'I64x');
   else
      sprintf(szFormat, "%s%c%d%c", szPrefix, '%', Width, 'I64x');

   sprintf(szTemp, szFormat, Value);

   (*this) = szTemp;

   return (*this);
}


//=============================================================================
// PROCEDURE:
//    ZString::DoOctal
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This method accepts an unsigned int and overwrites *this with an
// octal string.
// It returns a reference to *this.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the prefix "0". (character representing number 0).
//    Width - the minimum number of digits in the return string.
//=============================================================================
ZString & ZString::DoOctal(unsigned int Value, bool Syntax, int Width)
{
   char  szTemp[100];

   if (Syntax)
      sprintf(szTemp, "0%o", Value);
   else
      sprintf(szTemp, "%o", Value);

   (*this) = szTemp;

   return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZString::DoBinary
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This method accepts an unsigned int and overwrites *this with a
// binary string.
// It returns a reference to *this.
//-----------------------------------------------------------------------------
// PARAMETERS:
//    Value - The integer source of the string.
//    Syntax - If true, include the suffix "b".
//    LeadingZeros - If true and the Width parameter is greater than the minimum
//       digits required to represent Value, then leading zeros are inserted
//       instead of spaces.
//    Width - the minimum number of digits in the return string.
//=============================================================================
ZString & ZString::DoBinary(  unsigned int Value, bool Syntax,
                              bool LeadingZeros, int Width)
{
   ZString        scTemp;
   unsigned int   nCompare = 0x80000000;
   int            nBit = 31;
   int            nWidth;
   bool           bHighOneFound;

   nWidth = Width;
   if (nWidth < 1)
      nWidth = 1;

   //-------------
   // Handle zero case
   //-------------
   if (Value == 0)
   {
      char  cInit = ' ';
      if (LeadingZeros)
         cInit = '0';
      ZString  scZero(Width+1, cInit);

      scZero[Width] = 'b';
      scZero[Width-1] = '0';
      *this = scZero;
      return *this;
   }

   //---------
   // Find the highest nonzero bit.
   //---------
   while (!(nCompare & Value) && (nBit >= 0))
   {
      nCompare >>= 1;
      nBit--;
   }

  if (nWidth < (nBit+1))
      nWidth = nBit+1;
   nCompare = 0x00000001 << (nWidth-1);

   bHighOneFound = false;
   for (nBit = nWidth-1; nBit >= 0; nBit--)
   {
      if (nCompare & Value)
      {
         scTemp += "1";
         bHighOneFound = true;
      }
      else if (!bHighOneFound && !LeadingZeros)
         scTemp += " ";
      else
         scTemp += "0";
      nCompare >>= 1;
   }

   if (!bHighOneFound && !LeadingZeros)
      scTemp += "0";

   if (Syntax)
      scTemp += "b";

   *this = scTemp;

   return (*this);
}

//=============================================================================
// PROCEDURE:
//    ZString::ReportError
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Local procedure that chooses between throwing an exception or printing
// a command line message. This version takes two arguaments, a string and a
// error code.
//=============================================================================
void ZString::ReportError(ZString scErrMsg, int nErrorCode) const
{
   if (nErrorCode > 0)
      scErrMsg = ZString("ZString Error ") + ZString(nErrorCode) + ZString(": ") + scErrMsg;
   else
      scErrMsg = ZString("ZString Error: ") + scErrMsg;

#if defined(ZSTRING_DONT_THROW)
   // This version is for Visual C++ Win32. Eventually we'll throw an exception
   printf("%s", scErrMsg.c_str());
#else
   throw(ZException(scErrMsg.c_str(), nErrorCode));
#endif
}

//=============================================================================
// PROCEDURE:
//    ZString::ReportError
//-----------------------------------------------------------------------------
// DESCRIPTION:
//
//=============================================================================
void ZString::ReportError(int nErrorCode) const
{
   if (nErrorCode == ZSTRING_NO_ERROR)
      return;

   if (nErrorCode > MAX_ERROR_CODES)
      ReportError(ZSTRING_ERROR_CODES[0], -1);
   else
      ReportError(ZSTRING_ERROR_CODES[nErrorCode], nErrorCode);
}

#ifdef __BORLANDC__
//=============================================================================
// PROCEDURE:
//    ZString::ZString(const AnsiString &)
//-----------------------------------------------------------------------------
// DESCRIPTION:
//
//=============================================================================
ZString::ZString(const AnsiString & scStr)
{
   m_szStr = new char [16];
   if (m_szStr == NULL)
   {
      ReportError(ZSTRING_NEW_FAILED);
      return;
   }
#ifdef DEBUG_MEMORY
   DebugMemAllocate((pvoid)m_szStr, "ZString::ZString(const AnsiString &)");
#endif

   m_szStr[0] = '\0';
   m_nStrSize = 0;
   m_nMemSize = 16;

   m_cDummyChar = '\0';
   m_nTokenIndex = -1;

   this->operator=(scStr.c_str());
}
#endif __BORLANDC__

/*
#ifdef __BORLANDC__
//=============================================================================
// PROCEDURE:
//    ZString::operator=(const AnsiString &
//-----------------------------------------------------------------------------
// DESCRIPTION:
//
//=============================================================================
ZString & ZString::operator=(const AnsiString & rhs)
{
   this->operator=(rhs.c_str());

   return *this;
}
#endif __BORLANDC__
*/

#ifdef __BORLANDC__
//=============================================================================
// PROCEDURE:
//    ZString::operator AnsiString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//
//=============================================================================
ZString::operator AnsiString() const
{
   AnsiString  scReturn;

   scReturn = this->c_str();

   return scReturn;
}
#endif __BORLANDC__
