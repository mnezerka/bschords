/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Stylesheet Dialog Declaration
 */

#ifndef SONGSTYLESHEETDLG_H
#define SONGSTYLESHEETDLG_H

#include <wx/button.h>
#include <wx/statline.h>
#include "app.h"
#include "songstylesheet.h"

namespace bschords
{

/** \brief Structure for paper size and its name */
struct PaperSize
{
    PaperSize(wxSize s, wxString n) : mSize(s), mName(n) { };
    wxSize mSize;
    wxString mName;
};

/** \brief Class for dialog with stylesheet properties */
class SongStyleSheetDlg: public wxDialog
{
public:
    /// Ctor
    SongStyleSheetDlg(wxDialog *dlg, const wxString& title, SongStyleSheet *styleSheet);
    bool TransferDataToWindow();
    bool TransferDataFromWindow();
    //wxFont m_fonts[BS_FONT_LAST];
    /// Width of page in mm
    wxString m_pageWidth;
    /// Height of page in mm
    wxString m_pageHeight;
    /// Width of left margin in mm
    wxString m_marginLeft;
    /// Height of top margin in mm
    wxString m_marginTop;
    /// Width of right margin in mm
    wxString m_marginRight;
    /// Height of bottom margin in mm
    wxString m_marginBottom;
    /// Indentation of chorus in mm
    wxString m_indentChorus;
    /// Number of page columns for typesetting
    wxString m_cols;
    /// Height of space to be used between two adjacent lines in mm
    wxString m_lineSpacing;
    /// Height of space between chord and text line in mm
    wxString m_lineChordSpacing;

    // style sheet
    SongStyleSheet mStyleSheet;

protected:
    enum
    {
        idBtnQuit = 1000,
        idBtnSelFont,
        idBtnLoad,
        idBtnSave,
        idBtnSelBackgroundImagePath,
        idComboPaperSize,
    };
    wxStaticText* m_staticText1;
    wxStaticLine* m_staticline1;
    wxButton* BtnQuit;
    wxStaticText *fontTestTitle;
    wxStaticText *fontTextTitles[BS_FONT_LAST];
    wxComboBox *fontSelType;

private:
    void SetPaperSize(unsigned int index);
    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnSelFont(wxCommandEvent &event);
    void OnLoad(wxCommandEvent &event);
    void OnSave(wxCommandEvent &event);
    void OnSelBackgroundImagePath(wxCommandEvent &event);
    void OnPaperSizeChnage(wxCommandEvent &event);

    wxPanel* CreatePageSetupPage(wxWindow* parent);
    wxPanel* CreateFontsPage(wxWindow* parent);
    wxPanel* CreateContentPage(wxWindow* parent);
    wxTextCtrl *mBgImagePathCtrl;
    wxComboBox *mComboPaperSize;
    wxTextCtrl *mCtrlPaperWidth;
    wxTextCtrl *mCtrlPaperHeight;
    std::vector<PaperSize> mPaperSizes;

    DECLARE_EVENT_TABLE()
};

} // namespace

#endif // SONGSTYLESHEETDLG_H
