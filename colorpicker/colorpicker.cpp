#include "pch.h"
#include "framework.h"
#include "colorpicker.h"
#include "colorpickerDlg.h"
#include <tchar.h>
#include <thread>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CsteamuploadApp

BEGIN_MESSAGE_MAP(colorpickerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

colorpickerApp::colorpickerApp()
{
	
}

colorpickerApp theApp;
BOOL colorpickerApp::InitInstance()
{
	AfxInitRichEdit2();
	CWinApp::InitInstance();
	colorpickerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
	}
	else if (nResponse == IDABORT)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly2.\n");
	}


#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}
int colorpickerApp::ExitInstance()
{
	colorpickerDlg abc;
	Shell_NotifyIcon(NIM_DELETE, &abc.m_nidIconData);
	return 0;
}
