// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TS_EXPORTS
#define TS_API __declspec(dllexport)
#else
#define TS_API __declspec(dllimport)
#endif

// This class is exported from the TS.dll
//class TS_API CTS {
//public:
//	CTS(void);
	// TODO: add your methods here.
//};

extern TS_API int nTS;

TS_API int fnTS(void);
