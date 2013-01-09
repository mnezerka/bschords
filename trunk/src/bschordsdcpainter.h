#ifndef BSCHORDSDCPAINTER_H
#define BSCHORDSDCPAINTER_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>

#include "bschordpro.h"


//#define BSCHP_OFFSET_X 30
//#define BSCHP_OFFSET_Y 30
//#define BSCHP_LINE_SPACING 0
//#define BSCHP_CHORD_OFFSET 0

// page size
//#define BSCHP_X 210
//#define BSCHP_Y 297

/* mm per inch */
#define MM_PER_IN (25.4)
namespace bschords
{

class TSetDCPainter;

struct TSetLineItem
{
	wxString *txt;
	wxRect m_bRect;

	virtual ~TSetLineItem() { if (txt) delete txt; };
};

struct TSetLine
{
	long m_posY;
	std::vector<TSetLineItem*> m_textItems;
	std::vector<TSetLineItem*> m_chordItems;
	TSetLine() : m_posY(0) {};
	virtual ~TSetLine(){
		while (!m_textItems.empty()) { delete m_textItems.back(); m_textItems.pop_back(); }
		while (!m_chordItems.empty()) { delete m_chordItems.back(); m_chordItems.pop_back(); }
	}
};

struct TSetBlock
{
	TSetDCPainter *m_painter;
	enum TBlockType { BLTYPE_NONE, BLTYPE_TEXT, BLTYPE_CHORUS, BLTYPE_HSPACE, BLTYPE_TITLE, BLTYPE_TAB };

	TSetBlock(TSetDCPainter *painter) : m_painter(painter) { };
	virtual ~TSetBlock() { }
	virtual void draw(wxPoint pos) {};
	virtual void drawBoundingRect(const wxPoint pos);
	virtual wxRect getBoundingRect() { return wxRect(0, 0, 0, 0); };
	virtual TBlockType getType() { return BLTYPE_NONE; };
};

struct TSetBlockHSpace : public TSetBlock
{
	TSetBlockHSpace(TSetDCPainter *painter) : TSetBlock(painter) { };
	virtual ~TSetBlockHSpace() { }
	virtual TBlockType getType() { return BLTYPE_HSPACE; };
	virtual wxRect getBoundingRect();
};

struct TSetBlockTitle : public TSetBlock
{
	TSetLineItem* m_title;
	TSetBlockTitle(TSetDCPainter *painter) : TSetBlock(painter), m_title(NULL) { };
	virtual ~TSetBlockTitle() { delete m_title; }
	virtual void draw(wxPoint pos);
	virtual TBlockType getType() { return BLTYPE_TITLE; };
	virtual wxRect getBoundingRect();
};

struct TSetBlockText : public TSetBlock
{
	std::vector<TSetLine*> m_lines;
	TSetBlockText(TSetDCPainter *painter) : TSetBlock(painter) { };
	virtual ~TSetBlockText() { while (!m_lines.empty()) { delete m_lines.back(); m_lines.pop_back(); } }
	virtual void draw(wxPoint pos);
	virtual TBlockType getType() { return BLTYPE_TEXT; };
	virtual wxRect getBoundingRect();
};

struct TSetBlockChorus : public TSetBlockText
{
	TSetBlockChorus(TSetDCPainter *painter) : TSetBlockText(painter) { };
	virtual TBlockType getType() { return BLTYPE_CHORUS; };
};

struct TSetBlockTab : public TSetBlock
{
	std::vector<wxString*> m_lines;
	TSetBlockTab(TSetDCPainter *painter) : TSetBlock(painter) { };
	virtual ~TSetBlockTab() { while (!m_lines.empty()) { delete m_lines.back(); m_lines.pop_back(); } }
	virtual TBlockType getType() { return BLTYPE_TAB; };
	virtual void draw(wxPoint pos);
	virtual wxRect getBoundingRect();
};

struct TSetPage
{
	std::vector<TSetBlock*> m_blocks;
	virtual ~TSetPage() { while (!m_blocks.empty()) { delete m_blocks.back(); m_blocks.pop_back(); } }
};

class TSetDCPainter : public bschordpro::EventHandler
{
	public:
		TSetDCPainter(wxDC& dc, float scale = 1);
		virtual ~TSetDCPainter();
		virtual void onBegin();
		virtual void onEnd();
		virtual void onLineBegin();
		virtual void onLineEnd();
		virtual void onCommand(const bschordpro::CommandType command, const std::wstring& value);
		virtual void onChord(const std::wstring& chord);
		virtual void onText(const std::wstring& text);
		virtual void onLine(const std::wstring& line);
		wxCoord getDeviceX(int numMM);
		wxCoord getDeviceY(int numMM);

		bool m_drawTsetBlocks;
		bool m_drawTsetMargins;

		SongStyleSheet *m_ss;
		wxDC& m_dc;
		wxSize m_dcPPI;
		int m_posX;
		int m_posXChord;
		int m_eMHeight;
		bool m_hasChords;
		//std::vector<TSetLineItem*> m_chordLine;
		//std::vector<TSetLineItem*> m_textLine;
		enum { SECTION_NONE, SECTION_VERSE, SECTION_CHORUS } m_section;
		int m_verseCounter;
		bool m_isLineEmpty;
		float m_scale;
		std::vector<TSetPage*> m_pages;
		TSetBlock *m_curBlock;
		TSetPage *m_curPage;
		TSetLine *m_curLine;
};

}

#endif // BSCHORDSDCPAINTER_H
