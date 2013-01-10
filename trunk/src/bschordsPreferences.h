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
        bool m_showTsetBlocks;
        bool m_showTsetMargins;

    protected:
        enum
        {
            idBtnSelFont = 1000,
        };

/*        wxStaticText* m_staticText1;
        wxStaticLine* m_staticline1;
        wxStaticText *fontTestTitle;
        wxStaticText *fontTextTitles[BS_FONT_LAST];
        wxComboBox *fontSelType; */
		wxStaticText *m_editorFontPreview;
    private:
        void OnClose(wxCloseEvent& event);
        void OnSelFont(wxCommandEvent &event);

        wxPanel* CreateGeneralPage(wxWindow* parent);
        wxPanel* CreateViewPage(wxWindow* parent);
        wxPanel* CreateEditorPage(wxWindow* parent);

        DECLARE_EVENT_TABLE()
};

#endif // BSCHORDSPREFERENCES_H
