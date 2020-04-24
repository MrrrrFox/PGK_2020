#include "GUIMyFrame1.h"

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
	m_staticText1->SetLabel(_(L"Jasno\u015B\u0107"));
	m_b_threshold->SetLabel(_(L"Pr\u00F3g 128"));
	this->SetBackgroundColour(wxColor(192, 192, 192));
	m_scrolledWindow->SetScrollbars(25, 25, 52, 40);
	m_scrolledWindow->SetBackgroundColour(wxColor(192, 192, 192));
}

void GUIMyFrame1::m_scrolledWindow_update( wxUpdateUIEvent& event )
{
	Repaint();
}

void GUIMyFrame1::m_b_grayscale_click( wxCommandEvent& event )
{
	// TO DO: Konwersja do skali szarosci    
	Img_Cpy = Img_Org.ConvertToGreyscale();	// niepotrzebna kopia
}

void GUIMyFrame1::m_b_blur_click( wxCommandEvent& event )
{
	// TO DO: Rozmywanie obrazu (blur)
	Img_Cpy = Img_Org.Blur(5);	// niepotrzebna kopia
}

void GUIMyFrame1::m_b_mirror_click( wxCommandEvent& event )
{
	// TO DO: Odbicie lustrzane
	Img_Cpy = Img_Org.Mirror();	// niepotrzebna kopia

}

void GUIMyFrame1::m_b_replace_click( wxCommandEvent& event )
{
	// TO DO: Zamiana kolorow
	Img_Cpy = Img_Org.Copy();	// potrzebna kopia (rzeczywiœcie zmieniamy kolory)
	Img_Cpy.Replace(254, 0, 0, 0, 0, 255);
}

void GUIMyFrame1::m_b_rescale_click( wxCommandEvent& event )
{
	// TO DO: Zmiana rozmiarow do 320x240
	Img_Cpy = Img_Org.Copy();	// potrzebna kopia (rzeczywiœcie zmieniamy rozmiar)
	Img_Cpy.Rescale(320, 240);
	// oczyszczenie tla i ustawienie pozycji
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
}

void GUIMyFrame1::m_b_rotate_click( wxCommandEvent& event )
{
	// TO DO: Obrot o 30 stopni
	// obrot o 30 stopni wokol punktu ktorym jest srodek obrazka oryginalnego
	Img_Cpy = Img_Org.Rotate(30. * acos(-1.) / 180.,wxPoint(Img_Org.GetSize().GetWidth() / 2, Img_Org.GetSize().GetHeight() /2)); // niepotrzebna kopia
}

void GUIMyFrame1::m_b_rotate_hue_click( wxCommandEvent& event )
{
	// TO DO: Przesuniecie Hue o 180 stopni
	Img_Cpy = Img_Org.Copy(); // potrzebna kopia (rzeczywiscie zmieniamy hue kazdego pixela) 
	Img_Cpy.RotateHue(0.5);	// -1. ~ 1. <=> -360 stopni ~ 360 stopni
}

void GUIMyFrame1::m_b_mask_click( wxCommandEvent& event )
{
	// TO DO: Ustawienie maski obrazu
	Img_Cpy = Img_Org.Copy(); // potrzebna kopia (rzeczywiscie bedziemy ustawiac maske na obrazie)
	Img_Cpy.SetMaskFromImage(Img_Mask, 0, 0, 0); // bo to co jest na czarno w masce bedzie zaslaniac obraz
	// oczyszczenie tla i ustawienie pozycji
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
}

void GUIMyFrame1::m_s_brightness_scroll( wxScrollEvent& event )
{
	// Tutaj, w reakcji na zmiane polozenia suwaka, wywolywana jest funkcja
	// Brightness(...), ktora zmienia jasnosc. W tym miejscu nic nie
	// zmieniamy. Do uzupelnienia pozostaje funkcja Brightness(...)
	Brightness(m_s_brightness->GetValue() - 100);
	Repaint();
}

void GUIMyFrame1::Brightness(int value)
{
	// TO DO: Zmiana jasnosci obrazu. value moze przyjmowac wartosci od -100 do 100
	Img_Cpy = Img_Org.Copy(); // potrzebna kopia (bêdziemy zmieniaæ wartoœci RGB pixeli)
	// "baza danych" - dla kazdego pixela R, G i B
	int size = Img_Cpy.GetSize().GetWidth() * Img_Cpy.GetSize().GetHeight() * 3;
	unsigned char * data = Img_Cpy.GetData();
	// wyliczenie jasnosci - wzor z wykladu + obsluga wyjscia z przedzialu [0,255]
	for (int i = 0; i< size; ++i)
	{
		data[i] = data[i] + value < 0 ? 0 : data[i] + value > 255 ? 255 : data[i] + value;
	}
	// Repaint() wywo³uje siê potem w m_s_brightness_scroll()
}

void GUIMyFrame1::m_s_contrast_scroll( wxScrollEvent& event )
{
	// Tutaj, w reakcji na zmiane polozenia suwaka, wywolywana jest funkcja
	// Contrast(...), ktora zmienia kontrast. W tym miejscu nic nie
	// zmieniamy. Do uzupelnienia pozostaje funkcja Contrast(...)
	Contrast(m_s_contrast->GetValue() - 100);
	Repaint();
}

void GUIMyFrame1::Contrast(int value)
{
	// TO DO: Zmiana kontrastu obrazu. value moze przyjmowac wartosci od -100 do 100
	Img_Cpy = Img_Org.Copy(); // potrzebna kopia (bêdziemy zmieniaæ wartoœci RGB pixeli)
	// "baza danych" - dla kazdego pixela R, G i B
	int size = Img_Cpy.GetSize().GetWidth() * Img_Cpy.GetSize().GetHeight() * 3;
	unsigned char * data = Img_Cpy.GetData();
	// wyliczenie kontrastu
	double contrast = (101. + value) / (101. - value);
	// wyliczenie kontrastu - wzor z wykladu (0.5 = 128) + obsluga wyjscia z przedzialu [0,255]
	for (int i = 0; i < size; ++i)
	{
		data[i] = (data[i] - 128) * contrast + 128 < 0 ? 0 : (data[i] - 128) * contrast + 128 > 255 ? 255 : (data[i] - 128) * contrast + 128;
	}
	// Repaint() wywo³uje siê potem w m_s_contrast_scroll()
}

void GUIMyFrame1::m_b_prewitt_click( wxCommandEvent& event )
{
	// TO DO: Pionowa maska Prewitta
	Img_Cpy = Img_Org.Copy(); // potrzebna kopia (bêdziemy zmieniaæ wartoœci RGB pixeli)
	// "baza danych" - dla kazdego pixela R, G i B
	int size = Img_Cpy.GetSize().GetWidth() * Img_Cpy.GetSize().GetHeight() * 3;
	unsigned char * data = Img_Cpy.GetData();
	// tablica pomocnicza
	int * finalData = new int[size];
	// zmienne pomocnicze (szerokosc, indeksy)
	int width = Img_Cpy.GetSize().GetWidth(), i1, i2, i3, i4, i5, i6;
	// wyliczenie filtru
	for (int i = 0; i < size; ++i)
	{
		// wyliczenie odpowiednich indeksow: %width*3 => modulo wiersz, -3 => obsluga rozszerzenia
		i1 = (i%(width *3)) - 3 < 0 ? i - width *3 : i - width*3 - 3;
		i1 = i1 < 0 ? i1 + width * 3 : i1;
		i2 = (i % (width * 3)) - 3 < 0 ? i : i - 3;
		i3 = (i % (width * 3)) - 3 < 0 ? i + width * 3 : i + width * 3 - 3;
		i3 = i3 > size ? i3 - width * 3 : i3;

		i4 = (i % (width * 3)) + 3 > width ? i - width * 3 : i - width * 3 + 3;
		i4 = i4 < 0 ? i4 + width * 3 : i4;
		i5 = (i % (width * 3)) + 3 > width ? i : i + 3;
		i6 = (i % (width * 3)) + 3 > width ? i + width * 3 : i + width * 3 + 3;
		i6 = i6 > size ? i6 - width * 3 : i6;

		// wyliczenie sumy, podzielenie przez 3 przyblizylo finalny obraz do dema
		finalData[i] = abs(data[i4] + data[i5] + data[i6] - data[i1] - data[i2] - data[i3])/3;
	}
	// przepisanie wartosci
	for (int i = 0; i < size; ++i)
	{
		data[i] = finalData[i] < 0 ? 0 : finalData[i] > 255 ? 255 : finalData[i];
	}
	delete finalData;

	// oczyszczenie tla i ustawienie pozycji
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
	Img_Cpy.SetMask(false);
}

void GUIMyFrame1::m_b_threshold_click( wxCommandEvent& event )
{
	// TO DO: Prog o wartosci 128 dla kazdego kanalu niezaleznie
	Img_Cpy = Img_Org.Copy(); // potrzebna kopia (bêdziemy zmieniaæ wartoœci RGB pixeli)
	// "baza danych" - dla kazdego pixela R, G i B
	int size = Img_Cpy.GetSize().GetWidth() * Img_Cpy.GetSize().GetHeight() * 3;
	unsigned char * data = Img_Cpy.GetData();
	// wyliczenie progu - wartosc graniczna z polecenia
	for (int i = 0; i < size; ++i)
	{
		data[i] = data[i] > 128 ? 255 : 0;
	}
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
	Img_Cpy.SetMask(false);
}

void GUIMyFrame1::Repaint()
{
	wxBitmap bitmap(Img_Cpy);          // Tworzymy tymczasowa bitmape na podstawie Img_Cpy
	wxClientDC dc(m_scrolledWindow);   // Pobieramy kontekst okna
	m_scrolledWindow->DoPrepareDC(dc); // Musimy wywolac w przypadku wxScrolledWindow, zeby suwaki prawidlowo dzialaly
	dc.DrawBitmap(bitmap, 0, 0, true); // Rysujemy bitmape na kontekscie urzadzenia
}

