/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Song Properties Dialog Implementation
 */

#include <iostream>
#include <wx/treebook.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/statbox.h>
#include <wx/valgen.h>
#include "songdlg.h"

namespace bschords
{

BEGIN_EVENT_TABLE(SongDlg, wxDialog)
    EVT_CLOSE(SongDlg::OnClose)
END_EVENT_TABLE()

SongDlg::SongDlg(wxDialog *dlg, const wxString &title, SongBookItem& songbookItem) : wxDialog(dlg, -1, title), mSongbookItem(songbookItem)
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

SongDlg::~SongDlg() { }

wxPanel* SongDlg::CreateGeneralPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    // description
    wxStaticBox* descStaticBox = new wxStaticBox(panel, wxID_ANY, _("Song Comment"));
    wxBoxSizer* descStaticBoxSizer = new wxStaticBoxSizer(descStaticBox, wxVERTICAL );
    topSizer->Add(descStaticBoxSizer, 0, wxGROW|wxALL, 0);
    mCommentCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400, 200), wxTE_MULTILINE);
    descStaticBoxSizer->Add(mCommentCtrl);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* SongDlg::CreateViewPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

void SongDlg::OnClose(wxCloseEvent &event)
{
    Destroy();
}

bool SongDlg::TransferDataToWindow()
{
    bool result = wxDialog::TransferDataToWindow();

    mCommentCtrl->SetValue(mSongbookItem.getComment());

    return result;
}

bool SongDlg::TransferDataFromWindow()
{
    bool result = wxDialog::TransferDataFromWindow();

    mSongbookItem.setComment(mCommentCtrl->GetValue());

    return result;
}

} // namespace bschords
