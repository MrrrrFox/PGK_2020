#include <wx/wx.h>
#include "GUIMyFrame1.h"
// UWAGA: TO JEST JEDYNY PLIK, KTORY NALEZY EDYTOWAC **************************

#include <vector>

// pomocnicze zdefiniowanie zmiennych podanych w treœci zadania
#define P 2					// potega w mianowniku wagi Sheparda
#define MIN_VALUE -2.5		// minimalny x i y
#define MAX_VALUE 2.5		// maksymalny x i y
#define MAP_SIZE 500		// rozmiar bitmapy (500x500)

// funkcja obliczajaca wage interpolacji Sheparda (wzor z wykladu)
float Shepard_Waga(float x, float x_k, float y, float y_k)
{
	return 1 / pow( sqrt(pow(x-x_k,2) + pow(y-y_k, 2)), P );
}

// funckja obliczajaca wartosci funkcji (wzor z wykladu)
float Shepard(float x, float y, int n, float punkty[100][3])
{
	float numerator = 0.0, denominator = 0.0, weight;

	// wyznaczenie wartosci z(x,y)
	for (int i = 0; i < n; ++i)
	{
		// sprawdzenie pierwszego przypadku
		if (punkty[i][0] == x && punkty[i][1] == y) return punkty[i][2];
		// drugi przypadek (sumy)
		weight = Shepard_Waga(x, punkty[i][0], y, punkty[i][1]);
		numerator += weight * punkty[i][2];
		denominator += weight;
	}

	return numerator / denominator;
}

// wyznaczenie kanalu czerownego (wzor z wykladu)
int MapColorR(int mappingType, float value)
{
	if (mappingType == 3)	// mapa szara
	{
		return value;
	}
	else
	{
		if (mappingType == 1)	// mapa niebiesko-czerwona
		{
			return 255 - value;
		}
		else	// mapa niebiesko-zielono -czerwona
		{
			return value < 128 ? (128 - value) * 2 : 0;
		}
	}
}

// wyznaczenie kanalu zielonego (wzor z wykladu)
int MapColorG(int mappingType, float value)
{
	if (mappingType == 3)	// mapa szara
	{
		return value;
	}
	else
	{
		if (mappingType == 1)	// mapa niebiesko-czerwona
		{
			return 0;
		}
		else	// mapa niebiesko-zielono -czerwona
		{
			return value < 128 ? value * 2 : (255 - value) * 2;
		}
	}
}

// wyznaczenie kanalu niebieskiego (wzor z wykladu)
int MapColorB(int mappingType, float value)
{
	if (mappingType == 3)	// mapa szara
	{
		return value;
	}
	else
	{
		if (mappingType == 1)	// mapa niebiesko-czerwona
		{
			return value;
		}
		else	// mapa niebiesko-zielono -czerwona
		{
			return value < 128 ? 0 : (value - 128) * 2;
		}
	}
}

// wyznaczanie jaki kontur ma byc narysowany (wzory z wykladu)
// zastanawialem sie nad switchem, ale na oop ciagle dostaje za niego po glowie :( - dlatego tutaj ciag warunkow zoptymalizowany elsami
std::vector<std::pair<wxPoint, wxPoint>> getContourLines(int x, int y, int code)
{
	// kontur ktory bedziemy wstawiac do listy konturow
	std::vector<std::pair<wxPoint, wxPoint>> kontury;
	// ponizej odpowiednie warunki na podstawie wykladu
	// nie ma zera, bo wtedy nie rysujemy - puszczamy pusty kontur
	if (code == 1 || code == 14)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x - 1, y), wxPoint(x, y - 1)));
	}
	if (code == 2 || code == 13)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x, y-1), wxPoint(x+1, y)));
	}
	if (code == 3 || code == 12)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x - 1, y), wxPoint(x+1, y)));
	}
	if (code == 4 || code == 11)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x, y+1), wxPoint(x+1, y)));
	}
	if (code == 5)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x - 1, y), wxPoint(x, y + 1)));
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x, y-1), wxPoint(x+1, y)));
	}
	if (code == 6 || code == 9)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x, y-1), wxPoint(x, y + 1)));
	}
	if (code == 7 || code == 8)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x - 1, y), wxPoint(x, y + 1)));
	}
	if (code == 10)
	{
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x - 1, y), wxPoint(x, y - 1)));
		kontury.push_back(std::pair<wxPoint, wxPoint>(wxPoint(x, y+1), wxPoint(x+1, y)));
	}
	return kontury;
}

void GUIMyFrame1::DrawMap(int N, float d[100][3], bool Contour, int MappingType, int NoLevels, bool ShowPoints)
{
	wxMemoryDC memDC;
	memDC.SelectObject(MemoryBitmap);
	memDC.SetBackground(*wxWHITE_BRUSH);
	memDC.Clear();

	// demo.....
	memDC.SetPen(*wxBLACK_PEN);
	memDC.SetBrush(*wxTRANSPARENT_BRUSH);

	float punkty[MAP_SIZE][MAP_SIZE];
	float f_min = static_cast<float>(10e6), f_max = static_cast<float>(10e-6);
	float step_value = static_cast<float>((MAX_VALUE - MIN_VALUE) / MAP_SIZE);
	// wyznaczenie wartosci punktow na bitmapie
	for (int i = 0; i < MAP_SIZE; ++i)
	{
		for (int j = 0; j < MAP_SIZE; ++j)
		{
			punkty[i][j] = Shepard(MIN_VALUE + i * step_value,MAX_VALUE - j * step_value, N,d);

			// szukanie wartosci minimalnej i maksymanej (pod pozniejsze w'_xy)
			if (punkty[i][j] < f_min) f_min = punkty[i][j];
			if (punkty[i][j] > f_max) f_max = punkty[i][j];
		}
	}

	// rysowanie bitmapy
	if (MappingType != 0)
	{
		int size = MAP_SIZE * MAP_SIZE * 3;
		auto data_to_draw = image.GetData();
		float w_p_xy;
		// obliczenie wartosci
		for (int i = 0; i < MAP_SIZE; ++i)
		{
			for (int j = 0; j < MAP_SIZE; ++j)
			{
				// wyliczenie w'_xy (wzor z wykladu)
				w_p_xy = (punkty[i][j] - f_min) * 255 / (f_max - f_min);
				// skladowe RGB pikseli
				data_to_draw[j * MAP_SIZE * 3 + i * 3 + 0] = MapColorR(MappingType, w_p_xy);
				data_to_draw[j * MAP_SIZE * 3 + i * 3 + 1] = MapColorG(MappingType, w_p_xy);
				data_to_draw[j * MAP_SIZE * 3 + i * 3 + 2] = MapColorB(MappingType, w_p_xy);
			}
		}
		// rysowanie
		wxBitmap bitmap(image);
		memDC.DrawBitmap(bitmap, 0, 0, true);
	}

	// rysowaniu konturow
	if (Contour)
	{
		// wektor par punktow tworzacych linie
		std::vector<std::pair<wxPoint, wxPoint>> listaKonturow, linia;

		float step = (f_max - f_min) / (NoLevels + 1), poziomica;
		int index;
		// dla kazdej poziomicy...
		for (int i = 1; i <= NoLevels; ++i)
		{
			// wyliczenie poziomicy
			poziomica = f_min + i * step;
			for (int j = 1; j < MAP_SIZE - 1; j += 2)
			{
				for (int k = 1; k < MAP_SIZE - 1; k += 2)
				{
					// obliczenie indeksu - mamy 4-bitowy kod, stad mno¿enia *1 (nie zaznaczone z braku potrzeby),*2,*4,*8
					index = 0;
					index += punkty[j - 1][k - 1] < poziomica ? 0 : 1;
					index += (punkty[j + 1][k - 1] < poziomica ? 0 : 1) * 2;
					index += (punkty[j + 1][k + 1] < poziomica ? 0 : 1) * 4;
					index += (punkty[j - 1][k + 1] < poziomica ? 0 : 1) * 8;
					// obsluga siodla
					if (index == 5 && punkty[i][j] < poziomica)
					{
						index = 10;
					}
					else
					{
						if (index == 10 && punkty[i][j] < poziomica)
						{
							index = 5;
						}
					}
					// wyliczenie linii na podstawie indeksu
					linia = getContourLines(j, k, index);
					listaKonturow.insert(listaKonturow.end(), linia.begin(), linia.end());
				}
			}
		}
		// rysowanie
		for (std::pair<wxPoint, wxPoint> line : listaKonturow)
		{
			memDC.DrawLine(line.first, line.second);
		}
	}

	// pokazywanie punktow
	if (ShowPoints)
	{
		int point_x, point_y, point_size = 3;
		// dla kazdego punktu...
		for (int i = 0; i < N; ++i)
		{
			// ...obliczanie x i y
			point_x = 100 * (d[i][0] - MIN_VALUE);
			point_y = 100 * (d[i][1] - MIN_VALUE);
			// ...rysowanie punktu (krzyzyk) - standard :P
			//memDC.DrawLine(point_x - point_size, point_y + point_size, point_x + point_size+1, point_y - point_size-1);
			//memDC.DrawLine(point_x - point_size, point_y - point_size, point_x + point_size+1, point_y + point_size+1);
		
			// ...rysowanie punktu (minka :) ) - to ja rozumiem :)
			// g³owa

			memDC.SetBrush(wxBrush(wxColour(255, 255, 0), wxBRUSHSTYLE_SOLID));
			memDC.DrawCircle(point_x, 500-point_y, 9);
			memDC.SetBrush(wxBrush(wxColour(0, 0, 0), wxBRUSHSTYLE_SOLID));
			// oczy
			memDC.DrawCircle(point_x-2, 500-point_y-2, 2);
			memDC.DrawCircle(point_x+2, 500-point_y-2, 2);
			// usta
			memDC.DrawEllipticArc(point_x - 4, 500-point_y+1, 8, 5, 180, 360);
			memDC.SetBrush(*wxTRANSPARENT_BRUSH);
		}
	}
}
