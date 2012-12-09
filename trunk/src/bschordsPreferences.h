/***************************************************************
 * Name:      xxxMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2012-12-08
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef BSCHORDSPREFERENCES_H
#define BSCHORDSPREFERENCES_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "bschordsApp.h"

#include <wx/button.h>
#include <wx/statline.h>

class bschordsPreferences: public wxDialog
{
    public:
        bschordsPreferences(wxDialog *dlg, const wxString& title);
        ~bschordsPreferences();

    protected:
        enum
        {
            idBtnQuit = 1000,
            idBtnAbout
        };
        wxStaticText* m_staticText1;
        wxButton* BtnAbout;
        wxStaticLine* m_staticline1;
        wxButton* BtnQuit;

    private:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};

#endif // BSCHORDSPREFERENCES_H
