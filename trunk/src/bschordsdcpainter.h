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

struct BSLineItem
{
	wxString *txt;
	int posX;
	long width;
};

struct BSLine
{
	vector<BSLineItem*> m_textItems;
	vector<BSLineItem*> m_chordItems;
};

struct BSBlock
{
	vector<BSLine*> m_lines;
};

struct BSPage
{
	vector<BSBlock*> m_blocks;
};

class BSChordsDCPainter : public BSChordProEventHandler
{
	public:
		BSChordsDCPainter(wxDC& dc, float scale = 1);
		virtual ~BSChordsDCPainter();
		virtual void onBegin();
		virtual void onEnd();
		virtual void onLineBegin();
		virtual void onLineEnd();
		virtual void onCommand(const wstring& command, const wstring& value);
		virtual void onChord(const wstring& chord);
		virtual void onText(const wstring& text);
		wxCoord getDeviceX(int numMM);
		wxCoord getDeviceY(int numMM);

	private:
		SongStyleSheet *m_ss;
		wxDC& m_dc;
		wxSize m_dcPPI;
		int m_posY;
		int m_posX;
		int m_posXChord;
		int m_eMHeight;
		bool m_hasChords;
		vector<BSLineItem*> m_chordLine;
		vector<BSLineItem*> m_textLine;
		enum { SECTION_NONE, SECTION_VERSE, SECTION_CHORUS } m_section;
		int m_verseCounter;
		bool m_isLineEmpty;
		float m_scale;
		BSPage m_page;
		BSBlock *m_curBlock;
};

#endif // BSCHORDSDCPAINTER_H
