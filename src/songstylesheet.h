/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Stylesheet Declaration
 */

#ifndef SONGSTYLESHEET_H
#define SONGSTYLESHEET_H

#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/font.h>
#include <wx/gdicmn.h>

namespace bschords
{

/// Fonts for various typesetting blocks
enum
{
    BS_FONT_TITLE,          /**< Font for typesetting song titles */
    BS_FONT_TITLE_SUB,      /**< Font for typesetting song subtitles */
    BS_FONT_TEXT,           /**< Font for typesetting song verses, choruses, comments and other common text */
    BS_FONT_CHORDS,         /**< Font for typesetting song chords */
    BS_FONT_TAB,            /**< Font for typesetting song tab sections (shouldn't be proportional) */
    BS_FONT_STRUCT,         /**< Font for typesetting song structure sections */
    BS_FONT_MAIN_TITLE,     /**< Font for typesetting songbook title */
    BS_FONT_SECTION_TITLE,  /**< Font for typesetting songbook section titles */
    BS_FONT_TOC_SECTION,    /**< FOnt for typesetting songbook toc section entries */
    BS_FONT_TOC_ITEM,       /**< FOnt for typesetting songbook toc items (songs) */
    BS_FONT_LAST            /**< Not used */
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

    /// path to background image
    wxString mBackgroundImagePath;

    /// Ctor
    SongStyleSheet();

    /// Dtor
    ~SongStyleSheet();

    /// Load all configuration parameters from Config class (ini file, registry, etc.)
    void LoadFromConfig(const wxConfigBase *config);

    /// Save all configuration parameters to Config class (ini file, registry, etc.)
    void SaveToConfig(wxConfigBase *config);

    /// Get name of the font (for use in GUI)
    static wxString getFontName(unsigned int index);

protected:

private:

};

} // namespace

#endif // SONGSTYLESHEET_H
