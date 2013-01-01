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

	BSChordsDCPainter y(dc);

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

