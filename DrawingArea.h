#pragma once
#include <vector>
#include <thread>
#include <algorithm>
// DrawingArea

class DrawingArea : public CStatic
{
	DECLARE_DYNAMIC(DrawingArea)

public:
	Gdiplus::GdiplusStartupInput input;
	ULONG_PTR token;
	bool lBtnDown; //зажата ли левая кнопка мыши
	//размеры поля
	int width;
	int height;
	//мин и макс потенциала (для градуировки рисования)
	double UMax;
	double UMin;
	bool toDraw;
	double radius;
	int numIsolines;
protected:
	DECLARE_MESSAGE_MAP()

public:
	std::vector<std::vector<double>> potentialStart; //значения потенциала стартовые
	std::vector<std::vector<double>> potentialInTime; //значения потенциала меняющиеся во времени
	double U; //каким значением потенциала рисовать

	DrawingArea();
	virtual ~DrawingArea();
	void XYScale(int& ind_x, int& ind_y);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void FindMaxMin();
	void DrawIsolines(Gdiplus::Graphics* grr, std::vector<std::vector<double>> tempPotential);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


