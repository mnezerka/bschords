#ifndef BSCHORDSPREVIEW_H
#define BSCHORDSPREVIEW_H

#include <wx/scrolwin.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>

class BSChordsPreviewCanvas : public wxScrolledWindow
{
	public:
		BSChordsPreviewCanvas(wxWindow *parent, wxRichTextCtrl *sourceCtrl);
		virtual ~BSChordsPreviewCanvas();
		virtual void OnDraw(wxDC& dc);
		void setZoom(float zoom);
		wxCoord getDeviceX(int numMM);
		wxCoord getDeviceY(int numMM);
	protected:
	private:
		wxRichTextCtrl *m_sourceCtrl;
		//wxStyledTextCtrl *m_sourceCtrl;
		float m_zoom;
		wxSize m_screenPPI;
};

class BSChordsPreview : public wxWindow
{
	public:
		BSChordsPreview(wxWindow *parent, wxRichTextCtrl *sourceCtrl);
		virtual ~BSChordsPreview();
		void setZoom(float zoom);

	private:
		BSChordsPreviewCanvas *m_canvas;
		wxComboBox *m_zoomCtrl;

		void OnSize(wxSizeEvent& event);
		void OnZoomChanged(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};


#endif // BSCHORDSPREVIEW_H
