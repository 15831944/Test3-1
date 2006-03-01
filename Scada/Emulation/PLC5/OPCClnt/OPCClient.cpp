//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPC ClientDoc.cpp
//  $Author    :  Jim Hansen
//
//  Description:  The OPC actions are mostly handled by this Doc.
//
//
//**************************************************************************
#include "stdafx.h"
#include "OPCClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The OPC data formats
UINT OPCSTMFORMATDATA = RegisterClipboardFormat(_T("OPCSTMFORMATDATA"));
UINT OPCSTMFORMATDATATIME = RegisterClipboardFormat(_T("OPCSTMFORMATDATATIME"));
UINT OPCSTMFORMATWRITECOMPLETE = RegisterClipboardFormat(_T("OPCSTMFORMATWRITECOMPLETE"));

/////////////////////////////////////////////////////////////////////////////
// COpcC_Client

//**************************************************************************
// COpcC_Client construction/destruction

COpcC_Client::COpcC_Client()
{
   groupHandle = 0;
//   pCurrentItem = NULL;
//   hView = NULL;
   transactionID = 0;
   dwConnection1 = 0;
   dwConnection2 = 0;
   testSink = new COpcC_AdviseSink;   // create the advise sink for notifications
   testSink->AddRef();

   callbackCP = new OPCCallbackObject;   // create the ConnectionPoint for notifications
   callbackCP->AddRef();
   shutdownCP = new OPCShutdownObject;   // create the ConnectionPoint for notifications
   shutdownCP->AddRef();
   dwShutdownConnection = 0;
   usingCP = FALSE;

   AfxOleLockApp();
   //theDoc = this;

//   // Everyone can connect back to IAdviseSink
//   HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
//      RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IDENTIFY, NULL, EOAC_NONE, NULL);
//   if (FAILED(hr))
//   {
//      TRACE(_T("CoInitializeSecurity failed, %lx"), hr);
//   }
}

COpcC_Client::~COpcC_Client()
{
   AfxOleUnlockApp();
   if( opcServer.IsOk() )
      DoOpcDisconnect();
   testSink->Release(); // OLE should clean this up, but may not have time!
   callbackCP->Release();
   shutdownCP->Release();
   Sleep( 100 );
}


//*******************************************************************
void COpcC_Client::DoOpcDisconnect()
{
   CWaitCursor wait;
   HRESULT hr = S_OK;
   if( opcServer.IsOk() && opcGroup.IsOk() )
   {
      if( dwShutdownConnection )
      {
         IConnectionPointContainer *pCPC = 0;
         hr = opcServer.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
         if( SUCCEEDED(hr) )
         {
            IConnectionPoint *pCallbackCP = 0;
            hr = pCPC->FindConnectionPoint(IID_IOPCShutdown, &pCallbackCP);
            if( SUCCEEDED(hr) )
            {
               hr = pCallbackCP->Unadvise(dwShutdownConnection);
               pCallbackCP->Release();
            }
            pCPC->Release();
         }
      }
      if( usingCP )
      {
         // OPC 2.0 ConnectionPoints
         IConnectionPointContainer *pCPC = 0;
         hr = opcGroup.QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
         if( SUCCEEDED(hr) && dwConnection1 )  // This server supports 2.0
         {
            IConnectionPoint *pCallbackCP = 0;
            hr = pCPC->FindConnectionPoint(IID_IOPCDataCallback, &pCallbackCP);
            if( SUCCEEDED(hr) )
            {
               hr = pCallbackCP->Unadvise(dwConnection1);
               pCallbackCP->Release();
            }
            pCPC->Release();
         }
      }
      else
      // call IDataObject::DUnadvise to turn off data notification
      {
          DataObject dataObject;
          HRESULT hr = dataObject.Attach( opcGroup );
          if( SUCCEEDED(hr) )
          {
             if( dwConnection1 )
                hr = dataObject.DUnadvise(dwConnection1);
             if( dwConnection2 )
                hr = dataObject.DUnadvise(dwConnection2);
             dataObject.Detach();
          }
       }

       opcServer.RemoveGroup(groupHandle, FALSE);

   }
   opcGroup.Detach();
   opcServer.Detach();

   Sleep( 100 );

   // now that the group is released and unadvised, no more data will
   // be sent from the server. It is safe to delete the items
   while( !items.IsEmpty() )
      delete items.RemoveTail();
}


//*******************************************************************
CString COpcC_Client::GetErrorString(LPCTSTR description, HRESULT error)
{
   CString temp(description);
   temp += _T(" ");
   TCHAR msg[MAX_PATH*5];
   DWORD ok = FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                  msg, MAX_PATH, NULL );
   if( ok )
   {
      TRACE(msg);
      TRACE(_T("\n"));
      temp += msg;
      return temp;
   }

   // else try the server
   LPWSTR pString = NULL;
   if( opcServer.IsOk() )
   {
      HRESULT hr = opcServer.GetErrorString( error, LOCALE_SYSTEM_DEFAULT, &pString );
      if( SUCCEEDED(hr) )
      {
         temp += pString;
         TRACE(temp);
         CoTaskMemFree( pString );
      }
      else
         temp += _T("OPC Server failed GetErrorString.");
   }
   else
      temp += _T("Unknown Error.");
   return temp;
}

//*******************************************************************
void COpcC_Client::ReportError(LPCTSTR description, HRESULT error)
{
   CString msg( GetErrorString(description,error) );
   CString appName((LPCSTR)AFX_IDS_APP_TITLE);
   MessageBox(0, msg, appName, MB_OK);
}

//**************************************************************************
//
// VariantToString() convert the value to String format
//
//**************************************************************************
void VariantToString(VARIANT value, CString& strText)
{
    switch(value.vt)
    {
        case VT_BOOL:
            strText = value.boolVal ? _T("On") : _T("Off");
            break;
        case VT_UI1:
            strText.Format( _T("%hd"), (USHORT)value.bVal );
            break;
        case VT_UI2:
        case VT_I2:
            strText.Format( _T("%hd"), value.iVal );
            break;
        case VT_I4:
            strText.Format( _T("%d"), value.lVal );
            break;
        case VT_R4:
            strText.Format( _T("%g"), value.fltVal );
            break;
        case VT_R8:
            strText.Format( _T("%g"), value.dblVal );
            break;
        case VT_BSTR:
            strText = value.bstrVal;
            break;
        default: // Arrays of simple types
        {
            if( (value.vt & VT_ARRAY)==VT_ARRAY )
            {
                CString temp;
                SAFEARRAY* pArray = value.parray;
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound( pArray, 1, &lBound );
                SafeArrayGetUBound( pArray, 1, &uBound );
                for( long element=lBound; element<=uBound; element++ )
                {
                    if( !strText.IsEmpty() )
                        strText += _T(", ");
                    switch( value.vt & ~VT_ARRAY )
                    {
                    case VT_BOOL:
                        {
                            VARIANT_BOOL b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp = b ? _T("1") : _T("0");
                        }
                        break;
                    case VT_UI1:
                        {
                            BYTE b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp.Format( _T("%hd"), b );
                        }
                        break;
                    case VT_UI2:
                    case VT_I2:
                        {
                            short b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp.Format( _T("%hd"), b );
                        }
                        break;
                    case VT_I4:
                        {
                            long b=0;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp.Format( _T("%d"), b );
                        }
                        break;
                    case VT_R4:
                        {
                            float d=0;
                            SafeArrayGetElement(pArray, &element, &d);
                            temp.Format( _T("%g"), d );
                        }
                        break;
                    case VT_R8:
                        {
                            double d=0;
                            SafeArrayGetElement(pArray, &element, &d);
                            temp.Format( _T("%g"), d );
                        }
                        break;
                    case VT_BSTR:
                        {
                            BSTR b;
                            SafeArrayGetElement(pArray, &element, &b);
                            temp = b;
                        }
                        break;
                    }
                    strText += temp;
                }
            }
            else
                strText = _T("?");
        }
    }
}
