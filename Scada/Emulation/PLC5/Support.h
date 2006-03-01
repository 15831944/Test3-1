  // -------------------------------------------------------------------------
// SysCAD Wrapper class - used to ensure that the LOCALE remains "C"

class CSCDFileDialog : public CFileDialog
  {
  protected:
    BOOL    m_ForOpen;
  public:
    BOOL    m_DoLCTests;
    BOOL    m_DoingPrjOpen;
    
    CSCDFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );
    ~CSCDFileDialog();
    virtual int DoModal( );
//    virtual BOOL OnFileNameOK( );
  };

// -------------------------------------------------------------------------
//
///*#C:This class is used to access INI type files. It provides more than the
//standard "Set/Get Profile" options.*/
//class CProfINIFile
//  {
//  public:
//    CString sFilename;
//    CString sTmpRdStr;
//
//    CProfINIFile(char* pFilename = "");
//    CProfINIFile(char* pDir, char* pFilename);
//    ~CProfINIFile();
//    double RdDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault);
//    void WrDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);
//    long RdLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nDefault);
//    void WrLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, long nValue);
//    int RdInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
//    void WrInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
//    CString &RdStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pDefault="");
//    void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, char* pValue);
//    DWORD RdSection(LPCTSTR lpszSection, char* pBuff, int BuffLen);
//    void WrSection(LPCTSTR lpszSection, char* pBuff);
//    DWORD RdSectionNames(char* pBuff, int BuffLen);
//    void Flush();
//    static void GetProfItem(char* &pBuff, char* &pName, char* &pExp);
//    inline void SetProfFilename(char* pFilename) { sFilename = pFilename; };
//    inline LPCTSTR Filename() { return sFilename; }
//    inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, CString& sValue) { WrStr(lpszSection, lpszEntry, (char*)(const char*)sValue); };
//    inline void WrStr(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR pValue) { WrStr(lpszSection, lpszEntry, (char*)pValue); };
//  };
//
// -------------------------------------------------------------------------
