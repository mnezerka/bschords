/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Preview Window Implementation
 */

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

namespace bschords
{

/** \brief Structure for zoom */
struct ZoomItem
{
    ZoomItem(int s, wxString l) : mSize(s), mLabel(l) { };
    int mSize;
    wxString mLabel;
};

/// list of zoom items
std::vector<ZoomItem> gZoomItems;

// ------- PreviewWndCanvas --------------------------------------------------------

//PreviewWndCanvas::PreviewWndCanvas(wxWindow *parent, wxRichTextCtrl *sourceCtrl, wxStaticText *infoCtrl)
PreviewWndCanvas::PreviewWndCanvas(wxWindow *parent, wxStyledTextCtrl *sourceCtrl, wxStaticText *infoCtrl)
    : wxScrolledWindow(parent), m_sourceCtrl(sourceCtrl), m_infoCtrl(infoCtrl), m_zoom(100)
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
    float zoomRel = m_zoom / 100.0;

    dc.SetUserScale(zoomRel, zoomRel);

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

void PreviewWndCanvas::setZoom(int zoom)
{
    m_zoom = zoom;

    float zoomRel = m_zoom / 100.0;

    wxSize newVirtSize;
    newVirtSize.SetWidth((int)(wxGetApp().m_styleSheet.m_pageSize.GetWidth() * zoomRel * m_screenPPI.GetWidth() / MM_PER_IN));
    newVirtSize.SetHeight((int)(wxGetApp().m_styleSheet.m_pageSize.GetHeight() * zoomRel * m_screenPPI.GetHeight() / MM_PER_IN));

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
    EVT_CLOSE(PreviewWnd::OnClose)
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
    gZoomItems.push_back(ZoomItem(10, wxT("10%")));
    gZoomItems.push_back(ZoomItem(20, wxT("20%")));
    gZoomItems.push_back(ZoomItem(30, wxT("30%")));
    gZoomItems.push_back(ZoomItem(40, wxT("40%")));
    gZoomItems.push_back(ZoomItem(50, wxT("50%")));
    gZoomItems.push_back(ZoomItem(60, wxT("60%")));
    gZoomItems.push_back(ZoomItem(70, wxT("70%")));
    gZoomItems.push_back(ZoomItem(80, wxT("80%")));
    gZoomItems.push_back(ZoomItem(90, wxT("90%")));
    gZoomItems.push_back(ZoomItem(100, wxT("100%")));
    gZoomItems.push_back(ZoomItem(110, wxT("110%")));
    gZoomItems.push_back(ZoomItem(120, wxT("120%")));
    gZoomItems.push_back(ZoomItem(130, wxT("130%")));
    gZoomItems.push_back(ZoomItem(140, wxT("140%")));
    gZoomItems.push_back(ZoomItem(150, wxT("150%")));
    gZoomItems.push_back(ZoomItem(175, wxT("175%")));
    gZoomItems.push_back(ZoomItem(200, wxT("200%")));
    gZoomItems.push_back(ZoomItem(250, wxT("250%")));
    gZoomItems.push_back(ZoomItem(300, wxT("300%")));
    gZoomItems.push_back(ZoomItem(400, wxT("400%")));
    gZoomItems.push_back(ZoomItem(500, wxT("500%")));
    gZoomItems.push_back(ZoomItem(600, wxT("600%")));

    m_zoomCtrl = new wxComboBox(panel, ID_COMBO_ZOOM, wxEmptyString, wxDefaultPosition, wxSize(100, wxDefaultCoord), 0, NULL, wxCB_READONLY);
    unsigned int ix100 = 0;
    for (unsigned int i = 0; i < gZoomItems.size(); i++)
    {
        m_zoomCtrl->Append(wxString::Format(wxT("%s"), gZoomItems[i].mLabel.c_str()));
        if (gZoomItems[i].mSize == 100)
            ix100 = i;
    }
    m_zoomCtrl->Select(ix100);
    panelSizer->Add(m_zoomCtrl, 0, wxALL | wxCENTER, 2);

    panelSizer->AddSpacer(5);

    m_info = new wxStaticText(panel, wxID_ANY, wxT("Ok"));
    panelSizer->Add(m_info, 1, wxALL | wxCENTER, 2);

    sizer->Add(panel, 0, wxALL | wxEXPAND, 3);

    m_canvas = new PreviewWndCanvas(this, sourceCtrl, m_info);
    sizer->Add(m_canvas, 1, wxALL | wxEXPAND);

    int zoom = wxGetApp().config->Read(wxT("/preview/zoom"), 100);

    //Resize();
    setZoom(zoom);
}

PreviewWnd::~PreviewWnd()
{
    //dtor
}

void PreviewWnd::setZoom(int zoom)
{
    if (m_canvas)
    {
        m_canvas->setZoom(zoom);
        m_canvas->Refresh();
        m_canvas->Update();

        // select appropriate entry in combo box
        int ix = -1;

        for (unsigned int i = 0; i < gZoomItems.size(); i++)
        {
            if (gZoomItems[i].mSize == zoom)
            {
                ix = i;
                break;
            }
        }
        if (ix >= 0)
             m_zoomCtrl->Select(ix);
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

    if (zoom > 0 && zoom < 20000)
    {
        setZoom(zoom);
    }
}

void PreviewWnd::OnClose(wxCloseEvent &event)
{
    wxLogDebug(wxT("Preview - OnClose()"));
    wxGetApp().config->Write(_("/preview/zoom"), m_canvas->getZoom());

    Destroy();
}

} // namespace
