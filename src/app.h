/***************************************************************
 * Name:      bschordsApp.h
 * Purpose:   Defines Application Class
 * Author:     ()
 * Created:   2012-12-01
 * Copyright:  ()
 * License:
 **************************************************************/

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

// TODO: show warning when overwriting existing file
// TODO: songboook - parse info from items in list (e.g. Title, Subtitle, commments, etc.)
// TODO: additional song properties in songbook - comment, chordpro prefix (inserted after title), postfix (at the bottom)
// TODO: separate section for song title (especially in case of multicolumn stylesheet)
// TODO: song directives (e.g. request for 2 cols)

namespace bschords
{
	struct AppSettings
	{
		// path of the songs repository root
		wxString m_rootPath;

		// font for song editor
		wxFont m_editorFont;

		// colors for song editor
		wxColor m_editorColorText;
		wxColor m_editorColorChords;
		wxColor m_editorColorCommands;

		AppSettings();
		void LoadFromConfig(wxConfig *config);
		void SaveToConfig(wxConfig *config);
	};

	class App : public wxApp
	{
		public:
			// current application configuration
			wxConfig *config;

			// global print data, to remember settings during the session
			wxPrintData *m_printData;

			// global page setup data
			wxPageSetupDialogData* m_pageSetupData;

			// current stylesheet
			SongStyleSheet m_styleSheet;

			// application settings
			AppSettings *m_settings;

			// current songbook
			bschords::SongBook m_songBook;


			virtual bool OnInit();
			virtual int OnExit();
			virtual wxLog* CreateLogTarget();

		private:
			//std::list<SongStylesheet> m_styleSheets;

			// main window
			//MainWnd *mFrame;
	};
}

DECLARE_APP(bschords::App);

#endif // BSCHORDSAPP_H
