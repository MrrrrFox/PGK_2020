#ifndef __ChartClass__
#define __ChartClass__

#include <memory>
#include "ConfigClass.h"
class Matrix;

class ChartClass
{
private:
	std::shared_ptr<ConfigClass> cfg;
	int x_step;         // liczba odcinkow z jakich bedzie sie skladal wykres
	double x_min,x_max; // zakres zmiennej x
	double y_min,y_max; // zakres wartosci przyjmowanych przez funkcje
	double GetFunctionValue(double x); // zwraca wartosci rysowanej funkcji

	// funkcja obliczajaca i zwracajaca macierz transformacji - szerokosci+wysokosc
	Matrix getTransformationMatrix(const int, const int);

	// funkcja odpowiedzialna za rysowanie ukladu wspolrzednych (osie) - szerokosc+wysokosc, macierz przejscia i wxDC
	void drawAxes(const int, const int, const Matrix &, wxDC *);
	// funkcja odpowiedzialna za wypisywanie tekstu (przedzialka osi) - punkt, tekst, wysokosc, macierz przejscia i wxDC
	void drawText(const double, const double, wxString &, const int, const Matrix &, wxDC *);

	// funkcja transformujaca odcinki - odcinek miedzy dwoma punktami, wysokosc, macierz przejscia i wxDC
	void line2d(const double, const double, const double, const double, const int, const Matrix &, wxDC *);
       
public:
	ChartClass(std::shared_ptr<ConfigClass> c);
	void Set_Range();   // ustala wartosci zmiennych x_min,y_min,x_max,y_max
	double Get_Y_min(); // zwraca y_min
	double Get_Y_max(); // zwraca y_max
	void Draw(wxDC *dc, int w, int h);  // rysuje wykres
};

#endif
