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

// TODO: implement chord transform utility
// TODO: solve CR LF problem - one checkbox in preferences could be nice
// TODO: warning widnow with typesetting problems - clipping, page full, etc.

class bschordsApp : public wxApp
{
    public:
		wxConfig *config;
        //wxFont m_fonts[BS_FONT_LAST];
		wxFont m_editorFont;
        // Global print data, to remember settings during the session
		wxPrintData *m_printData;
		// Global page setup data
		wxPageSetupDialogData* m_pageSetupData;

		SongStyleSheet m_styleSheet;

		// current songbook
		SongBook m_songBook;

        virtual bool OnInit();
        virtual int OnExit();

	private:
		//std::list<SongStylesheet> m_styleSheets;

};

DECLARE_APP(bschordsApp);

#endif // BSCHORDSAPP_H