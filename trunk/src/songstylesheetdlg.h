/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Stylesheet Dialog Definition
 */

#ifndef SONGSTYLESHEETDLG_H
#define SONGSTYLESHEETDLG_H

#include <wx/button.h>
#include <wx/statline.h>
#include "app.h"
#include "songstylesheet.h"

namespace bschords
{

class SongStyleSheetDlg: public wxDialog
{
    public:
        SongStyleSheetDlg(wxDialog *dlg, const wxString& title, SongStyleSheet *styleSheet);
        bool TransferDataToWindow();
        bool TransferDataFromWindow();
        //wxFont m_fonts[BS_FONT_LAST];
        wxString m_pageWidth;
        wxString m_pageHeight;
        wxString m_marginLeft;
        wxString m_marginTop;
        wxString m_marginRight;
        wxString m_marginBottom;
        wxString m_indentChorus;
        wxString m_cols;
        wxString m_lineSpacing;
        wxString m_lineChordSpacing;
		SongStyleSheet mStyleSheet;

    protected:
        enum
        {
            idBtnQuit = 1000,
            idBtnSelFont,
            idBtnLoad,
            idBtnSave
        };
        wxStaticText* m_staticText1;
        wxStaticLine* m_staticline1;
        wxButton* BtnQuit;
        wxStaticText *fontTestTitle;
        wxStaticText *fontTextTitles[BS_FONT_LAST];
        wxComboBox *fontSelType;


    private:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnSelFont(wxCommandEvent &event);
        void OnLoad(wxCommandEvent &event);
        void OnSave(wxCommandEvent &event);

		wxPanel* CreatePageSetupPage(wxWindow* parent);
        wxPanel* CreateFontsPage(wxWindow* parent);
        wxPanel* CreateContentPage(wxWindow* parent);


        DECLARE_EVENT_TABLE()
};

} // namespace

#endif // SONGSTYLESHEETDLG_H
