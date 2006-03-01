//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __TKNPARS_H
#define __TKNPARS_H

#ifdef __TKNPARS_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#ifndef __GPFUNCS_H
  #include "gpfuncs.h"
#endif
//----------------------------------------------------------------------------
#ifndef __TKNPARSE_H
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

const TP_MaxLineLen = 4096;       //maximum length of a line in the files being parsed
const TP_MaxBackLineLen = 512;
const TP_MaxWhiteSpace = 16;
const TP_MaxSeperators = 128;
const TP_MaxIncludeFiles = 8;     //maximum "depth" of include files
const TP_MaxTtlIncludeFiles = 32; //maximum number of include files used
//#E

//----------------------------------------------------------------------------
class DllImportExport CTokenFileInfo
  {
  friend class CTokenParser;
  public:
    CTokenFileInfo(CTokenParser* Parser,
                   byte Places,
                   char* FileName,
                   BOOL UseIncludes);
    ~CTokenFileInfo();          

  protected:
    void SetFileName(char* FileName);
    BOOL Open();              
    void Close();             
    BOOL CheckForInclude();
    void SetParameters();
    void NextLine();
    BOOL AtEOF() { return bAtEOF; };

    byte          bFnPlaces;                   // Places the file could be 
    UCHAR         bUseIncludes:1,              //True if must try load file following include chars
                  bAtEOF:1,                    //
                  bPartLine:1;                 //
    CTokenParser* pParser;                     //
    CString       sIncludeChars;               //Characters after which an include file is given
    FILE*         File[TP_MaxIncludeFiles];       //Pointer to array of text file handles being read
    CString       FileNames[TP_MaxIncludeFiles];  //File names of the text files currently being parsed
    FILETIME      FileTimes[TP_MaxIncludeFiles];  //File modify times of the text files currently being parsed
    WORD          FileLineNo[TP_MaxIncludeFiles]; //Current line number for the files being parsed
    int           iFileIndex;                  //Index of currently used file handle
    CString       AllFileNames[TP_MaxTtlIncludeFiles]; //File names of all the text files parsed
    FILETIME      AllFileTimes[TP_MaxTtlIncludeFiles]; //File modify times of all the text files parsed
    int           iIncFileCnt;                 //Number of files used while parsing
    char          Back[TP_MaxBackLineLen];        //Unused chars (used to store chars if the line buffer is filled and a token is broken)
  };

//----------------------------------------------------------------------------
class DllImportExport CTokenBufferInfo
  {
  friend class CTokenParser;
  public:
    CTokenBufferInfo(CTokenParser* Parser, char* pTxt);
    CTokenBufferInfo(CTokenParser* Parser);
    ~CTokenBufferInfo();          

  protected:
    BOOL Open();              
    void Close();             
    void SetParameters();
    void NextLine();
    void SetBuffer(char* pTxt);
    void SetFarBuffer(char* pTxt);
    BOOL AtEOBuff() { return bAtEOBuff; };

    UCHAR         bAtEOBuff:1,          //
                  bMyBigBuff:1,         //
                  bUseLineEnd:1,        //
                  bPartLine:1;          //
    CTokenParser* pParser;              //
    char*         pBigBuff;             //
    int           iBuffPos;             //
    CString       sLineEnd;             //
  };

//----------------------------------------------------------------------------

typedef void (*fn_ParseFail)(char*, DWORD, void*);

/*#C:#D:This class is used as a general text file parser. Tokens are returned 
according to how the flexible rules are configered.*/
class DllImportExport CTokenParser
  {
  friend class CTokenFileInfo;
  friend class CTokenBufferInfo;
  public:
    CTokenParser(BOOL IgnoreComments = TRUE,
                 BOOL UseStringChar = TRUE);
    CTokenParser(byte Places,
                 char* FileName,
                 BOOL IgnoreComments = TRUE,
                 BOOL UseStringChar = TRUE,
                 BOOL UseIncludes = TRUE);
    CTokenParser(BOOL IgnoreComments,
                 BOOL UseStringChar,
                 char* pTxt);
    ~CTokenParser();
    
  //general functions...
    BOOL Start();
    void End();
    void SetWhiteSpace(char* pNewWhiteSpace);
    void SetSeperators(char* pNewSeperators);
    void AddToken2Queue(LPCTSTR Tok);
    const char* NextToken();
    const char* NextCSVToken(CString &s);
    const char* GetNextLine();
    static TokenTypes GetTokenType(const char* p);
    void GetFirstToken(CString &s, CString &Token);
    /*#F:#R:True if the last token returned was the last token on the current line.*/
    BOOL AtLastToken() { return bLastToken; };
    /*#F:#R:True if the last token returned was the first token on the current line.*/
    BOOL AtFirstToken() { return bFirstToken; };
    /*#F:#R:True if ALL tokens have been returned.*/
    BOOL AtEnd() { return bAtEnd; };
    /*#F:#R:The type of token the last token returned was.*/
    TokenTypes GetCurTokenType() { return eTokType; };
    /*#F:#R:The type of token the last token returned was.*/
    TokenTypes TokType() { return eTokType; };
    /*#F:#R:The current token.*/
    const char* GetCurToken() { return (const char*)sCurTok; };
    /*#F:#R:Return white space characters.*/
    char* GetWhiteSpaceChars() { return &WhiteSpaceSet[0]; };
    /*#F:#R:Return seperator charcters.*/
    char* GetSeperators() { return SeperatorSet; };
    /*#F:Set to True if the text following the ; on the line must be ignored. 
    Remember that ";" must be set as a token seperator for comments midway through a line to be ignored.*/
    void SetIgnoreComments(BOOL IgnoreComments) { bIgnoreComments = IgnoreComments; };
    /*#F:Set the character to be used to encapsulate a "string". Remember to set UseStringChar 
    to True and include nStringChar as a token seperator for this to work.*/
    void SetStringChar(char StringChar, BOOL UseStringChar=TRUE) { cStringChar = StringChar; bUseStringChar = UseStringChar; };
    /*#F:Set to True if the specified StringChar is to be used. Remember to include StringChar as a token seperator for this to work.*/
    void SetUseStringChar(BOOL UseStringChar) { bUseStringChar = UseStringChar; };
    /*#F:Set to True if next time NextToken is called, the same token must be returned without further parsing.*/
    void SetReturnSameToken(BOOL ReturnSameToken = TRUE) { bReturnSameToken = ReturnSameToken; };
    /*#F:#R:The line number of the current line (this includes nested include file lines).*/
    const WORD GetLineNo() { return wLineNo; };
    /*#F:#R:The whole of the current line regardless of what or where the current token in the line is.*/
    const char* GetWholeLine() { return (const char*)&Line[0]; }
    void SetFailFn(fn_ParseFail FailFn, void* FailExtraPtr = NULL) { pFailFn = FailFn; pFailExtraPtr = FailExtraPtr; };
    BOOL ParsingFile() { return (pFileInfo!=NULL); };
    BOOL ParsingBuffer() { return (pBuffInfo!=NULL); };

  //file parsing functions...
    void SetFileName(char* FileName);
    /*#F:This sets the characters to be found at the start of a line (or directly
    after a token) that indicate that all the remaining characters on the line refer 
    to a file to be included. Remember to include them as seperators.*/
    void SetIncludeChars(char* pNewIncludeChars) { pFileInfo->sIncludeChars = pNewIncludeChars; };  
    /*#F:#R:The name of a file that is being parsed. Default 0 corresponds to the primary file.*/
    const char* GetFileName(int index = 0) { return (const char*)(pFileInfo->FileNames[index]); };
    const FILETIME &GetFileTime(int index = 0) { return pFileInfo->FileTimes[index]; };
    /*#F:#R:The index to the file that is being parsed (>0 for include files).*/
    int GetFileIndex() { return pFileInfo->iFileIndex; };
    /*#F:#R:The name of a file that was parsed. Default 0 corresponds to the primary file.*/
    const char* GetIncFileName(int index = 0) { return (const char*)(pFileInfo->AllFileNames[index]); };
    const FILETIME &GetIncFileTime(int index = 0) { return pFileInfo->AllFileTimes[index]; };
    /*#F:#R:The number of files parsed.*/
    int GetFilesUsedCount() { return pFileInfo->iIncFileCnt; };
    /*#F:#R:The number of lines parsed for a file. Default 0 corresponds to the primary file.*/
    const WORD GetFileLineNo(int index = 0) { return pFileInfo->FileLineNo[index]; };

  //buffer parsing functions...
    void SetBuffer(char* pTxt);
    void SetFarBuffer(char* pTxt);
    /*#F:Set the characters to be used to determine a line end in a buffer. Remember to include the
    LineEnd characters as a token seperator for this to work.*/
    void SetLineEnd(char* LineEnd, BOOL UseLineEnd=TRUE) { pBuffInfo->sLineEnd = LineEnd; pBuffInfo->bUseLineEnd = UseLineEnd; };
    /*#F:Set to True if the specified LineEnd characters are to be used.*/
    void SetUseLineEnd(BOOL UseLineEnd) { pBuffInfo->bUseLineEnd = UseLineEnd; };

  protected:
    void CommonConstruct();
    void SetParameters();
    void NextLine();
    void SkipWhiteSpace();
    void SkipToSeperator(CString & TempStr);

  protected:
    byte          bFnPlaces;          //File Places
    TokenTypes    eTokType;           //Current token type
    CString       sCurTok;            //The current token
    WORD          bLastToken:1,       //True if just read last token of line
                  bFirstToken:1,      //True if just read first token of line
                  bAtEnd:1,           //True if just read last token of the file
                  bReturnSameToken:1, //If True NextToken() returns the current token again
                  bIgnoreComments:1,  //True if all characters after a comment be ignored
                  bUseStringChar:1,   //True we return all characters between a specified char as a single token
                  bNewLine:1,         //BOOL set True if just starting new line
                  bDoneStart:1,       //
                  bDoneEnd:1;         //
    char          cStringChar;        //Character that encloses strings
    WORD          wLineNo;            //Current line number
    CString       sTokenLine;         //Remaining part of line to be interpreted
    char          WhiteSpaceSet[TP_MaxWhiteSpace];  //Characters that must be ignored as whitespace
    int           iWhiteSpaceSetLen;  //Number of characters in whitespace set
    char          SeperatorSet[TP_MaxSeperators];   //Characters that must be interpreted as seperators between tokens
    int           iSeperatorSetLen;   //Number of chars in seperator set
    CTokenFileInfo* pFileInfo;        //helper class if tokens are read from a file
    CTokenBufferInfo* pBuffInfo;      //helper class if tokens are read from a buffer
    char          Line[TP_MaxLineLen];//The complete current line
    fn_ParseFail  pFailFn;            //pointer to function that must be called if an error occurs
    void*         pFailExtraPtr;      //extra pointer that gets passed back in the callback on a failure
    Strng_List    m_TokenQ; 
  };

//----------------------------------------------------------------------------

#undef DllImportExport 

#endif
