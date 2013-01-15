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

class bschordsApp : public wxApp
{
    public:
		// current application configuration
		wxConfig *config;

		// font for song editor
		wxFont m_editorFont;

        // global print data, to remember settings during the session
		wxPrintData *m_printData;

		// global page setup data
		wxPageSetupDialogData* m_pageSetupData;

		// current stylesheet
		SongStyleSheet m_styleSheet;

		// current songbook
		bschords::SongBook m_songBook;

        virtual bool OnInit();
        virtual int OnExit();

	private:
		//std::list<SongStylesheet> m_styleSheets;

};

DECLARE_APP(bschordsApp);

#endif // BSCHORDSAPP_H