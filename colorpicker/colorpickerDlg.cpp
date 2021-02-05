
// hasher2Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "colorpicker.h"
#include "colorpickerDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"
#include <thread>
#include <fstream>
#include <Windows.h>
template<class TYPE>
bool RegSetKey(HKEY key, LPSTR keyloc, unsigned long type, REGSAM access, LPSTR name, TYPE indatax);
int RegCrtKey(HKEY key, LPSTR keyloc, REGSAM access);
template<class TYPE>
int RegGetKey(HKEY key, LPSTR keyloc, unsigned long type, REGSAM access, LPSTR name, TYPE outdatax);
DWORD g_BytesTransferred = 0;

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped
);
VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped)
{
	_tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	_tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	g_BytesTransferred = dwNumberOfBytesTransfered;
}
#pragma warning( disable:4244)
#pragma warning(disable:6387)
#pragma warning(disable:6011)
#pragma warning(disable:28182)
#pragma warning(disable:4805)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
void colorpickerDlg::convertohex(int value, char* buffer)
{
	int writes=0;
	char buffs[50] = {};
	char hexarray[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	if (value == 0)
	{
		sprintf_s(buffer,5, "00");
	} else
	{ 

		int val = value;
		while (val > 0) {
			int rem = val % 16;
			if(value<15)
			{
				char buf[5] = {};
				char point = hexarray[rem];
				sprintf_s(buf, "0%s",&point);
				strcat_s(buffs, buf);
			}
			else
			{
				char point = hexarray[rem];
				strcat_s(buffs, &point);
			}
			writes++;
			val = val / 16;
		}
		int tmp=writes;
		if(value < 15)
		{ 
			strcpy_s(buffer,7, buffs);
		} else
		{ 
			for (int i = 0; i <= writes; i++)
			{
				char point = buffs[tmp];
				strcat_s(buffer,7, &point);
				tmp--;
			}
		}
	}
}
colorpickerDlg::colorpickerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_COLORPICKER_DIALOG, pParent)
{
	m_nidIconData.cbSize = sizeof(NOTIFYICONDATA);
	m_nidIconData.hWnd = 0;
	m_nidIconData.uID = 1;
	m_nidIconData.uCallbackMessage = WM_TRAY_ICON_NOTIFY_MESSAGE;
	m_nidIconData.hIcon = 0;
	m_nidIconData.szTip[0] = 0;
	m_nidIconData.uFlags = NIF_MESSAGE;
	m_bTrayIconVisible = FALSE;
	m_nDefaultMenuItem = 0;
	m_bMinimizeToTray = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_ICON);
}

void colorpickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
template<class TYPE>
bool RegSetKey(HKEY key,LPSTR keyloc,unsigned long type, REGSAM access,LPSTR name,TYPE indatax)
{
	unsigned long size = sizeof(type);
	char errorbuf[200];
	HKEY keyval;
	bool onerr = 1;
	int err;
	err = RegOpenKeyExA(key, keyloc, NULL, access, &keyval);
	if (err != ERROR_SUCCESS)
	{
		sprintf_s(errorbuf, "%i\n", err);
		onerr = 0;
		ASSERT(errorbuf);
	} else if(err == ERROR_SUCCESS)
	{ 
		err = RegSetValueExA(keyval, name, NULL, type, (BYTE*)indatax, size);
		if (err != ERROR_SUCCESS)
		{
			sprintf_s(errorbuf, "%i\n", err);
			onerr = 0;
			ASSERT(errorbuf);
		}
	}
	
		CloseHandle(keyval);
	return onerr;
}
int RegCrtKey(HKEY key, LPSTR keyloc, REGSAM access)
{
	HKEY keyval;
	int err;
	char errorbuf[200];
	DWORD dispvalue;
	err = RegCreateKeyExA(key, keyloc, NULL, NULL, REG_OPTION_NON_VOLATILE, access,NULL, &keyval, &dispvalue);
	CloseHandle(keyval);
	if (err == ERROR_SUCCESS)
	{
		if (dispvalue == REG_CREATED_NEW_KEY)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		sprintf_s(errorbuf, "%i\n", err);
		ASSERT(errorbuf);
		return 0;
	}
	//return onerr;
}
template<class TYPE>
int RegGetKey(HKEY key, LPSTR keyloc, unsigned long type, REGSAM access, LPSTR name, TYPE outdatax)
{
	unsigned long size = sizeof(type);
	char errorbuf[200];
	HKEY keyval;
	int onerr = 0;
	int err;
	err = RegOpenKeyExA(key, keyloc, NULL, access, &keyval);
	if (err != ERROR_SUCCESS)
	{
		onerr = false;
	}
	err = RegQueryValueExA(keyval, name, NULL, &type, (BYTE*)outdatax, &size);
	switch (err)
	{
	case ERROR_FILE_NOT_FOUND:
	{
		onerr = 2;
		break;
	} 
	case ERROR_MORE_DATA:
	{
		onerr = 3;
		break;
	}
	case ERROR_SUCCESS:
	{
		onerr = 1;
		break;
	}
	default:
	{
		sprintf_s(errorbuf, "%i\n", err);
		ASSERT(errorbuf);
		onerr = 0;
		break;
	}
	}
		CloseHandle(keyval);
	return onerr;
}

BEGIN_MESSAGE_MAP(colorpickerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE, OnTrayNotify)
	ON_BN_CLICKED(IDC_TRAYEN, &colorpickerDlg::OnBnClickedTrayEn)
	ON_BN_CLICKED(IDC_MINEN, &colorpickerDlg::OnBnClickedMinEn)
	ON_BN_CLICKED(IDC_EXIT, &colorpickerDlg::OnBnClickedExit)
	ON_COMMAND(ID_MENU_EXIT, &colorpickerDlg::OnBnClickedExit)
	ON_COMMAND(ID_MENU_MAX, &colorpickerDlg::OnOpen)
	ON_COMMAND(ID_MENU_MIN, &colorpickerDlg::OnMinimize)
	ON_BN_CLICKED(IDC_HEX1, &colorpickerDlg::OnBnClickedHex1)
	ON_BN_CLICKED(IDC_HEX2, &colorpickerDlg::OnBnClickedHex2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_RED, &colorpickerDlg::OnNMCustomdraw)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_GREEN, &colorpickerDlg::OnNMCustomdraw)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BLUE, &colorpickerDlg::OnNMCustomdraw)
END_MESSAGE_MAP()
BOOL colorpickerDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	MSGFILTER* lpMsgFilter = (MSGFILTER*)lParam;

	/*if ((wParam == IDC_MD5OUT) && (lpMsgFilter->nmhdr.code == EN_MSGFILTER)
		&& (lpMsgFilter->msg == WM_RBUTTONDOWN))

	{
		//if we get through here, we have trapped the right click event of the richeditctrl! 
		CPoint point;
		::GetCursorPos(&point); //where is the mouse?
		CMenu menu; //lets display out context menu :) 
		UINT dwSelectionMade;
		VERIFY(menu.LoadMenu(IDR_HASH));
		CMenu* pmenuPopup = menu.GetSubMenu(0);
		ASSERT(pmenuPopup != NULL);
		dwSelectionMade = pmenuPopup->TrackPopupMenu((TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD),
			point.x, point.y, this
		);

		pmenuPopup->DestroyMenu();
		PostMessage(dwSelectionMade, 0, 0);
	}*/

	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL colorpickerDlg::OnInitDialog()
{
	returned = (wchar_t*)malloc(300);
	int out;
	DWORD indata =  1;
	DWORD outdata = 0;
	BYTE cmp = 1;
	type = REG_DWORD;

	//AfxInitRichEdit2();
	
	colorpickerDlg::ShowWindow(SW_SHOW);
	colorpickerDlg::RedrawWindow();
	colorpickerDlg::CenterWindow();
	CDialog::OnInitDialog();
	out = RegCrtKey(HKEY_CURRENT_USER, "Software\\ColorPicker", KEY_ALL_ACCESS | KEY_WOW64_64KEY);
	if (out == 1)
	{
		RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &indata);
		RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
	}
	else if(out == 0)
	{
		trayenable = 1;
		minimizeen = 1;
		m_bMinimizeToTray = TRUE;
	}
	else if (out == 2)
	{
		out = RegGetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", NULL, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &outdata);
		if (out == 2)
		{
			
			RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
			
			minimizeen = 1;
			m_bMinimizeToTray = TRUE;
		} else if (out == 1)
		{ 
			
			if (outdata == 1)
			{
				minimizeen = 1;
				m_bMinimizeToTray = TRUE;
			}
			else 
			{
				minimizeen = 0;
				m_bMinimizeToTray = FALSE;
			}
		}
		out = RegGetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &outdata);
		if (out == 2)
		{

			RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
			trayenable = 1;
		}
		else if (out == 1)
		{ 
			if (outdata == 1)
			{
				trayenable = 1;
			}
			else
			{
				trayenable = 0;
			}
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	TraySetIcon(m_hIcon);
	TraySetToolTip(L"Steamcloud");
	TraySetMenu(ID_MENU_MINIMIZE);
	
	if (colorpickerDlg::IsWindowVisible() != 0)
	{
		
		color = (CMFCButton*)GetDlgItem(IDC_COLORBUTTON);
		checkbox = (CButton*)GetDlgItem(IDC_MINEN);
		trayen = (CButton*)GetDlgItem(IDC_TRAYEN);
		red = (CSliderCtrl*)GetDlgItem(IDC_RED);
		green = (CSliderCtrl*)GetDlgItem(IDC_GREEN);
		blue = (CSliderCtrl*)GetDlgItem(IDC_BLUE);
		hex1 = (CButton*)GetDlgItem(IDC_HEX1);
		hex2 = (CButton*)GetDlgItem(IDC_HEX2);
		hex1->SetCheck(BST_CHECKED);
		hex2->SetCheck(BST_UNCHECKED);
		red->SetRange(0, 255, 1);
		blue->SetRange(0, 255, 1);
		green->SetRange(0, 255, 1);
		color->SetFaceColor(RGB(0, 0, 0));
		SetDlgItemTextA(this->m_hWnd, IDC_HEXOUT, "000000");
		if (minimizeen)
		{
			CheckDlgButton(IDC_MINEN, BST_CHECKED);
		}
		else
		{
			CheckDlgButton(IDC_MINEN, BST_UNCHECKED);
		}
		if (trayenable)
		{
			CheckDlgButton(IDC_TRAYEN, BST_CHECKED);
			TrayShow();
		}
		else
		{
			CheckDlgButton(IDC_TRAYEN, BST_UNCHECKED);
			TrayHide();
		}
		
	}
	init = true;
	return TRUE;
}
void colorpickerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	colorpickerDlg::ShowWindow(SW_SHOW);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR colorpickerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void colorpickerDlg::OnBnClickedExit()
{
	colorpickerDlg::OnDestroy();
	exit(2);
}

void colorpickerDlg::OnBnClickedMinEn()
{
	DWORD indata = 0;
	int boxcheck = checkbox->GetCheck();
	if (boxcheck == BST_CHECKED)
	{
		indata = 1;
		RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
		m_bMinimizeToTray = TRUE;
		minimizeen = true;
	}
	else
	{
		indata = 0;
		RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
		m_bMinimizeToTray = FALSE;
		minimizeen = false;
	}
}
void colorpickerDlg::OnBnClickedTrayEn()
{
	int tren;
	DWORD indata = 0;
	tren = trayen->GetCheck();
	if (tren == BST_CHECKED)
	{
		indata = 1;
		RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &indata);
		trayenable = true;
		checkbox->EnableWindow();
		TrayShow();
	}
	else
	{
		indata = 0;
		RegSetKey(HKEY_CURRENT_USER, "Software\\ColorPicker", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &indata);
		trayenable = false;
		m_bMinimizeToTray = FALSE;
		checkbox->EnableWindow(0);
		TrayHide();
	}
}
void colorpickerDlg::OnOpen()
{
	if (this->IsWindowVisible())
	{ 
		this->ShowWindow(SW_NORMAL);
		this->SetFocus();
	}
	else
	{	
		this->ShowWindow(SW_SHOW);
		this->SetFocus();
	}
	
	
}

int colorpickerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nidIconData.hWnd = this->m_hWnd;
	m_nidIconData.uID = 1;

	return 0;
}

void colorpickerDlg::OnDestroy()
{
	CDialog::OnDestroy();
	if (m_nidIconData.hWnd && m_nidIconData.uID > 0 && TrayIsVisible())
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nidIconData);
	}
}

BOOL colorpickerDlg::TrayIsVisible()
{
	return m_bTrayIconVisible;
}

void colorpickerDlg::TraySetIcon(HICON hIcon)
{
	ASSERT(hIcon);

	m_nidIconData.hIcon = hIcon;
	m_nidIconData.uFlags |= NIF_ICON;
}

void colorpickerDlg::TraySetIcon(UINT nResourceID)
{
	ASSERT(nResourceID > 0);
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(nResourceID);
	if (hIcon)
	{
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void colorpickerDlg::TraySetIcon(LPCTSTR lpszResourceName)
{
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
	if (hIcon)
	{
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void colorpickerDlg::TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT(strlen((char*)lpszToolTip) > 0 && strlen((char*)lpszToolTip) < 64);

	wcscpy_s(m_nidIconData.szTip,lpszToolTip);
	m_nidIconData.uFlags |= NIF_TIP;
}

BOOL colorpickerDlg::TrayShow()
{
	BOOL bSuccess = FALSE;
	if (!m_bTrayIconVisible && trayenable)
	{
		bSuccess = Shell_NotifyIcon(NIM_ADD, &m_nidIconData);
		if (bSuccess)
			m_bTrayIconVisible = TRUE;
	}
	else
	{
		TRACE0("ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

BOOL colorpickerDlg::TrayHide()
{
	BOOL bSuccess = FALSE;
	if (m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_DELETE, &m_nidIconData);
		if (bSuccess)
			m_bTrayIconVisible = FALSE;
	}
	else
	{
		TRACE0("ICON ALREADY HIDDEN");
	}
	bSuccess = TRUE;
	return bSuccess;
}

BOOL colorpickerDlg::TrayUpdate()
{
	BOOL bSuccess = FALSE;
	if (m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_MODIFY, &m_nidIconData);
	}
	else
	{
		TRACE0("ICON NOT VISIBLE");
	}
	return bSuccess;
}


BOOL colorpickerDlg::TraySetMenu(UINT nResourceID, UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = m_mnuTrayMenu.LoadMenu(nResourceID);
	return bSuccess;
}


BOOL colorpickerDlg::TraySetMenu(LPCTSTR lpszMenuName, UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = m_mnuTrayMenu.LoadMenu(lpszMenuName);
	return bSuccess;
}

BOOL colorpickerDlg::TraySetMenu(HMENU hMenu, UINT nDefaultPos)
{
	m_mnuTrayMenu.Attach(hMenu);
	return TRUE;
}

LRESULT colorpickerDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID;
	UINT uMsg;

	uID = (UINT)wParam;
	uMsg = (UINT)lParam;

	if (uID != 1)
		return false;

	CPoint pt;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayMouseMove(pt);
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDown(pt);
		break;
	case WM_LBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDblClk(pt);
		break;

	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		//ClientToScreen(&pt);
		OnTrayRButtonDown(pt);
		break;
	case WM_RBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayRButtonDblClk(pt);
		break;
	}
	return true;
}
void colorpickerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (m_bMinimizeToTray)
	{
		if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
		
			if (TrayShow() || m_bTrayIconVisible)
			{
				this->ShowWindow(SW_HIDE);
			}
		}
		else
		{
			CDialog::OnSysCommand(nID, lParam);
		}
	}
	else
	{ 
		CDialog::OnSysCommand(nID, lParam);
	}
}
void colorpickerDlg::TraySetMinimizeToTray(BOOL bMinimizeToTray)
{
	m_bMinimizeToTray = bMinimizeToTray;
}


void colorpickerDlg::OnTrayRButtonDown(CPoint pt)
{
	m_mnuTrayMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	m_mnuTrayMenu.GetSubMenu(0)->SetDefaultItem(m_nDefaultMenuItem, TRUE);
}

void colorpickerDlg::OnTrayLButtonDown(CPoint pt)
{

}

void colorpickerDlg::OnTrayLButtonDblClk(CPoint pt)
{
	if (m_bMinimizeToTray)
	{


		this->ShowWindow(SW_SHOW);
		this->ShowWindow(SW_MAXIMIZE);
		this->SetFocus();
	}
}

void colorpickerDlg::OnTrayRButtonDblClk(CPoint pt)
{
}

void colorpickerDlg::OnTrayMouseMove(CPoint pt)
{
}
void colorpickerDlg::OnMinimize()
{
	if(this->IsWindowVisible())
	{ 
		if (m_bMinimizeToTray && trayenable)
		{
			if (minimizeen)
			{
				ShowWindow(SW_HIDE);
				ShowWindow(SW_MINIMIZE);
			}
		}	
	}
}

void colorpickerDlg::OnBnClickedHex1()
{
	if (hexmode == 2)
	{
		hexmode = 1;
		hex1->SetCheck(BST_CHECKED);
		hex2->SetCheck(BST_UNCHECKED);
		OnNMCustomdraw(0, 0);
	}
}


void colorpickerDlg::OnBnClickedHex2()
{
	if (hexmode == 1)
	{
		hexmode = 2;
		hex1->SetCheck(BST_UNCHECKED);
		hex2->SetCheck(BST_CHECKED);
		OnNMCustomdraw(0, 0);
	}
}

void colorpickerDlg::OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	if(init==true)
	{ 
	char buffer[30];
	int rmod = red->GetPos();
	int gmod = green->GetPos();
	int bmod = blue->GetPos();
	color->SetFaceColor(RGB(rmod, gmod, bmod));
	if (hexmode == 1)
	{
		char red[7] = {}, green[7] = {}, blue[7] = {};
		//sprintf(red, "%0X", rmod);
		//sprintf(green, "%0X", gmod);
		//sprintf(blue, "%0X", bmod);
		convertohex(rmod, red);
		convertohex(gmod, green);
		convertohex(bmod, blue);
		sprintf_s(buffer, "%s%s%s", red, green, blue);
	}
	else
	{
		char red[7] = {}, green[7] = {}, blue[7] = {};
		//sprintf(red, "%0X", rmod);
		//sprintf(green, "%0X", gmod);
		//sprintf(blue, "%0X", bmod);
		convertohex(rmod, red);
		convertohex(gmod, green);
		convertohex(bmod, blue);
		sprintf_s(buffer, "0x%s%s%s", red, green, blue);
	}
	SetDlgItemTextA(this->m_hWnd, IDC_HEXOUT, buffer);
	}
	//*pResult = 0;
}