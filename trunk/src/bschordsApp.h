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

class bschordsApp : public wxApp
{
    public:
		wxConfig *config;

        virtual bool OnInit();
        virtual int OnExit();
};

DECLARE_APP(bschordsApp);

#endif // BSCHORDSAPP_H
