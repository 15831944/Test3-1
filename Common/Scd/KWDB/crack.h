// crack.h

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef  __CRACK_H
#define  __CRACK_H

#define DllExport __declspec( dllexport )
#define DllImport __declspec( dllimport )

#ifdef __CRACK_CPP
  #define DllImportExport DllExport
#elif !defined(KWDB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class DllImportExport CCrack
{
public:
	static LPCTSTR strFieldType(ADOX::DataTypeEnum/*short*/ sType);
	static LPCTSTR strQueryDefType(ADOX::DataTypeEnum/*short*/ sType);
	static LPCTSTR strBOOL(BOOL bFlag);
	static CString strVARIANT(const COleVariant& var);
};

DllImportExport ADOX::DataTypeEnum/*short*/ ConvertStringTypeToDBType( const CString& strVar );

#undef DllImportExport

#endif