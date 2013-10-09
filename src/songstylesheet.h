#ifndef SONGSTYLESHEET_H
#define SONGSTYLESHEET_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/font.h>

enum {
    BS_FONT_TITLE,
    BS_FONT_TITLE_SUB,
    BS_FONT_TEXT,
    BS_FONT_CHORDS,
    BS_FONT_TAB,
    BS_FONT_STRUCT,
    BS_FONT_MAIN_TITLE,
    BS_FONT_SECTION_TITLE,
    BS_FONT_LAST
};

/** \brief Class encapsulates all properties related to visual representation of song or songbook */
class SongStyleSheet
{
	public:
		///fonts for block types
		wxFont m_fonts[BS_FONT_LAST];
		/// page size in milimeters
		wxSize m_pageSize;
		/// top margin
		wxCoord m_marginTop;
		/// left margin
		wxCoord m_marginLeft;
		/// bottom margin
		wxCoord m_marginBottom;
		/// right margin
		wxCoord m_marginRight;
		/// indentation of chorus blocks
		wxCoord m_indentChorus;
		/// nember of columns
		int m_cols;
		/// space between two adjacent lines in block
		int m_lineSpacing;
		/// space between chord and text
		int m_chordLineSpacing;
		/// automatic verse numbering
		bool m_verseNumbering;
		/// show chords
		bool m_showChords;
		/// show subtitles
		bool m_showSubtitles;
		/// show tab sections
		bool m_showTabs;
		/// show structure sections
		bool m_showStructs;
		/// print chord lines even if no chord is present (same line spacing for whole verse/chorus)
		bool m_equalLineHeights;
		/// force new page for song (starting with song title)
		bool m_songNewPage;
		/// show (print) songbook title page
		bool m_songbookTitlePage;
		/// show (print) songbook section pages
		bool m_songbookSectionPages;
		/// show (print) songbook table of contents
		bool m_songbookToc;

		SongStyleSheet();
		~SongStyleSheet();
		void LoadFromConfig(const wxConfigBase *config);
		void SaveToConfig(wxConfigBase *config);
		static wxString getFontName(unsigned int index);

	protected:

	private:

};

#endif // SONGSTYLESHEET_H
