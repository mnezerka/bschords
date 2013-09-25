
#include <wx/tokenzr.h>

#include "app.h"
#include "dcpainter.h"


using namespace bschords;

// ------- TSetBlock  --------------------------------------------------------

void TSetBlock::drawBoundingRect()
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
	r.SetPosition(mPos);
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

void TSetBlockText::draw()
{
	wxPoint pos = mPos;
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

void TSetBlockTitle::draw()
{
	// typeset chord line (chord items)
	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TITLE]);
	m_painter->m_dc.DrawText(*m_title->txt, mPos.x, mPos.y);
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

void TSetBlockTab::draw()
{
	wxPoint pos = mPos;

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

// ------- TSetBlockStruct  ----------------------------------------------------------

bool TSetBlockStruct::isVisible()
{
	return m_painter->m_ss->m_showStructs;
}

std::vector<wxString> TSetBlockStruct::prepare()
{
	wxStringTokenizer tkBars;

	std::vector<wxString> result;

	std::cout << "tstruct prepare" << std::endl;

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		wxString *line = m_lines[lineIx];
		tkBars.SetString(*line, wxT("/"), wxTOKEN_RET_EMPTY);
		// loop over bars
		while (tkBars.HasMoreTokens())
		{

			// analyze individual bars
			wxString bar = tkBars.GetNextToken();

			std::wcout << L"analyzing bar <" << bar.wc_str() << L">" << std::endl;

			wxString chord;
			enum {READING_CHORD_SPACE, READING_CHORD_SYMBOL} state = READING_CHORD_SYMBOL;
			unsigned int i = 0;
			while (i < bar.Length())
			{
				switch (state)
				{
					case READING_CHORD_SYMBOL:
						if (bar[i] == wxChar(' '))
						{
							state = READING_CHORD_SPACE;
						}
						chord.Append(bar[i]);
						break;
					case READING_CHORD_SPACE:
						if (bar[i] != wxChar(' '))
						{
							//std::wcout << L"Chord found <" << chord.wc_str() << L">" << (chord.Length() / (float)bar.Length()) << std::endl;
							result.push_back(chord);
							chord.Empty();
							state = READING_CHORD_SYMBOL;
						}
						chord.Append(bar[i]);
						break;
				}
				i++;
			}
			if (chord.Length() > 0)
				result.push_back(chord);

		}
	}

	for (std::vector<wxString>::iterator it = result.begin(); it != result.end(); it++)
		//std::wcout << L"Chord found <" << (*it).wc_str() << L">" << ((*it).Length() / (float)bar.Length()) << std::endl;
		std::wcout << L"Chord found <" << (*it).wc_str() << std::endl;

	return result;
}

void TSetBlockStruct::draw()
{
	wxPoint pos = mPos;

	prepare();

	// loop through block lines
	for (size_t lineIx = 0; lineIx < m_lines.size(); lineIx++)
	{
		wxString *line = m_lines[lineIx];

		m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_STRUCT]);
		m_painter->m_dc.DrawText(*line, pos);
		pos.y += m_painter->m_dc.GetCharHeight();
	}
}

wxRect TSetBlockStruct::getBoundingRect()
{
	wxRect result;

	m_painter->m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_STRUCT]);
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

// ------- TSetPage -------------------------------------------------------------
TSetPage::TSetPage(TSetDCPainter *painter, wxRect pageRect) : mPageRect(pageRect), mPainter(painter), mCol(0)
{
	mPos = mPageRect.GetTopLeft();
	wxCoord columnWidth = pageRect.GetWidth() / mPainter->m_ss->m_cols;
	mColRect = pageRect;
	mColRect.SetWidth(columnWidth);
}

TSetPage::TPageAddResult TSetPage::addBlock(TSetBlock *block)
{
	//bool clippingDetected = false;

	// skip hidden blocks
	if (!block->isVisible())
		return ADD_OK;

	// get bounding rectangle of new block
	wxRect blockRect = block->getBoundingRect();

	// check if block is not too heigh for this page
	if (blockRect.GetHeight() > mPageRect.GetHeight())
		return(ADD_PAGE_OVERSIZE);

	// check if there is enough horizontal space for block to be drawn
	// in current column
	if (blockRect.GetWidth() > mColRect.GetWidth())
	{
		//clippingDetected = true;
		std::cout << "problem with horizontal clipping found" << std::endl;
	}

	// check if we have enough of horizontal space to draw block
	if (mPos.y + blockRect.GetHeight() > mPageRect.GetBottom())
	{
		//cout << "not enough space for block, pos.y: " << pos.y << " block height: " << blockHeight << endl;

		if (mCol < mPainter->m_ss->m_cols - 1)
		{
			//cout << "starting new column" << endl;
			mCol++;
			mColRect.SetLeft(mColRect.GetLeft() + mColRect.GetWidth());
			mPos.x = mColRect.GetLeft();
			mPos.y = mColRect.GetTop();
		}
		else
		{
			std::cout << "TSetPage -> not enougth space on this page" << std::endl;
			return(ADD_PAGE_FULL);
		}
	}

	// set block position on page
	block->setPosition(mPos);

	// add block to current page
	m_blocks.push_back(block);

	// move page typesetting cursor
	mPos.y += block->getBoundingRect().GetHeight();

	//std::cout << "addBlock - block height: " << block->getBoundingRect().GetHeight() << ", new pos.y: " << mPos.y << std::endl;

	return(ADD_OK);
}

void TSetPage::draw()
{
	// draw white (paper) background
	mPainter->m_dc.DrawRectangle(0, 0, mPainter->getDeviceX(mPainter->m_ss->m_pageSize.GetWidth()) , mPainter->getDeviceX(mPainter->m_ss->m_pageSize.GetHeight()));
	mPainter->m_dc.SetBackgroundMode(wxTRANSPARENT);

	// draw gray border to see typesetting area (margins)
	if (mPainter->m_drawTsetMargins)
	{
		wxPen pen(wxColour(200, 200, 200), 0.1); // red pen of width 1
		mPainter->m_dc.SetPen(pen);
		mPainter->m_dc.DrawRectangle(mPageRect);
	}

	// loop through current page blocks
    for (size_t blockIx = 0; blockIx < m_blocks.size(); blockIx++)
    {
        TSetBlock *block = m_blocks[blockIx];

		// draw bounding box
        if (mPainter->m_drawTsetBlocks)
			block->drawBoundingRect();

		//std::cout << "drawing block " << blockIx << " of type " << block->getType() << " height " << block->getBoundingRect().GetHeight() << " y: " << block->getPosition().y << std::endl;
		// draw block content
        block->draw();

        // draw warning
        /*if (clippingDetected)^M
        //{^M
            // draw bounding rect^M
            wxPen pen(wxColor(255, 0, 0), 5);^M
            m_dc.SetPen(pen);^M
            m_dc.DrawLine(colRect.GetRight(), pos.y, colRect.GetRight(), pos.y + blockRect.GetHeight());^M
            m_stat.m_clippings++;^M
        }*/

    }

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

// --------------------------------- TSetDCPainter ----------------------------

void TSetDCPainter::onBegin()
{
	//cout << "OnBegin" << endl;

	// prepare rectangle representation of "drawable" page space (paper - margins)
	//wxPoint pos(getDeviceX(m_ss->m_marginLeft), getDeviceY(m_ss->m_marginTop));
	wxRect pageRect(
		getDeviceX(m_ss->m_marginLeft),
		getDeviceY(m_ss->m_marginTop),
		getDeviceX(m_ss->m_pageSize.GetWidth() - m_ss->m_marginLeft - m_ss->m_marginRight),
		getDeviceY(m_ss->m_pageSize.GetHeight() - m_ss->m_marginTop - m_ss->m_marginBottom));
	mPageRect = pageRect;

	// create page structure
	m_curPage = new TSetPage(this, mPageRect);
	m_pages.push_back(m_curPage);
	m_stat.m_pages++;
}

void TSetDCPainter::onEnd()
{
	// close any open block
	if (m_curBlock != NULL)
	{
		addBlock(m_curBlock);
		m_curBlock = NULL;
	}

	//cout << "OnBegin" << endl;

	// loop over pages

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
    	// check if new page is required for each song (which starts with title commnad)
    	if (wxGetApp().m_styleSheet.m_songNewPage)
    	{
			AddPageBreak();
			m_curBlock = NULL;
    	}

		TSetLineItem *item = new TSetLineItem();
		m_dc.SetFont(wxGetApp().m_styleSheet.m_fonts[BS_FONT_TITLE]);
		item->txt = new wxString(value);
		item->m_bRect.SetSize(m_dc.GetTextExtent(*item->txt));
		item->m_bRect.SetPosition(wxPoint(0, 0));

		TSetBlockTitle *block = new TSetBlockTitle(this);
		block->m_title = item;
		addBlock(block);
		m_curBlock = NULL;
	}
	else if (command == bschordpro::CMD_CHORUS_START)
	{
		TSetBlockChorus *block = new TSetBlockChorus(this);
		m_curBlock = block;
	}
	else if (command == bschordpro::CMD_CHORUS_END)
	{
		// if we are still in chorus block
		if (m_curBlock->getType() == TSetBlock::BLTYPE_CHORUS)
		{
			// finish block
			addBlock(m_curBlock);
			m_curBlock = NULL;
		}
		// else igonre end of chorus command
	}
	else if (command == bschordpro::CMD_TAB_START)
	{
		TSetBlockTab *block = new TSetBlockTab(this);
		m_curBlock = block;
	}
	else if (m_curBlock && command == bschordpro::CMD_TAB_END)
	{
		// if we are still in tab block
		if (m_curBlock->getType() == TSetBlock::BLTYPE_TAB)
		{
			// finish block
			addBlock(m_curBlock);
			m_curBlock = NULL;
		}
		// else igonre end of chorus command
	}
	else if (command == bschordpro::CMD_STRUCT_START)
	{
		TSetBlockStruct *block = new TSetBlockStruct(this);
		m_curBlock = block;
	}
	else if (m_curBlock && command == bschordpro::CMD_STRUCT_END)
	{
		// if we are still in struct block
		if (m_curBlock->getType() == TSetBlock::BLTYPE_STRUCT)
		{
			// finish block
			addBlock(m_curBlock);
			m_curBlock = NULL;
		}
		// else igonre end of chorus command
	}
}

void TSetDCPainter::onLine(const std::wstring& line)
{
	// for tab and struct, capture block directly
	if (m_curBlock != NULL)
	{
		if (m_curBlock->getType() == TSetBlock::BLTYPE_TAB)
		{
			static_cast<TSetBlockTab*>(m_curBlock)->m_lines.push_back(new wxString(line));
			return;
		}
		else if (m_curBlock->getType() == TSetBlock::BLTYPE_STRUCT)
		{
			static_cast<TSetBlockStruct*>(m_curBlock)->m_lines.push_back(new wxString(line));
			return;
		}
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
			addBlock(m_curBlock);
			m_curBlock = NULL;
		}
		else
		{
			// create new text block and add current line
			TSetBlockVerse *block = new TSetBlockVerse(this, m_verseCounter);
			block->m_lines.push_back(m_curLine);
			// todel addBlock(block);
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
					addBlock(m_curBlock);
					addBlock(new TSetBlockHSpace(this));
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
			case TSetBlock::BLTYPE_STRUCT:
			case TSetBlock::BLTYPE_TITLE:
			case TSetBlock::BLTYPE_HSPACE:
			case TSetBlock::BLTYPE_NONE:
			default:
				;
		}
	}
}

TSetPage::TPageAddResult TSetDCPainter::addBlock(TSetBlock *block)
{
	TSetPage::TPageAddResult addResult = m_curPage->addBlock(block);
	switch (addResult)
	{
		case TSetPage::ADD_PAGE_FULL:
			// create new page
			m_curPage = new TSetPage(this, mPageRect);
			m_pages.push_back(m_curPage);
			m_stat.m_pages++;
			addResult = m_curPage->addBlock(block);
			break;
		case TSetPage::ADD_OK:
			break;
		default:
			;
	}

	return(addResult);
}

void TSetDCPainter::AddPageBreak()
{
	if (!m_curPage)
		return;

	// ignore page break if page is empty
	if (m_curPage->isEmpty())
		return;

	// create new page
	m_curPage = new TSetPage(this, mPageRect);
	m_pages.push_back(m_curPage);
	m_stat.m_pages++;
}

void TSetDCPainter::drawPage(unsigned int i)
{
	if (i < m_pages.size())
		m_pages[i]->draw();

}
