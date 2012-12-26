#ifndef BSCHORDSPREVIEW_H
#define BSCHORDSPREVIEW_H

#include <wx/scrolwin.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>

class bschordsPreview : public wxScrolledWindow
{
	public:
		bschordsPreview(wxWindow *parent, wxRichTextCtrl *sourceCtrl);
		//bschordsPreview(wxWindow *parent, wxStyledTextCtrl *sourceCtrl);
		virtual ~bschordsPreview();
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

#endif // BSCHORDSPREVIEW_H
