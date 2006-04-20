//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TKNPARSE_H
#define __TKNPARSE_H

#ifdef __TKNPARSE_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//----------------------------------------------------------------------------
#ifndef __TKNPARS_H
/*#G:*/
enum TokenTypes { //list of possible token types that may be returned
  TokNumeric, 
  TokAlpha, 
  TokComment, 
  TokSymbol, 
  TokWhiteSpace, 
  TokNull, 
  TokOther, 
  TokString };
#endif

const int MaxLineLen = 4096;       //maximum length of a line in the files being parsed
const int MaxBackLineLen = 512;
const int MaxWhiteSpace = 16;
const int MaxSeperators = 128;
const int MaxIncludeFiles = 8;     //maximum "depth" of include files
const int MaxTtlIncludeFiles = 32; //maximum number of include files used
//#E

//----------------------------------------------------------------------------

typedef void (*fn_IncLoadFail)(char*, void*);

/*#C:#D:This class is used as a general text file parser. Tokens are returned 
according to how the flexible rules are configered.*/
class DllImportExport CTokenFile
{
public:
  CTokenFile( byte Places,
              const char* nFileName,
              flag nIgnoreComments = True,
              flag nUseStringChar = True,
              flag nUseIncludes = True);
  ~CTokenFile();          
    
  flag Open();              
  void Close();             
  char* GetWhiteSpaceChars() { return &m_WhiteSpaceSet[0]; };
  char* GetSeperators() {return &m_SeperatorSet[0]; };
  void SetWhiteSpace(char* pNewWhiteSpace); 
  void SetSeperators(char* pNewSeperators); 
  const char* NextToken();      
  const char* GetNextLine();    
  const char* TokenLine();    
  void SetIncludeChars(char* p); 
  TokenTypes GetTokenType(const char* p);
  void GetFirstToken(Strng &s,Strng &Token);

  /*#F:#R:True if the last token returned was the last token on the current line.*/
  flag AtLastToken()
    { return m_LastToken; };     
  /*#F:#R:True if the last token returned was the first token on the current line.*/
  flag AtFirstToken()
    { return m_FirstToken; };
  /*#F:#R:True if ALL tokens have been returned.*/
  flag AtEOF()
    { return m_AtEOF; };
  /*#F:#R:The type of token the last token returned was.*/
  TokenTypes GetCurTokenType()
    { return m_TokType; };
  /*#F:Set to True if the text following the ; on the line must be ignored. 
  Remember that ";" must be set as a token seperator for comments midway through a line to be ignored.*/
  void SetIgnoreComments( flag nIgnoreComments)
    { m_IgnoreComments = nIgnoreComments; };
  /*#F:Set the character to be used to encapsulate a "string". Remember to set UseStringChar 
  to True and include nStringChar as a token seperator for this to work.*/
  void SetStringChar(char nStringChar)
    { m_StringChar = nStringChar; };
  /*#F:Set to True if the specified StringChar is to be used. Remember to include StringChar as a token seperator for this to work.*/
  void SetUseStringChar(flag nUseStringChar)
    { m_UseStringChar = nUseStringChar; };
  /*#F:Set to True if next time NextToken is called, the same token must be returned without further parsing.*/
  void SetReturnSameToken(flag nReturnSameToken = True)
    { m_ReturnSameToken = nReturnSameToken; };
  /*#F:#R:The line number of the current line (this includes nested include file lines).*/
  const WORD GetLineNo()
    { return m_LineNo; };
  /*#F:#R:The name of a file that is being parsed. Default 0 corresponds to the primary file.*/
  const char* GetFileName(int index = 0)
    { return (const char*)(m_FileNames[index]()); };
  const FILETIME GetFileTime(int index = 0)
    { return m_FileTimes[index]; };
  /*#F:#R:The index to the file that is being parsed (>0 for include files).*/
  int GetFileIndex()
    { return m_FileIndex; };
  /*#F:#R:The name of a file that was parsed. Default 0 corresponds to the primary file.*/
  const char* GetIncFileName(int index = 0)
    { return (const char*)(m_AllFileNames[index]()); };
  const FILETIME GetIncFileTime(int index = 0)
    { return m_AllFileTimes[index]; };
  /*#F:#R:The number of files parsed.*/
  int GetFilesUsedCount()
    { return m_IncFileCnt; };
  /*#F:#R:The number of lines parsed for a file. Default 0 corresponds to the primary file.*/
  const WORD GetFileLineNo(int index = 0)
    { return m_FileLineNo[index]; };
  /*#F:#R:The whole of the current line regardless of what or where the current token in the line is.*/
  const char* GetWholeLine()
    { return (const char*)&m_Line[0]; }
  void SetFailFn(fn_IncLoadFail pFailFn,
                 void* pFailExtraPtr = NULL)
    { m_pFailFn = pFailFn; m_pFailExtraPtr = pFailExtraPtr; };

public:
  TokenTypes m_TokType;    //Current token type
  flag m_LastToken;        //True if just read last token of line
  flag m_FirstToken;       //True if just read first token of line
  flag m_AtEOF;            //True if just read last token of the file
  Strng m_CurTok;          //The current token
  flag m_ReturnSameToken;  //If True NextToken() returns the current token again

protected:
  void SetParameters();
  void NextLine();
  void SkipWhiteSpace();
  //char* SkipToSeperator();
  Strng SkipToSeperator() const;

  byte m_bPlaces;                 //Places a file could be
  flag m_IgnoreComments;          //True if all characters after a comment be ignored
  flag m_UseStringChar;           //True we return all characters between a specified char as a single token
  char m_StringChar;			        //Character that encloses strings
  flag m_UseIncludes;             //True if must try load file following include chars
  Strng m_IncludeChars;			      //Characters after which an include file is given
  WORD m_LineNo;                  //Current line number
  Strng m_TokenLine;              //Remaining part of line to be interpreted
  char m_WhiteSpaceSet[MaxWhiteSpace];   //Characters that must be ignored as whitespace
  WORD m_WhiteSpaceSetLen;        //Number of characters in whitespace set
  char m_SeperatorSet[MaxSeperators];    //Characters that must be interpreted as seperators between tokens
  WORD m_SeperatorSetLen;         //Number of chars in seperator set
  flag m_NewLine;                 //Flag set True if just starting new line
  FILE* m_File[MaxIncludeFiles];  //Pointer to array of text file handles being read
  Strng m_FileNames[MaxIncludeFiles];    //File names of the text files currently being parsed
  FILETIME m_FileTimes[MaxIncludeFiles];    //File times of the text files currently being parsed
  WORD m_FileLineNo[MaxIncludeFiles];    //Current line number for the files being parsed
  int m_FileIndex;                //Index of currently used file handle
  Strng m_AllFileNames[MaxTtlIncludeFiles]; //File names of all the text files parsed
  FILETIME m_AllFileTimes[MaxTtlIncludeFiles]; //File times of all the text files parsed
  int m_IncFileCnt;               //Number of files used while parsing
  char m_Line[MaxLineLen];        //The complete current line
  char m_Back[MaxBackLineLen];    //Unused chars (used to store chars if the line buffer is filled and a token is broken)
  fn_IncLoadFail m_pFailFn;       //pointer to function that must be called if an include file open fails
  void* m_pFailExtraPtr;          //extra pointer that gets passed back in the callback on a failure
};

//----------------------------------------------------------------------------

#undef DllImportExport 

#endif
