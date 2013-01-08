#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/pen.h>
#include <vector>

#include "bschordsApp.h"
#include "bschordsPreview.h"
#include "bschordpro.h"
#include "bschordsdcpainter.h"


// http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#std::string_to_wxString


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
	// parse and draw
	bschords::TSetDCPainter y(dc, m_screenPPI.GetWidth() / MM_PER_IN);
	BSChordProParser p(&y);
	p.parse(std::wstring(text.wc_str()));
}

void bschordsPreview::setZoom(float zoom)
{
	m_zoom = zoom;

	wxSize newVirtSize;
	newVirtSize.SetWidth((int)(wxGetApp().m_styleSheet.m_pageSize.GetWidth() * m_zoom * m_screenPPI.GetWidth() / MM_PER_IN));
	newVirtSize.SetHeight((int)(wxGetApp().m_styleSheet.m_pageSize.GetWidth() * m_zoom * m_screenPPI.GetHeight() / MM_PER_IN));

	SetVirtualSize(newVirtSize);
	SetScrollRate(10, 10);

	Refresh();
}

