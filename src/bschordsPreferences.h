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
        wxFont m_fonts[BS_FONT_LAST];

    protected:
        enum
        {
            idBtnQuit = 1000,
            idBtnAbout,
            idBtnSelFont
        };
        wxStaticText* m_staticText1;
        wxButton* BtnAbout;
        wxStaticLine* m_staticline1;
        wxButton* BtnQuit;
        wxStaticText *fontTestTitle;
        wxStaticText *fontTextTitles[BS_FONT_LAST];
        wxComboBox *fontSelType;

    private:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSelFont(wxCommandEvent &event);
        DECLARE_EVENT_TABLE()
};

#endif // BSCHORDSPREFERENCES_H
