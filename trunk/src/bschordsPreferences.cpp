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

#include "bschordsPreferences.h"
#include <wx/treebook.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>

BEGIN_EVENT_TABLE(bschordsPreferences, wxDialog)
    EVT_CLOSE(bschordsPreferences::OnClose)
    EVT_BUTTON(idBtnQuit, bschordsPreferences::OnQuit)
    EVT_BUTTON(idBtnAbout, bschordsPreferences::OnAbout)
END_EVENT_TABLE()

bschordsPreferences::bschordsPreferences(wxDialog *dlg, const wxString &title)
    : wxDialog(dlg, -1, title)
{
    /*this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxHORIZONTAL);
    m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Welcome To\nwxWidgets"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText1->SetFont(wxFont(20, 74, 90, 90, false, wxT("Arial")));
    bSizer1->Add(m_staticText1, 0, wxALL|wxEXPAND, 5);
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxVERTICAL);
    BtnAbout = new wxButton(this, idBtnAbout, wxT("&About"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer2->Add(BtnAbout, 0, wxALL, 5);
    m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bSizer2->Add(m_staticline1, 0, wxALL|wxEXPAND, 5);
    BtnQuit = new wxButton(this, idBtnQuit, wxT("&Quit"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer2->Add(BtnQuit, 0, wxALL, 5);
    bSizer1->Add(bSizer2, 1, wxEXPAND, 5);
    this->SetSizer(bSizer1);
    this->Layout();
    bSizer1->Fit(this);*/

	// create a dummy image list with a few icons
    const wxSize imageSize(32, 32);

    wxImageList *m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
	m_imageList->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
	m_imageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
    m_imageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
    m_imageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	// create buttons

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *BtnCancel = new wxButton(this, wxID_ANY, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton *BtnOk = new wxButton(this, wxID_ANY, wxT("&Ok"), wxDefaultPosition, wxDefaultSize, 0);
    btnSizer->Add(BtnCancel, 0, wxALL, 5);
    btnSizer->Add(BtnOk, 0, wxALL, 5);

	// create tree control
    wxTreebook* m_bookCtrl = new wxTreebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    m_bookCtrl->SetImageList(m_imageList);

	wxPanel *panel1 = new wxPanel(m_bookCtrl);
	wxBoxSizer* panel1Sizer = new wxBoxSizer(wxVERTICAL);
    wxButton* btn1 = new wxButton(panel1, wxID_ANY, wxT("Button 1"), wxPoint(10, 10), wxDefaultSize );
	wxStaticText *st1 = new wxStaticText(panel1, wxID_ANY, _("Welcome To\nwxWidgets\nThis is example of first dialog line\nhere will be added some controls"), wxDefaultPosition, wxDefaultSize, 0);

	panel1Sizer->Add(btn1);
	panel1Sizer->Add(st1);
	panel1->SetSizer(panel1Sizer);

	wxPanel *panel2 = new wxPanel(m_bookCtrl);
    (void) new wxButton( panel2, wxID_ANY, wxT("Button 2"), wxPoint(10, 10), wxDefaultSize );

	m_bookCtrl->AddPage(panel1, _("General"), false, 0);
	m_bookCtrl->AddPage(panel2, _("Page"), false, 1);

	mainSizer->Add(m_bookCtrl, 0, wxALL | wxEXPAND, 5);

	// static line
	wxStaticLine * staticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	mainSizer->Add(staticLine1, 0, wxALL | wxEXPAND, 5);

	// sizing stuff
	mainSizer->Add(btnSizer);
    this->SetSizer(mainSizer);
    this->Layout();
    mainSizer->Fit(this);
}


bschordsPreferences::~bschordsPreferences()
{
}

void bschordsPreferences::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void bschordsPreferences::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void bschordsPreferences::OnAbout(wxCommandEvent &event)
{
    //wxString msg = wxbuildinfo(long_f);
    wxMessageBox(_("Text"), _("Welcome to..."));
}
