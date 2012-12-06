#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/richtext/richtextbuffer.h>
#include "bschordsPreview.h"



bschordsPreview::bschordsPreview(wxWindow *parent, wxRichTextCtrl *sourceCtrl)
	: wxScrolledWindow(parent)
{
	m_sourceCtrl = sourceCtrl;
}

bschordsPreview::~bschordsPreview()
{
	//dtor
}

void bschordsPreview::OnDraw(wxDC& dc)
{
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawLine(0, 0, 100, 100);

	//wxRichTextBuffer textBuffer = m_sourceCtrl->GetBuffer();

	int lines = m_sourceCtrl->GetNumberOfLines();

	wxString title = wxString::Format(_("Number of lines: %d"), lines);

	dc.DrawText(title, 10, 10);

}
