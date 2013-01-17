/***************************************************************
 * Name:      bschordsMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2012-12-01
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

#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/dir.h>
#include <wx/toolbar.h>
#include <wx/spinctrl.h>
#include <wx/srchctrl.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/printdlg.h>
#include <wx/artprov.h>

#include "bschordsMain.h"
#include "bschordsPreferences.h"
#include "songstylesheetdlg.h"
#include "bschordsicon.xpm"
#include "bschordsdcpainter.h"

using namespace bschords;

//helper functions
enum wxbuildinfoformat {
	short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
	wxString wxbuild(wxVERSION_STRING);

	if (format == long_f )
	{
#if defined(__WXMSW__)
		wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
		wxbuild << _T("-Mac");
#elif defined(__UNIX__)
		wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
		wxbuild << _T("-Unicode build");
#else
		wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
	}

	return wxbuild;
}

/*
#include "res/new.xpm"
#include "res/open.xpm"
#include "res/save.xpm"
#include "res/copy.xpm"
#include "res/cut.xpm"
#include "res/preview.xpm"  // paste XPM
#include "res/print.xpm"
#include "res/help.xpm"
#include "res/chord.xpm"
*/

enum
{
	idMenuQuit = 1000,
	idMenuAbout,
	idMenuPreferences,
	ID_MENU_STYLESHEET,

	idMenuFileNewSongBook,
	idMenuFileOpenSongBook,
	idMenuFileSaveSongBook,
	idMenuFileSaveAsSongBook,
	idMenuFileCloseSongBook,

	idMenuViewFileBrowser,
	idMenuViewEditor,
	idMenuViewSongPreview,
	idMenuViewSongBook,
	idMenuViewTbMain,
	idMenuViewTbChords,

	idMenuSongInsertChorus,
	idMenuSongInsertTab,
	idMenuSongAddToSongbook,

	idFSBrowserAddToSongbook,

	ID_COMBO_CHORD,
	ID_COMBO_CMD,
	//idMenuViewTSetBook,
	ID_MENU_FILE_EXPORT,
	ID_FSBROWSER,
	ID_TOOLBAR_CHORD,
	ID_SONG_EDITOR,
};

BEGIN_EVENT_TABLE(bschordsFrame, wxFrame)
	EVT_CLOSE(bschordsFrame::OnClose)
	EVT_ERASE_BACKGROUND(bschordsFrame::OnEraseBackground)
	EVT_SIZE(bschordsFrame::OnSize)
	EVT_MENU(wxID_NEW, bschordsFrame::OnFileNewSong)
	EVT_MENU(wxID_OPEN, bschordsFrame::OnFileOpenSong)
	EVT_MENU(wxID_SAVE, bschordsFrame::OnFileSaveSong)
	EVT_MENU(wxID_SAVEAS, bschordsFrame::OnFileSaveAsSong)
	EVT_MENU(wxID_CLOSE, bschordsFrame::OnFileCloseSong)
	EVT_MENU(idMenuFileNewSongBook, bschordsFrame::OnFileNewSongBook)
	EVT_MENU(idMenuFileOpenSongBook, bschordsFrame::OnFileOpenSongBook)
	EVT_MENU(idMenuFileSaveSongBook, bschordsFrame::OnFileSaveSongBook)
	EVT_MENU(idMenuFileSaveAsSongBook, bschordsFrame::OnFileSaveAsSongBook)
	EVT_MENU(idMenuFileCloseSongBook, bschordsFrame::OnFileCloseSongBook)
	EVT_MENU(ID_MENU_FILE_EXPORT, bschordsFrame::OnFileExportSong)
	EVT_MENU(wxID_PRINT, bschordsFrame::OnFilePrint)
	EVT_MENU(wxID_PREVIEW, bschordsFrame::OnFilePrintPreview)
	EVT_MENU(wxID_PRINT_SETUP, bschordsFrame::OnFilePageSetup)
	EVT_MENU(idMenuQuit, bschordsFrame::OnQuit)
	EVT_MENU(idMenuPreferences, bschordsFrame::OnPreferences)
	EVT_MENU(ID_MENU_STYLESHEET, bschordsFrame::OnStyleSheet)
	EVT_MENU(idMenuAbout, bschordsFrame::OnAbout)
	EVT_MENU(idMenuViewFileBrowser, bschordsFrame::OnViewPane)
	EVT_MENU(idMenuViewEditor, bschordsFrame::OnViewPane)
	EVT_MENU(idMenuViewSongPreview, bschordsFrame::OnViewPane)
	EVT_MENU(idMenuViewSongBook, bschordsFrame::OnViewPane)
	EVT_MENU(idMenuViewTbMain, bschordsFrame::OnViewToolbar)
	EVT_MENU(idMenuViewTbChords, bschordsFrame::OnViewPane)
	EVT_MENU(idMenuSongInsertChorus, bschordsFrame::OnSongInsert)
	EVT_MENU(idMenuSongInsertTab, bschordsFrame::OnSongInsert)
	EVT_MENU(idMenuSongAddToSongbook, bschordsFrame::OnSongAddToSongbook)
	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, bschordsFrame::OnSongContentChange)
	EVT_STC_MODIFIED(ID_SONG_EDITOR, bschordsFrame::OnSongEditorChange)
	/*EVT_TREE_SEL_CHANGED(wxID_TREECTRL, bschordsFrame::OnFSBrowserSelChanged)*/
	EVT_TREE_ITEM_ACTIVATED(wxID_TREECTRL, bschordsFrame::OnFSBrowserSelChanged)
	EVT_TREE_ITEM_MENU(wxID_TREECTRL, bschordsFrame::OnFSBrowserItemMenu)
	EVT_MENU(idFSBrowserAddToSongbook, bschordsFrame::OnFSBrowserItemAddToSongbook)
	EVT_AUI_PANE_CLOSE(bschordsFrame::OnPaneClose)
	EVT_COMBOBOX(ID_COMBO_CHORD, bschordsFrame::OnChordProToken)
	EVT_COMBOBOX(ID_COMBO_CMD, bschordsFrame::OnChordProToken)
END_EVENT_TABLE()

bschordsFrame::bschordsFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title), m_isInEditMode(false)
{
	SetIcon(wxICON(bschordsicon));

	// tell wxAuiManager to manage this frame
    m_auiMgr.SetManagedWindow(this);

	// create a menu bar
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_T(""));
	fileMenu->Append(wxID_NEW, _("&New song"), _("Create a new song file"));
	fileMenu->Append(wxID_OPEN, _("&Open song..."), _("Open a song file"));
	fileMenu->Append(wxID_SAVE, _("&Save song"), _("Save the active song file"));
	fileMenu->Append(wxID_SAVEAS, _("Save song &as..."), _("Save the active song under different a name"));
	fileMenu->Append(wxID_CLOSE, _("&Close song"), _("Close song file"));
	fileMenu->AppendSeparator();

	fileMenu->Append(idMenuFileNewSongBook, _("New songbook"), _("Create a new songbook file"));
	fileMenu->Append(idMenuFileOpenSongBook, _("Open songbook ..."), _("Open a songbook file"));
	fileMenu->Append(idMenuFileSaveSongBook, _("Save songbook"), _("Save the active songbook file"));
	fileMenu->Append(idMenuFileSaveAsSongBook, _("Save songbook as..."), _("Save the active songbook under different a name"));
	fileMenu->Append(idMenuFileCloseSongBook, _("Close songbook"), _("Close songbook file"));
	fileMenu->AppendSeparator();

	fileMenu->Append(ID_MENU_FILE_EXPORT, _("&Export to PDF ..."), _("Export song file to PDF"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_PRINT_SETUP, _("Page setup..."), _("Page setup for printing"));
    fileMenu->Append(wxID_PRINT, _("&Print..."), _("Print"));
    fileMenu->Append(wxID_PREVIEW, _("Print Pre&view"), _("Preview"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_ANY, _("Song &information..."), _("Close song file"));
	fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
	mbar->Append(fileMenu, _("&File"));

	wxMenu* editMenu = new wxMenu(_T(""));
	editMenu->Append(idMenuPreferences, _("&Preferences..."), _("Application preferences"));
	editMenu->Append(ID_MENU_STYLESHEET, _("&Song Stylesheet..."), _("Song Stylesheet preferences"));
	mbar->Append(editMenu, _("&Edit"));

	wxMenu* viewMenu = new wxMenu(_T(""));
	viewMenu->Append(idMenuViewFileBrowser, _("&File browser"), _("View file broser"), true);
	viewMenu->Append(idMenuViewEditor, _("&Editor"), _("Song editor"), true);
	viewMenu->Append(idMenuViewSongPreview, _("Song &Preview"), _("View preview of the song"), true);
	viewMenu->Append(idMenuViewSongBook, _("Song &Book"), _("View song book"), true);
	viewMenu->AppendSeparator();
	viewMenu->Append(idMenuViewTbMain, _("Main toolbar"), _("View main toolbar"), true);
	viewMenu->Append(idMenuViewTbChords, _("Chords toolbar"), _("View chords toolbar"), true);
	mbar->Append(viewMenu, _("&View"));

	wxMenu* songMenu = new wxMenu(_T(""));
	songMenu->Append(idMenuSongInsertChorus, _("&Insert chorus"), _("Insert chorus section"));
	songMenu->Append(idMenuSongInsertTab, _("Insert &tab"), _("Insert tab section"));
	songMenu->AppendSeparator();
	songMenu->Append(idMenuSongAddToSongbook, _("Add to songbook"), _("Add item to current songbook"));

	songMenu->Append(wxID_ANY, _("Trans&pose.."), _("Transpose song chords to different key"));
	mbar->Append(songMenu, _("&Song"));

	wxMenu* songbookMenu = new wxMenu(_T(""));
	songbookMenu->Append(wxID_ANY, _("&Songbook properties..."), _("Song book properties"));
	mbar->Append(songbookMenu, _("Song&book"));

	wxMenu* helpMenu = new wxMenu(_T(""));
	helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
	mbar->Append(helpMenu, _("&Help"));

	// these items should be initially checked
	mbar->Check(idMenuViewFileBrowser, true);
	mbar->Check(idMenuViewEditor, true);
	mbar->Check(idMenuViewSongPreview, true);
	mbar->Check(idMenuViewSongBook, true);

	mbar->Check(idMenuViewTbMain, true);
	mbar->Check(idMenuViewTbChords, true);

	SetMenuBar(mbar);

	// -------------------- Create the toolbar
	// create some toolbars
    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->SetToolBitmapSize(wxSize(16, 16));
    tb1->AddTool(wxID_NEW, _("New song file"), wxArtProvider::GetBitmap(wxART_NEW, wxART_OTHER, wxSize(16, 16)));
    tb1->AddTool(wxID_OPEN, _("Open song file"), wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 16)));
    tb1->AddTool(wxID_SAVE, _("Save song file"), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, wxSize(16, 16)));
    tb1->Realize();
    m_auiMgr.AddPane(tb1, wxAuiPaneInfo().Name(wxT("toolbar-main")).Caption(wxT("Mainx Toolbar")).ToolbarPane().Top().Row(0).LeftDockable(false).RightDockable(false));

	tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_HORZ_TEXT);
    tb2->SetToolBitmapSize(wxSize(16,16));
    wxBitmap tb2_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16,16));

	m_chordCtrl = new wxComboBox(tb2, ID_COMBO_CHORD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	m_chordCtrl->Append(_("Chords"));
	m_chordCtrl->Append(_("[]"));
	m_chordCtrl->Append(_("[C]"));
	m_chordCtrl->Append(_("[Dm]"));
	m_chordCtrl->Append(_("[Em]"));
	m_chordCtrl->Append(_("[F]"));
	m_chordCtrl->Append(_("[G]"));
	m_chordCtrl->Append(_("[Am]"));
	m_chordCtrl->Append(_("[Hm5b]"));
	m_chordCtrl->SetSelection(0);
	tb2->AddControl(m_chordCtrl);

	m_cmdCtrl = new wxComboBox(tb2, ID_COMBO_CMD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	m_cmdCtrl->Append(_("Commands"));
	m_cmdCtrl->Append(_("{title: }"));
	m_cmdCtrl->Append(_("{subtitle: }"));
	m_cmdCtrl->Append(_("{start_of_chorus}"));
	m_cmdCtrl->Append(_("{end_of_chorus}"));
	m_cmdCtrl->Append(_("{start_of_tab}"));
	m_cmdCtrl->Append(_("{end_of_tab}"));
	m_cmdCtrl->SetSelection(0);
	tb2->AddControl(m_cmdCtrl);
    tb2->Realize();
	m_auiMgr.AddPane(tb2, wxAuiPaneInfo().Name(wxT("toolbar-chords")).Caption(wxT("Chords Toolbar)")).ToolbarPane().Top().Row(1).LeftDockable(false).RightDockable(false));

	// -------------------- create the status bar
	// create a status bar with some information about the used wxWidgets version
	CreateStatusBar(2);
	SetStatusText(_("This is BSChords application"),0);
	SetStatusText(wxbuildinfo(short_f), 1);

	// -------------------- window size
	int top = wxGetApp().config->Read(_("/global/top"), 100);
	int left = wxGetApp().config->Read(_("/global/left"), 100);
	int width = wxGetApp().config->Read(_("/global/width"), 500);
	int height = wxGetApp().config->Read(_("/global/height"), 500);
	SetSize(left, top, width, height, 0);

	// create song content window
	//m_songContent = new wxRichTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
	m_songContent = new wxStyledTextCtrl(this, ID_SONG_EDITOR);
	m_songContent->SetMarginWidth(0, 15);
    m_songContent->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour (75, 75, 75) );
    m_songContent->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour (220, 220, 220));
    m_songContent->SetMarginType(0, wxSTC_MARGIN_NUMBER);

    m_songContent->SetWrapMode (wxSTC_WRAP_NONE);

    //m_songContent->SetText(source);

    m_songContent->StyleClearAll();
    m_songContent->SetLexer(wxSTC_LEX_HTML);
    m_songContent->StyleSetForeground (wxSTC_H_DOUBLESTRING,     wxColour(255,0,0));
    m_songContent->StyleSetForeground (wxSTC_H_SINGLESTRING,     wxColour(255,0,0));
    m_songContent->StyleSetForeground (wxSTC_H_ENTITY,           wxColour(255,0,0));
    m_songContent->StyleSetForeground (wxSTC_H_TAG,              wxColour(0,150,0));
    m_songContent->StyleSetForeground (wxSTC_H_TAGUNKNOWN,       wxColour(0,150,0));
    m_songContent->StyleSetForeground (wxSTC_H_ATTRIBUTE,        wxColour(0,0,150));
    m_songContent->StyleSetForeground (wxSTC_H_ATTRIBUTEUNKNOWN, wxColour(0,0,150));
    m_songContent->StyleSetForeground (wxSTC_H_COMMENT,          wxColour(150,150,150));

	//m_songContent->DiscardEdits();
	//m_songContent->SetFont(wxGetApp().m_editorFont);
    m_auiMgr.AddPane(m_songContent, wxAuiPaneInfo().Name(_("song-editor")).Caption(wxT("Song Editor")).Center().CloseButton(false));

	// create prview window
	m_preview = new BSChordsPreview(this, m_songContent);
    m_auiMgr.AddPane(m_preview, wxAuiPaneInfo().Name(_("song-preview")).Caption(wxT("Song Preview")).Right().CloseButton(false).MinSize(200, wxDefaultCoord));

	// create file browser window
	m_dirCtrl = new wxGenericDirCtrl(this, ID_FSBROWSER, _T(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("Chordpro songs (*.txt)|*.txt"), 0 );
	wxString path = wxGetApp().config->Read(_("/global/path"));
	m_dirCtrl->SetPath(path);
	m_auiMgr.AddPane(m_dirCtrl, wxAuiPaneInfo().Name(_("file-browser")).Caption(wxT("File Browser")).Left().CloseButton(false).MinSize(200, wxDefaultCoord));

	// create song book window
	m_songBookWnd = new SongBookWnd(this);
    m_auiMgr.AddPane(m_songBookWnd, wxAuiPaneInfo().Name(_("song-book")).Caption(wxT("Song Book")).Left().CloseButton(false).MinSize(200, wxDefaultCoord));

	// load perspective
	wxString perspective;
	if (wxGetApp().config->Read(_("/global/perspective"), &perspective))
	{
		m_auiMgr.LoadPerspective(perspective);

		GetMenuBar()->Check(idMenuViewFileBrowser, m_auiMgr.GetPane(m_dirCtrl).IsShown());
		GetMenuBar()->Check(idMenuViewEditor, m_auiMgr.GetPane(m_songContent).IsShown());
		GetMenuBar()->Check(idMenuViewSongPreview, m_auiMgr.GetPane(m_preview).IsShown());
		GetMenuBar()->Check(idMenuViewSongBook, m_auiMgr.GetPane(m_songBookWnd).IsShown());
		GetMenuBar()->Check(idMenuViewTbMain, m_auiMgr.GetPane(wxT("toolbar-main")).IsShown());
		GetMenuBar()->Check(idMenuViewTbChords, m_auiMgr.GetPane(wxT("toolbar-chords")).IsShown());
		//m_isInEditMode = m_auiMgr.GetPane(m_songContent).IsShown();
		SetEditMode(m_auiMgr.GetPane(m_songContent).IsShown());
	}

	m_auiMgr.Update();
}

bschordsFrame::~bschordsFrame()
{
	// save perspective
	wxGetApp().config->Write(_("/global/perspective"), m_auiMgr.SavePerspective());

	m_auiMgr.UnInit();
};

void bschordsFrame::OnClose(wxCloseEvent &event)
{
	// close open file (if any)
	CloseFile();

	Destroy();
}

void bschordsFrame::SetEditMode(bool newEditMode)
{
	m_isInEditMode = newEditMode;

	// controls
	m_chordCtrl->Enable(m_isInEditMode);
	m_cmdCtrl->Enable(m_isInEditMode);

	// menu items
	GetMenuBar()->Enable(idMenuSongInsertChorus, m_isInEditMode);
	GetMenuBar()->Enable(idMenuSongInsertTab, m_isInEditMode);

}

void bschordsFrame::OnFileNewSong(wxCommandEvent& event)
{
	CloseFile();
}

void bschordsFrame::OnFileOpenSong(wxCommandEvent& event)
{
	wxFileName fileName(m_dirCtrl->GetPath());
	wxString dir(fileName.GetPath());

	wxFileDialog* openFileDialog = new wxFileDialog(this, _("Open file"), dir, _(""), _("*.txt"), wxOPEN, wxDefaultPosition);

	if (openFileDialog->ShowModal() == wxID_OK )
	{
		OpenFile(openFileDialog->GetPath());
	}
}

void bschordsFrame::OnFileSaveSong(wxCommandEvent& event)
{
	SaveFile();
}

void bschordsFrame::OnFileSaveAsSong(wxCommandEvent& event)
{
	wxString dir;
	wxString name;

	if (m_file.m_path.Length() > 0)
	{
		wxFileName fileName(m_file.m_path);
		dir = fileName.GetPath();
		name = fileName.GetFullName();
	}
	else
	{
		wxFileName fileName(m_dirCtrl->GetPath());
		dir = fileName.GetPath();
		name = _("untitled.txt");
	}

	wxFileDialog* saveDlg = new wxFileDialog(this, _("Save file as"), dir, name, _("*.txt"), wxSAVE, wxDefaultPosition);

	if (saveDlg->ShowModal() == wxID_OK )
	{
		m_file.m_path = saveDlg->GetPath();
		SaveFile();
	}
}

void bschordsFrame::OnFileCloseSong(wxCommandEvent& event)
{
	CloseFile();
}

void bschordsFrame::OnFileNewSongBook(wxCommandEvent& event)
{
	/*
	CloseFile();
	*/
}

void bschordsFrame::OnFileOpenSongBook(wxCommandEvent& event)
{
	/*
	wxFileName fileName(m_dirCtrl->GetPath());
	wxString dir(fileName.GetPath());

	wxFileDialog* openFileDialog = new wxFileDialog(this, _("Open file"), dir, _(""), _("*.txt"), wxOPEN, wxDefaultPosition);

	if (openFileDialog->ShowModal() == wxID_OK )
	{
		OpenFile(openFileDialog->GetPath());
	}
	*/
}

void bschordsFrame::OnFileSaveSongBook(wxCommandEvent& event)
{
	/*
	SaveFile();
	*/
}

void bschordsFrame::OnFileSaveAsSongBook(wxCommandEvent& event)
{
	/*
	wxString dir;
	wxString name;

	if (m_file.m_path.Length() > 0)
	{
		wxFileName fileName(m_file.m_path);
		dir = fileName.GetPath();
		name = fileName.GetFullName();
	}
	else
	{
		wxFileName fileName(m_dirCtrl->GetPath());
		dir = fileName.GetPath();
		name = _("untitled.txt");
	}

	wxFileDialog* saveDlg = new wxFileDialog(this, _("Save file as"), dir, name, _("*.txt"), wxSAVE, wxDefaultPosition);

	if (saveDlg->ShowModal() == wxID_OK )
	{
		m_file.m_path = saveDlg->GetPath();
		SaveFile();
	}
	*/
}

void bschordsFrame::OnFileCloseSongBook(wxCommandEvent& event)
{

}


void bschordsFrame::OnFileExportSong(wxCommandEvent& event)
{
	wxString dir;
	wxString name;

	if (m_file.m_path.Length() == 0)
		return;

	wxFileName fileName(m_file.m_path);
	fileName.SetExt(_("pdf"));
	dir = fileName.GetPath();
	name = fileName.GetFullName();

	wxFileDialog* saveDlg = new wxFileDialog(this, _("Save file as"), dir, name, _("*.pdf"), wxSAVE, wxDefaultPosition);

	if (saveDlg->ShowModal() == wxID_OK )
	{
		//m_filePath = saveDlg->GetPath();
		//SaveFile();
/*		wxPdfDocument pdf;
		pdf.Open();
		pdf.SetFont(_("Helvetica"), _(""), 10);

		// Page 1
		pdf.AddPage();
		pdf.Write(20, _("This is sample text"));
		pdf.SaveAsFile(saveDlg->GetPath());
		*/
	}
}

void bschordsFrame::OnFilePrint(wxCommandEvent& event)
{

}

void bschordsFrame::OnFilePrintPreview(wxCommandEvent& event)
{
	// Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* wxGetApp().m_printData);
    wxPrintPreview *preview = new wxPrintPreview(new BSChordsPrintout(this), new BSChordsPrintout(this), &printDialogData);

    if (!preview->IsOk())
    {
        delete preview;
        wxLogError(wxT("There was a problem previewing.\nPerhaps your current printer is not set correctly?"));
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, wxT("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}

void bschordsFrame::OnFilePageSetup(wxCommandEvent& event)
{
	(*wxGetApp().m_pageSetupData) = *wxGetApp().m_printData;

	wxPageSetupDialog pageSetupDialog(this, wxGetApp().m_pageSetupData);
	pageSetupDialog.ShowModal();

	(*wxGetApp().m_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
	(*wxGetApp().m_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

void bschordsFrame::OnQuit(wxCommandEvent &event)
{
	// store window size
	int x, y;
	GetSize(&x, &y);

	wxGetApp().config->Write(_("/global/width"), x);
	wxGetApp().config->Write(_("/global/height"), y);

	GetPosition(&x, &y);
	wxGetApp().config->Write(_("/global/left"), x);
	wxGetApp().config->Write(_("/global/top"), y);

	wxGetApp().config->Write(_("/global/path"), m_dirCtrl->GetPath());

	Destroy();
}

void bschordsFrame::OnPreferences(wxCommandEvent &event)
{
	bschordsPreferences* dlg = new bschordsPreferences(0L, _("wxWidgets Application Template"));

	dlg->m_showTsetBlocks = wxGetApp().config->Read(_("/global/show-tset-blocks"), 0l) > 0;
	dlg->m_showTsetMargins = wxGetApp().config->Read(_("/global/show-tset-margins"), 0l) > 0;

	if (dlg->ShowModal() == wxID_OK)
    {
		wxGetApp().config->Write(_("/global/show-tset-blocks"), dlg->m_showTsetBlocks ? 1 : 0);
		wxGetApp().config->Write(_("/global/show-tset-margins"), dlg->m_showTsetMargins ? 1 : 0);
		m_preview->Refresh();
		m_preview->Update();

		m_songContent->SetFont(wxGetApp().m_editorFont);
    }
    dlg->Destroy();
}

void bschordsFrame::OnStyleSheet(wxCommandEvent &event)
{
	SongStyleSheetDlg* dlg = new SongStyleSheetDlg(0L, _("Song Stylesheet Preferences"), &wxGetApp().m_styleSheet);
	//dlg->m_pageWidth << wxGetApp().m_styleSheet.m_pageSize.GetWidth();

	if (dlg->ShowModal() == wxID_OK)
    {
        // copy font information from preferences to application
        for (int i = 0; i < BS_FONT_LAST; i++)
        {
            std::cout << i << " native info: " << dlg->m_fonts[i].GetNativeFontInfoDesc().mb_str(wxConvUTF8) << std::endl;
            wxGetApp().m_styleSheet.m_fonts[i] = dlg->m_fonts[i];
        }
		m_preview->Refresh();
		m_preview->Update();
    }
    dlg->Destroy();
}

void bschordsFrame::OnViewPane(wxCommandEvent& event)
{
	wxMenuBar *mBar = GetMenuBar();

	switch (event.GetId())
	{
		case idMenuViewEditor:;
			m_auiMgr.GetPane(m_songContent).Show(mBar->IsChecked(event.GetId()));
			SetEditMode(mBar->IsChecked(event.GetId()));
			break;
		case idMenuViewFileBrowser:
			m_auiMgr.GetPane(m_dirCtrl).Show(mBar->IsChecked(event.GetId()));
			break;

		case idMenuViewSongPreview:
			m_auiMgr.GetPane(m_preview).Show(mBar->IsChecked(event.GetId()));
			break;

		case idMenuViewSongBook:
			m_auiMgr.GetPane(m_songBookWnd).Show(mBar->IsChecked(event.GetId()));
			break;

		case idMenuViewTbChords:
			m_auiMgr.GetPane(m_chordsPanel).Show(mBar->IsChecked(event.GetId()));
			break;
	}

	m_auiMgr.Update();
}

void bschordsFrame::OnViewToolbar(wxCommandEvent& event)
{
	wxMenuBar *mBar = GetMenuBar();

	wxString toolBarId;

	switch (event.GetId())
	{
		case idMenuViewTbMain: toolBarId = _("toolbar-main"); break;
		case idMenuViewTbChords: toolBarId = _("toolbar-chords"); break;
	}

	// no window for menu id from event
	if (toolBarId.Length() == 0)
		return;

	// show or hide appropriate pane
	if (mBar->IsChecked(event.GetId()))
		m_auiMgr.GetPane(toolBarId).Show();
	else
		m_auiMgr.GetPane(toolBarId).Hide();

	m_auiMgr.Update();
}

void bschordsFrame::OnSongInsert(wxCommandEvent& event)
{
	switch (event.GetId())
	{
		case idMenuSongInsertChorus:
			m_songContent->AddText(_("{start_of_chorus}\n{end_of_chorus}"));
			break;
		case idMenuSongInsertTab:
			m_songContent->AddText(_("{start_of_tab}\n{end_of_tab}"));
			break;
	}
}

void bschordsFrame::OnSongAddToSongbook(wxCommandEvent& event)
{
	std::cout << "Adding from menu" << std::endl;
}

void bschordsFrame::OnAbout(wxCommandEvent &event)
{
	wxString msg = wxbuildinfo(long_f);
	msg.append(_("\n\nYet another application for typesetting song lyrics."));
	msg.append(_("\n\nFor more information visit project web page at:\nhttps://code.google.com/p/bschords/"));
	msg.append(_("\n\nAuthor: michal.nezerka@gmail.com"));
	msg.append(_("\nhttp://blue.pavoucek.cz"));

	wxMessageBox(msg, _("BSChords Application"));
}

void bschordsFrame::OnSongContentChange(wxCommandEvent& event)
{
	m_file.m_changed = true;
	UpdateTitle();
	m_preview->Refresh();
	m_preview->Update();
}

void bschordsFrame::OnSongEditorChange(wxStyledTextEvent& event)
{
	m_file.m_changed = true;
	UpdateTitle();
	m_preview->Refresh();
	m_preview->Update();
}

void bschordsFrame::OnChordProToken(wxCommandEvent& event)
{
	if (!m_auiMgr.GetPane(m_songContent).IsShown())
		return;

	if (event.GetId() == ID_COMBO_CHORD && m_chordCtrl->GetSelection() != 0)
	{
		m_songContent->AddText(m_chordCtrl->GetValue());
		m_chordCtrl->SetSelection(0);
	}
	else if	(event.GetId() == ID_COMBO_CMD && m_cmdCtrl->GetSelection() != 0)
	{
		m_songContent->AddText(m_cmdCtrl->GetValue());
		m_cmdCtrl->SetSelection(0);
	}
}

void bschordsFrame::OnFSBrowserSelChanged(wxTreeEvent& event)
{
	wxTreeItemId id = event.GetItem();
	if (!id.IsOk())
		return;

	if (!m_dirCtrl)
		return;

	wxTreeCtrl *treeCtrl = m_dirCtrl->GetTreeCtrl();
	wxDirItemData* data = (wxDirItemData*) treeCtrl->GetItemData(id);
	if (data)
	{
		if (!data->m_isDir)
		{
			OpenFile(data->m_path);
		}
	}
}

void bschordsFrame::OnFSBrowserItemAddToSongbook(wxCommandEvent& event)
{
	std::cout << "Adding from fs browser" << std::endl;

	wxTreeCtrl *treeCtrl = m_dirCtrl->GetTreeCtrl();

	wxTreeItemId id = treeCtrl->GetSelection();
	if (!id.IsOk())
		return;

	wxDirItemData* data = (wxDirItemData*) treeCtrl->GetItemData(id);
	if (data)
	{
		if (!data->m_isDir)
		{
			std::cout << "  adding file: " << data->m_path.wc_str() << std::endl;
			wxGetApp().m_songBook.add(data->m_path);
			m_songBookWnd->UpdateContent();
		}
	}
}

void bschordsFrame::OnFSBrowserItemMenu(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    //MyTreeItemData *item = itemId.IsOk() ? (MyTreeItemData *)GetItemData(itemId) : NULL;
    wxPoint clientpt = event.GetPoint();
    //wxPoint screenpt = m_dirCtrl->ClientToScreen(clientpt);
    //wxLogMessage(wxT("OnItemMenu for item \"%s\" at screen coords (%i, %i)"),
    //             item ? item->GetDesc() : _T(""), screenpt.x, screenpt.y);

#if wxUSE_MENUS
    wxMenu menu(_("menu title"));
    menu.Append(idFSBrowserAddToSongbook, wxT("&Add to songbook"));
    m_dirCtrl->PopupMenu(&menu, clientpt);
#endif // wxUSE_MENUS
    event.Skip();
}

void bschordsFrame::OpenFile(const wxString filePath)
{
	CloseFile();

	wxFileName fileName(filePath);

	if (fileName.IsDir())
	{
		wxMessageBox(_("Cannot open directory"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	if (!fileName.FileExists())
	{
		wxMessageBox(_("File doesn't exist"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	std::wcout << L"loading file " << fileName.GetFullPath().wc_str() << std::endl;


	//wxMessageBox(data->m_path);
	wxTextFile fileIn;
	wxString lines;
	if (fileIn.Open(fileName.GetFullPath()))
	{
		m_file.m_type = fileIn.GuessType();

		lines = fileIn.GetFirstLine();
		// Read all the lines (one by one)
		while(!fileIn.Eof())
		{
			if (lines.size() > 0)
				lines += wxT('\n');
			lines += fileIn.GetNextLine();
		}
		fileIn.Close(); // Close the opened file
		m_songContent->Clear();
		m_songContent->AppendText(lines);
		//TODO m_songContent->DiscardEdits();

		m_file.m_path = fileName.GetFullPath();
		m_file.m_changed = false;

		UpdateTitle();
	}
}

void bschordsFrame::CloseFile()
{
	/* TODO track modifications
	if (m_songContent->IsModified())
	{
		if (wxMessageBox(_("Do you want to save changes?"), wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_ASTERISK) == wxYES)
		{
			SaveFile();
		}
	}
	*/
	m_file.clear();
	m_songContent->Clear();
	UpdateTitle();
}

void bschordsFrame::SaveFile()
{
	std::cout << "SaveFile called" << std::endl;

	if (m_file.m_path.Length() == 0)
	{
		wxFileName fileName(m_dirCtrl->GetPath());
		wxString dir = fileName.GetPath();
		wxString name = _("untitled.txt");
		wxFileDialog* saveDlg = new wxFileDialog(this, _("Save file as"), dir, name, _("*.txt"), wxSAVE, wxDefaultPosition);

		if (saveDlg->ShowModal() == wxID_OK )
			m_file.m_path = saveDlg->GetPath();
		else
			return;
	}

	wxFileName fileName(m_file.m_path);

	if (fileName.IsDir())
	{
		wxMessageBox(_("Cannot save song to directory"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	std::wcout << L"saving song to file " << fileName.GetFullPath().wc_str() << std::endl;

	//wxMessageBox(data->m_path);
	wxTextFile fileOut;
	//wxString lines;
	bool fileIsOk = fileName.FileExists() ? fileOut.Open(fileName.GetFullPath()) : fileOut.Create(fileName.GetFullPath());
	if (fileIsOk)
	{
		fileOut.Clear();
		fileOut.AddLine(m_songContent->GetText());

		// preserve file eol encoding if available (remembered from file opening)
		if (m_file.m_type != wxTextFileType_None)
			fileOut.Write();
		else
			fileOut.Write(m_file.m_type);
		fileOut.Close(); // Close the opened file
		//m_songContent->DiscardEdits();

		UpdateTitle();
	}

}

void bschordsFrame::UpdateTitle()
{
	wxString title;

	if (m_file.m_path.Length() > 0)
	{
		wxFileName tmp(m_file.m_path);

		//TODO uncomment if (m_songContent->IsModified())
		//if (m_fileChanged)
		//TODO uncomment 	title += _("*");
		title += tmp.GetFullName();
	}
	else
		title += _("untitled.txt");

	title += _(" - BSChords");
	SetTitle(title);

	// update status bar
	wxString t;
	t += m_file.m_path;
	switch (m_file.m_type)
	{
		case wxTextFileType_Unix: t += _(" (Unix)"); break;
		case wxTextFileType_Dos: t += _(" (Dos)"); break;
		case wxTextFileType_Mac: t += _(" (Mac)"); break;
		case wxTextFileType_Os2: t += _(" (Os2)"); break;
		case wxTextFileType_None:
		default:
			t += _(" (no type)");
	}
	SetStatusText(t, 1);
}

void bschordsFrame::OnPaneClose(wxAuiManagerEvent& evt)
{
	std::wcout << L"OnPaneClose for " << evt.pane->name.wc_str() << std::endl;

    if (evt.pane->name == _("file-browser"))
    {
		GetMenuBar()->Check(idMenuViewFileBrowser, false);
    }
    else if (evt.pane->name == _("song-editor"))
    {
		SetEditMode(false);
    }

	m_auiMgr.Update();
}

// ----------------------------------------------------------------------------
// BSChordsPrintout
// ----------------------------------------------------------------------------
bool BSChordsPrintout::OnPrintPage(int page)
{
	std::cout << "OnPrintPage" << std::endl;
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
            DrawPageOne();
        else if (page == 2)
            DrawPageTwo();

        // Draw page numbers at top left corner of printable area, sized so that
        // screen size of text matches paper size.
        //MapScreenSizeToPage();

        //dc->DrawText(wxString::Format(wxT("PAGE %d"), page), 0, 0);

        return true;
    }
    else
        return false;
}

bool BSChordsPrintout::OnBeginDocument(int startPage, int endPage)
{
	std::cout << "OnBeginDocument" << std::endl;
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

	// set user scale to fit a4
	//GetDC()->G
	int pageSizeXMM, pageSizeYMM;
	GetPageSizeMM(&pageSizeXMM, &pageSizeYMM);
	//std::cout << "page size mm: " << pageSizeXMM << "x" << pageSizeYMM << std::endl;

	int pageSizeXPx, pageSizeYPx;
	GetPageSizePixels(&pageSizeXPx, &pageSizeYPx);
	//cout << "page size px: " << pageSizeXPx << "x" << pageSizeYPx << endl;

	//wxSize dcSize = GetDC()->GetSize();
	//cout << "dc size in px: " << dcSize.GetWidth() << "x" << dcSize.GetHeight() << endl;

	//wxRect paperRect = GetPaperRectPixels();
	//cout << "paper rect in px: " << paperRect.GetLeft() << "," << paperRect.GetTop() << " - " << paperRect.GetRight() << "," << paperRect.GetBottom() << " - " << paperRect.GetWidth() << "x" << paperRect.GetHeight() << endl;

    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    //cout << "screen ppi: " << ppiScreenX << "x" << ppiScreenY << endl;
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    //cout << "printer ppi: " << ppiPrinterX << "x" << ppiPrinterY << endl;

	wxSize paperPixels;
	if (IsPreview())
	{
		paperPixels.SetWidth(ppiScreenX * 210 / MM_PER_IN);
		paperPixels.SetHeight(ppiScreenY * 290 / MM_PER_IN);
		//cout << "fitting size to preview page " << paperPixels.GetWidth() << "x" << paperPixels.GetHeight() << endl;
	}
	else
	{
		//paperPixels.SetWidth(ppiPrinterX * 210 / MM_PER_IN);
		//paperPixels.SetHeight(ppiPrinterY * 290 / MM_PER_IN);
		paperPixels.SetWidth(ppiScreenX * 210 / MM_PER_IN);
		paperPixels.SetHeight(ppiScreenY * 290 / MM_PER_IN);
		//cout << "fitting size to printer page " << paperPixels.GetWidth() << "x" << paperPixels.GetHeight() << endl;
	}
	FitThisSizeToPaper(paperPixels);
	double newScaleX, newScaleY;
	GetDC()->GetUserScale(&newScaleX, &newScaleY);
	//cout << "new user scale: " << newScaleX << "x" << newScaleY << endl;

    return true;
}

void BSChordsPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
	//std::cout << "OnGetPageInfo" << std::endl;
    *minPage = 1;
    *maxPage = 2;
    *selPageFrom = 1;
    *selPageTo = 2;
}

bool BSChordsPrintout::HasPage(int pageNum)
{
	std::cout << "HasPage" << std::endl;
    return (pageNum == 1 || pageNum == 2);
}

void BSChordsPrintout::DrawPageOne()
{
	std::cout << "DrawPageOne" << std::endl;
	double newScaleX, newScaleY;
	GetDC()->GetUserScale(&newScaleX, &newScaleY);
	//cout << "  user scale: " << newScaleX << "x" << newScaleY << endl;

	GetDC()->SetBackground(*wxWHITE_BRUSH);

    // get lines from song book control
	/*
	int lines = m_frame->m_songContent->GetNumberOfLines();
	wxString text;
	for (int i = 0; i < lines; i++)
    {
        if (text.size() > 0)
			text.Append(wxT("\n"));
		text.Append(m_frame->m_songContent->GetLineText(i));
	}
	*/
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

	//float scale = IsPreview() ? ppiScreenX / MM_PER_IN : ppiPrinterX / MM_PER_IN;
	float scale = ppiScreenX / MM_PER_IN;

	bschords::TSetDCPainter y(*GetDC(), scale);
	bschordpro::Parser p(&y);

	//wcout << text.wc_str() << endl;
	//p.parse(std::wstring(text.wc_str()));
}

void BSChordsPrintout::DrawPageTwo()
{
	std::cout << "DrawPageTwo" << std::endl;
    // You might use THIS code to set the printer DC to ROUGHLY reflect
    // the screen text size. This page also draws lines of actual length
    // 5cm on the page.

    // Compare this to DrawPageOne(), which uses the really convenient routines
    // from wxPrintout to fit the screen image onto the printed page. This page
    // illustrates how to do all the scaling calculations yourself, if you're so
    // inclined.

    wxDC *dc = GetDC();

    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    // This scales the DC so that the printout roughly represents the screen
    // scaling. The text point size _should_ be the right size but in fact is
    // too small for some reason. This is a detail that will need to be
    // addressed at some point but can be fudged for the moment.
    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

    // Now we have to check in case our real page size is reduced (e.g. because
    // we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);

    // If printer pageWidth == current DC width, then this doesn't change. But w
    // might be the preview bitmap width, so scale down.
    float overallScale = scale * (float)(w/(float)pageWidth);
    //cout << "second page overall scale: " << overallScale << endl;
    dc->SetUserScale(overallScale, overallScale);

    // Calculate conversion factor for converting millimetres into logical
    // units. There are approx. 25.4 mm to the inch. There are ppi device units
    // to the inch. Therefore 1 mm corresponds to ppi/25.4 device units. We also
    // divide by the screen-to-printer scaling factor, because we need to
    // unscale to pass logical units to DrawLine.

    // Draw 50 mm by 50 mm L shape
    float logUnitsFactor = (float)(ppiPrinterX/(scale*25.4));
    float logUnits = (float)(50*logUnitsFactor);
    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine(50, 250, (long)(50.0 + logUnits), 250);
    dc->DrawLine(50, 250, 50, (long)(250.0 + logUnits));

    dc->SetBackgroundMode(wxTRANSPARENT);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);

    { // GetTextExtent demo:
        wxString words[7] = { wxT("This "), wxT("is "), wxT("GetTextExtent "),
                             wxT("testing "), wxT("string. "), wxT("Enjoy "), wxT("it!") };
        wxCoord w, h;
        long x = 200, y= 250;
        wxFont fnt(15, wxSWISS, wxNORMAL, wxNORMAL);

        dc->SetFont(fnt);

        for (int i = 0; i < 7; i++)
        {
            wxString word = words[i];
            word.Remove( word.Len()-1, 1 );
            dc->GetTextExtent(word, &w, &h);
            dc->DrawRectangle(x, y, w, h);
            dc->GetTextExtent(words[i], &w, &h);
            dc->DrawText(words[i], x, y);
            x += w;
        }

    }

    //dc->SetFont(wxGetApp().GetTestFont());

    dc->DrawText(wxT("Some test text"), 200, 300 );

    // TESTING

    int leftMargin = 20;
    int rightMargin = 20;
    int topMargin = 20;
    int bottomMargin = 20;

    int pageWidthMM, pageHeightMM;
    GetPageSizeMM(&pageWidthMM, &pageHeightMM);

    float leftMarginLogical = (float)(logUnitsFactor*leftMargin);
    float topMarginLogical = (float)(logUnitsFactor*topMargin);
    float bottomMarginLogical = (float)(logUnitsFactor*(pageHeightMM - bottomMargin));
    float rightMarginLogical = (float)(logUnitsFactor*(pageWidthMM - rightMargin));

    dc->SetPen(* wxRED_PEN);
    dc->DrawLine( (long)leftMarginLogical, (long)topMarginLogical,
        (long)rightMarginLogical, (long)topMarginLogical);
    dc->DrawLine( (long)leftMarginLogical, (long)bottomMarginLogical,
        (long)rightMarginLogical, (long)bottomMarginLogical);

    WritePageHeader(this, dc, wxT("A header"), logUnitsFactor);
}

// Writes a header on a page. Margin units are in millimetres.
bool BSChordsPrintout::WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString&text, float mmToLogical)
{
	std::cout << "WritePageHeader" << std::endl;
#if 0
	static wxFont *headerFont = (wxFont *) NULL;
	if (!headerFont)
	{
	headerFont = wxTheFontList->FindOrCreateFont(16, wxSWISS, wxNORMAL, wxBOLD);
	}
	dc->SetFont(headerFont);
#endif

    int pageWidthMM, pageHeightMM;

    printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);
    wxUnusedVar(pageHeightMM);

    int leftMargin = 10;
    int topMargin = 10;
    int rightMargin = 10;

    float leftMarginLogical = (float)(mmToLogical*leftMargin);
    float topMarginLogical = (float)(mmToLogical*topMargin);
    float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMargin));

    wxCoord xExtent, yExtent;
    dc->GetTextExtent(text, &xExtent, &yExtent);

    float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
    dc->DrawText(text, (long)xPos, (long)topMarginLogical);

    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine( (long)leftMarginLogical, (long)(topMarginLogical+yExtent),
                  (long)rightMarginLogical, (long)topMarginLogical+yExtent );

    return true;
}
