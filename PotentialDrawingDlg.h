
// PashaPotentialDrawingDlg.h: файл заголовка
//

#pragma once
#include "DrawingArea.h"
#include "afxcmn.h"

// Диалоговое окно CPashaPotentialDrawingDlg
class CPotentialDrawingDlg : public CDialogEx
{
// Создание
public:
	CPotentialDrawingDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PASHAPOTENTIALDRAWING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;
	int sizeOfDrawingArea;
	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	DrawingArea myDrawingArea;
	double U;
	double radius;
	int numIsolines;
	bool timer;
	bool lock;
public:
	void EvolvePotential();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedDraw();
	afx_msg void OnBnClickedCreset();
	afx_msg void OnBnClickedapply();
	afx_msg void OnEnChangeU();
	afx_msg void OnEnChangeradius();
	afx_msg void OnEnChangenumisolines();
};

std::vector<std::vector<double>> PotentialInNextTime(std::vector<std::vector<double>> potentialInTime, std::vector<std::vector<double>> potentialStart);

enum Timers {
	invalidate,
	potential
};