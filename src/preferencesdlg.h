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

#include <wx/button.h>
#include <wx/statline.h>

#include "app.h"

class PreferencesDlg: public wxDialog
{
    public:
        wxFont m_fonts[BS_FONT_LAST];
        bool m_showTsetBlocks;
        bool m_showTsetMargins;
		wxColor m_editorColorText;
		wxColor m_editorColorChords;
		wxColor m_editorColorCommands;

        PreferencesDlg(wxDialog *dlg, const wxString& title);
        ~PreferencesDlg();
        bool TransferDataToWindow();
        bool TransferDataFromWindow();

    protected:
		wxStaticText *m_editorFontPreview;
		wxBitmapButton *m_btnEditorColorText;
		wxBitmapButton *m_btnEditorColorChords;
		wxBitmapButton *m_btnEditorColorCommands;

    private:
        void OnClose(wxCloseEvent& event);
        void OnSelFont(wxCommandEvent &event);
		void OnSelColor(wxCommandEvent &event);
		void OnSelRootPath(wxCommandEvent &event);

        wxPanel* CreateGeneralPage(wxWindow* parent);
        wxPanel* CreateViewPage(wxWindow* parent);
        wxPanel* CreateEditorPage(wxWindow* parent);
        wxTextCtrl *m_rootPathCtrl;

		wxBitmap createColorBitmap(const wxColour& c);
		void updateColors();

        DECLARE_EVENT_TABLE()
};

#endif // BSCHORDSPREFERENCES_H
