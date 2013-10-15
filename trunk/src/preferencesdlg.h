/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Preferences Dialog Declaration
 */

#ifndef BSCHORDSPREFERENCES_H
#define BSCHORDSPREFERENCES_H

#include <wx/button.h>
#include <wx/statline.h>

#include "app.h"

namespace bschords
{

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

} // namespace

#endif // BSCHORDSPREFERENCES_H

