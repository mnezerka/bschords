#include "bschordsApp.h"
#include "bschordsdcpainter.h"

using namespace bschords;

// ------- TSetBlocks ----------------------------------------------------------------

void TSetBlockText::draw(const wxPoint pos)
{
	wxCoord posY = pos.y;

	wxRect r = getBoundingRect();
	m_painter->m_dc.DrawRectangle(pos.x, pos.y, r.GetWidth(), r.GetHeight());

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		TSetLine *line = m_lines[lineIx];

		// typeset chord line (chord items)
		m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
		for (size_t i = 0; i < line->m_chordItems.size(); i++)
			m_painter->m_dc.DrawText(*line->m_chordItems[i]->txt, pos.x + line->m_chordItems[i]->m_bRect.GetLeft(), posY);
		if (line->m_chordItems.size() > 0)
			posY += m_painter->m_dc.GetCharHeight();

		// typeset text line (text items)
		m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
		for (size_t i = 0; i < line->m_textItems.size(); i++)
			m_painter->m_dc.DrawText(*line->m_textItems[i]->txt, pos.x + line->m_textItems[i]->m_bRect.GetLeft(), posY);
		posY += m_painter->m_dc.GetCharHeight();
	}
}

wxRect TSetBlockText::getBoundingRect()
{
	wxRect result;

	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
	wxCoord lineHeightChord = m_painter->m_dc.GetCharHeight();
	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
	wxCoord lineHeightText = m_painter->m_dc.GetCharHeight();


	wxCoord maxWidth = 0;
	wxCoord height = 0;

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		TSetLine *line = m_lines[lineIx];

		// -------- compute width
		// get last chord from chord line
		if (line->m_chordItems.size() > 0)
		{
			TSetLineItem *lineItem = line->m_chordItems.back();
			if (lineItem->m_bRect.GetRight() > maxWidth)
				maxWidth = lineItem->m_bRect.GetRight();
		}

		// get last text item from text line
		if (line->m_textItems.size() > 0)
		{
			TSetLineItem *lineItem = line->m_textItems.back();
			if (lineItem->m_bRect.GetRight() > maxWidth)
				maxWidth = lineItem->m_bRect.GetRight();
		}

		// -------- compute height
		if (line->m_chordItems.size() > 0)
			height += lineHeightChord;
		height += lineHeightText;
	}
	result.SetWidth(maxWidth);
	result.SetHeight(height);

	return result;
}


void TSetBlockTitle::draw(const wxPoint pos)
{
	// typeset chord line (chord items)
	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TITLE]);
	m_painter->m_dc.DrawText(*m_title->txt, pos.x, pos.y);
}

wxRect TSetBlockTitle::getBoundingRect()
{
	return m_title->m_bRect;
}

// ------- TSetDCPainter -------------------------------------------------------------

TSetDCPainter::TSetDCPainter(wxDC& dc, float scale)
	: m_ss(NULL), m_dc(dc), m_posX(0), m_posXChord(0), m_eMHeight(0), m_section(SECTION_NONE),
	m_verseCounter(1), m_isLineEmpty(true), m_scale(scale), m_curBlock(NULL)
{
	//m_dcPPI = dc.GetPPI();
	//cout << "DC Painter PPI: (" << m_dcPPI.GetWidth() << "x" << m_dcPPI.GetHeight() << ")" << endl;

	cout << "DC Painter Scale: " << scale << endl;

	m_ss = &wxGetApp().m_styleSheet;

	// draw white (paper) background
	m_dc.DrawRectangle(0, 0, getDeviceX(m_ss->m_pageSize.GetWidth()) , getDeviceX(m_ss->m_pageSize.GetHeight()));

	m_dc.SetBackgroundMode(wxTRANSPARENT);

	// draw gray border to see typesetting area (margins)
	wxPen pen(wxColour(200, 200, 200), 0.1); // red pen of width 1
	m_dc.SetPen(pen);

	// draw border line
	m_dc.DrawRectangle(
		getDeviceX(m_ss->m_marginLeft),
		getDeviceY(m_ss->m_marginTop),
		getDeviceX(m_ss->m_pageSize.GetWidth() - m_ss->m_marginLeft - m_ss->m_marginRight),
		getDeviceY(m_ss->m_pageSize.GetHeight() - m_ss->m_marginTop - m_ss->m_marginBottom));
}

TSetDCPainter::~TSetDCPainter()
{
	while (!m_pages.empty())
	{
		delete m_pages.back();
		m_pages.pop_back();
	}
}

wxCoord TSetDCPainter::getDeviceX(int numMM)
{
	return numMM * m_scale;

	//return numMM / MM_PER_IN * m_dcPPI.GetWidth();
}

wxCoord TSetDCPainter::getDeviceY(int numMM)
{
	return numMM * m_scale;
	//return numMM / MM_PER_IN * m_dcPPI.GetHeight();
}

void TSetDCPainter::onBegin()
{
	cout << "OnBegin" << endl;
	// create page structure
	m_curPage = new TSetPage();
}

void TSetDCPainter::onEnd()
{
	cout << "OnBegin" << endl;

	wxPoint pos;
	pos.y = getDeviceY(m_ss->m_marginTop);
	pos.x = getDeviceX(m_ss->m_marginLeft);

	// loop through current page blocks
	for (size_t blockIx = 0; blockIx < m_curPage->m_blocks.size(); blockIx++)
	{
		TSetBlock *block = m_curPage->m_blocks[blockIx];

		block->draw(pos);
		pos.y += block->getBoundingRect().GetHeight();
	}
}

void TSetDCPainter::onText(const wstring& text)
{
	// create new text line item
	TSetLineItem *item = new TSetLineItem();
	m_isLineEmpty = false;
	item->txt = new wxString(text);
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
	item->m_bRect.SetSize(m_dc.GetTextExtent(*item->txt));
	item->m_bRect.SetPosition(wxPoint(m_posX, 0));
	m_curLine->m_textItems.push_back(item);

	// update text line position
	m_posX += item->m_bRect.GetWidth();

	// if we are in front of chord line position, update it
	if (m_posX > m_posXChord)
		m_posXChord = m_posX;
}

void TSetDCPainter::onChord(const wstring& chord)
{
	// check if we are not overlapping already typeset chord item
	if (m_posXChord > m_posX)
		m_posX = m_posXChord;

	// create new chord line item
	TSetLineItem *item = new TSetLineItem();
    m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
	// append one space for separation from next typeset chord
	item->txt = new wxString(chord);
	item->txt->Append(_(" "));
	item->m_bRect.SetSize(m_dc.GetTextExtent(*item->txt));
	item->m_bRect.SetPosition(wxPoint(m_posXChord, 0));
	m_curLine->m_chordItems.push_back(item);

	// update chord line position
	m_posXChord += item->m_bRect.GetWidth();
}

void TSetDCPainter::onCommand(const wstring& command, const wstring& value)
{
	//m_dc.SetFont(*fontTitle_);
	if (command == L"title")
    {
		TSetLineItem *item = new TSetLineItem();
		m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TITLE]);
		item->txt = new wxString(value);
		item->m_bRect.SetSize(m_dc.GetTextExtent(*item->txt));
		item->m_bRect.SetPosition(wxPoint(0, 0));

		TSetBlockTitle *block = new TSetBlockTitle(this);
		block->m_title = item;
		m_curPage->m_blocks.push_back(block);
		m_curBlock = NULL;
	}
}

void TSetDCPainter::onLineBegin()
{
	cout << "OnLineBegin" << endl;

	// default line settings
	m_posX = m_posXChord = 0;
	m_hasChords = false;
	m_isLineEmpty = true;
	m_curLine = new TSetLine();
}

void TSetDCPainter::onLineEnd()
{
	cout << "OnLineEnd" << endl;

	// check if we are inside some block
	if (m_curBlock == NULL)
	{
		// start new block
		if (m_isLineEmpty)
		{
			// create new hspace (empty line) block and close it
			m_curBlock = new TSetBlockHSpace(this);
			m_curPage->m_blocks.push_back(m_curBlock);
			m_curBlock = NULL;
		}
		else
		{
			// create new text block and add current line
			TSetBlockText *block = new TSetBlockText(this);
			block->m_lines.push_back(m_curLine);
			m_curPage->m_blocks.push_back(block);
			m_curBlock = block;
		}
	}
	// else we are inside some block
	else
	{
		// we are inside some block
		switch (m_curBlock->getType())
		{
			case TSetBlock::BLTYPE_TEXT:
				// close block if line is empty
				if (m_isLineEmpty)
				{
					m_curPage->m_blocks.push_back(new TSetBlockHSpace(this));
					m_curBlock = NULL;
				}
				// line is not empty -> add it to the block contents
				else
					static_cast<TSetBlockText*>(m_curBlock)->m_lines.push_back(m_curLine);
				break;

			case TSetBlock::BLTYPE_CHORUS:
			case TSetBlock::BLTYPE_HSPACE:
			case TSetBlock::BLTYPE_NONE:
				break;
		}
	}
}

