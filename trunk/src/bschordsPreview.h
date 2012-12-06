#ifndef BSCHORDSPREVIEW_H
#define BSCHORDSPREVIEW_H

#include <wx/scrolwin.h>
#include <wx/richtext/richtextctrl.h>


class bschordsPreview : public wxScrolledWindow
{
	public:
		bschordsPreview(wxWindow *parent, wxRichTextCtrl *sourceCtrl);
		virtual ~bschordsPreview();
		virtual void OnDraw(wxDC& dc);
	protected:
	private:
		wxRichTextCtrl *m_sourceCtrl;
};

#endif // BSCHORDSPREVIEW_H
