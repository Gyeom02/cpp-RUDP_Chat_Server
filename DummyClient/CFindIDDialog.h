#pragma once
#include "resource.h"

class CFindIdDialog : public CDialog
{
	

public:
	CFindIdDialog(CWnd* pParent = nullptr);
	virtual ~CFindIdDialog();

	enum { IDD = IDD_FINDID_DIALOG };

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//
	DECLARE_MESSAGE_MAP()

	CBrush m_bgBrush;
	COLORREF m_resultColor;
public:
	CString m_strEmail;
	CStatic m_staticResult;
	CMFCButton m_findBtn;
	afx_msg void OnBnClickedFind();
	afx_msg void OnBnClickedExit();

	
public:
	int32 GetFindCode() { return findcode; }
	void SetFindCode(int32 code) { findcode = code; }
	void SetResultColor(int32 code);
	void SetFindID(string id) { findId = id; }
	string GetFindID() { return findId; }
private:
	int32 findcode = -1;
	string findId = "";
};