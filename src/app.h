/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Application Class Declaration
 */

#ifndef BSCHORDSAPP_H
#define BSCHORDSAPP_H

#include <list>

#include <wx/app.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/font.h>
#include <wx/print.h>
#include "songstylesheet.h"
#include "songbook.h"
#include "mainwnd.h"

/*
ChordPro links:
http://home.online.no/~oeborger/skript/choprodirectives.html
http://linkesoft.com/songbook/chordproformat.html
http://en.wikipedia.org/wiki/Chord_%28software%29
*/

// TODO: remember position and size of print preview window
// TODO: fix bitmap reloading and scaling for each draw
// TODO: additional song properties in songbook - comment, chordpro prefix (inserted after title), postfix (at the bottom), columns, font size etc.
// TODO: apply stylesheet to Txt and Html outputs (hiding or showning appropriate sections)
// TODO: more icons on toolbar - different icons for songbook, new icon for stylesheet
// TODO: recent files and recent songbooks

namespace bschords
{

/** \brief Structure which encapsulates all application configuration parameters */
struct AppSettings
{
    /// Path of the songs repository root
    wxString m_rootPath;

    /// Font for song editor
    wxFont m_editorFont;

    /// Colors for song editor text
    wxColor m_editorColorText;

    /// Colors for song editor chords
    wxColor m_editorColorChords;

    /// Colors for song editor commands
    wxColor m_editorColorCommands;

    /// Ctor
    AppSettings();

    /// Load configuration data from Config class
    void LoadFromConfig(wxConfig *config);

    /// Save configuration data to Config class
    void SaveToConfig(wxConfig *config);
};

/** \brief Application class singleton */
class App : public wxApp
{
public:
    /// Current application configuration
    wxConfig *config;

    /// Global print data, to remember settings during the session
    wxPrintData *m_printData;

    /// Global page setup data
    wxPageSetupDialogData* m_pageSetupData;

    /// Current stylesheet
    SongStyleSheet m_styleSheet;

    /// Application settings
    AppSettings *m_settings;

    /// Current songbook
    bschords::SongBook m_songBook;

    /// Called by wx framework on initialization of application
    virtual bool OnInit();

    /// Called by wx framework when application is closed
    virtual int OnExit();

    /// Called by wx framework when log target is created
    virtual wxLog* CreateLogTarget();

private:

};

} // namespace

DECLARE_APP(bschords::App);

#endif // BSCHORDSAPP_H
