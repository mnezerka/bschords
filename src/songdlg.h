/***************************************************************
 * Name:      xxxMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2012-12-08
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef BSCHORDS_SONGDLG_H
#define BSCHORDS_SONGDLG_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/statline.h>

#include "app.h"
namespace bschords
{

class SongDlg: public wxDialog
{
    public:
        bool m_showTsetBlocks;
        bool m_showTsetMargins;

        SongDlg(wxDialog *dlg, const wxString& title, SongBookItem& songbookItem);
        ~SongDlg();
        bool TransferDataToWindow();
        bool TransferDataFromWindow();

    protected:
		wxStaticText *m_editorFontPreview;
		wxBitmapButton *m_btnEditorColorText;
		wxBitmapButton *m_btnEditorColorChords;
		wxBitmapButton *m_btnEditorColorCommands;

    private:
        void OnClose(wxCloseEvent& event);
		void OnSelRootPath(wxCommandEvent &event);

		SongBookItem &mSongbookItem;
        wxPanel* CreateGeneralPage(wxWindow* parent);
        wxPanel* CreateViewPage(wxWindow* parent);
        wxPanel* CreateEditorPage(wxWindow* parent);
        wxTextCtrl *mCommentCtrl;

        DECLARE_EVENT_TABLE()
};

} // namespace bschords

#endif // BSCHORDS_SONGDLG_H
