#include "stdafx.h"
#include "StackerManager.h"
#include "atlsafe.h"

CStackerManager::CStackerManager(void)
{
	m_bTerminate = false;
	m_hViewerUpdate = CreateEvent( 
        NULL,         // default security attributes
        TRUE,         // manual-reset event
        false,         // initial state is not signaled
        "DrawUpdateEvent"  // object name
        );

	m_hViewerReturn = CreateEvent( 
        NULL,         // default security attributes
        TRUE,         // manual-reset event
        false,         // initial state is not signaled
        "ViewerReturnEvent"  // object name
        ); 

	m_bViewerStarted = false;

}

CStackerManager::~CStackerManager(void)
{
	m_bTerminate = true;

	// Terminate viewer thread
	if(m_bViewerStarted)
	{
		ResetEvent(m_hViewerReturn);
		SetEvent(m_hViewerUpdate);

		// Mind you don't wait forever choose 1s
		WaitForSingleObject(m_hViewerReturn,1000);
	}

}

bool CStackerManager::Remove(CStacker * pStacker)
{
	vector<CStacker*>::iterator i;
	for( i = StackerArray.begin(); i != StackerArray.end(); i++)
	{
		CStacker * pSt = *i;
		if(pSt == pStacker)
		{
			StackerArray.erase(i);
			break;
		}
	}

	if(i == StackerArray.end()) return false;  

	int j = 0;
	for( i = StackerArray.begin(); i != StackerArray.end(); i++)
	{
		CStacker * pSt = *i;
		pSt->m_nArrayIndex = j;
		pSt->m_bTestMap.erase(pStacker);
		break;
		j++;
	}

	return false;
}

int CStackerManager::Add(CStacker * pStacker)
{
	int pos = StackerArray.size();
	StackerArray.push_back(pStacker); 
	return pos;

}

int CStackerManager::Count(void)
{
	return StackerArray.size();
}

CStacker * CStackerManager::Get(int index)
{
	return StackerArray[index];
}

void CStackerManager::UpdateDrawing(void)
{
	DISPPARAMS dp = {NULL, NULL, 0, 0};
	DISPPARAMS dpNULL = {NULL, NULL, 0, 0};
	CComVariant vPars[2];
	CComSafeArray<double> sa(2);
	EXCEPINFO  einfo;
	HRESULT hr;
	DRAWING_MODE nMode;

	dp.cArgs = 2;
	dp.rgvarg = vPars;


	if(NULL != pOGLViewer)
	{

		// Clear Drawing
		hr = pOGLViewer->Invoke(4,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dpNULL,NULL,&einfo,NULL);

		int nSize = StackerArray.size();
		for(int i = 0; i < StackerArray.size(); i++)
		{
			int nCount = StackerArray[i]->GetNoDrawingPackets();
			for (int j = 1; j <= nCount; j++)
			{
				StackerArray[i]->GetDrawingPacket(j,nMode,sa);
				vPars[0] = sa;
				vPars[1] = nMode;

				hr = pOGLViewer->Invoke(2,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dp,NULL,&einfo,NULL);
			}
		}

		// Update Drawing
		hr = pOGLViewer->Invoke(5,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dpNULL,NULL,&einfo,NULL);
	}
}

void CStackerManager::LoadBackGround(void)
{
	DISPPARAMS dp = {NULL, NULL, 0, 0};
	//CComBSTR   bstrFile("Path");
	CComVariant vPars(m_szFileName);
	EXCEPINFO  einfo;
	HRESULT hr;
	DRAWING_MODE nMode;

	dp.cArgs = 1;
	dp.rgvarg = &vPars;

	if(NULL != pOGLViewer)
		hr = pOGLViewer->Invoke(6,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dp,NULL,&einfo,NULL);


}

void CStackerManager::UpdateViewerThread(void)
{
	ResetEvent(m_hViewerReturn);

	// Allows Viewer Update
	SetEvent(m_hViewerUpdate);

	// m_hReturn is set when viewer has finished updating
	// This ensures two threads are not trying to access Stacker data
	WaitForSingleObject(m_hViewerReturn,INFINITE);
}

void CStackerManager::StartViewerThread(void)
{
	m_bTerminate = false;
	ResetEvent(m_hViewerUpdate);

	if(!m_bViewerStarted) 
		CreateThread(NULL, 0, RunView, (LPVOID) this , 0, &m_dwExeThread);
	m_bViewerStarted = true;
}

void CStackerManager::StopViewerThread(void)
{
	if(m_bViewerStarted) 
	{
		m_bTerminate = true;
		ResetEvent(m_hViewerReturn);
		SetEvent(m_hViewerUpdate);

		// Wait for Return
		WaitForSingleObject(m_hViewerReturn,INFINITE);
	}
		
	m_bViewerStarted = false;
}

DWORD __stdcall RunView(LPVOID param)
{
	HRESULT hr = CoInitialize(NULL);

	CStackerManager * pStackerManager = reinterpret_cast<CStackerManager *> (param);
    pStackerManager->pOGLViewer.CreateInstance(__uuidof(COGLViewerDoc));

	// On initial instance update the drawing
	pStackerManager->UpdateDrawing();
	pStackerManager->LoadBackGround();
	
	while(!pStackerManager->m_bTerminate)
	{
		WaitForSingleObject(pStackerManager->m_hViewerUpdate,INFINITE);
		pStackerManager->UpdateDrawing();
		//pStackerManager->GetCmds();
		ResetEvent(pStackerManager->m_hViewerUpdate);
		SetEvent(pStackerManager->m_hViewerReturn);
	}

	pStackerManager->pOGLViewer = NULL;

	CoUninitialize();

	SetEvent(pStackerManager->m_hViewerReturn);
	return 0;
}
void CStackerManager::GetCmds(void)
{
	DISPPARAMS dp = {NULL, NULL, 0, 0};
	CComVariant vPars[2];
	CComSafeArray<double> sa(3);
	EXCEPINFO  einfo;
	HRESULT hr;
	DRAWING_MODE nMode;

	vPars[0] = sa;
	vPars[1] = 1;
	dp.cArgs = 2;
	dp.rgvarg = vPars;


	if(NULL != pOGLViewer)
	{
		// Get Commands
		hr = pOGLViewer->Invoke(7,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dp,NULL,&einfo,NULL);

		double *pData;
		hr = SafeArrayAccessData(vPars[0].parray, (void **)&pData);
		double x1 = pData[0] ;
		double x2 = pData[1] ;
		double x3 = pData[2] ;
		SafeArrayUnaccessData(vPars[0].parray);

	}

}
