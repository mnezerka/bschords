/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Songbook Properties Dialog Implementation
 */

#include <iostream>
#include <wx/treebook.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/statbox.h>
#include <wx/valgen.h>
#include "songbookdlg.h"

namespace bschords
{

BEGIN_EVENT_TABLE(SongbookDlg, wxDialog)
    EVT_CLOSE(SongbookDlg::OnClose)
END_EVENT_TABLE()

SongbookDlg::SongbookDlg(wxDialog *dlg, const wxString &title, SongBook& songbook) : wxDialog(dlg, -1, title), mSongbook(songbook)
{
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // create buttons
    wxSizer* btnSizer = CreateButtonSizer(wxOK | wxCANCEL);

    wxNotebook* m_bookCtrl = new wxNotebook(this, wxID_ANY); //, wxDefaultPosition, wxDefaultSize, wxWS_EX_VALIDATE_RECURSIVELY);

    wxPanel* pageGeneral = CreateGeneralPage(m_bookCtrl);
    wxPanel* pageView = CreateViewPage(m_bookCtrl);
    m_bookCtrl->AddPage(pageGeneral, _("General"), true);
    m_bookCtrl->AddPage(pageView, _("View"), false);

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

SongbookDlg::~SongbookDlg() { }

wxPanel* SongbookDlg::CreateGeneralPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    // name
    wxStaticBox* nameStaticBox = new wxStaticBox(panel, wxID_ANY, _("Sonbook Name"));
    wxBoxSizer* nameStaticBoxSizer = new wxStaticBoxSizer(nameStaticBox, wxVERTICAL );
    topSizer->Add(nameStaticBoxSizer, 0, wxGROW|wxALL, 0);
    mNameCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
    nameStaticBoxSizer->Add(mNameCtrl, 0, wxALL | wxEXPAND);

    // description
    wxStaticBox* descStaticBox = new wxStaticBox(panel, wxID_ANY, _("Sonbook Description"));
    wxBoxSizer* descStaticBoxSizer = new wxStaticBoxSizer(descStaticBox, wxVERTICAL );
    topSizer->Add(descStaticBoxSizer, 0, wxGROW|wxALL, 0);
    mDescriptionCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, 200), wxTE_MULTILINE);
    descStaticBoxSizer->Add(mDescriptionCtrl);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* SongbookDlg::CreateViewPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

void SongbookDlg::OnClose(wxCloseEvent &event)
{
    Destroy();
}

bool SongbookDlg::TransferDataToWindow()
{
    bool result = wxDialog::TransferDataToWindow();

    mNameCtrl->SetValue(mSongbook.getName());
    mDescriptionCtrl->SetValue(mSongbook.getDescription());

    return result;
}

bool SongbookDlg::TransferDataFromWindow()
{
    bool result = wxDialog::TransferDataFromWindow();

    mSongbook.setName(mNameCtrl->GetValue());
    mSongbook.setDescription(mDescriptionCtrl->GetValue());

    return result;
}

} // namespace bschords
