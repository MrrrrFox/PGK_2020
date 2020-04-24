#define _USE_MATH_DEFINES
#include <wx/dc.h>
#include <memory>

#include "ChartClass.h"
#include "vecmat.h"

#include <cmath>

ChartClass::ChartClass(std::shared_ptr<ConfigClass> c) 
{
	cfg = std::move(c);
	x_step = 200;
}

void ChartClass::Set_Range() 
{
	double xmin = 9999.9, xmax = -9999.9, ymin = 9999.9, ymax = -9999.9;
	double x, y, step;
	int i;

	xmin = cfg->Get_x_start();
	xmax = cfg->Get_x_stop();

	step = (cfg->Get_x_stop() - cfg->Get_x_start()) / (double)x_step;
	x = cfg->Get_x_start();

	for (i = 0; i <= x_step; i++) 
	{
		y = GetFunctionValue(x);
		if (y > ymax) ymax = y;
		if (y < ymin) ymin = y;
		x = x + step;
	}

	y_min = ymin;
	y_max = ymax;
	x_min = xmin;
	x_max = xmax;
}

double ChartClass::GetFunctionValue(const double x) 
{
	if (cfg->Get_F_type() == 1) return x * x;
	if (cfg->Get_F_type() == 2) return 0.5*exp(4 * x - 3 * x*x);
	return x + sin(x*4.0);
}

double ChartClass::Get_Y_min() 
{
	Set_Range();
	return y_min;
}

double ChartClass::Get_Y_max() 
{
	Set_Range();
	return y_max;
}

// funkcja rysujaca wykres
void ChartClass::Draw(wxDC *dc, int width, int height)
{
	// tlo + prostokat
	dc->SetBackground(wxBrush(RGB(255, 255, 255)));
	dc->Clear();
	dc->SetPen(wxPen(RGB(255, 0, 0)));
	dc->DrawRectangle(10, 10, width - 20, height - 20);
	// ClippingRegion - by nie rysowac wiecej niz trzeba = przyspieszenie pracy programy :D
	dc->SetClippingRegion(wxRect(11, 11, width - 21, height - 21));

	// pobranie macierzy przejscia
	Matrix finalMatrix = getTransformationMatrix(width, height);

	// uklad wspolrzednych - na niebiesko
	dc->SetPen(wxPen(RGB(0, 0, 255)));
	drawAxes(width, height, finalMatrix, dc);

	// wykres funkcji - na zielono
	dc->SetPen(wxPen(RGB(0, 255, 0)));
	// ustalenie punktow, czyli skoku po OX + obsluga gdy x1 jest prawie rowne x0
	double delta_x = (cfg->Get_x1() - cfg->Get_x0()) / x_step > 10e-4 ? (cfg->Get_x1() - cfg->Get_x0()) / x_step : 10e-4;
	// rysowanie wykresu odcinkami
	for (double x = cfg->Get_x_start(); x < cfg->Get_x_stop(); x += delta_x)
	{
		line2d(x, GetFunctionValue(x), x + delta_x, GetFunctionValue(x + delta_x), height, finalMatrix, dc);
	}
}

// funkcja obliczajaca i zwracajaca macierz transformacji - szerokosci+wysokosc
Matrix ChartClass::getTransformationMatrix(const int width, const int height)
{
	// skalowanie obrazu
	Matrix scale_Matrix;
	double value_shift = cfg->Get_x1() - cfg->Get_x0();
	if (abs(value_shift) > 10e-3)
	{
		scale_Matrix.data[0][0] = (width - 20.) / value_shift;
	}
	else // dzielenie przez 0 => INF, czyli chcemy duza liczbe, wiec dzielimy przez mala
	{
		scale_Matrix.data[0][0] = (width - 20.) / 10e-4;
	}

	value_shift = cfg->Get_y1() - cfg->Get_y0();
	if (abs(value_shift) > 10e-3)
	{
		scale_Matrix.data[1][1] = (height - 20.) / value_shift;
	}
	else // dzielenie przez 0 => INF, czyli chcemy duza liczbe, wiec dzielimy przez mala
	{
		scale_Matrix.data[1][1] = (height - 20.) / 10e-4;
	}

	scale_Matrix.data[0][2] = 10. - scale_Matrix.data[0][0] * cfg->Get_x0();
	scale_Matrix.data[1][2] = 10. - scale_Matrix.data[1][1] * cfg->Get_y0();

	// przesuniecie o wektor
	Matrix shift_Matrix;
	shift_Matrix.data[0][0] = shift_Matrix.data[1][1] = 1;
	shift_Matrix.data[0][2] = cfg->Get_dX();
	shift_Matrix.data[1][2] = cfg->Get_dY();

	// obrot o kat
	Matrix rotation_Matrix;
	const double sin_value = sin(cfg->Get_Alpha() * M_PI / 180.);
	const double cos_value = cos(cfg->Get_Alpha() * M_PI / 180.);
	rotation_Matrix.data[0][0] = rotation_Matrix.data[1][1] = cos_value;
	rotation_Matrix.data[0][1] = -sin_value;
	rotation_Matrix.data[1][0] = sin_value;

	// przesuniecie o wektor - rotacja wokol innego punktu
	Matrix move_Matrix, move_back_Matrix;
	move_Matrix.data[0][0] = move_Matrix.data[1][1] = move_back_Matrix.data[0][0] = move_back_Matrix.data[1][1] = 1.;
	if (cfg->RotateScreenCenter())	// rotacja wokol srodka ekranu
	{
		move_Matrix.data[0][2] = width / 2.;
		move_Matrix.data[1][2] = height / 2.;
	}
	else							// rotacja wokol punktu przeciecia osi
	{
		move_Matrix.data[0][2] = scale_Matrix.data[0][2];
		move_Matrix.data[1][2] = scale_Matrix.data[1][2];
	}
	move_back_Matrix.data[0][2] = -move_Matrix.data[0][2];
	move_back_Matrix.data[1][2] = -move_Matrix.data[1][2];
	// macierz przejscia dla rotacji 
	rotation_Matrix = move_Matrix * rotation_Matrix * move_back_Matrix;
	
	// jezeli wokol srodka ekranu to: skalowanie -> przesuniecie -> rotacja 
	if (cfg->RotateScreenCenter())
	{
		return rotation_Matrix * shift_Matrix * scale_Matrix;
	}
	// jezeli wokol przeciecia osi: skalowanie -> rotacja -> przesuniecie
	return shift_Matrix * rotation_Matrix * scale_Matrix;
}

// funkcja odpowiedzialna za rysowanie ukladu wspolrzednych (osie) - szerokosc+wysokosc, macierz przejscia i wxDC
void ChartClass::drawAxes(const int width, const int height, const Matrix & final_Matrix, wxDC *dc)
{
	// rysowanie osi
	line2d(0, Get_Y_min(), 0, Get_Y_max(), height, final_Matrix, dc);
	line2d(cfg->Get_x_start(), 0, cfg->Get_x_stop(), 0, height, final_Matrix, dc);

	// rysowanie przedzialek
	int division_count = 5;
	// krok na OX + obsluga gdy x1 jest prawie rowne x0
	double OX_step = (cfg->Get_x1() - cfg->Get_x0()) / division_count > 10e-4 ? (cfg->Get_x1() - cfg->Get_x0()) / division_count : 10e-4;
	// przedzalki na OX
	for (double i = x_min; i < x_max; i += OX_step)
	{
		if (abs(i) > 10e-3)	// nie chce wypisywac 0 - zrobie to potem, dokladnosc na podstawie formatu wypisywania
		{
			line2d(i, 0.03, i, -0.03, height, final_Matrix, dc);
			drawText(i - (i < 0 ? 0.06 : 0.05), -0.05, wxString::Format("%.2lf", i), height, final_Matrix, dc);		// przy wspolrzednej x obsluga minusa, bo tekst nie byl rowno z podzialka 
		}
	}
	// strzalka na OX
	line2d(x_max, 0, x_max - 0.05, 0.03, height, final_Matrix, dc);
	line2d(x_max, 0, x_max - 0.05, -0.03, height, final_Matrix, dc);

	// krok na OY + obsluga gdy y1 jest prawie rowne y0
	double OY_step = (cfg->Get_y1() - cfg->Get_y0()) / division_count > 10e-4 ? (cfg->Get_y1() - cfg->Get_y0()) / division_count : 10e-4;
	// przedzialki na OY
	for (double i = y_min; i < y_max; i += OY_step)
	{
		if (abs(i) > 10e-3)	// nie chce wypisywac 0 - zrobie to potem
		{
			line2d(0.03, i, -0.03, i, height, final_Matrix, dc);
			drawText(0.05, i + 0.1, wxString::Format("%.2lf", i), height, final_Matrix, dc);
		}
	}
	// strzalka na OY
	line2d(0, y_max, -0.03, y_max - 0.05, height, final_Matrix, dc);
	line2d(0, y_max, 0.03, y_max - 0.05, height, final_Matrix, dc);
	// dopisanie 0 na przedzialce - mozna bylo wczesniej, ale wg mojego wzoru i nie zawsze mialo wartosc 0, 
	// a mnie juz w podstawowce uczyli ze kazdy uklad powinien miec opisany poczatek :)
	drawText(-0.05, 0.1, wxString::Format("0"), height, final_Matrix, dc);

}

// funkcja odpowiedzialna za wypisywanie tekstu (przedzialka osi) - punkt, tekst, wysokosc, macierz przejscia i wxDC
void ChartClass::drawText(const double x, const double y, wxString &txt, const int height, const Matrix & final_Matrix, wxDC *dc)
{
	// obliczenie gdzie rysowac tekst
	Vector text_pos;
	text_pos.Set(x, y);
	text_pos = final_Matrix * text_pos;
	// rysowanie tekstu
	dc->DrawRotatedText(txt, text_pos.GetX(), height - text_pos.GetY(), cfg->Get_Alpha());
}

// funkcja transformujaca odcinki - odcinek miedzy dwoma punktami, wysokosc, macierz przejscia i wxDC
void ChartClass::line2d(const double x1, const double y1, const double x2, const double y2, const int height, const Matrix & final_Matrix, wxDC *dc)
{
	// ustawianie punktow
	Vector first_point, second_point;
	first_point.Set(x1, y1);
	second_point.Set(x2, y2);

	// tranformacja macierza przejscia
	first_point = final_Matrix * first_point;
	second_point = final_Matrix * second_point;

	// rysowanie
	dc->DrawLine(first_point.GetX(), height - first_point.GetY(), second_point.GetX(), height - second_point.GetY());
}

