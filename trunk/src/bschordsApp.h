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

#include <wx/app.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/font.h>

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


class bschordsApp : public wxApp
{
    public:
		wxConfig *config;
        wxFont m_fonts[BS_FONT_LAST];
        virtual bool OnInit();
        virtual int OnExit();
};

DECLARE_APP(bschordsApp);

#endif // BSCHORDSAPP_H
