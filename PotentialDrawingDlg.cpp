
// PashaPotentialDrawingDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "PotentialDrawing.h"
#include "PotentialDrawingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// Диалоговое окно CPashaPotentialDrawingDlg



CPotentialDrawingDlg::CPotentialDrawingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PASHAPOTENTIALDRAWING_DIALOG, pParent)
	, U(10)
	, sizeOfDrawingArea(100)
	, radius(5)
	, timer(false)
	, numIsolines(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPotentialDrawingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_U, U);
	DDX_Control(pDX, IDC_drawArea, myDrawingArea); //связываем область рисования

	DDX_Text(pDX, IDC_radius, radius);
	DDX_Text(pDX, IDC_numIsolines, numIsolines);
}

BEGIN_MESSAGE_MAP(CPotentialDrawingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_apply, &CPotentialDrawingDlg::OnBnClickedapply)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDDraw, &CPotentialDrawingDlg::OnBnClickedDraw)
	ON_BN_CLICKED(IDCReset, &CPotentialDrawingDlg::OnBnClickedCreset)
	ON_EN_CHANGE(IDC_U, &CPotentialDrawingDlg::OnEnChangeU)
	ON_EN_CHANGE(IDC_radius, &CPotentialDrawingDlg::OnEnChangeradius)
	ON_EN_CHANGE(IDC_numIsolines, &CPotentialDrawingDlg::OnEnChangenumisolines)
END_MESSAGE_MAP()


// Обработчики сообщений CPashaPotentialDrawingDlg

BOOL CPotentialDrawingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	myDrawingArea.potentialStart = myDrawingArea.potentialInTime = vector<vector<double>>(sizeOfDrawingArea, vector<double>(sizeOfDrawingArea, 0));
	myDrawingArea.Invalidate(TRUE);

	CRect rect;
	GetDlgItem(IDC_drawArea)->GetClientRect(&rect);
	myDrawingArea.width = rect.Width();
	myDrawingArea.height = rect.Height();
	OnEnChangeU();
	myDrawingArea.numIsolines = numIsolines;
	myDrawingArea.radius = radius;
	myDrawingArea.Invalidate(TRUE);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CPotentialDrawingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CPotentialDrawingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPotentialDrawingDlg::OnBnClickedapply()
{
	
}

void CPotentialDrawingDlg::EvolvePotential() {
	lock = true;
	myDrawingArea.potentialInTime = PotentialInNextTime(myDrawingArea.potentialInTime, myDrawingArea.potentialStart);
	lock = false;

}

void CPotentialDrawingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == Timers::invalidate) {
		while (lock) Sleep(1);
		thread myth(&DrawingArea::Invalidate, this, FALSE);
		myth.detach();
		
	}
	if (nIDEvent == Timers::potential) {
		thread myth(&CPotentialDrawingDlg::EvolvePotential, this);
		myth.join();
	}
	
	CDialogEx::OnTimer(nIDEvent);
}


void CPotentialDrawingDlg::OnBnClickedDraw()
{
	if (!timer) {
		SetTimer(Timers::invalidate, 200, NULL);
		SetTimer(Timers::potential, 1, NULL);
		timer = true;
		GetDlgItem(IDDraw)->SetWindowText(L"Остановить");
	}
	else {
		timer = false;
		KillTimer(Timers::invalidate);
		KillTimer(Timers::potential);
		GetDlgItem(IDDraw)->SetWindowText(L"Запустить");
	}
}


vector<vector<double>> PotentialInNextTime(vector<vector<double>> potentialInTime, vector<vector<double>> potentialStart) {
	vector<vector<double>> out(potentialInTime.size(), vector<double>(potentialInTime.size()));
	//усредняем значения в узлах соседних
	//шаг = 1
	for (int i = 0; i < potentialInTime.size(); i++) {
		for (int j = 0; j < potentialInTime.size(); j++) {
			if (potentialStart[i][j] != 0) {
				out[i][j] = potentialStart[i][j];
			}
			else {
				double summa = 0;
				int count = 0;
				if (i > 0) {
					summa += potentialInTime[i-1][j];
					count++;
				}
				if (j > 0) {
					summa += potentialInTime[i][j - 1];
					count++;
				}
				if (i < potentialInTime.size() - 1) {
					summa += potentialInTime[i + 1][j];
					count++;
				}
				if (j < potentialInTime.size() - 1) {
					summa += potentialInTime[i][j + 1];
					count++;
				}
				out[i][j] = summa / count;
			}
		}
	}
	return out;
}

void CPotentialDrawingDlg::OnBnClickedCreset()
{
	if(timer) OnBnClickedDraw();
	myDrawingArea.UMax = 0;
	myDrawingArea.UMin = 0;
	myDrawingArea.potentialStart = myDrawingArea.potentialInTime = vector<vector<double>>(sizeOfDrawingArea, vector<double>(sizeOfDrawingArea, 0));
	myDrawingArea.Invalidate(FALSE);
	OnEnChangeU();
}



void CPotentialDrawingDlg::OnEnChangeU()
{
	UpdateData();
	myDrawingArea.U = this->U;
	if (U > myDrawingArea.UMax) myDrawingArea.UMax = U;
	if (U < myDrawingArea.UMin) myDrawingArea.UMin = U;
}


void CPotentialDrawingDlg::OnEnChangeradius()
{
	UpdateData();
	myDrawingArea.radius = this->radius;
}


void CPotentialDrawingDlg::OnEnChangenumisolines()
{
	UpdateData();
	myDrawingArea.numIsolines = numIsolines;
}
