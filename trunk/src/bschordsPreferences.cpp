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

BEGIN_EVENT_TABLE(bschordsPreferences, wxDialog)
    EVT_CLOSE(bschordsPreferences::OnClose)
    EVT_BUTTON(idBtnQuit, bschordsPreferences::OnQuit)
    EVT_BUTTON(idBtnAbout, bschordsPreferences::OnAbout)
    EVT_BUTTON(idBtnSelFont, bschordsPreferences::OnSelFont)
END_EVENT_TABLE()

bschordsPreferences::bschordsPreferences(wxDialog *dlg, const wxString &title)
    : wxDialog(dlg, -1, title)
{

    // copy font information from application
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
         m_fonts[i] = wxGetApp().m_fonts[i];
    }

	// create a dummy image list with a few icons
    const wxSize imageSize(32, 32);

    wxImageList *m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
	m_imageList->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
	m_imageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
    m_imageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
    m_imageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));

	//this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // create buttons
    wxSizer* btnSizer = CreateButtonSizer(wxOK | wxCANCEL);

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

    //////////////////////// fonts panel ////////////////////////////////
	wxPanel *fontsPanel = new wxPanel(m_bookCtrl);
    wxBoxSizer* fontsPanelSizer = new wxBoxSizer(wxVERTICAL);

    // font selection controls
    wxPanel *fontSelPanel = new wxPanel(fontsPanel);
    wxBoxSizer* fontSelSizer = new wxBoxSizer(wxHORIZONTAL);
	fontSelType = new wxComboBox(fontSelPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100,-1) );
    for (int i = 0; i < BS_FONT_LAST; i++)
        fontSelType->Append(fontNames[i]);
	fontSelType->Select(0);
	fontSelSizer->Add(fontSelType, 0, wxALL | wxALIGN_CENTER, 2);
    wxButton *fontSelButton = new wxButton(fontSelPanel, idBtnSelFont, _("Font..."));
    fontSelSizer->Add(fontSelButton, 0, wxALL | wxALIGN_CENTER, 5);
    fontSelSizer->Add(new wxStaticText(fontSelPanel, wxID_ANY, _("Select font")), 0, wxALL | wxALIGN_CENTER, 10);
    //fontSelSizer->SetSizeHints(fontSelPanel);

    fontSelPanel->SetSizer(fontSelSizer);
    fontSelSizer->Layout();
    fontsPanelSizer->Add(fontSelPanel, 0, wxEXPAND);

    // font preview controls
    wxPanel *fontsPreviewPanel = new wxPanel(fontsPanel);
    fontsPreviewPanel->SetBackgroundColour(wxColour(255, 255, 255));
	fontsPreviewPanel->SetBackgroundStyle(wxBG_STYLE_COLOUR);
    fontsPanelSizer->Add(fontsPreviewPanel, 1, wxALL | wxEXPAND, 5);
    wxBoxSizer* fontsPreviewPanelSizer = new wxBoxSizer(wxVERTICAL);

    // create test tests to demonstrate font look
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
        fontTextTitles[i] = new wxStaticText(fontsPreviewPanel, wxID_ANY, fontNames[i]);
        fontTextTitles[i]->SetFont(m_fonts[i]);
        fontsPreviewPanelSizer->Add(fontTextTitles[i], 0, wxALL | wxALIGN_CENTER, 10);
    }
    fontsPreviewPanel->SetSizer(fontsPreviewPanelSizer);
    fontsPreviewPanel->Layout();
    fontsPanel->SetSizer(fontsPanelSizer);
    fontsPanel->Layout();

	m_bookCtrl->AddPage(panel1, _("General"), false, 0);
	m_bookCtrl->AddPage(fontsPanel, _("Fonts"), false, 1);

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

void bschordsPreferences::OnSelFont(wxCommandEvent &event)
{
    int fontIx = fontSelType->GetSelection();
    if (fontIx != wxNOT_FOUND && fontIx >= 0 && fontIx < BS_FONT_LAST)
    {
        std::cout << "selected item is " << fontIx << std::endl;

        // fonts stuff
        wxFontData fontData;
        fontData.SetInitialFont(m_fonts[fontIx]);
        //data.SetColour(canvasTextColour);
        wxFontDialog dialog(this, &fontData);
        if (dialog.ShowModal() == wxID_OK)
        {
            wxFontData retData = dialog.GetFontData();
            m_fonts[fontIx] = retData.GetChosenFont();
            fontTextTitles[fontIx]->SetFont(m_fonts[fontIx]);
        }
    }
}
