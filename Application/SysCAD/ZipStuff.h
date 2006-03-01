//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ZIPSTUFF_H
#define  __ZIPSTUFF_H

//---------------------------------------------------------------------------
#if WITHZIP

class CZipFile
  {
  protected:
    Strng      m_Name;
    Strng      m_Dest;
    int        m_nCmds;
    char **    m_Cmds;

  public:
    CZipFile(LPCTSTR Nm=NULL, LPCTSTR Dest=NULL)
      {
      m_nCmds=0;
      m_Cmds=NULL;
      m_Name=Nm ? Nm : "";
      m_Dest=Dest ? Dest : "";
      }

    ~CZipFile()
      {
      delete m_Cmds;
      }

    void Init() 
      {
      delete m_Cmds;
      m_Cmds=NULL;
      m_nCmds=0;
      };

    void SetName(LPCTSTR Nm=NULL) { m_Name=Nm; }
    LPCTSTR GetName() { return m_Name(); }
    void SetDest(LPCTSTR Nm=NULL) { m_Dest=Nm; }
    LPCTSTR GetDest() { return m_Dest(); }

    int AddCmd(LPCTSTR Cmd);
    int ZipIt();
    int UnZipIt();

    static int ZipOne(LPCTSTR FileName, bool DeleteOriginal);
    static int UnZipOne(LPCTSTR FileName);

  };

#endif
//---------------------------------------------------------------------------

#endif