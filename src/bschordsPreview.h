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
	protected:
	private:
		wxRichTextCtrl *m_sourceCtrl;
		//wxStyledTextCtrl *m_sourceCtrl;
};

#endif // BSCHORDSPREVIEW_H
