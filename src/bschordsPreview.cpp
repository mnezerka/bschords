#include <wx/dc.h>
#include <wx/dir.h>
#include <wx/richtext/richtextbuffer.h>
#include <wx/pen.h>
#include <vector>
#include "bschordsPreview.h"
#include "bschordpro.h"

// http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#std::string_to_wxString


#define BSCHP_OFFSET_X 30
#define BSCHP_OFFSET_Y 30
#define BSCHP_LINE_SPACING 30
#define BSCHP_CHORD_OFFSET 3

// page size
#define BSCHP_X 210
#define BSCHP_Y 297

int mm2Px(const int n, const int zoom)
{
    return n * zoom;
}

struct BSLineItem
{
    wxString *txt;
    int posX;
};

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
        bool m_hasChords;
        std::vector<BSLineItem*> m_chordLine;
        std::vector<BSLineItem*> m_textLine;
        // this macro declares and partly implements MyList class

	    //string m_chordBuffer;
        //string m_textBuffer;
};

BSChordProDCPainter::BSChordProDCPainter(wxDC& dc)
    : m_dc(dc), m_posY(0), m_posX(0), m_posXChord(0), m_eMHeight(0)
{
    // 1 logical unit = 1 mm
    //m_dc.SetFont();'

    // set map mode to milimeters
    m_dc.SetMapMode(wxMM_METRIC);

    // set default zoom
    m_dc.SetUserScale(0.5, 0.5);

    m_dc.SetFont(wxFont(6, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Arial")));

    // white background
    m_dc.DrawRectangle(0, 0, BSCHP_X, BSCHP_Y);
    //wxPen pen(*wxGREY_PEN, 1); // red pen of width 1
    //dc.SetPen(pen);
    //m_dc.SetPen(wxGREY_PEN);

    // draw border line
    m_dc.DrawRectangle(BSCHP_OFFSET_X, BSCHP_OFFSET_Y, BSCHP_X - 2 * BSCHP_OFFSET_X, BSCHP_Y - 2 * BSCHP_OFFSET_Y);

    wxSize eMSize = m_dc.GetTextExtent(_("M"));

    m_eMHeight = eMSize.GetHeight();

    m_posY = BSCHP_OFFSET_Y;

//    m_dc.SetFontSize()
};

void BSChordProDCPainter::onText(const std::string& text)
{
    BSLineItem *item = new BSLineItem();
    item->txt = new wxString(text.c_str(), wxConvUTF8);
    wxSize textSize = m_dc.GetTextExtent(*item->txt);
    item->posX = m_posX;

    m_textLine.push_back(item);

    //convert text to wxString;
    //wxString wxtxt(text.c_str(), wxConvUTF8);
    //wxSize textSize = m_dc.GetTextExtent(wxtxt);

    //m_dc.DrawText(wxtxt, m_posX, m_posY + BSCHP_OFFSET_Y);

    m_posX += textSize.GetWidth();

    if (m_posX > m_posXChord)
        m_posXChord = m_posX;
}

void BSChordProDCPainter::onChord(const std::string& chord)
{
    BSLineItem *item = new BSLineItem();
    item->txt = new wxString(chord.c_str(), wxConvUTF8);
    wxSize textSize = m_dc.GetTextExtent(*item->txt);
    item->posX = m_posX;

    m_chordLine.push_back(item);
    //m_dc.DrawText(wxtxt, m_posX, m_posY + BSCHP_OFFSET_Y - m_eMHeight - BSCHP_CHORD_OFFSET);

    m_posXChord += textSize.GetWidth();

};

void BSChordProDCPainter::onLineBegin() { m_posX = BSCHP_OFFSET_X; m_hasChords = false; };
void BSChordProDCPainter::onLineEnd()
{
    //std::cout << "OnLineEnd, m_posY is " << m_posY << endl;

    if (m_chordLine.size() > 0)
    {
        for (int i = 0; i < m_chordLine.size(); i++)
            m_dc.DrawText(*m_chordLine[i]->txt, m_chordLine[i]->posX, m_posY);

        m_posY += m_eMHeight + BSCHP_CHORD_OFFSET;

        while (!m_chordLine.empty())
            m_chordLine.pop_back();
    }

    for (int i = 0; i < m_textLine.size(); i++)
        m_dc.DrawText(*m_textLine[i]->txt, m_textLine[i]->posX, m_posY);

    while (!m_textLine.empty())
        m_textLine.pop_back();


    m_posY += BSCHP_LINE_SPACING;


};
void BSChordProDCPainter::onCommand(const std::string& command) { };

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

    std::cout << "OnDraw" << endl;

    BSChordProDCPainter y(dc);
	//x.onLine("ahoj");

	BSChordProParser p(&y);

	//p.parse("0123456789012345678\nand this is second line.\n\rAnd this is third line\r\r\rAnd last one");
	//       012345678901234567 89 01234567890
	//p.parse("{title: Song1}xxx\n \nmisa[]nez");
	//p.parse("{title: Wild Horses}\n[Hm]Childwood [G]living\n[Hm]Is easy to [G]do");

	//p.parse("All that you [Am]wanted\nis easy [D][G][A] to [C]do.");

	int lines = m_sourceCtrl->GetNumberOfLines();

    wxString text;
    for (int i = 0; i < lines; i++)
    {
        text += m_sourceCtrl->GetLineText(i);
        text += wxT('\n');
    }
    p.parse(std::string(text.mb_str()));

        //dc.DrawText(line, 10, i * 15 + 20);


	//wxString title = wxString::Format(_("Number of lines: %d"), lines);
	//dc.DrawText(title, 10, 10);

}
