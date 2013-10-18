/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Preferences Dialog Implemetation
 */

#include <iostream>
#include <wx/treebook.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/valgen.h>

#include "preferencesdlg.h"

namespace bschords
{

enum
{
    idBtnSelFont = 1000,
    idBtnSelColorText,
    idBtnSelColorChords,
    idBtnSelColorCommands,
    idBtnSelRootPath,
};


BEGIN_EVENT_TABLE(PreferencesDlg, wxDialog)
    EVT_CLOSE(PreferencesDlg::OnClose)
    EVT_BUTTON(idBtnSelFont, PreferencesDlg::OnSelFont)
    EVT_BUTTON(idBtnSelColorText, PreferencesDlg::OnSelColor)
    EVT_BUTTON(idBtnSelColorChords, PreferencesDlg::OnSelColor)
    EVT_BUTTON(idBtnSelColorCommands, PreferencesDlg::OnSelColor)
    EVT_BUTTON(idBtnSelRootPath, PreferencesDlg::OnSelRootPath)
END_EVENT_TABLE()

PreferencesDlg::PreferencesDlg(wxDialog *dlg, const wxString &title)
    : wxDialog(dlg, -1, title)
{
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // create buttons
    wxSizer* btnSizer = CreateButtonSizer(wxOK | wxCANCEL);

    wxNotebook* m_bookCtrl = new wxNotebook(this, wxID_ANY); //, wxDefaultPosition, wxDefaultSize, wxWS_EX_VALIDATE_RECURSIVELY);

    wxPanel* pageGeneral = CreateGeneralPage(m_bookCtrl);
    wxPanel* pageView = CreateViewPage(m_bookCtrl);
    wxPanel* pageEditor = CreateEditorPage(m_bookCtrl);
    m_bookCtrl->AddPage(pageGeneral, _("General"), true);
    m_bookCtrl->AddPage(pageView, _("View"), false);
    m_bookCtrl->AddPage(pageEditor, _("Editor"), false);

    mainSizer->Add(m_bookCtrl, 0, wxALL | wxEXPAND, 5);

    // static line
    wxStaticLine * staticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    mainSizer->Add(staticLine1, 0, wxALL | wxEXPAND, 5);

    // sizing stuff
    mainSizer->Add(btnSizer, 0, wxALL, 3);

    this->SetSizer(mainSizer);
    this->Layout();
    mainSizer->Fit(this);
}

PreferencesDlg::~PreferencesDlg() { }

wxPanel* PreferencesDlg::CreateGeneralPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    // root path
    wxStaticBox* rootPathStaticBox = new wxStaticBox(panel, wxID_ANY, _("Root path"));
    wxBoxSizer* rootPathStaticBoxSizer = new wxStaticBoxSizer(rootPathStaticBox, wxVERTICAL );
    topSizer->Add(rootPathStaticBoxSizer, 0, wxGROW|wxALL, 0);

    rootPathStaticBoxSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Song repository root path:")), 0, wxALL | wxEXPAND, 3);

    wxBoxSizer *rootPathSizer = new wxBoxSizer( wxHORIZONTAL );
    m_rootPathCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&(wxGetApp().m_settings->m_rootPath)));
    rootPathSizer->Add(m_rootPathCtrl, 1, wxALL | wxEXPAND | wxCENTER);
    rootPathSizer->AddSpacer(5);
    rootPathSizer->Add(new wxButton(panel, idBtnSelRootPath, wxT("..."), wxDefaultPosition, wxSize(30, wxDefaultCoord)), 0, wxALL | wxEXPAND | wxCENTER);
    rootPathStaticBoxSizer->Add(rootPathSizer, 1, wxALL | wxEXPAND, 3);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* PreferencesDlg::CreateViewPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    // --------------------------------------------------------------------------------------
    // preview panel
    wxStaticBox* previewStaticBox = new wxStaticBox(panel, wxID_ANY, _("Preview"));
    wxBoxSizer* previewStaticBoxSizer = new wxStaticBoxSizer(previewStaticBox, wxVERTICAL );
    item0->Add(previewStaticBoxSizer, 0, wxGROW|wxALL, 0);

    previewStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show margins"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_showTsetMargins)), 1, wxEXPAND | wxALL, 5);
    previewStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show typesetting blocks (colored bounding box)"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_showTsetBlocks)), 1, wxEXPAND | wxALL, 5);
    previewStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show typesetting clipping (red line on right side of clipped region)"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_showTsetClipping)), 1, wxEXPAND | wxALL, 5);

    // global stuff
    topSizer->Add(item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* PreferencesDlg::CreateEditorPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    // --------------------------------------------------------------------------------------
    // font panel
    wxStaticBox* fontStaticBox = new wxStaticBox(panel, wxID_ANY, _("Font"));
    wxBoxSizer* fontStaticBoxSizer = new wxStaticBoxSizer(fontStaticBox, wxVERTICAL );
    item0->Add(fontStaticBoxSizer, 0, wxGROW|wxALL, 0);

    fontStaticBoxSizer->Add(new wxButton(panel, idBtnSelFont, _("Font...")));
    m_editorFontPreview = new wxStaticText(panel, wxID_ANY, wxT("This is editor font"), wxDefaultPosition, wxSize(300, wxDefaultCoord));
    m_editorFontPreview->SetFont(wxGetApp().m_settings->m_editorFont);
    m_editorFontPreview->SetBackgroundColour(wxColour(255, 255, 255));
    m_editorFontPreview->SetBackgroundStyle(wxBG_STYLE_COLOUR);
    fontStaticBoxSizer->Add(m_editorFontPreview, 1, wxALL | wxEXPAND, 5);

    // --------------------------------------------------------------------------------------
    // colors panel
    wxStaticBox* colorsStaticBox = new wxStaticBox(panel, wxID_ANY, _("Colors"));
    wxBoxSizer* colorsStaticBoxSizer = new wxStaticBoxSizer(colorsStaticBox, wxVERTICAL );
    item0->Add(colorsStaticBoxSizer, 0, wxGROW|wxALL, 0);

    wxBitmap b = createColorBitmap(*wxBLACK);
    wxSize sizeBB(50, 25);

    //wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
    //m_sash_color->SetBitmapLabel(CreateColorBitmap(sash));

    wxBoxSizer* s1 = new wxBoxSizer(wxHORIZONTAL);
    m_btnEditorColorText = new wxBitmapButton(panel, idBtnSelColorText, b, wxDefaultPosition, sizeBB);
    s1->Add(new wxStaticText(panel, wxID_ANY, wxT("Text color:")));
    s1->Add(1, 1, 1, wxEXPAND);
    s1->Add(m_btnEditorColorText);
    s1->SetItemMinSize((size_t)1, 180, 20);
    colorsStaticBoxSizer->Add(s1, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
    m_btnEditorColorChords = new wxBitmapButton(panel, idBtnSelColorChords, b, wxDefaultPosition, sizeBB);
    s2->Add(new wxStaticText(panel, wxID_ANY, wxT("Chords color:")));
    s2->Add(1, 1, 1, wxEXPAND);
    s2->Add(m_btnEditorColorChords);
    s2->SetItemMinSize((size_t)1, 180, 20);
    colorsStaticBoxSizer->Add(s2, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
    m_btnEditorColorCommands = new wxBitmapButton(panel, idBtnSelColorCommands, b, wxDefaultPosition, sizeBB);
    s3->Add(new wxStaticText(panel, wxID_ANY, wxT("Commands color:")));
    s3->Add(1, 1, 1, wxEXPAND);
    s3->Add(m_btnEditorColorCommands);
    s3->SetItemMinSize((size_t)1, 180, 20);
    colorsStaticBoxSizer->Add(s3, 1, wxALL | wxEXPAND, 5);

    // --------------------------------------------------------------------------------------
    // global stuff
    topSizer->Add(item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    // update dialog button colors
    updateColors();

    return panel;
}

void PreferencesDlg::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void PreferencesDlg::OnSelFont(wxCommandEvent &event)
{
    // fonts stuff
    wxFontData fontData;
    fontData.SetInitialFont(wxGetApp().m_settings->m_editorFont);
    wxFontDialog dialog(this, fontData);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        wxGetApp().m_settings->m_editorFont = retData.GetChosenFont();
        m_editorFontPreview->SetFont(wxGetApp().m_settings->m_editorFont);
    }
}

void PreferencesDlg::OnSelColor(wxCommandEvent& event)
{
    wxColourData cd;
    cd.SetChooseFull(true);
    wxString title = _("Color Picker");
    switch (event.GetId())
    {
    case idBtnSelColorText:
        cd.SetColour(m_editorColorText);
        title = _("Text color picker");
        break;
    case idBtnSelColorChords:
        cd.SetColour(m_editorColorChords);
        title = _("Chords color picker");
        break;
    case idBtnSelColorCommands:
        cd.SetColour(m_editorColorCommands);
        title = _("Commands color picker");
        break;
    }

    wxColourDialog dlg(this, &cd);
    dlg.SetTitle(title);
    if (dlg.ShowModal() != wxID_OK)
        return;

    switch (event.GetId())
    {
    case idBtnSelColorText:
        m_editorColorText = dlg.GetColourData().GetColour();
        break;
    case idBtnSelColorChords:
        m_editorColorChords = dlg.GetColourData().GetColour();
        break;
    case idBtnSelColorCommands:
        m_editorColorCommands = dlg.GetColourData().GetColour();
        break;
    }

    // update dialog button colors
    updateColors();
}

void PreferencesDlg::OnSelRootPath(wxCommandEvent& event)
{
    wxDirDialog dlg(this, _("Choose root song directory"), m_rootPathCtrl->GetValue());
    if (dlg.ShowModal() != wxID_OK)
        return;
    m_rootPathCtrl->SetValue(dlg.GetPath());
}

// updates colors of appropriate dialog buttons
void PreferencesDlg::updateColors()
{
    m_btnEditorColorText->SetBitmapLabel(createColorBitmap(m_editorColorText));
    m_btnEditorColorChords->SetBitmapLabel(createColorBitmap(m_editorColorChords));
    m_btnEditorColorCommands->SetBitmapLabel(createColorBitmap(m_editorColorCommands));
}

bool PreferencesDlg::TransferDataToWindow()
{
    m_editorColorText = wxGetApp().m_settings->m_editorColorText;
    m_editorColorChords = wxGetApp().m_settings->m_editorColorChords;
    m_editorColorCommands = wxGetApp().m_settings->m_editorColorCommands;

    bool result = wxDialog::TransferDataToWindow();

    updateColors();

    return result;
}

bool PreferencesDlg::TransferDataFromWindow()
{
    bool result = wxDialog::TransferDataFromWindow();

    wxGetApp().m_settings->m_editorColorText = m_editorColorText;
    wxGetApp().m_settings->m_editorColorChords = m_editorColorChords;
    wxGetApp().m_settings->m_editorColorCommands = m_editorColorCommands;

    return result;
}

wxBitmap PreferencesDlg::createColorBitmap(const wxColour& c)
{
    wxImage image;
    image.Create(25,14);
    for (int x = 0; x < 25; ++x)
        for (int y = 0; y < 14; ++y)
        {
            wxColour pixcol = c;
            if (x == 0 || x == 24 || y == 0 || y == 13)
                pixcol = *wxBLACK;
            image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
        }
    return wxBitmap(image);
}

} // namespace
