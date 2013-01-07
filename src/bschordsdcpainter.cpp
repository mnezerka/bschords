#include "bschordsApp.h"
#include "bschordsdcpainter.h"

BSChordsDCPainter::BSChordsDCPainter(wxDC& dc)
	: m_ss(NULL), m_dc(dc), m_posY(0), m_posX(0), m_posXChord(0), m_eMHeight(0), m_section(SECTION_NONE), m_verseCounter(1), m_isLineEmpty(true)
{
	m_dcPPI = dc.GetPPI();

	m_ss = &wxGetApp().m_styleSheet;

	cout << "DC Painter PPI: (" << m_dcPPI.GetWidth() << "x" << m_dcPPI.GetHeight() << ")" << endl;

	// white background
	m_dc.DrawRectangle(0, 0, getDeviceX(m_ss->m_pageSize.GetWidth()) , getDeviceX(m_ss->m_pageSize.GetHeight()));
	wxPen pen(wxColour(200, 200, 200), 0.1); // red pen of width 1
	m_dc.SetPen(pen);
	//m_dc.SetPen(*wxGREY_PEN);

	// draw border line
	m_dc.DrawRectangle(
		getDeviceX(m_ss->m_marginLeft),
		getDeviceY(m_ss->m_marginTop),
		getDeviceX(m_ss->m_pageSize.GetWidth() - m_ss->m_marginLeft - m_ss->m_marginRight),
		getDeviceY(m_ss->m_pageSize.GetHeight() - m_ss->m_marginTop - m_ss->m_marginBottom));

	m_posY = getDeviceY(m_ss->m_marginTop);

	wxPen pen2(wxColour(0, 0, 200), 0.1); // red pen of width 1
	m_dc.SetPen(pen2);
}

BSChordsDCPainter::~BSChordsDCPainter()
{
	//dtor
}

wxCoord BSChordsDCPainter::getDeviceX(int numMM)
{
	return numMM / MM_PER_IN * m_dcPPI.GetWidth();
}

wxCoord BSChordsDCPainter::getDeviceY(int numMM)
{
	return numMM / MM_PER_IN * m_dcPPI.GetHeight();
}

void BSChordsDCPainter::onText(const wstring& text)
{
	BSLineItem *item = new BSLineItem();

	m_isLineEmpty = false;

	//item->txt = new wxString(text.c_str(), wxConvUTF8);
	item->txt = new wxString(text);

	//wcout << L"Converted >" << item->txt->wc_str() << L"<" << endl;
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
	wxSize textSize = m_dc.GetTextExtent(*item->txt);
	item->width = textSize.GetWidth();
	item->posX = m_posX;

	//wcout << L"On text >" << text << L"<" << text.size() << " width: " << textSize.GetWidth() << " posx: " << m_posX << endl;

	m_textLine.push_back(item);

	m_posX += textSize.GetWidth();

	if (m_posX > m_posXChord)
		m_posXChord = m_posX;
}

void BSChordsDCPainter::onChord(const wstring& chord)
{
	BSLineItem *item = new BSLineItem();
	//item->txt = new wxString(chord.c_str(), wxConvUTF8);
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
	item->txt = new wxString(chord);
	wxSize textSize = m_dc.GetTextExtent(*item->txt);
	item->width = textSize.GetWidth();
	item->posX = m_posX;

	// TODO: Shift text if chord takes too much space

	m_chordLine.push_back(item);

	m_posXChord += textSize.GetWidth();
}

void BSChordsDCPainter::onCommand(const wstring& command, const wstring& value)
{
	//m_dc.SetFont(*fontTitle_);
	if (command == L"title")
    {
        m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TITLE]);
		m_dc.DrawText(value, getDeviceX(m_ss->m_marginLeft), m_posY);
		m_posY += m_dc.GetCharHeight() + m_ss->m_lineSpacing;
	}
	//cout << "drawing command" << endl;
}

void BSChordsDCPainter::onLineBegin() {
	m_posX = getDeviceX(m_ss->m_marginLeft);
	m_hasChords = false;
	m_isLineEmpty = true;
};
void BSChordsDCPainter::onLineEnd()
{
	//std::cout << "OnLineEnd, m_posY is " << m_posY << endl;
	int lineOffset = 0;
	wxString linePrefix;
	switch(m_section)
	{
		case SECTION_NONE:
			if (!m_isLineEmpty)
			{
				linePrefix << m_verseCounter << _(". ");
				m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
				wxSize textSize = m_dc.GetTextExtent(linePrefix);
				lineOffset = textSize.GetWidth();
				m_section = SECTION_VERSE;
				m_verseCounter++;
			}
			break;
		case SECTION_VERSE:
			if (m_isLineEmpty)
			{
				m_section = SECTION_NONE;
			}
	}

	// typeset chordline
	if (m_chordLine.size() > 0)
		{
			//wcout << L"drawing chord line" << endl;
            m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
			for (size_t i = 0; i < m_chordLine.size(); i++)
				m_dc.DrawText(*m_chordLine[i]->txt, m_chordLine[i]->posX + lineOffset, m_posY);

			m_posY += m_dc.GetCharHeight() + m_ss->m_chordLineSpacing;

			while (!m_chordLine.empty())
				m_chordLine.pop_back();
		}

	// typeset text line
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
    if (linePrefix.Length() > 0)
		m_dc.DrawText(linePrefix, getDeviceX(m_ss->m_marginLeft), m_posY);
	for (size_t i = 0; i < m_textLine.size(); i++)
		{
			//wcout << L"drawing text: " <<
			m_dc.DrawText(*m_textLine[i]->txt, m_textLine[i]->posX + lineOffset, m_posY);

			//m_dc.DrawLine(m_textLine[i]->posX, m_posY, m_textLine[i]->posX + m_textLine[i]->width, m_posY + m_dc.GetCharHeight());
			//m_dc.DrawLine(m_textLine[i]->posX, m_posY, m_textLine[i]->posX, m_posY + m_dc.GetCharHeight());
		}


	while (!m_textLine.empty())
		m_textLine.pop_back();

	m_posY += m_dc.GetCharHeight() + m_ss->m_lineSpacing;
};
