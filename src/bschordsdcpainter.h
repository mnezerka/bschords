#ifndef BSCHORDSDCPAINTER_H
#define BSCHORDSDCPAINTER_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include "bschordpro.h"

/* mm per inch */
#define MM_PER_IN (25.4)

namespace bschords
{
	// forward declaration
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
		unsigned int m_pos;
		enum TBlockType { BLTYPE_NONE, BLTYPE_TEXT, BLTYPE_CHORUS, BLTYPE_HSPACE, BLTYPE_TITLE, BLTYPE_TAB, BLTYPE_VERSE };

		TSetBlock(TSetDCPainter *painter, unsigned int pos = 0) : m_painter(painter), m_pos(pos) { };
		virtual ~TSetBlock() { }
		virtual void draw(wxPoint pos) {};
		virtual void drawBoundingRect(const wxPoint pos);
		virtual wxRect getBoundingRect() { return wxRect(0, 0, 0, 0); };
		virtual TBlockType getType() { return BLTYPE_NONE; };
		virtual bool isVisible() { return true; };
	};

	struct TSetBlockHSpace : public TSetBlock
	{
		TSetBlockHSpace(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos) { };
		virtual ~TSetBlockHSpace() { }
		virtual TBlockType getType() { return BLTYPE_HSPACE; };
		virtual wxRect getBoundingRect();
	};

	struct TSetBlockTitle : public TSetBlock
	{
		TSetLineItem* m_title;
		TSetBlockTitle(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos), m_title(NULL) { };
		virtual ~TSetBlockTitle() { delete m_title; }
		virtual void draw(wxPoint pos);
		virtual TBlockType getType() { return BLTYPE_TITLE; };
		virtual wxRect getBoundingRect();
	};

	struct TSetBlockText : public TSetBlock
	{
		std::vector<TSetLine*> m_lines;
		TSetBlockText(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos) { };
		virtual ~TSetBlockText() { while (!m_lines.empty()) { delete m_lines.back(); m_lines.pop_back(); } }
		virtual void draw(wxPoint pos);
		virtual TBlockType getType() { return BLTYPE_TEXT; };
		virtual wxRect getBoundingRect();
		virtual bool hasChords();
		virtual bool hasNumbering() { return false; };
		virtual unsigned int getPos() { return m_pos; };
	};

	struct TSetBlockVerse : public TSetBlockText
	{
		unsigned int m_counter;
		TSetBlockVerse(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlockText(painter, pos), m_counter(0) { };
		virtual TBlockType getType() { return BLTYPE_VERSE; };
		virtual bool hasNumbering();
	};

	struct TSetBlockChorus : public TSetBlockText
	{
		TSetBlockChorus(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlockText(painter, pos) { };
		virtual TBlockType getType() { return BLTYPE_CHORUS; };
	};

	struct TSetBlockTab : public TSetBlock
	{
		std::vector<wxString*> m_lines;
		TSetBlockTab(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos) { };
		virtual ~TSetBlockTab() { while (!m_lines.empty()) { delete m_lines.back(); m_lines.pop_back(); } }
		virtual TBlockType getType() { return BLTYPE_TAB; };
		virtual void draw(wxPoint pos);
		virtual wxRect getBoundingRect();
		virtual bool isVisible();
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
			unsigned int m_verseCounter;
			//std::vector<TSetLineItem*> m_chordLine;
			//std::vector<TSetLineItem*> m_textLine;
			enum { SECTION_NONE, SECTION_VERSE, SECTION_CHORUS } m_section;
			bool m_isLineEmpty;
			float m_scale;
			std::vector<TSetPage*> m_pages;
			TSetBlock *m_curBlock;
			TSetPage *m_curPage;
			TSetLine *m_curLine;
	};
}

#endif // BSCHORDSDCPAINTER_H
