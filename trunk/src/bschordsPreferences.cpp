/***************************************************************
 * Name:      xxxMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2012-12-08
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <iostream>

#include "bschordsPreferences.h"
#include <wx/treebook.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/valgen.h>

BEGIN_EVENT_TABLE(bschordsPreferences, wxDialog)
    EVT_CLOSE(bschordsPreferences::OnClose)
    EVT_BUTTON(idBtnSelFont, bschordsPreferences::OnSelFont)
END_EVENT_TABLE()

bschordsPreferences::bschordsPreferences(wxDialog *dlg, const wxString &title)
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

bschordsPreferences::~bschordsPreferences() { }

wxPanel* bschordsPreferences::CreateGeneralPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

	/*
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

	// --------------------------------------------------------------------------------------
	// margins panel
	wxStaticBox* marginsStaticBox = new wxStaticBox(panel, wxID_ANY, _("XXX"));
    wxBoxSizer* marginsStaticBoxSizer = new wxStaticBoxSizer(marginsStaticBox, wxVERTICAL );
    item0->Add(marginsStaticBoxSizer, 0, wxGROW|wxALL, 5);

	marginsSizer->AddSpacer(1);
    marginsStaticBoxSizer->Add(marginsSizer, 0, wxALL, 3);

	// global stuff
    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);
	*/

    return panel;
}

wxPanel* bschordsPreferences::CreateViewPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

	// --------------------------------------------------------------------------------------
	// preview panel
	wxStaticBox* previewStaticBox = new wxStaticBox(panel, wxID_ANY, _("Preview"));
    wxBoxSizer* previewStaticBoxSizer = new wxStaticBoxSizer(previewStaticBox, wxVERTICAL );
    item0->Add(previewStaticBoxSizer, 0, wxGROW|wxALL, 0);

	previewStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show margins"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_showTsetMargins)));
	previewStaticBoxSizer->Add(new wxCheckBox(panel, wxID_ANY, wxT("Show typesetting blocks"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_showTsetBlocks)));

	// global stuff
    topSizer->Add(item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

wxPanel* bschordsPreferences::CreateEditorPage(wxWindow* parent)
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
	m_editorFontPreview->SetFont(wxGetApp().m_editorFont);
    m_editorFontPreview->SetBackgroundColour(wxColour(255, 255, 255));
	m_editorFontPreview->SetBackgroundStyle(wxBG_STYLE_COLOUR);
	fontStaticBoxSizer->Add(m_editorFontPreview, 1, wxALL | wxEXPAND, 5);

	// global stuff
    topSizer->Add(item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}

void bschordsPreferences::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void bschordsPreferences::OnSelFont(wxCommandEvent &event)
{
	// fonts stuff
	wxFontData fontData;
	fontData.SetInitialFont(wxGetApp().m_editorFont);
	wxFontDialog dialog(this, &fontData);
	if (dialog.ShowModal() == wxID_OK)
	{
		wxFontData retData = dialog.GetFontData();
		wxGetApp().m_editorFont = retData.GetChosenFont();
		//fontTextTitles[fontIx]->SetFont(m_fonts[fontIx]);
	}
}