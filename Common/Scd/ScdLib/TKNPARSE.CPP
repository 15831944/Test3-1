//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"    

#define __TKNPARSE_CPP

#include "SC_DEFS.h"    
#ifndef __GPFUNCS_H
#include "gpfuncs.h"
#endif

#include "stdafx.h"    
#include "SC_DEFS.h"    
#ifndef __GPFUNCS_H
#include "gpfuncs.h"
#endif

#include "tknparse.h"
#include "licbase.h"

#include <fstream>

//==========================================================================
/*#f:Constructer for token file parser. The file name is the only required parameter.
Defaults are set: whitespace (space and tab), seperators (,), string char (") and file 
include characters (>>).*/
CTokenFile::CTokenFile( byte Places,
                        const char* nFileName,  //path and file name
                        flag nIgnoreComments,   //default = True
                        flag nUseStringChar,    //default = True
                        flag nUseIncludes)      //default = True
{
  m_bPlaces=Places;
  m_IgnoreComments = nIgnoreComments;
  m_UseStringChar = nUseStringChar;
  m_UseIncludes = nUseIncludes;
  SetParameters();
  m_FileNames[m_FileIndex] = (pchar)nFileName;
  //set default whitespace, seperators and StringChar...
  m_WhiteSpaceSet[0] = 32; //space
  m_WhiteSpaceSet[1] = 9;  //tab
  m_WhiteSpaceSet[2] = 0;
  m_WhiteSpaceSetLen = strlen(m_WhiteSpaceSet);
//  strncpy(SeperatorSet,"  =:;()[]{},+\"",sizeof(SeperatorSet));
//  SeperatorSet[1] = 9;
  strncpy(m_SeperatorSet,",",sizeof(m_SeperatorSet));
  m_SeperatorSetLen = strlen(m_SeperatorSet);
  m_StringChar = '"';
  m_IncludeChars = ">>";
  m_pFailFn = NULL;
  m_pFailExtraPtr = NULL;
};

//----------------------------------------------------------------------------
/*#F:Destructor, closes any unclosed files.*/
CTokenFile::~CTokenFile()
{
  Close();
};

//----------------------------------------------------------------------------
/*#F:Used to specify characters to be ignored as 'whitespace'.*/
void CTokenFile::SetWhiteSpace(char* pNewWhiteSpace)
{
  strncpy(m_WhiteSpaceSet, pNewWhiteSpace, sizeof(m_WhiteSpaceSet));
  m_WhiteSpaceSet[sizeof(m_WhiteSpaceSet)-1] = 0;
  m_WhiteSpaceSetLen = strlen(m_WhiteSpaceSet);
};

//----------------------------------------------------------------------------
/*#F:This sets the characters to treated as individual tokens and
therefore, token seperators. Every new line is automatically a seperator.
If comments are being used, (;) must be included as a seperator for
comments part way through a line to be ignored.
#E:f.SetSeperators("[]{}<>; /"-+/*=")*/
void CTokenFile::SetSeperators(char* pNewSeperators)
{
  strncpy(m_SeperatorSet, pNewSeperators, sizeof(m_SeperatorSet));
  m_SeperatorSet[sizeof(m_SeperatorSet)-1] = 0;
  m_SeperatorSetLen = strlen(m_SeperatorSet);
};

//----------------------------------------------------------------------------
/*#F:This sets the characters to be found at the start of a line (or directly
after a token) that indicate that all the remaining characters on the line refer 
to a file to be included. Remember to include them as seperators.*/
void CTokenFile::SetIncludeChars(char* pNewIncludeChars)
{
  m_IncludeChars = pNewIncludeChars;
};  

void CTokenFile::DecryptToBuffer(Strng encFnFull)
{
  fclose(m_File[m_FileIndex]);

  std::fstream fin;
  fin.open(encFnFull(), std::ios::in|std::ios::binary);
  if (fin.is_open())
  {
    fin.seekg(0, std::ios_base::end);
    int length = fin.tellg();
    fin.seekg(0, std::ios_base::beg);

    m_Buffer[m_FileIndex] = new char[length+1];
    fin.read(m_Buffer[m_FileIndex], length);
    fin.close();

    int count = fin.gcount();
    m_Buffer[m_FileIndex][count] = (char)length;

    int seed = length; 

    for (int i=count-1; i>=0; i--) // decrypt the buffer.
    {
      seed *= 1103515245;
      seed += 12345; // add some deterministic noise to the system.
      m_Buffer[m_FileIndex][i] = m_Buffer[m_FileIndex][i] ^ m_Buffer[m_FileIndex][i+1] ^ i ^ seed;
    }

    m_Buffer[m_FileIndex][count] = 0;
  }
  fin.close();

  m_SS[m_FileIndex] = new std::stringstream(m_Buffer[m_FileIndex]);

  m_File[m_FileIndex] = fopen(encFnFull(), "rt");
}

bool CTokenFile::OpenFile(Strng FnFull)
{
  Strng encFnFull;
  encFnFull = FnFull;
  encFnFull.Append(".x");
  m_File[m_FileIndex] = fopen(encFnFull(), "rt");
  if (m_File[m_FileIndex] == NULL)
  { // Encrypted file nonexistent.
    m_Buffer[m_FileIndex] = NULL;
    m_File[m_FileIndex] = fopen(FnFull(), "rt");
    if (m_File[m_FileIndex] == NULL)
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
flag CTokenFile::Open()
{
  ASSERT(m_File[0] == NULL); //file is allready open
  SetParameters();  
  Strng &Fn = m_FileNames[m_FileIndex];
  Strng FnO(Fn());
  
  Strng FnFull;
  Fn.FnDrivePath(FnO());
  if (Fn()==NULL && m_FileIndex>0)
    {// No Path Use Previous
    Fn.FnDrivePath(m_FileNames[m_FileIndex-1]());
    Fn+=FnO();
//                FileName=Fn();
    }
  else
    Fn=FnO;

  FnFull.FnSearchExpand(Fn(), m_bPlaces, AF_PrjFiles);
  Fn.FnContract();

  m_AllFileNames[m_IncFileCnt++] = Fn();

  bool result = OpenFile(FnFull);

  FnModifyTime(FnFull(), m_AllFileTimes[m_IncFileCnt-1]);
  
  return result;
};

//----------------------------------------------------------------------------
/*#F:Closes all remaining open files (if any)*/
void CTokenFile::Close()
{
  for (int i=m_FileIndex; i>=0; i--)
    {
    if (m_File[i] != NULL)
      {
      fclose(m_File[i]);
      m_File[i] = NULL;
      };
    };
};

//----------------------------------------------------------------------------
/*#F:#R:The token type of the specified string.*/
TokenTypes CTokenFile::GetTokenType(const char* p)
{
  if (p == NULL)    
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
    };
};

//----------------------------------------------------------------------------
/*#F:Parses the current line for a token. Skips whitespace and loads succesive 
lines if neccessary. If ReturnSameToken flag was set, the line is not parsed, the 
current token is returned and the flag unset. The new token type is determined.
#R:A pointer to the current token.*/
const char* CTokenFile::NextToken()
{
  if (m_ReturnSameToken)
    {
    m_ReturnSameToken = False;
    return (const char*)m_CurTok.Buffer();
    };
  m_CurTok = "";
  m_TokType = TokNull;
  while ((m_CurTok.Length() == 0) && (!m_AtEOF))
    {
    SkipWhiteSpace();
    while ((m_TokenLine.Length() == 0) && (!m_AtEOF))
      {
      NextLine();
      SkipWhiteSpace();
      };

    if (m_AtEOF)
      m_CurTok = "";
    else
      {
      m_FirstToken = m_NewLine; //kga 20/9/94
      char c = m_TokenLine[0];
      if (m_UseIncludes && (c==m_IncludeChars[0]))
        {
        Strng FileName = m_TokenLine;
        if (m_TokenLine.Find(m_IncludeChars) == 0)
          {
          FileName = FileName.Right(FileName.Length() - m_IncludeChars.Length());
          FileName.Trim(" \t");
          flag Failed = True;
          if (FileName.Length() > 0)
            {
            if ((FileName[0] == '\'') || (FileName[0] == '"'))
              FileName = FileName.Right(FileName.Length() - 1);
            if (FileName.Length() > 0)
              {
              if ((FileName[FileName.Length()-1] == '\'') || (FileName[FileName.Length()-1] == '"'))
                FileName = FileName.Left(FileName.Length() - 1);
              if (FileName.Length() > 0)
                {
                m_FileIndex++;
                if (m_FileIndex<MaxIncludeFiles)
                  {
                  Strng Fn, FnFull;
                  Fn.FnDrivePath(FileName());
                  if (Fn()==NULL && m_FileIndex>0)
                    {// No Path Use Previous
                    Fn.FnDrivePath(m_FileNames[m_FileIndex-1]());
                    Fn+=FileName();
                //          FileName=Fn();
                    }
                  else
                    Fn=FileName;

                  // CNM - Untested
                  if ((Fn[0]=='~') || (Fn[0]=='.')) // relative filename ?
                    { // yes - prepend current folder (defined by previously openned file - NOT O/S
                    Strng T;
                    T.FnDrivePath(m_AllFileNames[m_IncFileCnt-1]());
                    T+=Fn;
                    Fn=T;
                    }

                  FnFull.FnSearchExpand(Fn(), m_bPlaces);
                  Fn.FnContract();

                  m_FileNames[m_FileIndex] = Fn();
                  if (m_IncFileCnt<MaxTtlIncludeFiles)
                    m_AllFileNames[m_IncFileCnt++] = Fn();
                  m_LineNo--;
                  m_FileLineNo[m_FileIndex] = 0;

                  Failed = !OpenFile(FnFull);

                  //m_File[m_FileIndex] = fopen(FnFull(), "rt");
                  //if (m_File[m_FileIndex]!=NULL)
                  //  Failed = False;

                  FnModifyTime(FnFull(), m_FileTimes[m_FileIndex]);
                  if (m_IncFileCnt<=MaxTtlIncludeFiles)
                    m_AllFileTimes[m_IncFileCnt-1] = m_FileTimes[m_FileIndex];
                  }
                //else
                  //too many nested includes
                }
              }
            }
          if (Failed && m_pFailFn)
            {
            (*m_pFailFn)(FileName(), m_pFailExtraPtr);
            }
          m_CurTok = "";
          m_TokenLine = "";
          return NextToken();
          }
        }
      if ((m_IgnoreComments) && (c == ';'))
        {//comment found
        m_CurTok = "";
        m_TokenLine = "";
        }
      else if ((m_UseStringChar) && (c == m_StringChar))
        {
        m_TokType = TokString;
        m_TokenLine = m_TokenLine.Right(m_TokenLine.Length() - 1);
        int APos = (int)m_TokenLine.Find('"');
        if (APos<0)
          {
          m_CurTok = m_TokenLine;
          m_TokenLine = "";
          }
        else
          {
          m_CurTok = m_TokenLine.Left(APos);
          m_TokenLine = m_TokenLine.Right(m_TokenLine.Length() - APos - 1);
          };
        }
      else    
        {
        WORD i = 0;
        while ((i < m_SeperatorSetLen) && (m_SeperatorSet[i] != c))
          i++;
        if (i<m_SeperatorSetLen)
          {//token character found
          m_TokenLine = m_TokenLine.Right(m_TokenLine.Length() - 1);
          m_CurTok = c; 
          }
        else
          {//default action
          m_CurTok = SkipToSeperator();
          m_TokenLine = m_TokenLine.Right(m_TokenLine.Length() - m_CurTok.Length());
          };  
        };  
      };
    };
  m_NewLine = False;
  m_LastToken = False;
  if (m_TokenLine.Length() == 0)
    m_LastToken = True;
  if (m_TokType != TokString)
    m_TokType = GetTokenType((const char*)m_CurTok.Buffer());
  return (const char *)m_CurTok.Buffer();
};    

//----------------------------------------------------------------------------
/*#F:#R:The complete current token line regardless of any token seperators contained
within it.*/
const char* CTokenFile::GetNextLine()
{
  if (m_NewLine)
    NextLine();
  m_NewLine = True;
  if (m_UseIncludes && m_TokenLine.Length()>m_IncludeChars.Length() && m_IncludeChars[0]==m_TokenLine[0])
    {
    Strng FileName = m_TokenLine;
    if (m_TokenLine.Find(m_IncludeChars) == 0)
      {
      FileName = FileName.Right(FileName.Length() - m_IncludeChars.Length());
      FileName.Trim();
      VERIFY(FileName.Length() > 0);
      if ((FileName[0] == '\'') || (FileName[0] == '"'))
        FileName = FileName.Right(FileName.Length() - 1);
      VERIFY(FileName.Length() > 0);
      if ((FileName[FileName.Length()-1] == '\'') || (FileName[FileName.Length()-1] == '"'))
        FileName = FileName.Left(FileName.Length() - 1);
      VERIFY(FileName.Length() > 0);
      m_FileIndex++;

      Strng Fn, FnFull;
      Fn.FnDrivePath(FileName());
      if (Fn()==NULL && m_FileIndex>0)
        {// No Path Use Previous
        Fn.FnDrivePath(m_FileNames[m_FileIndex-1]());
        Fn+=FileName();
    //          FileName=Fn();
        }
      else
        Fn=FileName;

      FnFull.FnSearchExpand(Fn(), m_bPlaces);
      Fn.FnContract();

      VERIFY(m_FileIndex<MaxIncludeFiles);
      
      OpenFile(FnFull);
      //m_File[m_FileIndex] = fopen(FnFull(),"rt");
      m_FileNames[m_FileIndex]=Fn();

      FnModifyTime(FnFull(), m_FileTimes[m_FileIndex]);
//      if (m_IncFileCnt<=MaxTtlIncludeFiles)
//        m_AllFileTimes[m_IncFileCnt-1] = m_FileTimes[m_FileIndex];
      
      m_CurTok = "";
      m_TokenLine = "";
      return GetNextLine();
      }
    }
  m_CurTok = m_TokenLine;
  m_TokenLine = "";  
  return (const char*)m_CurTok.Buffer();
};

//----------------------------------------------------------------------------

const char* CTokenFile::TokenLine()
  {
  return (const char*)m_TokenLine.Buffer();
  }

//----------------------------------------------------------------------------
/*#F:#R:The first token found in the line supplied.*/
void CTokenFile::GetFirstToken( Strng &s,  //string to parse
                                Strng &Token)  //returned token string
{
  Strng Temp = m_TokenLine;
  m_TokenLine = s;
  SkipWhiteSpace();
  Strng Temp2 = m_TokenLine.Right(m_TokenLine.Length() - 1);
  Token = m_TokenLine.Left(1);
  if (strchr(m_SeperatorSet, (int)(Token[0])) == NULL)
    Token += Temp2.SpanExcluding(m_SeperatorSet);
  s = m_TokenLine;
  m_TokenLine = Temp;
};

//----------------------------------------------------------------------------
/*#F:Used internally to reset flags etc.*/
void CTokenFile::SetParameters()
{
  memset(m_Line, 0, sizeof(m_Line));
  m_AtEOF = False;
  m_CurTok = "";
  m_TokenLine = "";
  m_LineNo = 0;
  m_LastToken = False;
  m_FirstToken = False;
  m_ReturnSameToken = False;
  m_NewLine = True;
  m_Back[0] = 0;
  for (int i=0; i<MaxIncludeFiles; i++)
    {
    m_File[i] = NULL;
    m_FileLineNo[i] = 0L;
    }
  m_FileIndex = 0;
  m_IncFileCnt = 0;
};

//----------------------------------------------------------------------------
/*#F:Removes all the "whitespace" characters from the left of the current line.*/
void CTokenFile::SkipWhiteSpace()
{
  Strng Temp = m_TokenLine.SpanIncluding((const char*)m_WhiteSpaceSet);
  if (Temp.Length() != 0)
    m_TokenLine = m_TokenLine.Right(m_TokenLine.Length() - Temp.Length());
};

//----------------------------------------------------------------------------
/*#F:#R:All the characters upto the first token seperator.*/
Strng CTokenFile::SkipToSeperator() const
{
  return m_TokenLine.SpanExcluding(m_SeperatorSet);
};

char* CTokenFile::ssgets(int k)
{
  char* p = NULL;

  if (!(m_SS[m_FileIndex]->eof()))
  {
    p = new char[sizeof(m_Line) - 1 - k];
    m_SS[m_FileIndex]->getline(p, sizeof(m_Line) - 1 - k);

    char* match = strchr(p, 13);
    if (match != NULL) match[0] = 0; // handle strange line-end characters.
  }

  return p; // will be null if at eof...
}
//----------------------------------------------------------------------------
/*#F:Reads the next line from the file.*/
void CTokenFile::NextLine()
{
  if (m_File[m_FileIndex] != NULL)
    {
    // Add any characters from last time round to buffer
    int k = strlen(m_Back);
    if (k>0)
      {
      strrev(m_Back);
      strncpy(m_Line, m_Back, k);
      }

    char* p = NULL;
    if (m_Buffer[m_FileIndex] == NULL) // No buffer -- reading straight from file.
      p = fgets((char*)&m_Line[k], sizeof(m_Line) - 1 - k, m_File[m_FileIndex]);
    else // we've got a buffer, use that instead.
    {
      p = ssgets(k);
    }

    if (p == NULL)
      {
      if (m_FileIndex > 0)
        {
        m_LineNo--;
        fclose(m_File[m_FileIndex]);
        m_File[m_FileIndex] = NULL;
        m_FileIndex--;
        NextLine();
        }
      else
        m_AtEOF = True;
      }
    else
      {
      k = strlen(m_Line);
      if (k==MaxLineLen-2)
        {//strip chars from the end of the buffer until the next seperator
        char *pos = &m_Line[k-1];
        char *buf = &m_Back[0];
        *buf = NULL;
        while( strchr(m_SeperatorSet,(int)(*pos)) == NULL )
          {
          *buf = *pos;
          buf++;
          *buf = NULL;
          *pos = NULL;
          pos--;
          VERIFY(pos != m_Line); //("Line (without seperator) longer than backup buffer");
          }
        }
      else
        m_Back[0] = 0;
      //if ((Line[strlen(p)-1] == 10) || (Line[strlen(p)-1] == 13))
      if (m_Line[strlen(p)-1] == 10)
        m_Line[strlen(p)-1] = 0;
      }
    }
  else
    {
    if (m_FileIndex > 0)
      {
      m_File[m_FileIndex] = NULL;
      m_FileIndex--;
      }
    else
      m_AtEOF = True;
    m_NewLine = True;
    m_TokenLine = "";
    return;
    }
  m_NewLine = True;
  m_LineNo++;
  m_FileLineNo[m_FileIndex]++;
  m_TokenLine = (char*)&m_Line[0];
};

//==========================================================================
