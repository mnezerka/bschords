#include "bschordsApp.h"
#include "bschordsdcpainter.h"

BSChordsDCPainter::BSChordsDCPainter(wxDC& dc, float scale)
	: m_ss(NULL), m_dc(dc), m_posY(0), m_posX(0), m_posXChord(0), m_eMHeight(0), m_section(SECTION_NONE),
	m_verseCounter(1), m_isLineEmpty(true), m_scale(scale), m_curBlock(NULL)
{
	//m_dcPPI = dc.GetPPI();
	//cout << "DC Painter PPI: (" << m_dcPPI.GetWidth() << "x" << m_dcPPI.GetHeight() << ")" << endl;

	cout << "DC Painter Scale: " << scale << endl;

	m_ss = &wxGetApp().m_styleSheet;

	// draw white (paper) background
	m_dc.DrawRectangle(0, 0, getDeviceX(m_ss->m_pageSize.GetWidth()) , getDeviceX(m_ss->m_pageSize.GetHeight()));

	// draw gray border to see typesetting area (margins)
	wxPen pen(wxColour(200, 200, 200), 0.1); // red pen of width 1
	m_dc.SetPen(pen);

	// draw border line
	m_dc.DrawRectangle(
		getDeviceX(m_ss->m_marginLeft),
		getDeviceY(m_ss->m_marginTop),
		getDeviceX(m_ss->m_pageSize.GetWidth() - m_ss->m_marginLeft - m_ss->m_marginRight),
		getDeviceY(m_ss->m_pageSize.GetHeight() - m_ss->m_marginTop - m_ss->m_marginBottom));

	// set default settings
	m_posY = getDeviceY(m_ss->m_marginTop);

	//wxPen pen2(wxColour(0, 0, 200), 0.1); // red pen of width 1
	//m_dc.SetPen(pen2);
}

BSChordsDCPainter::~BSChordsDCPainter()
{
	//dtor
}

wxCoord BSChordsDCPainter::getDeviceX(int numMM)
{
	return numMM * m_scale;

	//return numMM / MM_PER_IN * m_dcPPI.GetWidth();
}

wxCoord BSChordsDCPainter::getDeviceY(int numMM)
{
	return numMM * m_scale;
	//return numMM / MM_PER_IN * m_dcPPI.GetHeight();
}

void BSChordsDCPainter::onBegin()
{
	BSPage m_page;
	m_curBlock = NULL;

	// TODO: create page structure
}

void BSChordsDCPainter::onEnd()
{
	// TODO: draw page structure
}

void BSChordsDCPainter::onText(const wstring& text)
{
	// create new text line item
	BSLineItem *item = new BSLineItem();
	m_isLineEmpty = false;
	item->txt = new wxString(text);
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
	wxSize textSize = m_dc.GetTextExtent(*item->txt);
	item->width = textSize.GetWidth();
	item->posX = m_posX;
	m_textLine.push_back(item);

	// update text line position
	m_posX += textSize.GetWidth();

	// if we are in front of chord line position, update it
	if (m_posX > m_posXChord)
		m_posXChord = m_posX;
}

void BSChordsDCPainter::onChord(const wstring& chord)
{
	// check if we are not overlapping already typeset chord item
	if (m_posXChord > m_posX)
		m_posX = m_posXChord;

	// create new chord line item
	BSLineItem *item = new BSLineItem();
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
	// append one space for separation from next typeset chord
	item->txt = new wxString(chord);
	item->txt->Append(_(" "));
	wxSize textSize = m_dc.GetTextExtent(*item->txt);
	item->width = textSize.GetWidth();
	item->posX = m_posX;
	m_chordLine.push_back(item);

	// update chord line position
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
	if (command == L"subtitle")
	{


	}
	//cout << "drawing command" << endl;
}

void BSChordsDCPainter::onLineBegin()
{
	// default line settings
	m_posX = m_posXChord = getDeviceX(m_ss->m_marginLeft);
	m_hasChords = false;
	m_isLineEmpty = true;
}

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
			// else do nothing since one empty line has no role in song structure
			break;
		case SECTION_VERSE:
			if (m_isLineEmpty)
			{
				m_section = SECTION_NONE;
			}
			// else stay in verse
		case SECTION_CHORUS:
			// chorus could be closed only by special command
			// so always stay in chorus
			break;
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
}

