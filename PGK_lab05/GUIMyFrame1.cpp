#include "GUIMyFrame1.h"
#include <vector>
#include <fstream>
#include "vecmat.h"

#include <cmath> 

struct Point 
{
	float x, y, z;
	Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Color 
{
	int R, G, B;
	Color(int _R, int _G, int _B) : R(_R), G(_G), B(_B) {}
};

struct Segment 
{
	Point begin, end;
	Color color;
	Segment(Point _begin, Point _end, Color _color) : begin(_begin), end(_end), color(_color) {}
};

std::vector<Segment> data;

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
	m_button_load_geometry->SetLabel(_("Wczytaj Geometri\u0119"));
	m_staticText25->SetLabel(_("Obr\u00F3t X:"));
	m_staticText27->SetLabel(_("Obr\u00F3t Y:"));
	m_staticText29->SetLabel(_("Obr\u00F3t Z:"));

	WxSB_TranslationX->SetRange(0, 200); WxSB_TranslationX->SetValue(100);
	WxSB_TranslationY->SetRange(0, 200); WxSB_TranslationY->SetValue(100);
	WxSB_TranslationZ->SetRange(0, 200); WxSB_TranslationZ->SetValue(100);

	WxSB_RotateX->SetRange(0, 360); WxSB_RotateX->SetValue(0);
	WxSB_RotateY->SetRange(0, 360); WxSB_RotateY->SetValue(0);
	WxSB_RotateZ->SetRange(0, 360); WxSB_RotateZ->SetValue(0);

	WxSB_ScaleX->SetRange(1, 200); WxSB_ScaleX->SetValue(100);
	WxSB_ScaleY->SetRange(1, 200); WxSB_ScaleY->SetValue(100);
	WxSB_ScaleZ->SetRange(1, 200); WxSB_ScaleZ->SetValue(100);
}

void GUIMyFrame1::WxPanel_Repaint( wxUpdateUIEvent& event )
{
	Repaint();
}

void GUIMyFrame1::m_button_load_geometry_click( wxCommandEvent& event )
{
	wxFileDialog WxOpenFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("Geometry file (*.geo)|*.geo"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (WxOpenFileDialog.ShowModal() == wxID_OK)
	{
		double x1, y1, z1, x2, y2, z2;
		int r, g, b;

		std::ifstream in(WxOpenFileDialog.GetPath().ToAscii());
		if (in.is_open())
		{
			data.clear();
			while (!in.eof())
			{
				in >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b;
				data.push_back(Segment(Point(x1, y1, z1), Point(x2, y2, z2), Color(r, g, b)));
			}
			in.close();
		}
	}
}

void GUIMyFrame1::Scrolls_Updated( wxScrollEvent& event )
{
	WxST_TranslationX->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationX->GetValue() - 100) / 50.0));
	WxST_TranslationY->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationY->GetValue() - 100) / 50.0));
	WxST_TranslationZ->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationZ->GetValue() - 100) / 50.0));

	WxST_RotateX->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateX->GetValue()));
	WxST_RotateY->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateY->GetValue()));
	WxST_RotateZ->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateZ->GetValue()));

	WxST_ScaleX->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleX->GetValue() / 100.0));
	WxST_ScaleY->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleY->GetValue() / 100.0));
	WxST_ScaleZ->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleZ->GetValue() / 100.0));

	Repaint();
}

// funkcja rysujaca
void GUIMyFrame1::Repaint()
{
	// ustawienie panelu
	wxClientDC dc(WxPanel);
	wxBufferedDC dcBuffer(&dc);
	// ustawienie szerokosci i wysokosci
	int width, height;
	WxPanel->GetSize(&width, &height);
	// tlo
	dcBuffer.SetBackground(wxBrush(RGB(255, 255, 255)));
	dcBuffer.Clear();
	// ClippingRegion - by nie rysowac wiecej niz trzeba = przyspieszenie pracy programy :D
	dc.SetClippingRegion(wxRect(0,0,width,height));

	// macierz translacji o wektor - wzor z wykladu
	Matrix4 translationMatrix;
	translationMatrix.data[0][0] = translationMatrix.data[1][1] = translationMatrix.data[2][2] = 1;
	translationMatrix.data[0][3] = (WxSB_TranslationX->GetValue() - 100.) / 50.;
	translationMatrix.data[1][3] = -(WxSB_TranslationY->GetValue() - 100.) / 50.;
	translationMatrix.data[2][3] = (WxSB_TranslationZ->GetValue() - 100.) / 50. + 2.; // +2. by obraz byl od razu widoczny na ekranie

	// macierz rotacji
	Matrix4 rotationMatrix;
	// pomocnicze macierze rotacji - wzory z wykladu
	Matrix4 rotX, rotY, rotZ;
	// katy
	double angleX = WxSB_RotateX->GetValue() * M_PI / 180.;
	double angleY = WxSB_RotateY->GetValue() * M_PI / 180.;
	double angleZ = WxSB_RotateZ->GetValue() * M_PI / 180.;
	// rotacja wokol osi X
	rotX.data[0][0] = 1.;
	rotX.data[1][1] = rotX.data[2][2] = cos(angleX);
	rotX.data[2][1] = sin(angleX);
	rotX.data[1][2] = -sin(angleX);
	// rotacja wokol osi Y
	rotY.data[1][1] = 1.;
	rotY.data[0][0] = rotY.data[2][2] = cos(angleY);
	rotY.data[2][0] = -sin(angleY);
	rotY.data[0][2] = sin(angleY);
	// rotacja wokol osi Z
	rotZ.data[2][2] = 1.;
	rotZ.data[0][0] = rotZ.data[1][1] = cos(angleZ);
	rotZ.data[1][0] = sin(angleZ);
	rotZ.data[0][1] = -sin(angleZ);
	// macierz rotacji to iloczyn macierzy odpowiedzialnych za rotacje wokol poszczegolnych osi
	rotationMatrix = rotX * rotY * rotZ;

	// macierz skalowania - wzor z wykladu
	Matrix4 scaleMatrix;
	scaleMatrix.data[0][0] = WxSB_ScaleX->GetValue() / 100.;
	scaleMatrix.data[1][1] = - WxSB_ScaleY->GetValue() / 100.;
	scaleMatrix.data[2][2] = WxSB_ScaleZ->GetValue() / 100.;

	// macierz transformacji - iloczyn macierzy translacji, rotacji i skalowania
	const Matrix4 finalMatrix = translationMatrix * rotationMatrix * scaleMatrix;

	// srodek
	Matrix4 centerMatrix;
	centerMatrix.data[0][0] = centerMatrix.data[1][1] = centerMatrix.data[2][2] = 1;
	centerMatrix.data[0][3] = centerMatrix.data[1][3] = 0.5;

	// pobieranie odcinkow i ich rysowanie
	for(auto & segment : data)
	{
		// poczatek i koniec rysowania
		Vector4 first, second;
		first.Set(segment.begin.x, segment.begin.y, segment.begin.z);
		first = finalMatrix * first;
		second.Set(segment.end.x, segment.end.y, segment.end.z);
		second = finalMatrix * second;

		// obsluga perspektywy
		if (first.GetZ() < 0. && second.GetZ() > 0.)
		{
			first.data[2] = 10e-4;
		}
		if (first.GetZ() > 0. && second.GetZ() < 0.)
		{
			second.data[2] = 10e-4;
		}
		if (first.GetZ() > 0. && second.GetZ() > 0.)
		{
			first.Set(first.GetX() / first.GetZ(), first.GetY() / first.GetZ(), first.GetZ());
			first = centerMatrix * first;
			second.Set(second.GetX() / second.GetZ(), second.GetY() / second.GetZ(), second.GetZ());
			second = centerMatrix * second;

			// rysowanie
			dcBuffer.SetPen(wxPen(RGB(segment.color.R, segment.color.G, segment.color.B)));
			dcBuffer.DrawLine(first.GetX() * width, first.GetY() * height, second.GetX() * width, second.GetY() * height);
		}
	}
}