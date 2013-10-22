/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Preview Window Declaration
 */

#ifndef BSCHORDSPREVIEW_H
#define BSCHORDSPREVIEW_H

#include <wx/scrolwin.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>

namespace bschords
{

class PreviewWndCanvas : public wxScrolledWindow
{
public:
    //BSChordsPreviewCanvas(wxWindow *parent, wxRichTextCtrl *sourceCtrl, wxStaticText *infoCtrl = NULL);
    PreviewWndCanvas(wxWindow *parent, wxStyledTextCtrl *sourceCtrl, wxStaticText *infoCtrl = NULL);
    virtual ~PreviewWndCanvas();
    virtual void OnDraw(wxDC& dc);
    void setZoom(int zoom);
    int getZoom() { return m_zoom; };
    wxCoord getDeviceX(int numMM);
    wxCoord getDeviceY(int numMM);
protected:
private:
    //wxRichTextCtrl *m_sourceCtrl;
    wxStyledTextCtrl *m_sourceCtrl;
    wxStaticText *m_infoCtrl;
    //wxStyledTextCtrl *m_sourceCtrl;
    int m_zoom;
    wxSize m_screenPPI;
};

class PreviewWnd : public wxWindow
{
public:
    //BSChordsPreview(wxWindow *parent, wxRichTextCtrl *sourceCtrl);
    PreviewWnd(wxWindow *parent, wxStyledTextCtrl *sourceCtrl);
    virtual ~PreviewWnd();
    void setZoom(int zoom);

private:
    PreviewWndCanvas *m_canvas;
    wxComboBox *m_zoomCtrl;
    wxStaticText *m_info;

    void OnSize(wxSizeEvent& event);
    void OnZoomChanged(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};

} // namespace

#endif // BSCHORDSPREVIEW_H
