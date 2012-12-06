#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/richtext/richtextbuffer.h>
#include "bschordsPreview.h"
#include "bschordpro.h"

// http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#std::string_to_wxString

#define BSCHP_OFFSET_Y 10
#define BSCHP_OFFSET_X 10
#define BSCHP_LINE_OFFSET 10
#define BSCHP_CHORD_OFFSET 3


class BSChordProDCPainter : public BSChordProEventHandler
{
    public:
        BSChordProDCPainter(wxDC& dc);
		virtual void onLineBegin();
        virtual void onLineEnd();
		virtual void onCommand(const std::string& command);
		virtual void onChord(const std::string& chord);
		virtual void onText(const std::string& text);
    private:
        wxDC& m_dc;
        int m_posY;
        int m_posX;
        int m_posXChord;
        int m_eMHeight;

	    //string m_chordBuffer;
        //string m_textBuffer;
};

BSChordProDCPainter::BSChordProDCPainter(wxDC& dc)
    : m_dc(dc), m_posY(0), m_posX(0), m_posXChord(0), m_eMHeight(0)
{
    wxSize eMSize = m_dc.GetTextExtent(_("M"));
    m_eMHeight = eMSize.GetHeight();
};

void BSChordProDCPainter::onText(const std::string& text)
{
    //convert text to wxString;
    wxString wxtxt(text.c_str(), wxConvUTF8);

    wxSize textSize = m_dc.GetTextExtent(wxtxt);

    m_dc.DrawText(wxtxt, m_posX, m_posY + BSCHP_OFFSET_Y);

    m_posX += textSize.GetWidth();
    if (m_posX > m_posXChord)
        m_posXChord = m_posX;
}

void BSChordProDCPainter::onLineBegin() { m_posX = BSCHP_OFFSET_X; };
void BSChordProDCPainter::onLineEnd() { m_posY += 15; };
void BSChordProDCPainter::onCommand(const std::string& command) { };

void BSChordProDCPainter::onChord(const std::string& chord)
{
    //convert text to wxString;
    wxString wxtxt(chord.c_str(), wxConvUTF8);
    wxSize textSize = m_dc.GetTextExtent(wxtxt);

    m_dc.DrawText(wxtxt, m_posX, m_posY + BSCHP_OFFSET_Y - m_eMHeight - BSCHP_CHORD_OFFSET);

    m_posXChord += textSize.GetWidth();

};

/////////////////////////////////////////////////////////////////////////////////////////

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
	//dc.SetPen(*wxBLACK_PEN);
	//dc.DrawLine(0, 0, 100, 100);

	//wxRichTextBuffer &textBuffer = m_sourceCtrl->GetBuffer();

    BSChordProDCPainter y(dc);
	//x.onLine("ahoj");

	BSChordProParser p(&y);

	//p.parse("0123456789012345678\nand this is second line.\n\rAnd this is third line\r\r\rAnd last one");
	//       012345678901234567 89 01234567890
	//p.parse("{title: Song1}xxx\n \nmisa[]nez");
	//p.parse("{title: Wild Horses}\n[Hm]Childwood [G]living\n[Hm]Is easy to [G]do");

	//p.parse("All that you [Am]wanted\nis easy [D][G][A] to [C]do.");

	int lines = m_sourceCtrl->GetNumberOfLines();

    wxString line;
    for (int i = 0; i < lines; i++)
    {
        line = m_sourceCtrl->GetLineText(i);

        p.parse(std::string(line.mb_str()));

        //dc.DrawText(line, 10, i * 15 + 20);
    }
	//wxString title = wxString::Format(_("Number of lines: %d"), lines);
	//dc.DrawText(title, 10, 10);

}
