#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/pen.h>
#include <vector>

#include "app.h"
#include "previewwnd.h"
#include "bschordpro.h"
#include "dcpainter.h"

// http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#std::string_to_wxString

using namespace bschords;

// ------- PreviewWndCanvas --------------------------------------------------------

//PreviewWndCanvas::PreviewWndCanvas(wxWindow *parent, wxRichTextCtrl *sourceCtrl, wxStaticText *infoCtrl)
PreviewWndCanvas::PreviewWndCanvas(wxWindow *parent, wxStyledTextCtrl *sourceCtrl, wxStaticText *infoCtrl)
	: wxScrolledWindow(parent), m_sourceCtrl(sourceCtrl), m_infoCtrl(infoCtrl), m_zoom(1)
{
	wxClientDC dc(this);
	//DoPrepareDC(dc);

	m_screenPPI = dc.GetPPI();

	setZoom(m_zoom);
}

PreviewWndCanvas::~PreviewWndCanvas()
{
	//dtor
}

void PreviewWndCanvas::OnDraw(wxDC& dc)
{
	dc.SetUserScale(m_zoom, m_zoom);

	int cx, cy;
	GetViewStart(&cx, &cy);
	GetVirtualSize(&cx, &cy);

	// get lines from song book control
	wxString text = m_sourceCtrl->GetText();

	// parse and draw
	bschords::TSetDCPainter painter(dc, m_screenPPI.GetWidth() / MM_PER_IN);
	bschordpro::Parser p(&painter);
	p.parse(std::wstring(text.wc_str()));
	painter.drawPage(0);

	wxString infoText;
	bool infoIsWarning = false;
	bschords::TSetStat stat = painter.getTSetStat();

	infoText << _("pages: ") << stat.m_pages;
	if (stat.m_clippings > 0)
	{
		infoText << _(" clippings: ") << stat.m_clippings;
		infoIsWarning = true;
	}
	if (m_infoCtrl)
	{
		if (infoIsWarning)
			m_infoCtrl->SetBackgroundColour(wxColour(255, 200, 200));
		else
			m_infoCtrl->SetBackgroundColour(wxNullColour);

		m_infoCtrl->SetBackgroundStyle(wxBG_STYLE_COLOUR);

		m_infoCtrl->SetLabel(infoText);
	}
}

void PreviewWndCanvas::setZoom(float zoom)
{
	m_zoom = zoom;

	wxSize newVirtSize;
	newVirtSize.SetWidth((int)(wxGetApp().m_styleSheet.m_pageSize.GetWidth() * m_zoom * m_screenPPI.GetWidth() / MM_PER_IN));
	newVirtSize.SetHeight((int)(wxGetApp().m_styleSheet.m_pageSize.GetHeight() * m_zoom * m_screenPPI.GetHeight() / MM_PER_IN));

	SetVirtualSize(newVirtSize);
	SetScrollRate(10, 10);

	Refresh();
}


// ------- PreviewWnd -------------------------------------------------------------

enum
{
	ID_COMBO_ZOOM = 1000
};

BEGIN_EVENT_TABLE(PreviewWnd, wxWindow)
	EVT_SIZE(PreviewWnd::OnSize)
	EVT_COMBOBOX(ID_COMBO_ZOOM, PreviewWnd::OnZoomChanged)
END_EVENT_TABLE()


PreviewWnd::PreviewWnd(wxWindow *parent, wxStyledTextCtrl *sourceCtrl)
	: wxWindow(parent, wxID_ANY), m_canvas(NULL), m_zoomCtrl(NULL)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);

	// create control bar
	wxPanel *panel = new wxPanel(this);
	//wxStaticText *text = new wxStaticText(panel, wxID_ANY, _("ahoj"));

	wxBoxSizer *panelSizer = new wxBoxSizer(wxHORIZONTAL);
	panel->SetSizer(panelSizer);
	// adding a combo with zoom level
	m_zoomCtrl = new wxComboBox(panel, ID_COMBO_ZOOM, wxEmptyString, wxDefaultPosition, wxSize(100, wxDefaultCoord), 0, NULL, wxCB_READONLY);
	m_zoomCtrl->Append(_("10%"));
	m_zoomCtrl->Append(_("20%"));
	m_zoomCtrl->Append(_("30%"));
	m_zoomCtrl->Append(_("40%"));
	m_zoomCtrl->Append(_("50%"));
	m_zoomCtrl->Append(_("60%"));
	m_zoomCtrl->Append(_("70%"));
	m_zoomCtrl->Append(_("80%"));
	m_zoomCtrl->Append(_("90%"));
	int ix100 = m_zoomCtrl->Append(_("100%"));
	m_zoomCtrl->Append(_("110%"));
	m_zoomCtrl->Append(_("120%"));
	m_zoomCtrl->Append(_("130%"));
	m_zoomCtrl->Append(_("140%"));
	m_zoomCtrl->Append(_("150%"));
	m_zoomCtrl->Append(_("175%"));
	m_zoomCtrl->Append(_("200%"));
	m_zoomCtrl->Append(_("250%"));
	m_zoomCtrl->Append(_("300%"));
	m_zoomCtrl->Select(ix100);
	panelSizer->Add(m_zoomCtrl, 0, wxALL | wxCENTER, 2);

	panelSizer->AddSpacer(5);

	m_info = new wxStaticText(panel, wxID_ANY, wxT("Ok"));
	panelSizer->Add(m_info, 1, wxALL | wxCENTER, 2);

	sizer->Add(panel, 0, wxALL | wxEXPAND, 3);

	m_canvas = new PreviewWndCanvas(this, sourceCtrl, m_info);
	sizer->Add(m_canvas, 1, wxALL | wxEXPAND);

	//Resize();
	setZoom(1);
}

PreviewWnd::~PreviewWnd()
{
	//dtor
}

void PreviewWnd::setZoom(float zoom)
{
	if (m_canvas)
	{
		m_canvas->setZoom(zoom);
		m_canvas->Refresh();
		m_canvas->Update();
	}
}

void PreviewWnd::OnSize(wxSizeEvent& event)
{
	if (GetAutoLayout())
		Layout();
}

void PreviewWnd::OnZoomChanged(wxCommandEvent& event)
{
	int zoom = wxAtoi(m_zoomCtrl->GetValue());

	if (zoom > 0 && zoom < 2000)
	{
		setZoom((double)zoom / 100);
	}
}

