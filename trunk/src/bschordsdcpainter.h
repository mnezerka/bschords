#ifndef BSCHORDSDCPAINTER_H
#define BSCHORDSDCPAINTER_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>

#include "bschordpro.h"


#define BSCHP_OFFSET_X 30
#define BSCHP_OFFSET_Y 30
#define BSCHP_LINE_SPACING 0
#define BSCHP_CHORD_OFFSET 0

// page size
#define BSCHP_X 210
#define BSCHP_Y 297

/* mm per inch */
#define MM_PER_IN (25.4)

struct BSLineItem
{
	wxString *txt;
	int posX;
	long width;
};

class BSChordsDCPainter : public BSChordProEventHandler
{
	public:
		BSChordsDCPainter(wxDC& dc);
		virtual ~BSChordsDCPainter();
		virtual void onLineBegin();
		virtual void onLineEnd();
		virtual void onCommand(const wstring& command, const wstring& value);
		virtual void onChord(const wstring& chord);
		virtual void onText(const wstring& text);
		wxCoord getDeviceX(int numMM);
		wxCoord getDeviceY(int numMM);

	private:
		wxDC& m_dc;
		wxSize m_dcPPI;
		int m_posY;
		int m_posX;
		int m_posXChord;
		int m_eMHeight;
		bool m_hasChords;
		vector<BSLineItem*> m_chordLine;
		vector<BSLineItem*> m_textLine;
};

#endif // BSCHORDSDCPAINTER_H
