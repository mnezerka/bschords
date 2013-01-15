#include "bschordsApp.h"
#include "bschordsdcpainter.h"

using namespace bschords;

// ------- TSetBlock  --------------------------------------------------------

void TSetBlock::drawBoundingRect(const wxPoint pos)
{
	// select color according to block type
	wxColour rectColor;
	switch (getType())
	{
		case TSetBlock::BLTYPE_TITLE: rectColor.Set(0, 255, 0); break;
		case TSetBlock::BLTYPE_HSPACE: rectColor.Set(255, 100, 100); break;
		case TSetBlock::BLTYPE_CHORUS: rectColor.Set(255, 128, 0); break;
		case TSetBlock::BLTYPE_VERSE: rectColor.Set(50, 50, 255); break;
		default:
			rectColor.Set(128, 128, 128);
	}

	// draw bounding rect
	wxPen pen(rectColor, 0.1);
	m_painter->m_dc.SetPen(pen);
	wxRect r = getBoundingRect();
	r.SetPosition(pos);
	m_painter->m_dc.DrawRectangle(r);
	m_painter->m_dc.DrawLine(r.GetPosition(), r.GetPosition() + r.GetSize());
}

// ------- TSetBlockHSpace  --------------------------------------------------------

wxRect TSetBlockHSpace::getBoundingRect()
{
	wxRect result;

	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
	result.SetHeight(m_painter->m_dc.GetCharHeight());
	result.SetWidth(60);

	return result;
}

// ------- TSetBlockText  ----------------------------------------------------------
bool TSetBlockText::hasChords()
{
	// check if there are at least one line with chord item
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
		if (m_lines[lineIx]->m_chordItems.size() > 0)
			return true;
	return false;
}

void TSetBlockText::draw(wxPoint pos)
{
	wxCoord posY = pos.y;

	// add block indent for chorus
	if (getType() == TSetBlock::BLTYPE_CHORUS)
		pos.x += m_painter->getDeviceX(m_painter->m_ss->m_indentChorus);

	// verse numbering shifts line right a little
	if (hasNumbering())
	{
		wxSize s = m_painter->m_dc.GetTextExtent(wxT("0. "));
		pos.x += s.GetWidth();
	}

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		TSetLine *line = m_lines[lineIx];
		size_t lineOffset = 0;

		// line spacing (computed for all lines except for first one)
		if (lineIx > 0)
			posY += m_painter->getDeviceY(m_painter->m_ss->m_lineSpacing);

		// typeset chord line (chord items)
		if (m_painter->m_ss->m_showChords)
		{
			m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_CHORDS]);
			for (size_t i = 0; i < line->m_chordItems.size(); i++)
				m_painter->m_dc.DrawText(*line->m_chordItems[i]->txt, pos.x + lineOffset + line->m_chordItems[i]->m_bRect.GetLeft(), posY);
			if ((hasChords() && m_painter->m_ss->m_equalLineHeights) || (line->m_chordItems.size() > 0))
			{
				posY += m_painter->m_dc.GetCharHeight();
				posY += m_painter->getDeviceY(m_painter->m_ss->m_chordLineSpacing);
			}
		}

		// verse numbering shifts line right a little
		if (hasNumbering() && lineIx == 0)
		{
			m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
			wxString strNum = wxString::Format(wxT("%d. "), getPos() + 1);
			wxSize s = m_painter->m_dc.GetTextExtent(strNum);
			m_painter->m_dc.DrawText(strNum, pos.x - s.GetWidth(), posY);
		}

		// typeset text line (text items)
		m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TEXT]);
		for (size_t i = 0; i < line->m_textItems.size(); i++)
			m_painter->m_dc.DrawText(*line->m_textItems[i]->txt, pos.x + lineOffset + line->m_textItems[i]->m_bRect.GetLeft(), posY);
		if (line->m_textItems.size() > 0)
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
		if (m_painter->m_ss->m_showChords)
		{
			if (line->m_chordItems.size() > 0)
			{
				TSetLineItem *lineItem = line->m_chordItems.back();
				if (lineItem->m_bRect.GetRight() > maxWidth)
					maxWidth = lineItem->m_bRect.GetRight();
			}
		}

		// get last text item from text line
		if (line->m_textItems.size() > 0)
		{
			TSetLineItem *lineItem = line->m_textItems.back();
			if (lineItem->m_bRect.GetRight() > maxWidth)
				maxWidth = lineItem->m_bRect.GetRight();
		}

		// -------- compute height
		// line spacing (computed for all lines except for first one)
		if (lineIx > 0)
			height += m_painter->getDeviceY(m_painter->m_ss->m_lineSpacing);

		// height of chord line
		if (m_painter->m_ss->m_showChords)
		{
			if ((hasChords() && m_painter->m_ss->m_equalLineHeights) || (line->m_chordItems.size() > 0))
			{
				height += lineHeightChord;
				height += m_painter->getDeviceY(m_painter->m_ss->m_chordLineSpacing);
			}
		}
		// height of text line
		if (line->m_textItems.size() > 0)
			height += lineHeightText;
	}

	// add block indent for chorus
	if (getType() == TSetBlock::BLTYPE_CHORUS)
	{
		maxWidth += m_painter->getDeviceX(m_painter->m_ss->m_indentChorus);
	}
	// verse numbering shifts line right a little
	if (TSetBlock::BLTYPE_VERSE && m_painter->m_ss->m_verseNumbering)
	{
		wxSize s = m_painter->m_dc.GetTextExtent(wxT("0."));
		maxWidth += s.GetWidth();
	}

	result.SetWidth(maxWidth);
	result.SetHeight(height);

	return result;
}

// ------- TSetBlockVerse  ----------------------------------------------------------

bool TSetBlockVerse::hasNumbering()
{
	return m_painter->m_ss->m_verseNumbering;
}

// ------- TSetBlockTitle  ----------------------------------------------------------

void TSetBlockTitle::draw(wxPoint pos)
{
	// typeset chord line (chord items)
	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TITLE]);
	m_painter->m_dc.DrawText(*m_title->txt, pos.x, pos.y);
}

wxRect TSetBlockTitle::getBoundingRect()
{
	return m_title->m_bRect;
}

// ------- TSetBlockTab  ----------------------------------------------------------

bool TSetBlockTab::isVisible()
{
	return m_painter->m_ss->m_showTabs;
}

void TSetBlockTab::draw(wxPoint pos)
{

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		wxString *line = m_lines[lineIx];
		m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TAB]);
		m_painter->m_dc.DrawText(*line, pos);
		pos.y += m_painter->m_dc.GetCharHeight();
	}
}

wxRect TSetBlockTab::getBoundingRect()
{
	wxRect result;

	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TAB]);
	wxCoord lineHeight = m_painter->m_dc.GetCharHeight();

	wxCoord maxWidth = 0;
	wxCoord height = m_lines.size() * lineHeight;

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		wxString *line = m_lines[lineIx];
		wxSize lineSize = m_painter->m_dc.GetTextExtent(*line);
		if (lineSize.GetWidth() > maxWidth)
			maxWidth = lineSize.GetWidth();
	}

	result.SetWidth(maxWidth);
	result.SetHeight(height);

	return result;
}

// ------- TSetDCPainter -------------------------------------------------------------

TSetDCPainter::TSetDCPainter(wxDC& dc, float scale)
	: m_drawTsetBlocks(false), m_drawTsetMargins(false), m_ss(NULL), m_dc(dc), m_posX(0), m_posXChord(0),
	m_eMHeight(0),m_verseCounter(0), m_section(SECTION_NONE), m_isLineEmpty(true), m_scale(scale), m_curBlock(NULL)
{
	//m_dcPPI = dc.GetPPI();
	//cout << "DC Painter PPI: (" << m_dcPPI.GetWidth() << "x" << m_dcPPI.GetHeight() << ")" << endl;
	//cout << "DC Painter Scale: " << scale << endl;
	m_ss = &wxGetApp().m_styleSheet;
	m_drawTsetBlocks = wxGetApp().config->Read(_("/global/show-tset-blocks"), 0l) > 0;
	m_drawTsetMargins = wxGetApp().config->Read(_("/global/show-tset-margins"), 0l) > 0;
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
	//cout << "OnBegin" << endl;
	// create page structure
	m_curPage = new TSetPage();
	m_stat.m_pages++;
}

// all drawing is done here after parsing whole content and computing dimensions
// of each block
void TSetDCPainter::onEnd()
{
	//cout << "OnBegin" << endl;


	// prepare rectangle representation of "drawable" page space (paper - margins)
	wxPoint pos(getDeviceX(m_ss->m_marginLeft), getDeviceY(m_ss->m_marginTop));
	wxRect pageRect (
		getDeviceX(m_ss->m_marginLeft),
		getDeviceY(m_ss->m_marginTop),
		getDeviceX(m_ss->m_pageSize.GetWidth() - m_ss->m_marginLeft - m_ss->m_marginRight),
		getDeviceY(m_ss->m_pageSize.GetHeight() - m_ss->m_marginTop - m_ss->m_marginBottom));

	// draw white (paper) background
	m_dc.DrawRectangle(0, 0, getDeviceX(m_ss->m_pageSize.GetWidth()) , getDeviceX(m_ss->m_pageSize.GetHeight()));
	m_dc.SetBackgroundMode(wxTRANSPARENT);

	// draw gray border to see typesetting area (margins)
	if (m_drawTsetMargins)
	{
		wxPen pen(wxColour(200, 200, 200), 0.1); // red pen of width 1
		m_dc.SetPen(pen);
		m_dc.DrawRectangle(pageRect);
	}

	int curColumn = 0;
	wxCoord columnWidth = pageRect.GetWidth() / m_ss->m_cols;
	wxRect colRect(pageRect);
	colRect.SetWidth(columnWidth);
	m_dc.SetClippingRegion(colRect);

	// loop through current page blocks
	for (size_t blockIx = 0; blockIx < m_curPage->m_blocks.size(); blockIx++)
	{
		TSetBlock *block = m_curPage->m_blocks[blockIx];
		bool clippingDetected = false;

		if (!block->isVisible())
			continue;

		wxRect blockRect = block->getBoundingRect();

		// check if there is enough horizontal space for block to be drawn
		// in current column
		if (blockRect.GetWidth() > colRect.GetWidth())
		{
			clippingDetected = true;
			std::cout << "problem with horizontal clipping found" << std::endl;
		}

		wxCoord blockHeight = blockRect.GetHeight();

		// check if we have enough space to draw block
		if (pos.y + blockHeight > pageRect.GetBottom())
		{
			//cout << "not enough space for block, pos.y: " << pos.y << " block height: " << blockHeight << endl;
			if (curColumn < m_ss->m_cols - 1)
			{
				//cout << "starting new column" << endl;
				curColumn++;
				colRect.SetLeft(colRect.GetLeft() + colRect.GetWidth());
				m_dc.DestroyClippingRegion();
				m_dc.SetClippingRegion(colRect);
				pos.x = colRect.GetLeft();
				pos.y = colRect.GetTop();
			}
			else
			{
				m_stat.m_pages++;
				std::cout << "new page is required !!!" << std::endl;
				break;
			}
		}

		// draw bounding box
		if (m_drawTsetBlocks)
			block->drawBoundingRect(pos);
		block->draw(pos);

		// draw warning
		if (clippingDetected)
		{
			// draw bounding rect
			wxPen pen(wxColor(255, 0, 0), 5);
			m_dc.SetPen(pen);
			m_dc.DrawLine(colRect.GetRight(), pos.y, colRect.GetRight(), pos.y + blockRect.GetHeight());
			m_stat.m_clippings++;
		}

		pos.y += block->getBoundingRect().GetHeight();
		//cout << "block height: " << block->getBoundingRect().GetHeight() << ", new pos.y: " << pos.y << endl;
	}
}

void TSetDCPainter::onText(const std::wstring& text)
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

void TSetDCPainter::onChord(const std::wstring& chord)
{
	// check if we are not overlapping already typeset chord item
	if (m_posXChord > m_posX)
		m_posX = m_posXChord;

	// create new chord line item
	TSetLineItem *item = new TSetLineItem();
	m_isLineEmpty = false;
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

void TSetDCPainter::onCommand(const bschordpro::CommandType command, const std::wstring& value)
{
	//m_dc.SetFont(*fontTitle_);
	if (command == bschordpro::CMD_TITLE)
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
	else if (command == bschordpro::CMD_CHORUS_START)
	{
		TSetBlockChorus *block = new TSetBlockChorus(this);
		m_curPage->m_blocks.push_back(block);
		m_curBlock = block;
	}
	else if (command == bschordpro::CMD_CHORUS_END)
	{
		// if we are still in chorus block
		if (m_curBlock->getType() == TSetBlock::BLTYPE_CHORUS)
		{
			// finish block
			m_curBlock = NULL;
		}
		// else igonre end of chorus command
	}
	else if (command == bschordpro::CMD_TAB_START)
	{
		TSetBlockTab *block = new TSetBlockTab(this);
		m_curPage->m_blocks.push_back(block);
		m_curBlock = block;
	}
	else if (command == bschordpro::CMD_TAB_END)
	{
		// if we are still in chorus block
		if (m_curBlock->getType() == TSetBlock::BLTYPE_TAB)
		{
			// finish block
			m_curBlock = NULL;
		}
		// else igonre end of chorus command
	}
}

void TSetDCPainter::onLine(const std::wstring& line)
{
	// for tab, capture block directly
	if (m_curBlock != NULL)
		if (m_curBlock->getType() == TSetBlock::BLTYPE_TAB)
		{
			static_cast<TSetBlockTab*>(m_curBlock)->m_lines.push_back(new wxString(line));
			return;
		}
}

void TSetDCPainter::onLineBegin()
{
	// default line settings
	m_posX = m_posXChord = 0;
	m_hasChords = false;
	m_isLineEmpty = true;
	m_curLine = new TSetLine();
}

void TSetDCPainter::onLineEnd()
{
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
			TSetBlockVerse *block = new TSetBlockVerse(this, m_verseCounter);
			block->m_lines.push_back(m_curLine);
			m_curPage->m_blocks.push_back(block);
			m_curBlock = block;
			m_verseCounter++;
		}
	}
	// else we are inside some block
	else
	{
		// we are inside some block
		switch (m_curBlock->getType())
		{
			case TSetBlock::BLTYPE_VERSE:
				// close block if line is empty
				if (m_isLineEmpty)
				{
					m_curPage->m_blocks.push_back(new TSetBlockHSpace(this));
					m_curBlock = NULL;
				}
				// line is not empty -> add it to the block contents
				else
					static_cast<TSetBlockVerse*>(m_curBlock)->m_lines.push_back(m_curLine);
				break;

			case TSetBlock::BLTYPE_CHORUS:
				// all lines are used if we are inside chorus (empty lines doesn't end block)
				static_cast<TSetBlockChorus*>(m_curBlock)->m_lines.push_back(m_curLine);
				break;

			case TSetBlock::BLTYPE_TAB:
			case TSetBlock::BLTYPE_TITLE:
			case TSetBlock::BLTYPE_HSPACE:
			case TSetBlock::BLTYPE_NONE:
			default:
				;
		}
	}
}
