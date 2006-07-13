// ScdLicenseApp.cpp : Implementation of CScdLicense
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdLicenseApp.h"
#include "licbase.h"

/////////////////////////////////////////////////////////////////////////////
// CScdLicenseApp

STDMETHODIMP CScdLicenseApp::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdLicenseApp
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdLicenseApp::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

// =======================================================================
//
//
//
// =======================================================================

const int MaxLicOptions = 32;
LPCTSTR sOptions[MaxLicOptions]=
  {
  "59af52a4", // Option0
  "8dcdcf84", // Option1
  "7044f3aa", // Option2
  "3c789696", // Option3
  "79ecbcb1", // Option4
  "2c1fd2a9", // Option5
  "df19f750", // Option6
  "1dcc1b90", // Option7
  "d42b6293", // Option8
  "3baef293", // Option9
  "6cbf97fb", // Option10
  "ec0c7284", // Option11
  "c265b1f8", // Option12
  "805e4a43", // Option13
  "2cd91202", // Option14
  "ba59d0db", // Option15
  "e957eb19", // Option16
  "3662951d", // Option17
  "a66801db", // Option18
  "04fd309e", // Option19
  "3ae59281", // Option20
  "021ca62e", // Option21
  "a68e3fb0", // Option22
  "644eacfd", // Option23
  "393c86ef", // Option24
  "4041572e", // Option25
  "f8d87801", // Option26
  "c5ef89e6", // Option27
  "c2fd98f9", // Option28
  "c1c1c401", // Option29
  "63806175", // Option30
  "aaf0075b", // Option31
  };           


STDMETHODIMP CScdLicenseApp::GetKey(BSTR * KeyString)
  {
	appSCD_COMENTRYGET(long, KeyString)
		{
    if (m_iLicenseAttempt<3)
      {
      UUID AGuid;

      ASSERT_ALWAYS(UuidCreate(&AGuid)==RPC_S_OK, "Guid Not Created");

      m_sLicenseGUID.Format("%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x", 
        AGuid.Data1, AGuid.Data2, AGuid.Data3, 
        AGuid.Data4[0], AGuid.Data4[1], AGuid.Data4[2], AGuid.Data4[3], AGuid.Data4[4], AGuid.Data4[5], AGuid.Data4[6], AGuid.Data4[7]);
      }
    else
      m_sLicenseGUID="";
    
    _bstr_t t(m_sLicenseGUID);
		*KeyString=t.copy();
		}		
	SCD_COMEXIT
  }

STDMETHODIMP CScdLicenseApp::SetLicense(BSTR LicenseString, long * ErrorReturn)
  {
  appSCD_COMENTRY(long)
    {
    *ErrorReturn=-1;
    if (m_iLicenseAttempt++<3)
      {
      CString LicString;
      LicString=LPCTSTR(_bstr_t(LicenseString));
      CString TheKey;
      int iGuid=m_sLicenseGUID.GetLength()-1;
      for (int iKey=0; iKey<LicString.GetLength(); iKey+=2)
        {
        CString Part=LicString.Mid(iKey, 2);
        int iChar;
        sscanf(Part, "%x", &iChar);
        TheKey.AppendChar(iChar ^ m_sLicenseGUID[iGuid--]);
        if (iGuid<0)
          iGuid=m_sLicenseGUID.GetLength()-1;
        }

      CString kProduct; 
      CString kClient; 
      CString kExpTime;
      CString kScdVer;
      CString kDiskDrive;
      CString kDiskNo;
      CString kOption; 
      dword dwOptions=0;

      int iTkn=0;
      int iCurPos=0;
      CString Tkn=TheKey.Tokenize("\n", iCurPos);
      for ( ;Tkn!=""; iTkn++)
        {
        switch (iTkn)
          {
          case 0: kProduct    = Tkn;  break;
          case 1: kClient     = Tkn;  break;
          case 2: kExpTime    = Tkn;  break;
          case 3: kScdVer     = Tkn;  break;
          case 4: kDiskDrive  = Tkn;  break;
          case 5: kDiskNo     = Tkn;  break;
          default : // iTkn>=6
            {
            kOption = Tkn;
            for (int iOpt=0; iOpt<MaxLicOptions; iOpt++)
              {
              if (kOption==sOptions[iOpt])
                dwOptions |= (1<<(iTkn-6));  
              }            
            break;
            }
          }

        Tkn=TheKey.Tokenize("\n", iCurPos);
        }
      //CString kProduct; 
      //CString kClient; 
      //CString kOption; 
      //CString kDisk;
      //CString kExpTime;
      //dword dwOptions=0;

      //int iTkn=0;
      //int iCurPos=0;
      //CString Tkn=TheKey.Tokenize("\n", iCurPos);
      //for ( ;Tkn!=""; iTkn++)
      //  {
      //  switch (iTkn)
      //    {
      //    case 0: kProduct    = Tkn;  break;
      //    case 1: kClient     = Tkn;  break;
      //    case 2: kDisk       = Tkn;  break;
      //    case 3: kExpTime    = Tkn;  break;
      //    default : // iTkn>=4
      //      {
      //      kOption = Tkn;
      //      for (int iOpt=0; iOpt<MaxLicOptions; iOpt++)
      //        {
      //        if (kOption==sOptions[iOpt])
      //          dwOptions |= (1<<(iTkn-4));  
      //        }            
      //      break;
      //      }
      //    }


      //  Tkn=TheKey.Tokenize("\n", iCurPos);
      //  }

      //Strng Drive(ProgFiles());
      //Drive.FnDrive();
      //Drive+="\\";;

      char VolumeName[1024];
      char FSNameBuff[1024];
      DWORD dwVolSerialNo, dwMaxCompLen, dwFileSysFlags;
      BOOL GotIt=GetVolumeInformation(kDiskDrive, //Drive(),
                                      VolumeName, sizeof(VolumeName)-1,
                                      &dwVolSerialNo, &dwMaxCompLen, &dwFileSysFlags,
                                      FSNameBuff, sizeof(FSNameBuff)-1);

      CString m_sDisk; 
      m_sDisk.Format("%x", dwVolSerialNo);
                 
      if (m_sDisk.CompareNoCase(kDiskNo)==0)
        {
        m_iLicenseAttempt=0; // Reset Attempts
        if (kScdVer=="1.0")
          {
          *ErrorReturn=0;
          if (dwOptions & 0x00000001)
            gs_License.SetForMineServe(true);
          if (dwOptions & 0x00000002)
            {};
          if (dwOptions & 0x00000004)
            {};
          if (dwOptions & 0x00000005)
            {};
          }
        else
          *ErrorReturn=3;
        }
      else
        {
        dwOptions=0;
        *ErrorReturn=2;
        }
      }
    else
      *ErrorReturn=1;
    }
  SCD_COMEXIT
  }

// =======================================================================
//
//
//
// =======================================================================

/*
    if (0)
      {
                       
      Strng Drive(ProgFiles());
      Drive.FnDrive();
      Drive+="\\";;

      char VolumeName[1024];
      char FSNameBuff[1024];
      DWORD dwVolSerialNo, dwMaxCompLen, dwFileSysFlags;
      BOOL GotIt=GetVolumeInformation(Drive(),
                                      VolumeName, sizeof(VolumeName)-1,
                                      &dwVolSerialNo, &dwMaxCompLen, &dwFileSysFlags,
                                      FSNameBuff, sizeof(FSNameBuff)-1);

      UUID AGuid;

      ASSERT_ALWAYS(UuidCreate(&AGuid)==RPC_S_OK, "Guid Not Created");

      CString m_sGUID;
      m_sGUID.Format("%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x", 
          AGuid.Data1, AGuid.Data2, AGuid.Data3, 
          AGuid.Data4[0], AGuid.Data4[1], AGuid.Data4[2], AGuid.Data4[3], AGuid.Data4[4], AGuid.Data4[5], AGuid.Data4[6], AGuid.Data4[7]);

      CString m_sDisk; 
      CString m_sExpTime; 

      m_sDisk.Format("%08x", dwVolSerialNo);

      CString m_sKeyIn;
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< out 
      // Key goes out

      // ========== MineServe
      if (1)
        {
        LPCTSTR Product="Mineserve 1.0"; 
        LPCTSTR Client="AngloPlats RB"; 
        LPCTSTR ExpTime="20 Sept 2006";
        LPCTSTR Option="59af52a4"; 

        CString Key;
        Key.Format("%s\n%s\n%s\n%s\n%s", Product, Client, m_sDisk, ExpTime, Option);
        CString KeyOut;
        int iGuid=m_sGUID.GetLength()-1;
        for (int iKey=0; iKey<Key.GetLength(); iKey++)
          {
          int iChar= Key[iKey] ^ m_sGUID[iGuid--];
          CString sChar;
          sChar.Format("%02x", iChar);

          //KeyOut.SetAt(iKey, Key[iKey] ^ m_sGUID[iGuid--]);
          KeyOut+=sChar;
          if (iGuid<0)
            iGuid=m_sGUID.GetLength()-1;
          }

        m_sKeyIn=KeyOut;
        }

      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Back
      //Unscramble



      CString TheKey;
      int iGuid=m_sGUID.GetLength()-1;
      for (int iKey=0; iKey<m_sKeyIn.GetLength(); iKey+=2)
        {
        CString Part=m_sKeyIn.Mid(iKey, 2);
        int iChar;
        sscanf(Part, "%x", &iChar);
        TheKey.AppendChar(iChar ^ m_sGUID[iGuid--]);
        if (iGuid<0)
          iGuid=m_sGUID.GetLength()-1;
        }

      CString kProduct; 
      CString kClient; 
      CString kOption; 
      CString kDisk;
      CString kExpTime;
      dword dwOptions=0;

      int iTkn=0;
      int iCurPos=0;
      CString Tkn=TheKey.Tokenize("\n", iCurPos);
      for ( ;Tkn!=""; iTkn++)
        {
        switch (iTkn)
          {
          case 0: kProduct    = Tkn;  break;
          case 1: kClient     = Tkn;  break;
          case 2: kDisk       = Tkn;  break;
          case 3: kExpTime    = Tkn;  break;
          default : // iTkn>=4
            {
            kOption = Tkn;
            for (int iOpt=0; iOpt<MaxLicOptions; iOpt++)
              {
              if (kOption==sOptions[iOpt])
                dwOptions |= (1<<(iTkn-4));  
              }            
            break;
            }
          }



        Tkn=TheKey.Tokenize("\n", iCurPos);
        }

      if (m_sDisk==kDisk)
        {

        int xx=0;
        }
      else
        {
        dwOptions=0;
        int xx=0;
        }


      int xxx=0;
      }

*/
// =======================================================================
//
//
//
// =======================================================================
