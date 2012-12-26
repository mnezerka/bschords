#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/pen.h>
#include <vector>

#include "bschordsApp.h"
#include "bschordsPreview.h"
#include "bschordpro.h"

// http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#std::string_to_wxString

#define BSCHP_OFFSET_X 30
#define BSCHP_OFFSET_Y 30
#define BSCHP_LINE_SPACING 0
#define BSCHP_CHORD_OFFSET 0

// page size
#define BSCHP_X 210
#define BSCHP_Y 297

/* mm per inch */

#define MM_PER_IN (25.4)

struct BSLineItem
{
	wxString *txt;
	int posX;
	long width;
};

class BSChordProDCPainter : public BSChordProEventHandler
{
	public:
		BSChordProDCPainter(wxDC& dc, bschordsPreview *wnd);
		virtual void onLineBegin();
		virtual void onLineEnd();
		virtual void onCommand(const wstring& command, const wstring& value);
		virtual void onChord(const wstring& chord);
		virtual void onText(const wstring& text);
	private:
		wxDC& m_dc;
		bschordsPreview *m_wnd;
		int m_posY;
		int m_posX;
		int m_posXChord;
		int m_eMHeight;
		bool m_hasChords;
		vector<BSLineItem*> m_chordLine;
		vector<BSLineItem*> m_textLine;
};

BSChordProDCPainter::BSChordProDCPainter(wxDC& dc, bschordsPreview *wnd)
	: m_dc(dc), m_wnd(wnd), m_posY(0), m_posX(0), m_posXChord(0), m_eMHeight(0)
{
	// white background
	m_dc.DrawRectangle(0, 0, wnd->getDeviceX(BSCHP_X) , wnd->getDeviceX(BSCHP_Y));
	wxPen pen(wxColour(200, 200, 200), 0.1); // red pen of width 1
	m_dc.SetPen(pen);
	//m_dc.SetPen(*wxGREY_PEN);

	// draw border line
	m_dc.DrawRectangle(
		wnd->getDeviceX(BSCHP_OFFSET_X),
		wnd->getDeviceY(BSCHP_OFFSET_Y),
		wnd->getDeviceX(BSCHP_X - 2 * BSCHP_OFFSET_X),
		wnd->getDeviceY(BSCHP_Y - 2 * BSCHP_OFFSET_Y));

	m_posY = wnd->getDeviceY(BSCHP_OFFSET_Y);

	wxPen pen2(wxColour(0, 0, 200), 0.1); // red pen of width 1
	m_dc.SetPen(pen2);
}

void BSChordProDCPainter::onText(const wstring& text)
{

	BSLineItem *item = new BSLineItem();
	//item->txt = new wxString(text.c_str(), wxConvUTF8);
	item->txt = new wxString(text);
	//wcout << L"Converted >" << item->txt->wc_str() << L"<" << endl;
    m_dc.SetFont(wxGetApp().m_fonts[BS_FONT_TEXT]);
	wxSize textSize = m_dc.GetTextExtent(*item->txt);
	item->width = textSize.GetWidth();
	item->posX = m_posX;

	//wcout << L"On text >" << text << L"<" << text.size() << " width: " << textSize.GetWidth() << " posx: " << m_posX << endl;

	m_textLine.push_back(item);

	m_posX += textSize.GetWidth();

	if (m_posX > m_posXChord)
		m_posXChord = m_posX;
}

void BSChordProDCPainter::onChord(const wstring& chord)
{
	BSLineItem *item = new BSLineItem();
	//item->txt = new wxString(chord.c_str(), wxConvUTF8);
    m_dc.SetFont(wxGetApp().m_fonts[BS_FONT_CHORDS]);
	item->txt = new wxString(chord);
	wxSize textSize = m_dc.GetTextExtent(*item->txt);
	item->width = textSize.GetWidth();
	item->posX = m_posX;

	// TODO: Shift text if chord takes too much space

	m_chordLine.push_back(item);

	m_posXChord += textSize.GetWidth();
}

void BSChordProDCPainter::onCommand(const wstring& command, const wstring& value)
{
	//m_dc.SetFont(*fontTitle_);
	if (command == L"title")
    {
        m_dc.SetFont(wxGetApp().m_fonts[BS_FONT_TITLE]);
		m_dc.DrawText(value, m_wnd->getDeviceX(BSCHP_OFFSET_X), m_posY);
		m_posY += m_dc.GetCharHeight() + BSCHP_LINE_SPACING;
	}
	//cout << "drawing command" << endl;
}

void BSChordProDCPainter::onLineBegin() {
	m_posX = m_wnd->getDeviceX(BSCHP_OFFSET_X);
	m_hasChords = false;
};
void BSChordProDCPainter::onLineEnd()
{
	//std::cout << "OnLineEnd, m_posY is " << m_posY << endl;

	if (m_chordLine.size() > 0)
		{
			//wcout << L"drawing chord line" << endl;
            m_dc.SetFont(wxGetApp().m_fonts[BS_FONT_CHORDS]);
			for (size_t i = 0; i < m_chordLine.size(); i++)
				m_dc.DrawText(*m_chordLine[i]->txt, m_chordLine[i]->posX, m_posY);

			m_posY += m_dc.GetCharHeight() + BSCHP_CHORD_OFFSET;

			while (!m_chordLine.empty())
				m_chordLine.pop_back();
		}

    m_dc.SetFont(wxGetApp().m_fonts[BS_FONT_TEXT]);
	for (size_t i = 0; i < m_textLine.size(); i++)
		{
			//wcout << L"drawing text: " <<
			m_dc.DrawText(*m_textLine[i]->txt, m_textLine[i]->posX, m_posY);

			//m_dc.DrawLine(m_textLine[i]->posX, m_posY, m_textLine[i]->posX + m_textLine[i]->width, m_posY + m_dc.GetCharHeight());
			//m_dc.DrawLine(m_textLine[i]->posX, m_posY, m_textLine[i]->posX, m_posY + m_dc.GetCharHeight());
		}


	while (!m_textLine.empty())
		m_textLine.pop_back();

	m_posY += m_dc.GetCharHeight() + BSCHP_LINE_SPACING;
};

/////////////////////////////////////////////////////////////////////////////////////////

bschordsPreview::bschordsPreview(wxWindow *parent, wxRichTextCtrl *sourceCtrl)
	: wxScrolledWindow(parent), m_zoom(1)
{
	m_sourceCtrl = sourceCtrl;

	wxClientDC dc(this);
	//DoPrepareDC(dc);

	m_screenPPI = dc.GetPPI();

	setZoom(1);
}

bschordsPreview::~bschordsPreview()
{
	//dtor
}

void bschordsPreview::OnDraw(wxDC& dc)
{
	//std::wcout << L"OnDraw" << endl;

	dc.SetUserScale(m_zoom, m_zoom);

	int cx, cy;
	GetViewStart(&cx, &cy);
	//cout << "View start: " << cx << "x" << cy << " px" << endl;

	GetVirtualSize(&cx, &cy);
	//cout << "Virtual size: " << cx << "x" << cy << " px" << endl;

	// get lines from song book control
	int lines = m_sourceCtrl->GetNumberOfLines();
	wxString text;
	for (int i = 0; i < lines; i++)
		{
			if (text.size() > 0)
				text.Append(wxT("\n"));
			text.Append(m_sourceCtrl->GetLineText(i));
		}

	BSChordProDCPainter y(dc, this);

	BSChordProParser p(&y);

	//wcout << text.wc_str() << endl;
	p.parse(std::wstring(text.wc_str()));
}

void bschordsPreview::setZoom(float zoom)
{
	m_zoom = zoom;

	wxSize newVirtSize;
	newVirtSize.SetWidth((int)(BSCHP_X * m_zoom * m_screenPPI.GetWidth() / MM_PER_IN));
	newVirtSize.SetHeight((int)(BSCHP_Y * m_zoom * m_screenPPI.GetHeight() / MM_PER_IN));

	//cout << "Setting virtual size to: " << newVirtSize.GetWidth() << "x" << newVirtSize.GetHeight() << " px" << endl;

	//wxCoord x = dc.LogicalToDeviceX(wxCoord(BSCHP_X));
	//wxCoord y = dc.LogicalToDeviceY(wxCoord(BSCHP_Y));
	//cout << "setting virtual size to: " << x << " x " << y << " (" << BSCHP_X << " x " << BSCHP_Y << "mm), zoom: " << m_zoom << endl;

	//int lcx = dc.DeviceToLogicalX(x);
	//int lcy = dc.DeviceToLogicalY(y);

	//cout << "back computed: " << lcx << "x" << lcy << " mm" << endl;

	//wxCoord x1 = dc.LogicalToDeviceX(wxCoord(1000));
	//wxCoord y1 = dc.LogicalToDeviceY(wxCoord(1000));
	//cout << "1000 mm size: " << x1 << " x " << y1 << endl;

	//SetScrollbars(10, 10, x / 10, y / 10);

	SetVirtualSize(newVirtSize);
	SetScrollRate(10, 10);

	Refresh();
}

wxCoord bschordsPreview::getDeviceX(int numMM)
{
	return numMM / MM_PER_IN * m_screenPPI.GetWidth();
}

wxCoord bschordsPreview::getDeviceY(int numMM)
{
	return numMM / MM_PER_IN * m_screenPPI.GetHeight();
}
