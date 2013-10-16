/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Stylesheet Dialog Implementation
 */

#include <iostream>

#include "songstylesheetdlg.h"
#include <wx/treebook.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/valgen.h>
#include <wx/wfstream.h>

namespace bschords
{

#define CTRL_BORDER 3
//typedef struct
//wxSize paperSizes[] = {	wxSize(100, 100) };
//#define PAPER_A4 (210, 297)

BEGIN_EVENT_TABLE(SongStyleSheetDlg, wxDialog)
    EVT_CLOSE(SongStyleSheetDlg::OnClose)
    EVT_BUTTON(idBtnQuit, SongStyleSheetDlg::OnQuit)
    EVT_BUTTON(idBtnSelFont, SongStyleSheetDlg::OnSelFont)
    EVT_BUTTON(idBtnLoad, SongStyleSheetDlg::OnLoad)
    EVT_BUTTON(idBtnSave, SongStyleSheetDlg::OnSave)
    EVT_BUTTON(idBtnSelBackgroundImagePath, SongStyleSheetDlg::OnSelBackgroundImagePath)
END_EVENT_TABLE()

SongStyleSheetDlg::SongStyleSheetDlg(wxDialog *dlg, const wxString &title, SongStyleSheet *styleSheet)
    : wxDialog(dlg, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    // create copy of style sheet data
    mStyleSheet = *styleSheet;


    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxNotebook* m_bookCtrl = new wxNotebook(this, wxID_ANY); //, wxDefaultPosition, wxDefaultSize, wxWS_EX_VALIDATE_RECURSIVELY);

    wxPanel* pagePageSetup = CreatePageSetupPage(m_bookCtrl);
    wxPanel* pageFonts = CreateFontsPage(m_bookCtrl);
    wxPanel* pageContent = CreateContentPage(m_bookCtrl);
    m_bookCtrl->AddPage(pagePageSetup, _("Page"), false);
    m_bookCtrl->AddPage(pageContent, _("Content"), false);
    m_bookCtrl->AddPage(pageFonts, _("Fonts"), false);
    mainSizer->Add(m_bookCtrl, 0, wxALL | wxEXPAND, 5);

    // static line
    wxStaticLine * staticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(staticLine1, 0, wxALL | wxEXPAND, 5);

    // create buttons
    wxSizer* btnSizer = CreateButtonSizer(wxOK | wxCANCEL);
    btnSizer->Add(new wxButton(this, idBtnLoad, wxT("Load...")));
    btnSizer->Add(new wxButton(this, idBtnSave, wxT("Save...")));

    // sizing stuff
    mainSizer->Add(btnSizer, 0, wxALL, 3);

    this->SetSizer(mainSizer);
    this->Layout();
    mainSizer->Fit(this);
}

wxPanel* SongStyleSheetDlg::CreatePageSetupPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWS_EX_VALIDATE_RECURSIVELY);

    //wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxGridSizer *topSizer = new wxGridSizer(1, 2, 5, 5);

    // first column
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    // second column
    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    // --------------------------------------------------------------------------------------
    // page size panel
    wxStaticBox* pageStaticBox = new wxStaticBox(panel, wxID_ANY, _("Page size (milimeters)"));
    wxBoxSizer* pageStaticBoxSizer = new wxStaticBoxSizer(pageStaticBox, wxHORIZONTAL);
    item0->Add(pageStaticBoxSizer, 0, wxGROW|wxALL, 5);

    // TODO: handling of paper sizes
    wxComboBox *comboPageSize = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,-1));
    comboPageSize->Append(_T("User defined"));
    comboPageSize->Append(_T("A4 (210x219 mm)"));
    comboPageSize->Select(0);
    pageStaticBoxSizer->Add(comboPageSize, 0, wxALL | wxCENTER, CTRL_BORDER);

    pageStaticBoxSizer->Add(new wxStaticText(panel, wxID_ANY, _("=")), 0, wxALL | wxCENTER, CTRL_BORDER);
    pageStaticBoxSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_pageWidth)), 0, wxALL | wxCENTER, CTRL_BORDER);
    pageStaticBoxSizer->Add(new wxStaticText(panel, wxID_ANY, _(" x ")), 0, wxALL | wxCENTER, CTRL_BORDER);
    pageStaticBoxSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_pageHeight)), 0, wxALL | wxCENTER, CTRL_BORDER);

    // --------------------------------------------------------------------------------------
    // margins panel
    wxStaticBox* marginsStaticBox = new wxStaticBox(panel, wxID_ANY, _("Margins (milimeters)"));
    wxBoxSizer* marginsStaticBoxSizer = new wxStaticBoxSizer(marginsStaticBox, wxVERTICAL );
    item0->Add(marginsStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxGridSizer* marginsSizer = new wxGridSizer(3, 3, 4, 4);
    marginsSizer->AddSpacer(1);
    marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginTop)), 0, wxALL, CTRL_BORDER);
    marginsSizer->AddSpacer(1);
    marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginLeft)), 0, wxALL, CTRL_BORDER);
    wxPanel *whitePagePanel = new wxPanel(panel);
    whitePagePanel->SetBackgroundColour(wxColour(255, 255, 255));
    whitePagePanel->SetBackgroundStyle(wxBG_STYLE_COLOUR);
    marginsSizer->Add(whitePagePanel, 0, wxALL | wxEXPAND, CTRL_BORDER);
    marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginRight)), 0, wxALL, CTRL_BORDER);
    marginsSizer->AddSpacer(1);
    marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginBottom)), 0, wxALL, CTRL_BORDER);
    marginsStaticBoxSizer->Add(marginsSizer, 0, wxALL, 3);

    // --------------------------------------------------------------------------------------
    // lines etc..
    wxStaticBox* linesStaticBox = new wxStaticBox(panel, wxID_ANY, _("Text and chord lines (mm)"));
    wxBoxSizer* linesStaticBoxSizer = new wxStaticBoxSizer(linesStaticBox, wxVERTICAL );
    item1->Add(linesStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxGridSizer* linesSizer = new wxGridSizer(2, 4, 4);
    linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Columns:")), 0, wxALL, CTRL_BORDER);
    wxComboBox *comboCols = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord));
    comboCols->SetValidator(wxGenericValidator(&m_cols));
    comboCols->Append(_T("1"));
    comboCols->Append(_T("2"));
    comboCols->Append(_T("3"));
    comboCols->Append(_T("4"));
    linesSizer->Add(comboCols, 0, wxALL, CTRL_BORDER);

    linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Line spacing:")));
    linesSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_lineSpacing)), 0, wxALL, CTRL_BORDER);

    linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Chord line spacing:")));
    linesSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_lineChordSpacing)), 0, wxALL, CTRL_BORDER);

    linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Chorus indent:")));
    linesSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_indentChorus)), 0, wxALL, CTRL_BORDER);

    linesStaticBoxSizer->Add(linesSizer, 0, wxALL, 3);

    // --------------------------------------------------------------------------------------
    // background image
    wxStaticBox* bgImageStaticBox = new wxStaticBox(panel, wxID_ANY, wxT("Background image"));
    wxBoxSizer* bgImageStaticBoxSizer = new wxStaticBoxSizer(bgImageStaticBox, wxVERTICAL);
    item0->Add(bgImageStaticBoxSizer, 0, wxGROW|wxALL, 5);

    bgImageStaticBoxSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Image file path:")), 0, wxALL | wxEXPAND, 3);

    wxBoxSizer *bgImagePathSizer = new wxBoxSizer(wxHORIZONTAL);
    mBgImagePathCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.mBackgroundImagePath));
    bgImagePathSizer->Add(mBgImagePathCtrl, 1, wxALL | wxEXPAND | wxCENTER);
    bgImagePathSizer->AddSpacer(5);
    bgImagePathSizer->Add(new wxButton(panel, idBtnSelBackgroundImagePath, wxT("..."), wxDefaultPosition, wxSize(30, wxDefaultCoord)), 0, wxALL | wxEXPAND | wxCENTER);
    bgImageStaticBoxSizer->Add(bgImagePathSizer, 1, wxALL | wxEXPAND, 3);

     // --------------------------------------------------------------------------------------
    // global stuff
    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->Add( item1, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* SongStyleSheetDlg::CreateFontsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    // --------------------------------------------------------------------------------------
    // font selection controls
    wxStaticBox* fontsStaticBox = new wxStaticBox(panel, wxID_ANY, _("Fonts"));
    wxBoxSizer* fontsStaticBoxSizer = new wxStaticBoxSizer(fontsStaticBox, wxVERTICAL );
    item0->Add(fontsStaticBoxSizer, 0, wxGROW|wxALL, 5);

    //wxPanel *fontSelPanel = new wxPanel(ssPanel);
    wxBoxSizer* fontSelSizer = new wxBoxSizer(wxHORIZONTAL);
    fontSelType = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100,-1) );
    for (int i = 0; i < BS_FONT_LAST; i++)
        fontSelType->Append(SongStyleSheet::getFontName(i));
    fontSelType->Select(0);

    fontSelSizer->Add(fontSelType, 0, wxALL | wxALIGN_CENTER, 2);
    wxButton *fontSelButton = new wxButton(panel, idBtnSelFont, _("Font..."));
    fontSelSizer->Add(fontSelButton, 0, wxALL | wxALIGN_CENTER, 5);
    fontSelSizer->Add(new wxStaticText(panel, wxID_ANY, _("Select font")), 0, wxALL | wxALIGN_CENTER, 10);
    fontsStaticBoxSizer->Add(fontSelSizer);

    // font preview controls
    wxPanel *fontsPreviewPanel = new wxPanel(panel);
    fontsPreviewPanel->SetBackgroundColour(wxColour(255, 255, 255));
    fontsPreviewPanel->SetBackgroundStyle(wxBG_STYLE_COLOUR);
    fontsStaticBoxSizer->Add(fontsPreviewPanel, 1, wxALL | wxEXPAND, 5);
    wxBoxSizer* fontsPreviewPanelSizer = new wxBoxSizer(wxVERTICAL);

    // create test tests to demonstrate font look
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
        fontTextTitles[i] = new wxStaticText(fontsPreviewPanel, wxID_ANY, SongStyleSheet::getFontName(i));
        fontTextTitles[i]->SetFont(mStyleSheet.m_fonts[i]);
        fontsPreviewPanelSizer->Add(fontTextTitles[i], 0, wxALL | wxALIGN_CENTER, 10);
    }
    fontsPreviewPanel->SetSizer(fontsPreviewPanelSizer);

    // global stuff
    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* SongStyleSheetDlg::CreateContentPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWS_EX_VALIDATE_RECURSIVELY);

    //wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

    // first column
    wxBoxSizer *col0 = new wxBoxSizer( wxVERTICAL );

    // --------------------------------------------------------------------------------------
    // verses and choruses panel
    wxStaticBox* vchStaticBox = new wxStaticBox(panel, wxID_ANY, _("Verses and Choruses"));
    wxBoxSizer* vchStaticBoxSizer = new wxStaticBoxSizer(vchStaticBox, wxVERTICAL);
    col0->Add(vchStaticBoxSizer, 0, wxGROW|wxALL, 5);

    vchStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show chords"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_showChords)), 1, wxEXPAND | wxALL, 5);
    vchStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show subtitles"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_showSubtitles)), 1, wxEXPAND | wxALL, 5);
    vchStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show tabs"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_showTabs)), 1, wxEXPAND | wxALL, 5);
    vchStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show song structure sections"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_showStructs)), 1, wxEXPAND | wxALL, 5);

    vchStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Verse numbering"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_verseNumbering)), 1, wxEXPAND | wxALL, 5);
    vchStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Equal line heights (text lines without chords have equal height as lines chords)"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_equalLineHeights)), 1, wxEXPAND | wxALL, 5);

    // --------------------------------------------------------------------------------------
    // songbook
    wxStaticBox* sbkStaticBox = new wxStaticBox(panel, wxID_ANY, _("Songbook"));
    wxBoxSizer* sbkStaticBoxSizer = new wxStaticBoxSizer(sbkStaticBox, wxVERTICAL);
    col0->Add(sbkStaticBoxSizer, 0, wxGROW|wxALL, 5);

    sbkStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Force new page for each song"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_songNewPage)), 1, wxEXPAND | wxALL, 5);
    sbkStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show (print) table of contents"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_songbookToc)), 1, wxEXPAND | wxALL, 5);
    sbkStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show (print) title page"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_songbookTitlePage)), 1, wxEXPAND | wxALL, 5);
    sbkStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show (print) section pages"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&mStyleSheet.m_songbookSectionPages)), 1, wxEXPAND | wxALL, 5);

    // --------------------------------------------------------------------------------------
    // global stuff
    topSizer->Add(col0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}


void SongStyleSheetDlg::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void SongStyleSheetDlg::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void SongStyleSheetDlg::OnSelFont(wxCommandEvent &event)
{
    int fontIx = fontSelType->GetSelection();
    if (fontIx != wxNOT_FOUND && fontIx >= 0 && fontIx < BS_FONT_LAST)
    {
        // fonts stuff
        wxFontData fontData;
        fontData.SetInitialFont(mStyleSheet.m_fonts[fontIx]);
        //data.SetColour(canvasTextColour);
        wxFontDialog dialog(this, &fontData);
        if (dialog.ShowModal() == wxID_OK)
        {
            wxFontData retData = dialog.GetFontData();
            mStyleSheet.m_fonts[fontIx] = retData.GetChosenFont();
            fontTextTitles[fontIx]->SetFont(mStyleSheet.m_fonts[fontIx]);
        }
    }
}

void SongStyleSheetDlg::OnLoad(wxCommandEvent &event)
{
    wxFileDialog* openFileDialog = new wxFileDialog(this, _("Open stylesheet file"), wxEmptyString, wxEmptyString, wxT("*.sst"), wxOPEN, wxDefaultPosition);
    if (openFileDialog->ShowModal() == wxID_OK )
    {
        wxFileInputStream stream(openFileDialog->GetPath());
        wxFileConfig *config = new wxFileConfig(stream);
        mStyleSheet.LoadFromConfig(config);
        delete(config);
        TransferDataToWindow();
    }
}

void SongStyleSheetDlg::OnSave(wxCommandEvent &event)
{
    wxFileDialog* saveDlg = new wxFileDialog(this, _("Save stylesheet file"), wxEmptyString, wxEmptyString, wxT("*.sst"), wxSAVE, wxDefaultPosition);

    if (saveDlg->ShowModal() == wxID_OK )
    {
        wxFileOutputStream stream(saveDlg->GetPath());
        wxFileConfig *config = new wxFileConfig();
        mStyleSheet.SaveToConfig(config);
        config->Save(stream);
        delete(config);
    }
}

void SongStyleSheetDlg::OnSelBackgroundImagePath(wxCommandEvent& event)
{
    wxFileName fileName(mBgImagePathCtrl->GetValue());
    wxString dir(fileName.GetPath());
    wxFileDialog* openFileDialog = new wxFileDialog(this, _("Open file"), fileName.GetPath(), fileName.GetName(), _("*.png"), wxOPEN, wxDefaultPosition);
    if (openFileDialog->ShowModal() == wxID_OK )
    {
        mBgImagePathCtrl->SetValue(openFileDialog->GetPath());
    }
}

bool SongStyleSheetDlg::TransferDataToWindow()
{
    m_pageWidth = wxString::Format(wxT("%d"), mStyleSheet.m_pageSize.GetWidth());
    m_pageHeight = wxString::Format(wxT("%d"), mStyleSheet.m_pageSize.GetHeight());

    m_marginLeft = wxString::Format(wxT("%d"), mStyleSheet.m_marginLeft);
    m_marginTop = wxString::Format(wxT("%d"), mStyleSheet.m_marginTop);
    m_marginRight = wxString::Format(wxT("%d"), mStyleSheet.m_marginRight);
    m_marginBottom = wxString::Format(wxT("%d"), mStyleSheet.m_marginBottom);

    m_cols = wxString::Format(wxT("%d"), mStyleSheet.m_cols);
    m_lineSpacing = wxString::Format(wxT("%d"), mStyleSheet.m_lineSpacing);
    m_lineChordSpacing = wxString::Format(wxT("%d"), mStyleSheet.m_chordLineSpacing);
    m_indentChorus = wxString::Format(wxT("%d"), mStyleSheet.m_indentChorus);

    bool result = wxDialog::TransferDataToWindow();

    // These function calls have to be made here, after the
    // dialog has been created.
    //text->SetFocus();
    //combobox->SetSelection(0);

    return result;
}

bool SongStyleSheetDlg::TransferDataFromWindow()
{
    bool result = wxDialog::TransferDataFromWindow();

    mStyleSheet.m_pageSize.SetWidth(wxAtoi(m_pageWidth));
    mStyleSheet.m_pageSize.SetHeight(wxAtoi(m_pageHeight));

    mStyleSheet.m_marginLeft = wxAtoi(m_marginLeft);
    mStyleSheet.m_marginTop = wxAtoi(m_marginTop);
    mStyleSheet.m_marginRight = wxAtoi(m_marginRight);
    mStyleSheet.m_marginBottom = wxAtoi(m_marginBottom);

    mStyleSheet.m_cols = wxAtoi(m_cols);
    mStyleSheet.m_lineSpacing = wxAtoi(m_lineSpacing);
    mStyleSheet.m_chordLineSpacing = wxAtoi(m_lineChordSpacing);
    mStyleSheet.m_indentChorus = wxAtoi(m_indentChorus);

    return result;
}

} // namespace
