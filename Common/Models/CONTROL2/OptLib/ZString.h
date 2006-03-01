#ifndef Z_STRING_HPP
#define Z_STRING_HPP
//=============================================================================
// This is meant to be a Compiler independent version of C++Builder VCL
// AnsiString. In practice it has evolved into a more powerful class and will
// continue to evolve.
//=============================================================================
#ifdef ToHex
#undef ToHex
#endif

#ifdef	__BORLANDC__
#include <vcl.h>
#endif

#include <vector>
#include "TAsciiTypes.h"

//=============================================================================
// CLASS:
//    ZString
//-----------------------------------------------------------------------------
// DESCRIPTION:
//    A fantastic excellent cool string class that is compiler independent.
//=============================================================================
class ZString;
typedef std::vector<ZString> ZStringVect;
enum ZStringSplitMode { smAny,         // New string for each occurrence of any character in the separator string. Two adjacent separator characters result in a null string.
                        smAnyNoNull,   // New string for each group of characters in the separator string. Adjacent separator characters do not result in null strings.
                        smWhole,       // The separator string as a whole is treated as the separator element. Adjacent complete separator strings result in null strings. Case Sensitive.
                        smWholeNoCase};// The separator string as a whole is treated as the separator element. Adjacent complete separator strings result in null strings. Case Insensitive.

//#ifdef OPTIMISATIONLIB_EXPORTS
#define DllImportExport __declspec( dllexport ) 
//#else
//#define DllImportExport __declspec( dllimport )
//#endif

//-----------------------------------------------------------------------------

class  DllImportExport ZString
{
public:
   enum TIntType {itUnknown=0x00,
                  itHex=0x01,
                  itDec=0x02,
                  itOct=0x04,
                  itBin=0x08,
                  itHexOrDec=0x03,
                  itHexOrDecOrOct=0x07,
                  itAny=0x0f,
                  itNotAnInt=0x10};
   //----------
   //

private:
   char	   *m_szStr;
   int	   m_nMemSize;
   int	   m_nStrSize;
   char	   m_cDummyChar;	// used for returning char& on error
   int	   m_nTokenIndex;
public:
      // Constructors
   ZString(void);
   ZString(int initSize, char init);
   ZString(const ZString   &src);
   ZString(const char      *src);
   ZString(char            src);	// single character
   ZString(short           src);
   ZString(unsigned short  src);
   ZString(int             src);
   ZString(unsigned int    src);
   ZString(long            src);
   ZString(unsigned long   src);
   ZString(const __int64   src);
   ZString(const unsigned __int64 src);

   ZString(double          src);

      // Destructor
   ~ZString();

      // Assignments
   ZString &		operator=(const ZString & rhs);
   ZString &		operator=(const char * rhs); // This is effectively available by virtue
                                                // of one of the copy constructors. But I'll
                                                // include it because it is more efficient
   ZString &		operator+=(const ZString & rhs);
   ZString &		operator+=(const char * rhs);

      // Comparisons
   bool				operator==(const ZString & rhs) const;
   bool				operator==(const char *rhs) const;
   bool				operator!=(const ZString & rhs) const;
   bool				operator!=(const char *rhs) const;
   bool           operator>(const ZString & rhs) const;
   bool           operator<(const ZString & rhs) const;

      //	Index
   char &			operator[](int index);
   const char &	operator[](int index) const;

      // Concatenation
   ZString			operator+(const ZString & rhs) const;
   ZString 			operator+(const char * rhs) const;

      // C string operator
   const char *   c_str() const;

      // Query attributes of string
   int				Length(void) const;
   int				Count(void) const;
   bool				IsEmpty(void) const;

   bool           ContainsChars(const ZString &scTestStr) const;
   bool           NotContainsChars(const ZString &scTestStr) const;
   bool           OnlyContainsChars(const ZString &scTestStr) const;

   bool           ContainsStr(const ZString &scTestStr) const;
   bool           NotContainsStr(const ZString &scTestStr) const;

   bool           ContainsType(TAsciiTypes AsciiTypes) const;
   bool           NotContainsType(TAsciiTypes AsciiTypes) const;
   bool           OnlyContainsType(TAsciiTypes AsciiTypes) const;

      // Misc Utilities
   ZString        LowerCase(void) const;
   ZString &      DoLowerCase(void);
   ZString        UpperCase(void) const;
   ZString &      DoUpperCase(void);

   ZString			SubString(int index, int count) const;
   ZString			SubString(int index) const;	// To end of file
   void 				InitToken(void);
   ZString			NextToken(const ZString &delimiters);
   bool           TokenValid(void){return (m_nTokenIndex > 0);}
   int				FirstDelimiter(const ZString &delimiters) const;
   int				LastDelimiter(const ZString &delimiters) const;
   bool				IsPathDelimiter(int index) const;
   int				ToInt(void) const;
   TIntType       IntBaseType(void) const;
   double			ToDouble(void) const;
   bool           Search(  const ZString &Str,
                           int &StartIndex,
                           bool MatchCase=true);
   void           Split(const ZString &scSeperators, ZStringVect & vect, ZStringSplitMode enumMode = smAny) const;
   ZString        Trim();
   ZString &      DoTrim();

   unsigned int   ToHex(void) const;
   unsigned int   ToOctal(void) const;
   unsigned int   ToBinary(void) const;

   static ZString Hex(unsigned int Value, bool Syntax=true, bool LeadingZeros=false, int Width=1);
   static ZString Hex64(unsigned __int64 Value, bool Syntax=true, bool LeadingZeros=false, int Width=1);
   static ZString Octal(unsigned int Value, bool Syntax=true, int Width=1);
   static ZString Binary(unsigned int Value, bool Syntax=true, bool LeadingZeros=false, int Width=1);
      
   ZString &      DoHex(unsigned int Value, bool Syntax=true, bool LeadingZeros=false, int Width=1);
   ZString &      DoHex64(unsigned __int64 Value, bool Syntax=true, bool LeadingZeros=false, int Width=1);
   ZString &      DoOctal(unsigned int Value, bool Syntax=true, int Width=1);
   ZString &      DoBinary(unsigned int Value, bool Syntax=true, bool LeadingZeros=false, int Width=1);

protected:
   void           ReportError(ZString ErrMsg, int Code=-1) const;
   void				ReportError(int ErrorCode) const;

#ifdef __BORLANDC__
public:
   ZString(const AnsiString & scStr);
//   ZString & operator=(const AnsiString & rhs);
   operator AnsiString() const;
#endif __BORLANDC__
};

#undef DllImportExport

//-----------------------------------------------------------------------------
#endif Z_STRING_HPP