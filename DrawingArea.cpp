// DrawingArea.cpp: файл реализации
//

#include "pch.h"
#include "DrawingArea.h"

using namespace std;
// DrawingArea

IMPLEMENT_DYNAMIC(DrawingArea, CStatic)


DrawingArea::DrawingArea()
{
	Gdiplus::GdiplusStartup(&token, &input, NULL);
	U = UMax = UMin = 0;
	lBtnDown = false;
	toDraw = false;
}


DrawingArea::~DrawingArea()
{
	AfxOleTerm(FALSE);
	Gdiplus::GdiplusShutdown(token);
}



BEGIN_MESSAGE_MAP(DrawingArea, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// Обработчики сообщений DrawingArea

void DrawingArea::OnLButtonDown(UINT nFlags, CPoint point)
{
	lBtnDown = true;
	toDraw = true;
	CStatic::OnLButtonDown(nFlags, point);
}


void DrawingArea::OnLButtonUp(UINT nFlags, CPoint point)
{
	lBtnDown = false;
	toDraw = false;
	UMin = UMax = 0;
	FindMaxMin();
	Invalidate(FALSE);
	CStatic::OnLButtonUp(nFlags, point);
}

void DrawingArea::XYScale(int& ind_x, int& ind_y) {
	if (ind_x < 0) 
		ind_x = 0;
	else if (ind_x >= width) 
		ind_x = width - 1;
	
	ind_x = (double)ind_x * potentialStart.size() / width;

	if (ind_y < 0) 
		ind_y = 0;
	else if (ind_y >= height) 
		ind_y = height - 1;

	ind_y = (double)ind_y * potentialStart.size() / height;
}

void DrawingArea::OnMouseMove(UINT nFlags, CPoint point)
{
	if (lBtnDown) {
		
		int center_x = point.x - radius + 1,
			center_y = point.y - radius + 1;
		for (int i = 0; i < 2 * radius; i++) {
			for (int j = 0; j < 2*radius; j++) {
				int ind_x = center_x + i,
					ind_y = center_y + j;
				XYScale(ind_x, ind_y);
				potentialStart[ind_x][ind_y] = U;
				potentialInTime[ind_x][ind_y] = U;

			}
		}
		
	}
	CStatic::OnMouseMove(nFlags, point);
}


void DrawingArea::FindMaxMin()
{
	for (int i = 0; i < potentialStart.size(); i++) {
		double tempMin = *min_element(potentialStart[i].begin(), potentialStart[i].end());
		if (UMin > tempMin) 
			UMin = tempMin;

		double tempMax = *max_element(potentialStart[i].begin(), potentialStart[i].end());
		if (UMax < tempMax) 
			UMax = tempMax;
	}
}

void Sort(double arr[3], Gdiplus::PointF points[3]) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			if (arr[j] > arr[j + 1]) {
				swap(arr[j], arr[j + 1]);
				swap(points[j], points[j + 1]);
			}
		}
	}
}

void DrawingArea::DrawIsolines(Gdiplus::Graphics* grr, vector<vector<double>> tempPotential)
{
	for (int i = 1; i < numIsolines; i++) {
		double isoline = UMin + i * (UMax - UMin) / numIsolines;
		double nodes[3];
		Gdiplus::PointF points[3];
		for (int i = 0; i < tempPotential.size() - 1; i++) {
			for (int j = 0; j < tempPotential.size() - 1; j++) {
				for (int k = 0; k < 2; k++) {
					double values[2], keys[2];
					if (k == 0) {
						nodes[0] = tempPotential[i][j];
						nodes[1] = tempPotential[i + 1][j];
						nodes[2] = tempPotential[i][j + 1];

						points[0] = Gdiplus::PointF(i, j);
						points[1] = Gdiplus::PointF(i + 1, j);
						points[2] = Gdiplus::PointF(i, j + 1);
					}
					else {
						nodes[0] = tempPotential[i + 1][j];
						nodes[1] = tempPotential[i + 1][j + 1];
						nodes[2] = tempPotential[i][j + 1];

						points[0] = Gdiplus::PointF(i + 1, j);
						points[1] = Gdiplus::PointF(i + 1, j + 1);
						points[2] = Gdiplus::PointF(i, j + 1);
					}
					Sort(nodes, points);
				
					if (isoline < nodes[0] || isoline > nodes[2])
						continue;

					Gdiplus::REAL x1, y1, x2, y2;

					if (isoline >= nodes[0] && isoline < nodes[1]) {
						x1 = points[1].X * 1. - (nodes[1] - isoline) *
							(points[1].X * 1. - points[0].X * 1.) / (nodes[1] - nodes[0]);
						y1 = points[1].Y * 1. - (nodes[1] - isoline) *
							(points[1].Y * 1. - points[0].Y * 1.) / (nodes[1] - nodes[0]);
					}
					else{
						x1 = points[2].X * 1. - (nodes[2] - isoline) *
							(points[2].X * 1. - points[1].X * 1.) / (nodes[2] - nodes[1]);
						y1 = points[2].Y * 1. - (nodes[2] - isoline) *
							(points[2].Y * 1. - points[1].Y * 1.) / (nodes[2] - nodes[1]);
					}

					x2 = points[2].X * 1. - (nodes[2] - isoline) * (points[2].X * 1. - points[0].X * 1.) / (nodes[2] - nodes[0]);
					y2 = points[2].Y * 1. - (nodes[2] - isoline) * (points[2].Y * 1. - points[0].Y * 1.) / (nodes[2] - nodes[0]);

					Gdiplus::Pen p(Gdiplus::Color(200, 200, 200), 0.5);
					grr->DrawLine(&p, x1, y1, x2, y2);
				}
			}
		}
	}
}


void DrawingArea::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
	//запуск в фоне нахождения максимума и минимума потенциала
	vector<vector<double>> tempPotential = potentialInTime;
	CRect rect;
	GetClientRect(&rect);
	Gdiplus::Bitmap myBitmap(rect.Width(), rect.Height()); //создаем битовый образ
	Gdiplus::Graphics gr(lpDrawItemStruct->hDC); //создаем объект класса для рисования объектов, связанный с областью рисования
	Gdiplus::Graphics* grr = Gdiplus::Graphics::FromImage(&myBitmap); //создаем дополнительный объект класса для рисования объектов

	int numIsolines = 11;

	grr->SetSmoothingMode(Gdiplus::SmoothingModeHighSpeed); //устанавливаем сглаживание в режиме быстродействия

	//рисуем попиксельно, поэтому индексы - это пиксели
	Gdiplus::Matrix mtx(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);//создаем матрицу
	mtx.Scale(1.*rect.Width() / tempPotential.size(), 1.*rect.Height() / tempPotential.size()); //изменяем масштаб
	grr->SetTransform(&mtx);

	

	//Распараллелить рисование: создать функцию, которая рисует часть картинки

	double stepx = (double)rect.Width() / tempPotential.size();
	double stepy = (double)rect.Height() / tempPotential.size();
	

	//рисуем по элементно
	for (int i = 0; i < tempPotential.size() - 1; i++) {
		for (int j = 0; j < tempPotential.size() - 1; j++) {
			double module = tempPotential[i][j];
			//красным рисуем плюс, а синим минус
			Gdiplus::SolidBrush br(Gdiplus::Color(module > 0 ? module * 255. / UMax : 0, 0, module < 0 ? module * 255. / UMin : 0));
			grr->FillRectangle(&br, Gdiplus::REAL(i), Gdiplus::REAL(j), Gdiplus::REAL(stepx), Gdiplus::REAL(stepy));
		}
	}

	DrawIsolines(grr, tempPotential);
	

	//рисовка из буфера
	gr.DrawImage(&myBitmap, 0, 0, rect.Width(), rect.Height());
	delete grr;//очистка памяти
	
}