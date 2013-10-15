/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Definition of DCPainter class responsible for drawing song(s) to various Device Contexts
 */

#ifndef BSCHORDSDCPAINTER_H
#define BSCHORDSDCPAINTER_H

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
		public:
			wxRect m_bRect;
			virtual ~TSetLineItem() { };
			wxString getText() { return mText; };
			void setText(const wxString &text) { mText = text; };
			void appendText(const wxString &text) { mText += text; };
		private:
			wxString mText;

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
		/// painter used for drawing block
		TSetDCPainter *m_painter;

		/// index of block of the same type (e.g. for verse numbering)
		unsigned int m_pos;

		/// physical position of the block on the page
		wxPoint mPos;

		/// type of the block
		enum TBlockType {
			BLTYPE_NONE,
			BLTYPE_TEXT,
			BLTYPE_CHORUS,
			BLTYPE_HSPACE,
			BLTYPE_TITLE,
			BLTYPE_SUBTITLE,
			BLTYPE_COMMENT,
			BLTYPE_TAB,
			BLTYPE_STRUCT,
			BLTYPE_VERSE,
			BLTYPE_SECTION_TITLE,
			BLTYPE_MAIN_PAGE_TITLE,
			BLTYPE_TOC_TITLE,
			BLTYPE_TOC_ITEM };

		TSetBlock(TSetDCPainter *painter, unsigned int pos = 0) : m_painter(painter), m_pos(pos), mMaxWidth(0) { };
		virtual ~TSetBlock() { }
		virtual void setPosition(wxPoint p) { mPos = p; }
		virtual wxPoint getPosition() { return mPos; };
		virtual void draw() {};
		virtual void drawBoundingRect();
		virtual wxRect getBoundingRect() { return wxRect(0, 0, 0, 0); };
		void setMaxWidth(wxCoord width) { mMaxWidth = width; };
		wxCoord getMaxWidth() { return mMaxWidth; };
		virtual TBlockType getType() { return BLTYPE_NONE; };
		virtual bool isVisible() { return true; };

		private:
			/// detection of block clipping, setting this flag to true causes drawing of visual "clipping alarm"
			wxCoord mMaxWidth;
	};

	struct TSetBlockHSpace : public TSetBlock
	{
		TSetBlockHSpace(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos) { };
		virtual ~TSetBlockHSpace() { }
		virtual TBlockType getType() { return BLTYPE_HSPACE; };
		virtual wxRect getBoundingRect();
	};

	struct TSetBlockSingleLine : public TSetBlock
	{
		TSetLineItem mLine;
		TBlockType mType;
		wxFont mFont;

		TSetBlockSingleLine(
			TSetDCPainter *painter,
			TBlockType type,
			wxFont font,
			unsigned int pos = 0) : TSetBlock(painter, pos), mType(type), mFont(font) { };
		virtual ~TSetBlockSingleLine() { }
		virtual void setTxt(wxString text) { mLine.setText(text); };
		virtual void draw();
		virtual TBlockType getType() { return mType; };
		virtual wxRect getBoundingRect();
	};

	struct TSetBlockText : public TSetBlock
	{
		std::vector<TSetLine*> m_lines;
		TSetBlockText(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos) { };
		virtual ~TSetBlockText() { while (!m_lines.empty()) { delete m_lines.back(); m_lines.pop_back(); } }
		virtual void draw();
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
		virtual void draw();
		virtual wxRect getBoundingRect();
		virtual bool isVisible();
	};

	struct TSetStructItem {
		TSetStructItem(wxString &chord, float size) : mChord(chord), mSize(size) { };
		wxString mChord;
		float mSize;
	};

	struct TSetBlockStruct : public TSetBlock
	{
		std::vector<wxString*> m_lines;
		TSetBlockStruct(TSetDCPainter *painter, unsigned int pos = 0) : TSetBlock(painter, pos) { };
		virtual ~TSetBlockStruct() { while (!m_lines.empty()) { delete m_lines.back(); m_lines.pop_back(); } }
		virtual TBlockType getType() { return BLTYPE_STRUCT; };
		virtual void draw();
		virtual wxRect getBoundingRect();
		virtual bool isVisible();
		private:
			//std::vector<wxString> prepare();
			std::vector< std::vector<TSetStructItem> > prepare();
			void getSizeParams(std::vector< std::vector<TSetStructItem> > items, size_t &numCellSize, float &numMinSize, float &numMaxLineSize, size_t &numSeparatorSize);
	};

	/* Songbook page */
	struct TSetPage
	{
		public:
			enum TPageAddResult { ADD_OK, ADD_PAGE_FULL, ADD_PAGE_OVERSIZE };

			TSetPage(TSetDCPainter *painter, wxRect pageRect);
			virtual ~TSetPage() { while (!m_blocks.empty()) { delete m_blocks.back(); m_blocks.pop_back(); } }
			TPageAddResult addBlock(TSetBlock *block);
			bool isEmpty() { return mPos.y  == mPageRect.GetY(); };
			void draw();

		private:
			// list of vertical blocks
			std::vector<TSetBlock*> m_blocks;
			wxRect mPageRect;
			wxRect mColRect;
			wxPoint mPos;
			TSetDCPainter *mPainter;
			int mCol;
	};

	struct TSetStat
	{
		// number of clippings found during typesetting
		unsigned int m_clippings;
		// number of pages needed during typesetting
		unsigned int m_pages;

		TSetStat() : m_clippings(0), m_pages(0) { };
	};

	/* Class responsible for drawing based on events from bschords parser */
	class TSetDCPainter : public bschordpro::EventHandler
	{
		public:
			TSetDCPainter(wxDC& dc, float scale = 1);
			virtual ~TSetDCPainter();
			virtual void onBegin();
			virtual void onEnd();
			virtual void onLineBegin();
			virtual void onLineEnd();
			virtual void onCommand(const bschordpro::CommandType command, const std::wstring& value, const bschordpro::RawPos &pos);
			virtual void onCommandUnknown(const std::wstring &cmdId, const std::wstring &value, const bschordpro::RawPos &pos);
			virtual void onChord(const std::wstring& chord, const bschordpro::RawPos &pos);
			virtual void onText(const std::wstring& text, const bschordpro::RawPos &pos);
			virtual void onLine(const std::wstring& line, const bschordpro::RawPos &pos);
			wxCoord getDeviceX(int numMM);
			wxCoord getDeviceY(int numMM);
			TSetStat getTSetStat() { return m_stat; };
			TSetPage::TPageAddResult addBlock(TSetBlock *block);
			void AddPageBreak();

			unsigned int getPages() { return m_pages.size(); };
			void drawPage(unsigned int i);

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
			TSetStat m_stat;
			wxBitmap *mBitmapBackground;
		private:
			//TSetStat m_stat;
			wxRect mPageRect;
	};
}

#endif // BSCHORDSDCPAINTER_H
