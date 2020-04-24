#include <wx/wxprec.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/scrolbar.h>
#include <wx/gauge.h>
#include <wx/textctrl.h>
#include <wx/string.h>
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/colordlg.h>
#include <wx/font.h>

class MyFrame : public wxFrame
{
private:
	// funkcje menu (kontrolki)
	void Save(wxCommandEvent&);
	void Banana(wxCommandEvent&);
	void ScrollBar(wxScrollEvent&);
	void StarColorUpdate(wxCommandEvent&);
	void TextUpdate(wxCommandEvent&);
	void chooseCelestial(wxCommandEvent&);

	// rysowanie
	void Draw();
	// przerysowanie okna
	//void Form_Paint(wxPaintEvent&);	// myslalem o Paint, ale Update wystarcza - obie funkcje dzialaly by tak samo: tylko Draw(), wiec Update jest wystarczajacy)
	// przerysowanie okna w update
	void Form_Update(wxUpdateUIEvent&);

	// "tagi"
	enum 
	{
		ID_WX_SAVE_BUTTON = 1001,
		ID_WX_BANANA_CHECK_BOX = 1002,
		ID_WX_SCROLL_BAR = 1003,
		ID_WX_GAUGE = 1004,
		ID_WX_STAR_COLOR_BUTTON = 1005,
		ID_WX_TEXT_WINDOW = 1006,
		ID_WX_CELESTIAL_CHOISE = 1007,
	};
	// sizery
	wxBoxSizer *sizer1, *sizer2, *sizer3;

	// kontrolki
	wxPanel* panel;
	wxButton* saveButton;
	wxCheckBox* bananaCheckBox;
	wxScrollBar* scrollBar;
	wxGauge* gauge;
	wxButton* starColorButton;
	wxTextCtrl* textCtrl;
	wxChoice* celestialArray;

	wxString textToShow = "tekst";
	int celestialToShowIndex; // 0 -> gwiazda (domyœlnie), 1 -> ksiezyc, 2 -> slonce

	wxImage imageToSave;
	wxBitmap buffer;
	wxColour starColor;
	wxBitmap bananaImage;

public:
	MyFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 400), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	~MyFrame();
};

// konstruktor
MyFrame::MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style) { this->SetSizeHints(wxSize(600, 400), wxDefaultSize);

	sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer3 = new wxBoxSizer(wxVERTICAL);

	panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	panel->SetBackgroundColour(wxColour(255, 255, 255));
	sizer2->Add(panel, 1, wxEXPAND | wxALL, 5);
	sizer1->Add(sizer2, 1, wxEXPAND, 5);

	sizer3->SetMinSize(wxSize(200, 400));
	sizer1->Add(sizer3, 0, wxEXPAND, 5);
	// zapis
	saveButton = new wxButton(this, ID_WX_SAVE_BUTTON, "ZAPIS DO PLIKU", wxDefaultPosition, wxDefaultSize, 0);
	sizer3->Add(saveButton, 0, wxALIGN_CENTER | wxALL, 5);
	// banan
	bananaCheckBox = new wxCheckBox(this, ID_WX_BANANA_CHECK_BOX, "BANAN", wxDefaultPosition, wxDefaultSize, 0);
	sizer3->Add(bananaCheckBox, 0, wxALIGN_CENTER | wxALL, 5);
	// scroll + gauge
	scrollBar = new wxScrollBar(this, ID_WX_SCROLL_BAR, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
	scrollBar->SetScrollbar(0, 1, 100, 1);
	scrollBar->Disable();
	sizer3->Add(scrollBar, 0, wxALL | wxEXPAND, 5);

	gauge = new wxGauge(this, ID_WX_GAUGE, 99, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
	gauge->SetValue(0);
	sizer3->Add(gauge, 0, wxALL | wxEXPAND, 5);

	// kolor gwiazdy (poczatkowo czarny)
	starColorButton = new wxButton(this, ID_WX_STAR_COLOR_BUTTON, "KOLOR GWIAZDY", wxDefaultPosition, wxDefaultSize, 0);
	sizer3->Add(starColorButton, 0, wxALIGN_CENTER | wxALL, 5);
	starColor = wxColour(0, 0, 0);
	// wpisywany tekst
	textCtrl = new wxTextCtrl(this, ID_WX_TEXT_WINDOW, textToShow, wxDefaultPosition, wxDefaultSize, 0);
	sizer3->Add(textCtrl, 0, wxALIGN_CENTER | wxALL, 5);

	// opcje wyboru ciala niebieskiego
	celestialArray = new wxComboBox(this, ID_WX_CELESTIAL_CHOISE, "GWIAZDA", wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
	celestialArray->Append("GWIAZDA");
	celestialArray->Append("KSIEZYC");
	celestialArray->Append("SLONCE");
	celestialArray->SetSelection(0);
	sizer3->Add(celestialArray, 0, wxALIGN_CENTER | wxALL, 5);

	// bindowanie kontrolek z tagami
	Bind(wxEVT_BUTTON, &MyFrame::Save, this, ID_WX_SAVE_BUTTON);
	Bind(wxEVT_CHECKBOX, &MyFrame::Banana, this, ID_WX_BANANA_CHECK_BOX);
	Bind(wxEVT_SCROLL_THUMBTRACK, &MyFrame::ScrollBar, this, ID_WX_SCROLL_BAR);
	Bind(wxEVT_BUTTON, &MyFrame::StarColorUpdate, this, ID_WX_STAR_COLOR_BUTTON);
	Bind(wxEVT_TEXT, &MyFrame::TextUpdate, this, ID_WX_TEXT_WINDOW);
	Bind(wxEVT_COMBOBOX, &MyFrame::chooseCelestial, this, ID_WX_CELESTIAL_CHOISE);
	//Bind(wxEVT_PAINT, &MyFrame::Form_Paint, this);
	Bind(wxEVT_UPDATE_UI, &MyFrame::Form_Update, this);

	// wczytanie obrazka
	wxImage banana = wxImage();
	banana.AddHandler(new wxPNGHandler);
	banana.LoadFile("banan.png", wxBITMAP_TYPE_PNG);
	bananaImage = wxBitmap(banana);

	// ustawienie
	this->SetSizer(sizer1);
	this->Layout();
	this->Centre(wxBOTH);
}

// destruktor
MyFrame::~MyFrame()
{
	delete panel;
	delete saveButton;
	delete bananaCheckBox;
	delete scrollBar;
	delete gauge;
	delete starColorButton;
	delete textCtrl;
	delete celestialArray;
}

// zapis
void MyFrame::Save(wxCommandEvent& event)
{
	wxFileDialog saveFileDialog(this, "Choose a file", "", "", "JPG files (*.jpg)|*.jpg|PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
	imageToSave = buffer.ConvertToImage();
	imageToSave.AddHandler(new wxJPEGHandler);
	imageToSave.AddHandler(new wxPNGHandler);
	imageToSave.SaveFile(saveFileDialog.GetPath());
}

// banan
void MyFrame::Banana(wxCommandEvent& event)
{
	scrollBar->Enable(bananaCheckBox->IsChecked());
}

// scroll + gauge
void MyFrame::ScrollBar(wxScrollEvent&  event)
{
	gauge->SetValue(scrollBar->GetThumbPosition());
	Draw();
}

// kolor gwiazdy
void MyFrame::StarColorUpdate(wxCommandEvent& event)
{
	wxColourDialog chooseColourDialog(this, nullptr);
	if (chooseColourDialog.ShowModal() == wxID_CANCEL) return;
	starColor = chooseColourDialog.GetColourData().GetColour();
}

// zmiana tekstu
void MyFrame::TextUpdate(wxCommandEvent& event)
{
	textToShow = textCtrl->GetValue();
}

// wybor ciala niebieskiego
void MyFrame::chooseCelestial(wxCommandEvent&  event)
{
	celestialToShowIndex = celestialArray->GetSelection();
}

// malowanie
void MyFrame::Draw()
{
	wxClientDC __MyDC(panel);
	buffer = wxBitmap(panel->GetSize().x, panel->GetSize().y);
	wxBufferedDC _MyDC(&__MyDC, buffer);

	// wyliczenie centrum ekranu
	wxSize panelSize = panel->GetVirtualSize() / 2;
	int x = panelSize.x;
	int y = panelSize.y;

	_MyDC.SetBackground(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
	_MyDC.Clear();

	// banan
	if (bananaCheckBox->IsChecked()) 
	{
		_MyDC.DrawBitmap(bananaImage, x - 30 - bananaImage.GetWidth() / 2, y + 30 - 0.5 * gauge->GetValue() - bananaImage.GetHeight() / 2);
	}
	_MyDC.SetBrush(wxBrush(wxColour(100, 70, 50), wxBRUSHSTYLE_SOLID));

	// ogon
	_MyDC.SetBrush(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
	_MyDC.SetPen(wxPen(wxColour(100, 70, 50), 4, wxPENSTYLE_SOLID));
	_MyDC.DrawEllipticArc(x, y + 40, 30, 40, 180, 360);

	_MyDC.SetBrush(wxBrush(wxColour(100, 70, 50), wxBRUSHSTYLE_SOLID));
	_MyDC.SetPen(wxPen(wxColour(100, 70, 50), 10, wxPENSTYLE_SOLID));
	// glowa
	_MyDC.DrawEllipse(x - 15, y - 25, 30, 20);
	// tulow
	_MyDC.DrawLine(x, y, x, y + 50);
	// nogi
	_MyDC.DrawLine(x, y + 50, x - 20, y + 90);
	_MyDC.DrawLine(x, y + 50, x + 20, y + 90);
	//rece
	_MyDC.DrawLine(x, y, x + 30, y + 30);
	_MyDC.DrawLine(x, y, x - 30, y + 30 - 0.5 * gauge->GetValue()); // prawa reka z bananem

	_MyDC.SetBrush(wxBrush(wxColour(250, 200, 100), wxBRUSHSTYLE_SOLID));
	_MyDC.SetPen(wxPen(wxColour(250, 200, 100), 10, wxPENSTYLE_SOLID));
	// glowa
	_MyDC.DrawCircle(x, y-12, 7);
	// ³apy
	_MyDC.DrawCircle(x + 30, y + 30, 2);
	_MyDC.DrawCircle(x - 30, y + 30 - 0.5 * gauge->GetValue(), 2);
	// stopy
	_MyDC.DrawLine(x-20, y+90, x-25, y+90);
	_MyDC.DrawLine(x+20, y+90, x+25, y+90);
	// brzuch
	_MyDC.SetPen(wxPen(wxColour(250, 200, 100), 4, wxPENSTYLE_SOLID));
	_MyDC.DrawLine(x, y, x, y + 50);
	
	// oczy
	_MyDC.SetBrush(wxBrush(wxColour(0, 0, 0), wxBRUSHSTYLE_SOLID));
	_MyDC.SetPen(wxPen(wxColour(0, 0, 0), 2, wxPENSTYLE_SOLID));
	_MyDC.DrawCircle(x - 8, y - 15, 2);
	_MyDC.DrawCircle(x + 8, y - 15, 2);

	_MyDC.SetBrush(wxBrush(wxColour(250, 200, 100), wxBRUSHSTYLE_SOLID));
	// usta - zmienne w zaleznosci od tego czy mapla trzyma banan
	if (bananaCheckBox->IsChecked()) 
	{
		_MyDC.DrawEllipticArc(x - 8, y -9, 16, 7, 180, 360);
	}
	else
	{
		_MyDC.DrawEllipticArc(x -8, y -9, 16, 7, 0, 180);
	}

	// wypisanie tekstów
	wxString text1(textToShow);
	_MyDC.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial"));
	_MyDC.DrawText(text1, x -150, y +130);
	wxString text2(textToShow);
	_MyDC.SetFont(wxFont(36, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false, "Calibri"));
	_MyDC.DrawRotatedText(textToShow, x +100, y + 130, 90);

	// rysowanie cia³ niebieskich
	if (celestialToShowIndex == 0) // gwiazda
	{
		_MyDC.SetBrush(wxBrush(starColor));
		_MyDC.SetPen(wxPen(starColor));
		wxPoint verts[5] = { wxPoint(60, 5), wxPoint(85, 77), wxPoint(20, 32), wxPoint(100, 32), wxPoint(35, 77)};
		_MyDC.DrawPolygon(5, verts, 0, 0);
	}
	else
	{
		if (celestialToShowIndex == 1) // ksiezyc 
		{
			_MyDC.SetBrush(wxBrush(wxColour(96, 96, 96), wxBRUSHSTYLE_SOLID));
			_MyDC.SetPen(wxPen(wxColour(96, 96, 96), 1, wxPENSTYLE_SOLID));
			_MyDC.DrawCircle(50, 50, 30);
			_MyDC.SetBrush(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
			_MyDC.SetPen(wxPen(wxColour(255, 255, 255), 1, wxPENSTYLE_SOLID));
			_MyDC.DrawCircle(30, 40, 30);
		}
		else
		{
			if (celestialToShowIndex == 2) // slonce
			{
				_MyDC.SetBrush(wxBrush(wxColour(255, 255, 20), wxBRUSHSTYLE_SOLID));
				_MyDC.SetPen(wxPen(wxColour(255, 255, 20), 1, wxPENSTYLE_SOLID));
				_MyDC.DrawCircle(50, 50, 30);
			}
		}
	}
}

// przerysowanie okna
//void MyFrame::Form_Paint(wxPaintEvent& event)
//{
//	Draw();
//}

// przerysowywanie okna w update
void MyFrame::Form_Update(wxUpdateUIEvent& event)
{
	Draw();
}

// glowna klasa aplikacji
class MyApp : public wxApp
{
public:
	virtual bool OnInit() 
	{
		MyFrame *frame = new MyFrame(NULL, wxID_ANY, "PGK - LAB 03 - Jakub Perlak");
		frame->Show(true);
		return true;
	}
};

// makro - instancja aplikacji
wxIMPLEMENT_APP(MyApp);
