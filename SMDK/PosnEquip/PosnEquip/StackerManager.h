#pragma once
#include "Stacker.h"
#include <vector>
#import "OGLViewer.tlb"  no_namespace

using namespace std;

// Viewer Thread procedure
DWORD WINAPI RunView(LPVOID param);

class CStackerManager
{
public:
	CStackerManager(void);
public:
	~CStackerManager(void);

private :
vector<CStacker *> StackerArray;

public:
	bool Remove(CStacker * pStacker);
public:
	int Add(CStacker * pStacker);
	int Count(void);
	CStacker * Get(int index);
public:
	void UpdateDrawing(void);
public:
	void ThreadFunction(void);

	HANDLE m_hViewerUpdate,m_hViewerReturn;

	DWORD m_dwExeThread;
	bool m_bTerminate, m_bViewerStarted;
	IOGLViewerPtr pOGLViewer;
	CString m_szFileName;

public:
	void LoadBackGround(void);
	void UpdateViewerThread(void);
	void StartViewerThread(void);
	void StopViewerThread(void);
public:
	void GetCmds(void);
};
