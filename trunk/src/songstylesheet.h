#ifndef SONGSTYLESHEET_H
#define SONGSTYLESHEET_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/font.h>

/* TODO: new options:
	auto verse numbering,
	indent all verse lines
	print chords (yes/no)
	print chord lines even if no chord is present (same line spacing for whole verse/chorus)
	title centering
	user defined fields to be printed (see chorpro reference)*/

enum {
    BS_FONT_TITLE,
    BS_FONT_TITLE_SUB,
    BS_FONT_TEXT,
    BS_FONT_CHORDS,
    BS_FONT_LAST
};

static const wxString fontNames[] =
{
    _("Title"),
    _("Subtitle"),
    _("Text"),
    _("Chords")
};

struct FontInfo
{
    wxFont *font;
    FontInfo() : font(NULL) { font = new wxFont(*wxNORMAL_FONT); };
};

class SongStyleSheet
{
	public:
		wxFont m_fonts[BS_FONT_LAST];
		wxSize m_pageSize;
		wxCoord m_marginTop;
		wxCoord m_marginLeft;
		wxCoord m_marginBottom;
		wxCoord m_marginRight;
		wxString m_Name;
		wxString m_pageSizeName;
		enum { SS_ORIENTATION_PORTRAIT, SS_ORIENTATION_LANDSCAPE} m_pageOrientation;

		int m_cols;
		int m_lineSpacing;
		int m_chordLineSpacing;
		SongStyleSheet();
		virtual ~SongStyleSheet();
		void LoadFromConfig(const wxConfig *config);
		void SaveToConfig(wxConfig *config);


	protected:

	private:

};

#endif // SONGSTYLESHEET_H
