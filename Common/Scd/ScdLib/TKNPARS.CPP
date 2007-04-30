//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"    
#define __TKNPARS_CPP
#include "sc_defs.h"    
#include "tknpars.h"
#include "licbase.h"

#include <fstream>

//==========================================================================

CTokenFileInfo::CTokenFileInfo(CTokenParser* Parser, byte Places, char* FileName, BOOL UseIncludes)
  {
  bFnPlaces=Places;
  pParser = Parser;
  bUseIncludes = UseIncludes;
  SetParameters();
  FileNames[iFileIndex] = FileName;
  sIncludeChars = ">>";
  }

//----------------------------------------------------------------------------

CTokenFileInfo::~CTokenFileInfo()
  {
  Close();
  }

//----------------------------------------------------------------------------
/*#F:Used internally to reset flags etc.*/
void CTokenFileInfo::SetParameters()
  {
  Back[0] = 0;
  bAtEOF = 0;
  bPartLine = 0;
  for (int i=0; i<TP_MaxIncludeFiles; i++)
    {
    File[i] = NULL;
    FileLineNo[i] = 0L;
    }
  iFileIndex = 0;
  iIncFileCnt = 0;
  }

void CTokenFileInfo::DecryptToBuffer(Strng encFnFull)
{
  fclose(File[iFileIndex]);

  std::fstream fin;
  fin.open(encFnFull(), std::ios::in|std::ios::binary);
  if (fin.is_open())
  {
    fin.seekg(0, std::ios_base::end);
    int length = fin.tellg();
    fin.seekg(0, std::ios_base::beg);

    m_Buffer[iFileIndex] = new char[length+1];
    fin.read(m_Buffer[iFileIndex], length);
    fin.close();

    int count = fin.gcount();
    m_Buffer[iFileIndex][count] = (char)length;

    int seed = length;

    for (int i=count-1; i>=0; i--) // decrypt the buffer.
    {
      seed *= 1103515245;
      seed += 12345; // add some deterministic noise to the system.
      m_Buffer[iFileIndex][i] = m_Buffer[iFileIndex][i] ^ m_Buffer[iFileIndex][i+1] ^ i ^ seed;
    }

    m_Buffer[iFileIndex][count] = 0;
  }
  fin.close();

  m_SS[iFileIndex] = new std::stringstream(m_Buffer[iFileIndex]);

  File[iFileIndex] = fopen(encFnFull(), "rt");
}

bool CTokenFileInfo::OpenFile(Strng FnFull)
{
  gs_EncryptNDemos.CheckForDemo(NULL);

  Strng encFnFull;
  encFnFull = FnFull;
  encFnFull.Append(".x");
  File[iFileIndex] = fopen(encFnFull(), "rt");
  if (File[iFileIndex] == NULL)
  { // Encrypted file nonexistent.
    m_Buffer[iFileIndex] = NULL;
    File[iFileIndex] = fopen(FnFull(), "rt");
    if (File[iFileIndex] == NULL)
      return False;
  }
  else
  { // Encrypted file found.
    gs_EncryptNDemos.encryptedPGM = true;
    DecryptToBuffer(encFnFull);
  }
  return true;
}

//----------------------------------------------------------------------------
/*#F:Opens the primary file for parsing.#R:True if successfully opened.*/
BOOL CTokenFileInfo::Open()
  {
  ASSERT(File[0]==NULL); //file is allready open
  SetParameters();  
  CString &Fn = FileNames[iFileIndex];
  CString FnO = Fn;
  /*if (FnO.GetLength()>3 && FnO[0]=='.' && FnO[1]=='.' && FnO[2]=='\\')
    {
    FnO = PrjPrevDirectory();
    FnO += Fn.Right(Fn.GetLength()-3);
    }*/
  
  AllFileNames[iIncFileCnt] = FnO;
    
  Strng FnFull = FnO;
  bool result = OpenFile(FnFull);
  //File[iFileIndex] = fopen((const char*)FnO, "rt");

  FnModifyTime((char*)(const char*)FnO, FileTimes[iFileIndex]);
  AllFileTimes[iIncFileCnt]=FileTimes[iFileIndex];
  iIncFileCnt++;

  return result;
  //if (File[iFileIndex] == NULL)
  //  return FALSE;
  //return TRUE;
  }

//----------------------------------------------------------------------------
/*#F:Closes all remaining open files (if any)*/
void CTokenFileInfo::Close()
  {
  for (int i=iFileIndex; i>=0; i--)
    {
    if (File[i] != NULL)
      {
      fclose(File[i]);
      delete m_Buffer[i];
      File[i] = NULL;
      }
    }
  }

//----------------------------------------------------------------------------

void CTokenFileInfo::SetFileName(char* FileName)
  {
  SetParameters();
  FileNames[iFileIndex] = FileName;
  }

//----------------------------------------------------------------------------

BOOL CTokenFileInfo::CheckForInclude()
  {
  if (bUseIncludes && (pParser->sTokenLine[0]==sIncludeChars[0]))
    {
    CString FileName = pParser->sTokenLine;
    if (pParser->sTokenLine.Find((const char*)sIncludeChars)==0)
      {
      FileName = FileName.Mid(sIncludeChars.GetLength(), 1024);
      FileName = FileName.Right(FileName.GetLength() - FileName.SpanIncluding(" \t").GetLength());
      while (FileName.GetLength()>0 && (FileName[FileName.GetLength()-1]==' ' || FileName[FileName.GetLength()-1]=='\t'))
        FileName = FileName.Left(FileName.GetLength()-1);
      BOOL Failed = TRUE;
      DWORD ErrNo = 1;//file not specified
      if (FileName.GetLength()>0)
        {
        if (FileName[0]=='\'' || FileName[0]=='"')
          FileName = FileName.Right(FileName.GetLength() - 1);
        int len = FileName.GetLength();
        if (len>0)
          {
          if (FileName[len-1]=='\'' || FileName[len-1] == '"')
            FileName = FileName.Left(len-1);
          len = FileName.GetLength();
          if (len>0)
            {
            iFileIndex++;
            if (iFileIndex<TP_MaxIncludeFiles)
              {
              Strng Fn, FnFull;
              Fn.FnDrivePath((char*)(const char*)FileName);
              if (Fn()==NULL)
                {// No Path Use Previous
                Fn.FnDrivePath((char*)(const char*)FileNames[iFileIndex-1]);
                Fn+=(char*)(const char*)FileName;
//                FileName=Fn();
                }
              else
                Fn=(char*)(const char*)FileName;

              // CNM
              if ((Fn[0]=='~') || (Fn[0]=='.')) // relative filename ?
                { // yes - prepend current folder (defined by previously openned file - NOT O/S
                Strng T;
                T.FnDrivePath((LPTSTR)(LPCTSTR)AllFileNames[iIncFileCnt-1]);
                T+=Fn;
                Fn=T;
                }

              FnFull.FnSearchExpand(Fn(), bFnPlaces);

              Fn.FnContract();
              FileName=Fn();

              FileNames[iFileIndex] = Fn();
              pParser->wLineNo--;
              FileLineNo[iFileIndex] = 0;

              Failed = !OpenFile(FnFull);
              //File[iFileIndex] = fopen(FnFull(), "rt");

              FnModifyTime(FnFull(), FileTimes[iFileIndex]);
              if (iIncFileCnt<TP_MaxTtlIncludeFiles)
                {
                AllFileNames[iIncFileCnt]=Fn();
                AllFileTimes[iIncFileCnt]=FileTimes[iFileIndex];
                iIncFileCnt++;
                }

              if (Failed)
                ErrNo = 2;//cannot open file
              }
            else
              ErrNo = 3;//too many nested includes
            }
          }
        }
      if (Failed && pParser->pFailFn)
        {
        (*(pParser->pFailFn))((char*)(const char*)FileName, ErrNo, pParser->pFailExtraPtr);
        }
      return TRUE;
      }
    }
  return FALSE;
  }


char* CTokenFileInfo::ssgets(int k)
{
  //char* p = NULL;

  if (!(m_SS[iFileIndex]->eof()))
  {
    //p = new char[sizeof(pParser->Line) - 1 - k];
    m_SS[iFileIndex]->getline((char*)&(pParser->Line[k]), sizeof(pParser->Line) - 1 - k);

    // handle different line-end characters.
    char* match = strchr((char*)&(pParser->Line[k]), 13);
    while (match != NULL)
    {
      match[0] = 10; 
      match = strchr((char*)&(pParser->Line[k]), 13);
    }
  }

  //if (m_SS[iFileIndex]->eof())
  //  fseek(File[iFileIndex], 0, SEEK_END); // testing -- this will put the File at eof as well...
  // but this doesn't work either!!!

  return (char*)&(pParser->Line[k]); // will be null if at eof...
}
//----------------------------------------------------------------------------

void CTokenFileInfo::NextLine()
  {
  if (File[iFileIndex]!=NULL)
    {
    // Add any characters from last time round to buffer
    int k = strlen(Back);
    if (k>0)
      {
      strrev(Back);
      strncpy(pParser->Line, Back, k);
      }
    
    char* p = NULL;
    if (m_Buffer[iFileIndex] == NULL) // No buffer -- reading straight from file.
      p = fgets((char*)&(pParser->Line[k]), sizeof(pParser->Line) - 1 - k, File[iFileIndex]);
    else // we've got a buffer, use that instead.
    {
      p = ssgets(k);
    }

    //char* p = fgets((char*)&(pParser->Line[k]), sizeof(pParser->Line) - 1 - k, File[iFileIndex]);
    
    if (p==NULL)
      {
      if (iFileIndex>0)
        {
        pParser->wLineNo--;
        fclose(File[iFileIndex]);
        delete m_Buffer[iFileIndex];
        File[iFileIndex] = NULL;
        iFileIndex--;
        NextLine();
        }
      else
        bAtEOF = 1;
      }
    else
      {
      int j = strlen(pParser->Line);
      if (j==TP_MaxLineLen-2)
        {//strip chars from the end of the buffer until the next seperator
        if (bPartLine)
          pParser->wLineNo--;
        bPartLine = 1;
        char *pos = &(pParser->Line[j-1]);
        char *buf = &Back[0];
        *buf = 0;
        while (strchr(pParser->SeperatorSet, (int)(*pos))==NULL)
          {
          *buf = *pos;
          buf++;
          *buf = 0;
          *pos = 0;
          pos--;
          VERIFY(pos != pParser->Line); //("Line (without seperator) longer than backup buffer");
          }
        }
      else
        {
        if (bPartLine)
          pParser->wLineNo--;
        bPartLine = 0;
        Back[0] = 0;
        }
      //if ((pParser->Line[strlen(p)+k-1] == 10) || (pParser->Line[strlen(p)+k-1] == 13))
      if (pParser->Line[strlen(p)+k-1]==10)
        pParser->Line[strlen(p)+k-1] = 0;
      }
    }
  else
    {
    if (iFileIndex>0)
      {
      File[iFileIndex] = NULL;
      iFileIndex--;
      }
    else
      bAtEOF = 1;
    pParser->bNewLine = 1;
    pParser->sTokenLine = "";
    return;
    }
  pParser->bNewLine = 1;
  pParser->wLineNo++;
  FileLineNo[iFileIndex]++;
  pParser->sTokenLine = (char*)&(pParser->Line[0]);
  }

//==========================================================================

CTokenBufferInfo::CTokenBufferInfo(CTokenParser* Parser, char* pTxt)
  {
  bUseLineEnd = 0;
  pParser = Parser;
  pBigBuff = NULL;
  bMyBigBuff = 0;
  SetParameters();
  SetBuffer(pTxt);
  }

//----------------------------------------------------------------------------

CTokenBufferInfo::CTokenBufferInfo(CTokenParser* Parser)
  {
  bUseLineEnd = 0;
  pParser = Parser;
  pBigBuff = NULL;
  bMyBigBuff = 0;
  SetParameters();
  }

//----------------------------------------------------------------------------

CTokenBufferInfo::~CTokenBufferInfo()
  {
  Close();
  }

//----------------------------------------------------------------------------
/*#F:Used internally to reset flags etc.*/
void CTokenBufferInfo::SetParameters()
  {
  bAtEOBuff = 0;
  bPartLine = 0;
  iBuffPos = 0;
  }

//----------------------------------------------------------------------------
/*#F:Opens the primary file for parsing.#R:True if successfully opened.*/
BOOL CTokenBufferInfo::Open()
  {
  SetParameters();
  if (pBigBuff)
    return TRUE;
  return FALSE;
  }

//----------------------------------------------------------------------------
/*#F:Closes all remaining open files (if any)*/
void CTokenBufferInfo::Close()
  {
  if (pBigBuff && bMyBigBuff)
    {
    delete pBigBuff;
    pBigBuff = NULL;
    bMyBigBuff = 0;
    }
  }

//----------------------------------------------------------------------------

void CTokenBufferInfo::SetBuffer(char* pTxt)
  {
  if (pBigBuff && bMyBigBuff)
    delete pBigBuff;
  int len = 0;
  if (pTxt)
    len = strlen(pTxt);
  pBigBuff = new char[len+1];
  bMyBigBuff = 1;
  if (len>0)
    strcpy(pBigBuff, pTxt);
  pBigBuff[len] = 0;
  iBuffPos = 0;
  }

//----------------------------------------------------------------------------

void CTokenBufferInfo::SetFarBuffer(char* pTxt)
  {
  if (pBigBuff && bMyBigBuff)
    delete pBigBuff;
  pBigBuff = pTxt;
  bMyBigBuff = 0;
  iBuffPos = 0;
  }

//----------------------------------------------------------------------------

void CTokenBufferInfo::NextLine()
  {
  pParser->Line[0] = 0;
  if (pBigBuff[iBuffPos]==0)
    {
    bAtEOBuff = 1;
    pParser->bNewLine = 1;
    pParser->sTokenLine = "";
    return;
    }

  int CopyLen = (bUseLineEnd ? 0 : strlen(&pBigBuff[iBuffPos]));
  int SepLen = (bUseLineEnd ? sLineEnd.GetLength() : 0);
  if (bUseLineEnd)
    {
    BOOL Found = FALSE;
    int Pos = 0;
    while (!Found)
      {
      char* p = strchr(&pBigBuff[iBuffPos+Pos], sLineEnd[0]);
      if (p)
        {
        int Pos1 = (p - &pBigBuff[iBuffPos+Pos]);
        Found = (strncmp((const char*)sLineEnd, p, sLineEnd.GetLength())==0);
        if (Found)
          CopyLen = Pos+Pos1;
        else
          Pos += (Pos1 + 1);
        }
      else
        {
        Found = TRUE;
        SepLen = 0;
        CopyLen = strlen(&pBigBuff[iBuffPos]);
        }
      }
    }
  if (CopyLen>=TP_MaxLineLen)
    {
    if (bPartLine)
      pParser->wLineNo--;
    bPartLine = 1;
    CopyLen = TP_MaxLineLen-1;
    while (strchr(pParser->SeperatorSet, (int)(pBigBuff[iBuffPos+CopyLen]))==NULL && CopyLen>0)
      CopyLen--;
    //VERIFY(CopyLen>0);//("Line (without seperator) longer than line buffer length");
    if (CopyLen==0)
      CopyLen = TP_MaxLineLen-1;
    strncpy(pParser->Line, &pBigBuff[iBuffPos], CopyLen);
    pParser->Line[CopyLen] = 0;
    iBuffPos += CopyLen;
    }
  else
    {
    if (bPartLine)
      pParser->wLineNo--;
    bPartLine = 0;
    strncpy(pParser->Line, &pBigBuff[iBuffPos], CopyLen);
    pParser->Line[CopyLen] = 0;
    iBuffPos += (CopyLen+SepLen);
    }
  pParser->wLineNo++;
  pParser->bNewLine = 1;
  pParser->sTokenLine = pParser->Line;
  }

//==========================================================================

CTokenParser::CTokenParser(BOOL IgnoreComments/* = TRUE*/,   //default = True
                           BOOL UseStringChar/* = TRUE*/)    //default = True
  {
  bFnPlaces=0;
  bIgnoreComments = IgnoreComments;
  bUseStringChar = UseStringChar;
  CommonConstruct();
  }

//----------------------------------------------------------------------------
/*#f:Constructer for token file parser. The file name is the only required parameter.
Defaults are set: whitespace (space and tab), seperators (,), string char (") and file 
include characters (>>).*/
CTokenParser::CTokenParser(byte Places,
                          char* FileName,  //path and file name
                          BOOL IgnoreComments,   //default = True
                          BOOL UseStringChar,    //default = True
                          BOOL UseIncludes)      //default = True
  {
  bFnPlaces=Places;
  bIgnoreComments = IgnoreComments;
  bUseStringChar = UseStringChar;
  CommonConstruct();
  pFileInfo = new CTokenFileInfo(this, Places, FileName, UseIncludes);
  }

//----------------------------------------------------------------------------

CTokenParser::CTokenParser(BOOL IgnoreComments,
                           BOOL UseStringChar,
                           char* pTxt)
  {
  bFnPlaces=0;
  bIgnoreComments = IgnoreComments;
  bUseStringChar = UseStringChar;
  CommonConstruct();
  pBuffInfo = new CTokenBufferInfo(this, pTxt);
  }

//----------------------------------------------------------------------------
/*#F:Destructor, closes any unclosed files.*/
CTokenParser::~CTokenParser()
  {
  if (pFileInfo)
    delete pFileInfo;
  if (pBuffInfo)
    delete pBuffInfo;
  }

//----------------------------------------------------------------------------

void CTokenParser::CommonConstruct()
  {
  memset(Line, 0, sizeof(Line));
  sCurTok = "";
  sTokenLine = "";
  wLineNo = 0;
  bLastToken = 0;
  bFirstToken = 0;
  bReturnSameToken = 0;
  bNewLine = 1;
  bDoneStart = 0;
  bDoneEnd = 0;
  bAtEnd = 0;
  //SetParameters();
  WhiteSpaceSet[0] = 32; //space
  WhiteSpaceSet[1] = 9;  //tab
  WhiteSpaceSet[2] = 0;
  iWhiteSpaceSetLen = strlen(WhiteSpaceSet);
  strncpy(SeperatorSet, ",", sizeof(SeperatorSet));
  iSeperatorSetLen = strlen(SeperatorSet);
  cStringChar = '"';
  pFileInfo = NULL;
  pBuffInfo = NULL;
  pFailFn = NULL;
  pFailExtraPtr = NULL;
  }

//----------------------------------------------------------------------------
/*#F:Used to specify characters to be ignored as 'whitespace'.*/
void CTokenParser::SetWhiteSpace(char* pNewWhiteSpace)
  {
  strncpy(WhiteSpaceSet, pNewWhiteSpace, sizeof(WhiteSpaceSet));
  WhiteSpaceSet[sizeof(WhiteSpaceSet)-1] = 0;
  iWhiteSpaceSetLen = strlen(WhiteSpaceSet);
  }

//----------------------------------------------------------------------------
/*#F:This sets the characters to treated as individual tokens and
therefore, token seperators. Every new line is automatically a seperator.
If comments are being used, (;) must be included as a seperator for
comments part way through a line to be ignored.
#E:f.SetSeperators("[]{}<>; /"-+/*=")*/
void CTokenParser::SetSeperators(char* pNewSeperators)
  {
  strncpy(SeperatorSet, pNewSeperators, sizeof(SeperatorSet));
  SeperatorSet[sizeof(SeperatorSet)-1] = 0;
  iSeperatorSetLen = strlen(SeperatorSet);
  }

//----------------------------------------------------------------------------
/*#F:#R:True if successfully opened.*/
BOOL CTokenParser::Start()
  {
  ASSERT(bDoneStart==0);
  Line[0] = 0;
  sCurTok = "";
  sTokenLine = "";
  wLineNo = 0;
  bLastToken = 0;
  bFirstToken = 0;
  bReturnSameToken = 0;
  bAtEnd = 0;
  bNewLine = 1;
  BOOL b = FALSE;
  if (pFileInfo)
    b = pFileInfo->Open();
  else if (pBuffInfo)
    b = pBuffInfo->Open();
  bDoneStart = b;
  bDoneEnd = 0;
  return b;
  }

//----------------------------------------------------------------------------
/*#F:Closes all remaining open files (if any)*/
void CTokenParser::End()
  {
  ASSERT(bDoneStart);
  ASSERT(bDoneEnd==0);
  if (pFileInfo)
    pFileInfo->Close();
  bDoneEnd = 1;
  bDoneStart = 0;
  }

//----------------------------------------------------------------------------

void CTokenParser::SetFileName(char* FileName)
  {
  ASSERT(bDoneStart==0);
  if (!pFileInfo)
    pFileInfo = new CTokenFileInfo(this, bFnPlaces, FileName, FALSE);
  pFileInfo->SetFileName(FileName);
  }

//----------------------------------------------------------------------------

void CTokenParser::SetBuffer(char* pTxt)
  {
  ASSERT(bDoneStart==0);
  if (!pBuffInfo)
    pBuffInfo = new CTokenBufferInfo(this, pTxt);
  pBuffInfo->SetBuffer(pTxt);
  }

//----------------------------------------------------------------------------

void CTokenParser::SetFarBuffer(char* pTxt)
  {
  ASSERT(bDoneStart==0);
  if (!pBuffInfo)
    pBuffInfo = new CTokenBufferInfo(this);
  pBuffInfo->SetFarBuffer(pTxt);
  }

//----------------------------------------------------------------------------
/*#F:#R:The token type of the specified string.*/
TokenTypes CTokenParser::GetTokenType(const char* p)
  {
  if (p==NULL)
    {
    return TokNull;
    }
  else
    {
    char c = p[0];
    if ((c>96) && (c<123))
      return TokAlpha;
    else if ((c>64) && (c<91))
      return TokAlpha;
    else if ((c>47) && (c<58))
      return TokNumeric;
    else if (c == 59)
      return TokComment;
    else if ((c > 32) && (c < 127))
      return TokSymbol;
    else if ((c == 32) || (c == 9) || (c == 10))// || (c == 13))
      return TokWhiteSpace;
    else
      return TokOther;
    }
  }

//----------------------------------------------------------------------------
/*#F:Parses the current line for a token. Skips whitespace and loads succesive 
lines if neccessary. If ReturnSameToken BOOL was set, the line is not parsed, the 
current token is returned and the BOOL unset. The new token type is determined.
#R:A pointer to the current token.*/
const char* CTokenParser::NextToken()
  {
  if (bReturnSameToken)
    {
    bReturnSameToken = 0;
    return (const char*)sCurTok;
    }
  sCurTok = "";
  eTokType = TokNull;
  if (m_TokenQ.Length()>0)
    {
    Strng *p=m_TokenQ.First();
    sCurTok=(*p)();
    m_TokenQ.Remove(p);
    eTokType = GetTokenType((const char*)sCurTok);
    }
  else
    {
    BOOL BlankEnclosedStr = FALSE;
    while ((sCurTok.GetLength()==0) && (!bAtEnd) && !BlankEnclosedStr)
      {
      SkipWhiteSpace();
      while ((sTokenLine.GetLength()==0) && (!bAtEnd))
        {
        NextLine();
        SkipWhiteSpace();
        }

      if (bAtEnd)
        sCurTok = "";
      else
        {
        bFirstToken = bNewLine;
        char c = sTokenLine[0];
        if (pFileInfo && pFileInfo->bUseIncludes && c==pFileInfo->sIncludeChars[0])
          if (pFileInfo->CheckForInclude())
            {//include file found
            sCurTok = "";
            sTokenLine = "";
            return NextToken();
            }
        if (bIgnoreComments && c==';')
          {//comment found
          sCurTok = "";
          sTokenLine = "";
          }
        else if (bUseStringChar && c==cStringChar)
          {//start of enclosed string found
          eTokType = TokString;
          sTokenLine = sTokenLine.Right(sTokenLine.GetLength()-1);
          int APos = (int)sTokenLine.Find(cStringChar);
          if (APos<0)
            {
            sCurTok = sTokenLine;
            sTokenLine = "";
            }
          else if (APos==0)
            {
            sCurTok = "";
            sTokenLine = sTokenLine.Right(sTokenLine.GetLength() - 1);
            BlankEnclosedStr = TRUE;
            }
          else
            {
            sCurTok = sTokenLine.Left(APos);
            sTokenLine = sTokenLine.Right(sTokenLine.GetLength() - APos - 1);
            }
          }
        else    
          {
          WORD i = 0;
          while (i<iSeperatorSetLen && SeperatorSet[i]!=c)
            i++;
          if (i<iSeperatorSetLen)
            {//token seperator character found
            sTokenLine = sTokenLine.Right(sTokenLine.GetLength()-1);
            sCurTok = c; 
            }
          else
            {//default action
            SkipToSeperator(sCurTok);
            sTokenLine = sTokenLine.Right(sTokenLine.GetLength() - sCurTok.GetLength());
            }
          }
        }
      }
    if (eTokType!=TokString)
      eTokType = GetTokenType((const char*)sCurTok);
    bNewLine = 0;
    }
  bLastToken = (sTokenLine.GetLength()==0);
  return (const char*)sCurTok;
  }

//----------------------------------------------------------------------------

const char* CTokenParser::NextCSVToken(CString &s)
  {
  s = NextToken(); //get value
  if (s==",")
    s = "";
  else
    {
    if (!AtLastToken() && !AtEnd())
      NextToken(); //get ',' separater
    }
  return (const char*)s;
  }

//----------------------------------------------------------------------------
                         
void CTokenParser::AddToken2Queue(LPCTSTR Tok)
  {
  m_TokenQ.Append(Tok);
  };

//----------------------------------------------------------------------------
/*#F:#R:The complete current token line regardless of any token seperators contained
within it.*/
const char* CTokenParser::GetNextLine()
  {
  if (bNewLine)
    NextLine();
  bNewLine = 1;
  if (pFileInfo && pFileInfo->bUseIncludes && sTokenLine.GetLength()>pFileInfo->sIncludeChars.GetLength() && sTokenLine[0]==pFileInfo->sIncludeChars[0])
    if (pFileInfo->CheckForInclude())
      {//include file found
      sCurTok = "";
      sTokenLine = "";
      return GetNextLine();
      }
  sCurTok = sTokenLine;
  sTokenLine = "";
  return (const char*)sCurTok;
  }

//----------------------------------------------------------------------------
/*#F:#R:The first token found in the line supplied.*/
void CTokenParser::GetFirstToken(CString &s,  //string to parse
                                 CString &Token)  //returned token string
  {
  CString Temp = sTokenLine;
  sTokenLine = s;
  SkipWhiteSpace();
  CString Temp2 = sTokenLine.Right(sTokenLine.GetLength()-1);
  Token = sTokenLine.Left(1);
  if (strchr(SeperatorSet, (int)(Token[0]))==NULL)
    Token += Temp2.SpanExcluding(SeperatorSet);
  s = sTokenLine;
  sTokenLine = Temp;
  }

//----------------------------------------------------------------------------
/*#F:Removes all the "whitespace" characters from the left of the current line.*/
void CTokenParser::SkipWhiteSpace()
  {
  const int len = (sTokenLine.SpanIncluding(WhiteSpaceSet)).GetLength();
  if (len>0)
    sTokenLine = sTokenLine.Right(sTokenLine.GetLength() - len);
  }

//----------------------------------------------------------------------------
/*#F:#R:All the characters upto the first token seperator.*/
void CTokenParser::SkipToSeperator(CString & TempStr)
  {
  TempStr = sTokenLine.SpanExcluding(SeperatorSet);
  }

//----------------------------------------------------------------------------
/*#F:Reads the next line from the file.*/
void CTokenParser::NextLine()
  {
  if (pFileInfo)
    {
    pFileInfo->NextLine();
    bAtEnd = pFileInfo->AtEOF();
    }
  else if (pBuffInfo)
    {
    pBuffInfo->NextLine();
    bAtEnd = pBuffInfo->AtEOBuff();
    }
  }

//==========================================================================
