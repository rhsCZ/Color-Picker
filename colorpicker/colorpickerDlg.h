
// hasher2Dlg.h : header file
//
#pragma once
#include "afxdialogex.h"
#include "afxwin.h"
#include <thread>
//#include <fstream>
using namespace std;

#define bufferSize 10
#define WM_SHOWPAGE WM_APP+2
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)
// Chasher2Dlg dialog
class colorpickerDlg : public CDialog
{
private:
	BOOL m_bMinimizeToTray;
	BOOL			m_bTrayIconVisible;
	CMenu			m_mnuTrayMenu;
	UINT			m_nDefaultMenuItem;
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
// Construction
public:
	NOTIFYICONDATA	m_nidIconData;
	colorpickerDlg(CWnd* pParent = nullptr);
	void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
	BOOL TraySetMenu(UINT nResourceID, UINT nDefaultPos = 0);
	BOOL TraySetMenu(HMENU hMenu, UINT nDefaultPos = 0);
	BOOL TraySetMenu(LPCTSTR lpszMenuName, UINT nDefaultPos = 0);
	BOOL TrayUpdate();
	BOOL TrayShow();
	BOOL TrayHide();
	void TraySetToolTip(LPCTSTR lpszToolTip);
	void TraySetIcon(HICON hIcon);
	void TraySetIcon(UINT nResourceID);
	void TraySetIcon(LPCTSTR lpszResourceName);
	BOOL TrayIsVisible();
	virtual void OnTrayLButtonDown(CPoint pt);
	virtual void OnTrayLButtonDblClk(CPoint pt);
	virtual void OnTrayRButtonDown(CPoint pt);
	virtual void OnTrayRButtonDblClk(CPoint pt);
	virtual void OnTrayMouseMove(CPoint pt);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLORPICKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	
public:
	static inline wchar_t *returned;
	afx_msg void OnBnClickedExit();
	int hexmode = 1;
	bool trayenable = false;;
	bool minimizeen = false;;
	bool init=false;
	bool up = false;
	bool updated = false;
	bool lowercaseen = false;
	bool treden = false;
	bool tgreenen = false;
	bool tblueen = false;
	bool toutputen = false;
	thread tred = {};
	thread tgreen = {};
	thread tblue = {};
	thread toutput = {};
	CSliderCtrl* red = {};
	CSliderCtrl* green = {};
	CSliderCtrl* blue = {};
	CRichEditCtrl* rval = {};
	CRichEditCtrl* gval = {};
	CRichEditCtrl* bval = {};
	CButton* hex1 = {};
	CButton* hex2 = {};
	CMFCButton* color = {};
	CButton* trayen = {};
	CButton* lowercase = {};
	CButton* checkbox = {};
	HKEY traykey = {};
	DWORD traykeyvalue = 0;
	unsigned long type = 0;
	DWORD keycreate = 0;
	afx_msg void OnOpen();
	afx_msg void OnMinimize();
	afx_msg void OnBnClickedMinEn();
	afx_msg void OnBnClickedTrayEn();
	void dec2hex(int value, char* buffer);
	void hex2dec(char* buffer, int *value);
	afx_msg void OnBnClickedHex1();
	afx_msg void OnBnClickedHex2();
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	void OnEnChangeHexout();
	void OnEnChangeRval();
	void OnEnChangeGval();
	void OnEnChangeBval();
	afx_msg void OnBnClickedLowercase();
};
