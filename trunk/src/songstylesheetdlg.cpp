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

#include "songstylesheetdlg.h"
#include <wx/treebook.h>
//#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/fontdlg.h>
#include <wx/valgen.h>

BEGIN_EVENT_TABLE(SongStyleSheetDlg, wxDialog)
    EVT_CLOSE(SongStyleSheetDlg::OnClose)
    EVT_BUTTON(idBtnQuit, SongStyleSheetDlg::OnQuit)
    EVT_BUTTON(idBtnSelFont, SongStyleSheetDlg::OnSelFont)
END_EVENT_TABLE()

SongStyleSheetDlg::SongStyleSheetDlg(wxDialog *dlg, const wxString &title, SongStyleSheet *styleSheet)
    : wxDialog(dlg, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER), m_styleSheet(styleSheet)
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    // copy font information from application
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
         m_fonts[i] = wxGetApp().m_styleSheet.m_fonts[i];
    }

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // create buttons
    wxSizer* btnSizer = CreateButtonSizer(wxOK | wxCANCEL);

	wxNotebook* m_bookCtrl = new wxNotebook(this, wxID_ANY); //, wxDefaultPosition, wxDefaultSize, wxWS_EX_VALIDATE_RECURSIVELY);

	wxPanel* pagePageSetup = CreatePageSetupPage(m_bookCtrl);
	wxPanel* pageFonts = CreateFontsPage(m_bookCtrl);
	m_bookCtrl->AddPage(pagePageSetup, _("Page"), false);
	m_bookCtrl->AddPage(pageFonts, _("Fonts"), false);
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

	wxComboBox *comboPageSize = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100,-1));
	comboPageSize->Append(_T("User defined"));
	comboPageSize->Append(_T("A4 (210x219 mm)"));
	comboPageSize->Select(0);
	pageStaticBoxSizer->Add(comboPageSize);

	pageStaticBoxSizer->Add(new wxStaticText(panel, wxID_ANY, _("=")));
	pageStaticBoxSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_pageWidth)));
	pageStaticBoxSizer->Add(new wxStaticText(panel, wxID_ANY, _(" x ")));
	pageStaticBoxSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_pageHeight)));

	// --------------------------------------------------------------------------------------
	// margins panel
	wxStaticBox* marginsStaticBox = new wxStaticBox(panel, wxID_ANY, _("Margins (milimeters)"));
    wxBoxSizer* marginsStaticBoxSizer = new wxStaticBoxSizer(marginsStaticBox, wxVERTICAL );
    item0->Add(marginsStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxGridSizer* marginsSizer = new wxGridSizer(3, 3, 4, 4);
	marginsSizer->AddSpacer(1);
	marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginTop)));
	marginsSizer->AddSpacer(1);
	marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginLeft)));
	wxPanel *whitePagePanel = new wxPanel(panel);
    whitePagePanel->SetBackgroundColour(wxColour(255, 255, 255));
	whitePagePanel->SetBackgroundStyle(wxBG_STYLE_COLOUR);
    marginsSizer->Add(whitePagePanel, 0, wxALL | wxEXPAND);
	marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginRight)));
	marginsSizer->AddSpacer(1);
	marginsSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_marginBottom)));
    marginsStaticBoxSizer->Add(marginsSizer, 0, wxALL, 3);

	// --------------------------------------------------------------------------------------
	// lines etc..
	wxStaticBox* linesStaticBox = new wxStaticBox(panel, wxID_ANY, _("Text and chord lines (mm)"));
    wxBoxSizer* linesStaticBoxSizer = new wxStaticBoxSizer(linesStaticBox, wxVERTICAL );
    item1->Add(linesStaticBoxSizer, 0, wxGROW|wxALL, 5);

    wxGridSizer* linesSizer = new wxGridSizer(2, 4, 4);
	linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Columns:")));
	wxComboBox *comboCols = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord));
	comboCols->SetValidator(wxGenericValidator(&m_cols));
	comboCols->Append(_T("1"));
	comboCols->Append(_T("2"));
	comboCols->Append(_T("3"));
	comboCols->Append(_T("4"));
	linesSizer->Add(comboCols);

	linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Line spacing:")));
	linesSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_lineSpacing)));

    linesSizer->Add(new wxStaticText(panel, wxID_ANY, _("Chord line spacing:")));
	linesSizer->Add(new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, wxDefaultCoord), 0, wxTextValidator(wxFILTER_NUMERIC, &m_lineChordSpacing)));
    linesStaticBoxSizer->Add(linesSizer, 0, wxALL, 3);

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
        fontSelType->Append(fontNames[i]);
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
        fontTextTitles[i] = new wxStaticText(fontsPreviewPanel, wxID_ANY, fontNames[i]);
        fontTextTitles[i]->SetFont(m_fonts[i]);
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

SongStyleSheetDlg::~SongStyleSheetDlg()
{
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

bool SongStyleSheetDlg::TransferDataToWindow()
{
    m_pageWidth << m_styleSheet->m_pageSize.GetWidth();
    m_pageHeight << m_styleSheet->m_pageSize.GetHeight();

	m_marginLeft << m_styleSheet->m_marginLeft;
	m_marginTop << m_styleSheet->m_marginTop;
	m_marginRight << m_styleSheet->m_marginRight;
	m_marginBottom << m_styleSheet->m_marginBottom;

	m_cols << m_styleSheet->m_cols;
	m_lineSpacing << m_styleSheet->m_lineSpacing;
	m_lineChordSpacing << m_styleSheet->m_chordLineSpacing;

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

	m_styleSheet->m_pageSize.SetWidth(wxAtoi(m_pageWidth));
	m_styleSheet->m_pageSize.SetHeight(wxAtoi(m_pageHeight));

	m_styleSheet->m_marginLeft = wxAtoi(m_marginLeft);
	m_styleSheet->m_marginTop = wxAtoi(m_marginTop);
	m_styleSheet->m_marginRight = wxAtoi(m_marginRight);
	m_styleSheet->m_marginBottom = wxAtoi(m_marginBottom);

	m_styleSheet->m_cols = wxAtoi(m_cols);
	m_styleSheet->m_lineSpacing = wxAtoi(m_lineSpacing);
	m_styleSheet->m_chordLineSpacing = wxAtoi(m_lineChordSpacing);


	return result;
}
