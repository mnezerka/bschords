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

#include "mainwnd.h"
#include "preferencesdlg.h"
#include "songstylesheetdlg.h"
//#include "bschordsicon.xpm"
#include "dcpainter.h"

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

#define wxSTC_CHORDPRO_TEXT   0
#define wxSTC_CHORDPRO_CHORD  1
#define wxSTC_CHORDPRO_CMD    2

enum
{
	idMenuQuit = 1000,
	idMenuAbout,
	idMenuPreferences,
	ID_MENU_STYLESHEET,

	idMenuFileNewSongBook,
	idMenuFileOpenSongBook,
	idMenuFileSaveSongBook,
	idMenuFileSaveSongBookAs,
	idMenuFileCloseSongBook,
	idMenuFilePrintSongbook,
	idMenuFilePrintPreviewSongbook,

	idMenuViewFileBrowser,
	idMenuViewEditor,
	idMenuViewSongPreview,
	idMenuViewSongBook,
	idMenuViewTbMain,
	idMenuViewTbChords,

	idMenuSongInsertChorus,
	idMenuSongInsertTab,
	idMenuSongAddToSongbook,
	idMenuSongTrnUp1,
	idMenuSongTrnUp2,
	idMenuSongTrnUp3,
	idMenuSongTrnUp4,
	idMenuSongTrnUp5,
	idMenuSongTrnUp6,
	idMenuSongTrnUp7,
	idMenuSongTrnDown1,
	idMenuSongTrnDown2,
	idMenuSongTrnDown3,
	idMenuSongTrnDown4,
	idMenuSongTrnDown5,
	idMenuSongTrnDown6,
	idMenuSongTrnDown7,

	idFSBrowserAddToSongbook,

	ID_COMBO_CHORD,
	ID_COMBO_CMD,
	//idMenuViewTSetBook,
	ID_MENU_FILE_EXPORT,
	ID_FSBROWSER,
	ID_TOOLBAR_CHORD,
	ID_SONG_EDITOR,

	idWindowSongBook,
};

BEGIN_EVENT_TABLE(MainWnd, wxFrame)
	EVT_CLOSE(MainWnd::OnClose)
	EVT_ERASE_BACKGROUND(MainWnd::OnEraseBackground)
	EVT_SIZE(MainWnd::OnSize)
	EVT_MENU(wxID_NEW, MainWnd::OnFileNewSong)
	EVT_MENU(wxID_OPEN, MainWnd::OnFileOpenSong)
	EVT_MENU(wxID_SAVE, MainWnd::OnFileSaveSong)
	EVT_MENU(wxID_SAVEAS, MainWnd::OnFileSaveAsSong)
	EVT_MENU(wxID_CLOSE, MainWnd::OnFileCloseSong)
	EVT_MENU(idMenuFileNewSongBook, MainWnd::OnFileNewSongBook)
	EVT_MENU(idMenuFileOpenSongBook, MainWnd::OnFileOpenSongBook)
	EVT_MENU(idMenuFileSaveSongBook, MainWnd::OnFileSaveSongBook)
	EVT_MENU(idMenuFileSaveSongBookAs, MainWnd::OnFileSaveSongBookAs)
	EVT_MENU(idMenuFileCloseSongBook, MainWnd::OnFileCloseSongBook)
	EVT_MENU(ID_MENU_FILE_EXPORT, MainWnd::OnFileExportSong)
	EVT_MENU(wxID_PRINT, MainWnd::OnFilePrint)
	EVT_MENU(wxID_PREVIEW, MainWnd::OnFilePrintPreview)
	EVT_MENU(idMenuFilePrintSongbook, MainWnd::OnFilePrintSongBook)
	EVT_MENU(idMenuFilePrintPreviewSongbook, MainWnd::OnFilePrintPreviewSongBook)
	EVT_MENU(wxID_PRINT_SETUP, MainWnd::OnFilePageSetup)
	EVT_MENU(idMenuQuit, MainWnd::OnQuit)
	EVT_MENU(idMenuPreferences, MainWnd::OnPreferences)
	EVT_MENU(ID_MENU_STYLESHEET, MainWnd::OnStyleSheet)
	EVT_MENU(idMenuAbout, MainWnd::OnAbout)
	EVT_MENU(idMenuViewFileBrowser, MainWnd::OnViewPane)
	EVT_MENU(idMenuViewEditor, MainWnd::OnViewPane)
	EVT_MENU(idMenuViewSongPreview, MainWnd::OnViewPane)
	EVT_MENU(idMenuViewSongBook, MainWnd::OnViewPane)
	EVT_MENU(idMenuViewTbMain, MainWnd::OnViewToolbar)
	EVT_MENU(idMenuViewTbChords, MainWnd::OnViewPane)
	EVT_MENU(idMenuSongInsertChorus, MainWnd::OnSongInsert)
	EVT_MENU(idMenuSongInsertTab, MainWnd::OnSongInsert)
	EVT_MENU(idMenuSongAddToSongbook, MainWnd::OnSongAddToSongbook)

	EVT_MENU(idMenuSongTrnUp1, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnUp2, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnUp3, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnUp4, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnUp5, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnUp6, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnUp7, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown1, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown2, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown3, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown4, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown5, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown6, MainWnd::OnSongTranspose)
	EVT_MENU(idMenuSongTrnDown7, MainWnd::OnSongTranspose)

	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, MainWnd::OnSongContentChange)
	EVT_STC_MODIFIED(ID_SONG_EDITOR, MainWnd::OnSongEditorChange)
	EVT_STC_STYLENEEDED(ID_SONG_EDITOR, MainWnd::OnSongEditorStyleNeeded)
	/*EVT_TREE_SEL_CHANGED(wxID_TREECTRL, MainWnd::OnFSBrowserSelChanged)*/
	EVT_TREE_ITEM_ACTIVATED(wxID_TREECTRL, MainWnd::OnFSBrowserSelChanged)
	EVT_TREE_ITEM_MENU(wxID_TREECTRL, MainWnd::OnFSBrowserItemMenu)
	EVT_MENU(idFSBrowserAddToSongbook, MainWnd::OnFSBrowserItemAddToSongbook)
	EVT_AUI_PANE_CLOSE(MainWnd::OnPaneClose)
	EVT_COMBOBOX(ID_COMBO_CHORD, MainWnd::OnChordProToken)
	EVT_COMBOBOX(ID_COMBO_CMD, MainWnd::OnChordProToken)
END_EVENT_TABLE()

MainWnd::MainWnd(wxFrame *frame, const wxString& title)
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
	fileMenu->Append(idMenuFileSaveSongBookAs, _("Save songbook as..."), _("Save the active songbook under different a name"));
	fileMenu->Append(idMenuFileCloseSongBook, _("Close songbook"), _("Close songbook file"));
	fileMenu->AppendSeparator();

	fileMenu->Append(ID_MENU_FILE_EXPORT, _("&Export to PDF ..."), _("Export song file to PDF"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_PRINT_SETUP, _("Page setup..."), _("Page setup for printing"));
    fileMenu->Append(wxID_PRINT, _("&Print..."), _("Print"));
    fileMenu->Append(wxID_PREVIEW, _("Print Pre&view"), _("Preview"));
    fileMenu->Append(idMenuFilePrintSongbook, _("Print Songbook ..."), _("Print"));
    fileMenu->Append(idMenuFilePrintPreviewSongbook, _("Songbook Print Preview"), _("Preview"));
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

	//songMenu->Append(wxID_ANY, _("Trans&pose.."), _("Transpose song chords to different key"));

	wxMenu* songTransposeMenu = new wxMenu(_T(""));
	songTransposeMenu->Append(idMenuSongTrnDown7, wxT("-7 (perfect fifth)"), _("Transpose song 7 half steps down"));
	songTransposeMenu->Append(idMenuSongTrnDown6, wxT("-6 (diminished fifth)"), _("Transpose song 6 half steps dowm"));
	songTransposeMenu->Append(idMenuSongTrnDown5, wxT("-5 (perfect fourth)"), _("Transpose song 5 half steps down"));
	songTransposeMenu->Append(idMenuSongTrnDown4, wxT("-4 (major third)"), _("Transpose song 4 half steps down"));
	songTransposeMenu->Append(idMenuSongTrnDown3, wxT("-3 (minor third)"), _("Transpose song 3 half steps down"));
	songTransposeMenu->Append(idMenuSongTrnDown2, wxT("-2 (major second)"), _("Transpose song 2 half steps down"));
	songTransposeMenu->Append(idMenuSongTrnDown1, wxT("-1 (minor second)"), _("Transpose song 1 half step down"));
	songTransposeMenu->Append(idMenuSongTrnUp1, wxT("+1 (minor second)"), _("Transpose song 1 half step up"));
	songTransposeMenu->Append(idMenuSongTrnUp2, wxT("+2 (major second)"), _("Transpose song 2 half steps up"));
	songTransposeMenu->Append(idMenuSongTrnUp3, wxT("+3 (minor third)"), _("Transpose song 3 half steps up"));
	songTransposeMenu->Append(idMenuSongTrnUp4, wxT("+4 (major third)"), _("Transpose song 4 half steps up"));
	songTransposeMenu->Append(idMenuSongTrnUp5, wxT("+5 (perfect fourth)"), _("Transpose song 5 half steps up"));
	songTransposeMenu->Append(idMenuSongTrnUp6, wxT("+6 (diminished fifth)"), _("Transpose song 6 half steps up"));
	songTransposeMenu->Append(idMenuSongTrnUp7, wxT("+7 (perfect fifth)"), _("Transpose song 7 half steps up"));
	songMenu->AppendSubMenu(songTransposeMenu, _("Trans&pose.."), _("Transpose song chords to different key"));
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
	m_songContent = new wxStyledTextCtrl(this, ID_SONG_EDITOR);
	m_songContent->SetMarginWidth(0, 15);

    m_songContent->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_songContent->SetWrapMode (wxSTC_WRAP_NONE);
    m_songContent->StyleClearAll();
    updateEditorStyles();
    m_songContent->SetLexer(wxSTC_LEX_CONTAINER);
    m_songContent->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    m_songContent->StyleResetDefault();
	m_songContent->SetSavePoint();

	//m_songContent->SetFont(wxGetApp().m_editorFont);
    m_auiMgr.AddPane(m_songContent, wxAuiPaneInfo().Name(_("song-editor")).Caption(wxT("Song Editor")).Center().CloseButton(false));

	// create prview window
	m_preview = new PreviewWnd(this, m_songContent);
    m_auiMgr.AddPane(m_preview, wxAuiPaneInfo().Name(_("song-preview")).Caption(wxT("Song Preview")).Right().CloseButton(false).MinSize(200, wxDefaultCoord));

	// create file browser window
	m_dirCtrl = new wxGenericDirCtrl(this, ID_FSBROWSER, _T(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("Chordpro songs (*.txt)|*.txt"), 0 );
	m_dirCtrl->SetPath(wxGetApp().m_settings->m_rootPath);
	m_auiMgr.AddPane(m_dirCtrl, wxAuiPaneInfo().Name(_("file-browser")).Caption(wxT("File Browser")).Left().CloseButton(false).MinSize(200, wxDefaultCoord));

	// create song book window
	m_songBookWnd = new SongBookWnd(this, idWindowSongBook);
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

MainWnd::~MainWnd()
{
	// save perspective
	wxGetApp().config->Write(_("/global/perspective"), m_auiMgr.SavePerspective());

	m_auiMgr.UnInit();
};

void MainWnd::OnClose(wxCloseEvent &event)
{
	// close open file (if any)
	CloseFile();

	Destroy();
}

void MainWnd::SetEditMode(bool newEditMode)
{
	m_isInEditMode = newEditMode;

	// controls
	m_chordCtrl->Enable(m_isInEditMode);
	m_cmdCtrl->Enable(m_isInEditMode);

	// menu items
	GetMenuBar()->Enable(idMenuSongInsertChorus, m_isInEditMode);
	GetMenuBar()->Enable(idMenuSongInsertTab, m_isInEditMode);

}

void MainWnd::OnFileNewSong(wxCommandEvent& event)
{
	CloseFile();
}

void MainWnd::OnFileOpenSong(wxCommandEvent& event)
{
	wxFileName fileName(m_dirCtrl->GetPath());
	wxString dir(fileName.GetPath());

	wxFileDialog* openFileDialog = new wxFileDialog(this, _("Open file"), dir, _(""), _("*.txt"), wxOPEN, wxDefaultPosition);

	if (openFileDialog->ShowModal() == wxID_OK )
	{
		OpenFile(openFileDialog->GetPath());
	}
}

void MainWnd::OnFileSaveSong(wxCommandEvent& event)
{
	SaveFile();
}

void MainWnd::OnFileSaveAsSong(wxCommandEvent& event)
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

void MainWnd::OnFileCloseSong(wxCommandEvent& event)
{
	CloseFile();
}

void MainWnd::OnFileNewSongBook(wxCommandEvent& event)
{
	wxGetApp().m_songBook.empty();

}

void MainWnd::OnFileOpenSongBook(wxCommandEvent& event)
{
	wxFileName fileName(wxGetApp().m_settings->m_rootPath);
	wxString dir(fileName.GetFullPath());
	wxFileDialog* openFileDialog = new wxFileDialog(this, _("Open file"), dir, _(""), _("*.xml"), wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK )
	{
		OpenSongBook(openFileDialog->GetPath());
		m_songBookPath = openFileDialog->GetPath();
	}
}

void MainWnd::OnFileSaveSongBook(wxCommandEvent& event)
{
	SaveSongBook();
}

void MainWnd::OnFileSaveSongBookAs(wxCommandEvent& event)
{
	wxString dir;
	wxString name;

	if (m_songBookPath.Length() > 0)
	{
		wxFileName fileName(m_songBookPath);
		dir = fileName.GetPath();
		name = fileName.GetFullName();
	}
	else
	{
		wxFileName fileName(wxGetApp().m_settings->m_rootPath);
		dir = fileName.GetPath();
		name = _("untitled.xml");
	}

	wxFileDialog* saveDlg = new wxFileDialog(this, _("Save song book as"), dir, name, _("*.xml"), wxSAVE, wxDefaultPosition);

	if (saveDlg->ShowModal() == wxID_OK )
	{
		m_songBookPath = saveDlg->GetPath();
		SaveSongBook();
	}
}

void MainWnd::OnFileCloseSongBook(wxCommandEvent& event)
{

}


void MainWnd::OnFileExportSong(wxCommandEvent& event)
{
	wxMessageBox(_("Not implemented yet."));
	return;

	/*
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
		wxPdfDocument pdf;
		pdf.Open();
		pdf.SetFont(_("Helvetica"), _(""), 10);

		// Page 1
		pdf.AddPage();
		pdf.Write(20, _("This is sample text"));
		pdf.SaveAsFile(saveDlg->GetPath());
	}*/
}

void MainWnd::OnFilePrint(wxCommandEvent& event)
{
    wxPrintDialogData printDialogData(* wxGetApp().m_printData);

    wxPrinter printer(& printDialogData);
    BSChordsPrintout printout(this, m_songContent->GetText());
    if (!printer.Print(this, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_T("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("Printing"), wxOK);
        else
            wxMessageBox(_T("You canceled printing"), _T("Printing"), wxOK);
    }
    else
    {
        (*wxGetApp().m_printData) = printer.GetPrintDialogData().GetPrintData();
    }
}

void MainWnd::OnFilePrintPreview(wxCommandEvent& event)
{
	// Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* wxGetApp().m_printData);
    wxPrintPreview *preview = new wxPrintPreview(
		new BSChordsPrintout(this, m_songContent->GetText()),
		new BSChordsPrintout(this, m_songContent->GetText()),
		&printDialogData);

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

void MainWnd::OnFilePrintSongBook(wxCommandEvent& event)
{
	// prepare text to be printed
	wxString contents = wxGetApp().m_songBook.getContents();

    wxPrintDialogData printDialogData(* wxGetApp().m_printData);

    wxPrinter printer(& printDialogData);
    BSChordsPrintout printout(this, contents);
    if (!printer.Print(this, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_T("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("Printing"), wxOK);
        else
            wxMessageBox(_T("You canceled printing"), _T("Printing"), wxOK);
    }
    else
    {
        (*wxGetApp().m_printData) = printer.GetPrintDialogData().GetPrintData();
    }
}

void MainWnd::OnFilePrintPreviewSongBook(wxCommandEvent& event)
{
	// prepare text to be printed
	wxString contents = wxGetApp().m_songBook.getContents();

	//std::wcout << L"Songbook contents: " << contents.mb_str(wxConvUTF8) << std::endl;

	// Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* wxGetApp().m_printData);
    wxPrintPreview *preview = new wxPrintPreview(
		new BSChordsPrintout(this, contents),
		new BSChordsPrintout(this, contents),
		&printDialogData);

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

void MainWnd::OnFilePageSetup(wxCommandEvent& event)
{
	(*wxGetApp().m_pageSetupData) = *wxGetApp().m_printData;

	wxPageSetupDialog pageSetupDialog(this, wxGetApp().m_pageSetupData);
	pageSetupDialog.ShowModal();

	(*wxGetApp().m_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
	(*wxGetApp().m_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

void MainWnd::OnQuit(wxCommandEvent &event)
{
	// store window size
	int x, y;
	GetSize(&x, &y);

	wxGetApp().config->Write(_("/global/width"), x);
	wxGetApp().config->Write(_("/global/height"), y);

	GetPosition(&x, &y);
	wxGetApp().config->Write(_("/global/left"), x);
	wxGetApp().config->Write(_("/global/top"), y);

	//wxGetApp().m_settins->config->Write(_("/global/path"), m_dirCtrl->GetPath());
	//wxGetApp().config->Write(_("/global/path"), m_dirCtrl->GetPath());

	Destroy();
}

void MainWnd::OnPreferences(wxCommandEvent &event)
{
	PreferencesDlg* dlg = new PreferencesDlg(0L, _("wxWidgets Application Template"));

	dlg->m_showTsetBlocks = wxGetApp().config->Read(_("/global/show-tset-blocks"), 0l) > 0;
	dlg->m_showTsetMargins = wxGetApp().config->Read(_("/global/show-tset-margins"), 0l) > 0;

	if (dlg->ShowModal() == wxID_OK)
    {
		wxGetApp().config->Write(_("/global/show-tset-blocks"), dlg->m_showTsetBlocks ? 1 : 0);
		wxGetApp().config->Write(_("/global/show-tset-margins"), dlg->m_showTsetMargins ? 1 : 0);
		m_preview->Refresh();
		m_preview->Update();

		m_songContent->SetFont(wxGetApp().m_settings->m_editorFont);

		updateEditorStyles();

		//wxGetApp().m_songBook.setBasePath(wxGetApp.);
    }
    dlg->Destroy();
}

void MainWnd::OnStyleSheet(wxCommandEvent &event)
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

void MainWnd::OnViewPane(wxCommandEvent& event)
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

void MainWnd::OnViewToolbar(wxCommandEvent& event)
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

void MainWnd::OnSongInsert(wxCommandEvent& event)
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

void MainWnd::OnSongAddToSongbook(wxCommandEvent& event)
{
	std::cout << "Adding from menu" << std::endl;
}

void MainWnd::OnSongTranspose(wxCommandEvent& event)
{
	int distance = 0;

	switch(event.GetId())
	{
		case idMenuSongTrnUp1: distance = 1; break;
		case idMenuSongTrnUp2: distance = 2; break;
		case idMenuSongTrnUp3: distance = 3; break;
		case idMenuSongTrnUp4: distance = 4; break;
		case idMenuSongTrnUp5: distance = 5; break;
		case idMenuSongTrnUp6: distance = 6; break;
		case idMenuSongTrnUp7: distance = 7; break;
		case idMenuSongTrnDown1: distance = -1; break;
		case idMenuSongTrnDown2: distance = -2; break;
		case idMenuSongTrnDown3: distance = -3; break;
		case idMenuSongTrnDown4: distance = -4; break;
		case idMenuSongTrnDown5: distance = -5; break;
		case idMenuSongTrnDown6: distance = -6; break;
		case idMenuSongTrnDown7: distance = -7; break;
	}

	if (distance != 0)
	{
		std::wstring contents(m_songContent->GetText().wc_str());
		std::wstring contentsTransposed = bschordpro::Transposer::transpose(contents, distance);
		m_songContent->SetText(contentsTransposed);
	}
}

void MainWnd::OnAbout(wxCommandEvent &event)
{
	wxString msg = wxbuildinfo(long_f);
	msg.append(_("\n\nYet another application for typesetting song lyrics."));
	msg.append(_("\n\nFor more information visit project web page at:\nhttps://code.google.com/p/bschords/"));
	msg.append(_("\n\nAuthor: michal.nezerka@gmail.com"));
	msg.append(_("\nhttp://blue.pavoucek.cz"));

	wxMessageBox(msg, _("BSChords Application"));
}

void MainWnd::OnSongContentChange(wxCommandEvent& event)
{
	UpdateTitle();
	m_preview->Refresh();
	m_preview->Update();
}

void MainWnd::OnSongEditorChange(wxStyledTextEvent& event)
{
	UpdateTitle();
	m_preview->Refresh();
	m_preview->Update();
}

/*
Method creates meta information used by scintilla for syntax highlighting

BE AWARE !!! - stc uses internally UTF8 encoding and all position related
values are represented as offset in single byte character buffer - it means
that it is necessary to be careful for non-english characters (that occupy
more than one byte in UTF8 encoding)
*/
void MainWnd::OnSongEditorStyleNeeded(wxStyledTextEvent& event)
{
	/* mask values for bit pattern of first byte in multi-byte
     UTF-8 sequences:
       192 - 110xxxxx - for U+0080 to U+07FF
       224 - 1110xxxx - for U+0800 to U+FFFF
       240 - 11110xxx - for U+010000 to U+1FFFFF */
    static unsigned short mask[] = {192, 224, 240};

	int startPos = m_songContent->GetEndStyled();
    int firstLineNumber = m_songContent->LineFromPosition(startPos);
	int lastLineNumber = m_songContent->LineFromPosition(event.GetPosition());
	//std::cout << "EVENT - style needed from line " << firstLineNumber << " to " << lastLineNumber << std::endl;

	// loop through all lines which need to be styled
	for (int lineIx = firstLineNumber; lineIx <= lastLineNumber; lineIx++)
	{
		int style = wxSTC_CHORDPRO_TEXT;
		//int linePosMax = m_songContent->GetLineEndPosition(lineIx);
		int linePosMin = m_songContent->PositionFromLine(lineIx);
		size_t lineLength = m_songContent->LineLength(lineIx);

		m_songContent->StartStyling(linePosMin, 31);
		//m_songContent->SetStyling(linePosMax - linePosMin, wxSTC_CHORDPRO_TEXT);
		size_t stylingPos = 0;

		//std::cout << "..styling line " << lineIx << " (len: " << lineLength << ") linePosMin is " << linePosMin << std::endl;

		// loop through line characters
		wxCharBuffer buffer = m_songContent->GetLineRaw(lineIx);
		size_t linePos = 0;
		while (linePos < lineLength)
		{
			// get number of bytes for current UTF8 encoded character
			int charBytes = 1;
			if ((buffer[linePos] & mask[0]) == mask[0]) charBytes++;
			if ((buffer[linePos] & mask[1]) == mask[1]) charBytes++;
			if ((buffer[linePos] & mask[2]) == mask[2]) charBytes++;

			// skop all non ascii chars
			if (charBytes > 1)
			{
				linePos += charBytes;
				continue;
			}

			//std::cout << "....checking char " << (int)buffer[linePos] << std::endl;

			switch (buffer[linePos])
			{
				case wxChar('['):
					//std::cout << "....found [ at " << linePos << std::endl;

					// chord sections are allowed only in text context
					if (style == wxSTC_CHORDPRO_TEXT)
					{
						style = wxSTC_CHORDPRO_CHORD;
						// check if it is necessary to style previous text
						if (linePos > 0)
						{
							int ss = linePos - stylingPos;
							m_songContent->SetStyling(ss, wxSTC_CHORDPRO_TEXT);
							//std::cout << "....setting style TEXT for " << ss << std::endl;
							stylingPos += ss;
						}
					}
					break;

				case wxChar(']'):
					//std::cout << "....found ] at " << linePos << std::endl;
					if (linePos > 0 && style == wxSTC_CHORDPRO_CHORD)
					{
						int ss = linePos - stylingPos + 1;
						m_songContent->SetStyling(ss, wxSTC_CHORDPRO_CHORD);
						//std::cout << "....setting style CHORD for " << ss << std::endl;
						stylingPos += ss;
						style = wxSTC_CHORDPRO_TEXT;
					}
					break;

				case wxChar('{'):
					// cmd sections are allowed only in text context
					if (style == wxSTC_CHORDPRO_TEXT)
					{
						style = wxSTC_CHORDPRO_CMD;
						// check if it is necessary to style previous text
						if (linePos > 0)
						{
							int ss = linePos - stylingPos;
							m_songContent->SetStyling(ss, wxSTC_CHORDPRO_TEXT);
							//std::cout << "....setting style TEXT for " << ss << std::endl;
							stylingPos += ss;
						}
					}
					break;

				case wxChar('}'):
					if (linePos > 0 && style == wxSTC_CHORDPRO_CMD)
					{
						int ss = linePos - stylingPos + 1;
						m_songContent->SetStyling(ss, wxSTC_CHORDPRO_CMD);
						//std::cout << "....setting style CMD for " << ss << std::endl;
						stylingPos += ss;
						style = wxSTC_CHORDPRO_TEXT;
					}
					break;
			}

			linePos++;
		}
		// style rest of the line that wasn't catched by previous loop
		if (linePos > stylingPos)
		{
			//std::cout << " - styling rest after loop of size " << linePos - stylingPos << std::endl;
			m_songContent->SetStyling(linePos - stylingPos, wxSTC_CHORDPRO_TEXT);
			//std::cout << "....setting style TEXT (rest) for " << linePos - stylingPos << std::endl;
		}
	}
}

void MainWnd::OnChordProToken(wxCommandEvent& event)
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

void MainWnd::OnFSBrowserSelChanged(wxTreeEvent& event)
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

void MainWnd::OnFSBrowserItemAddToSongbook(wxCommandEvent& event)
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
			wxFileName fileName(data->m_path);
			fileName.MakeRelativeTo(wxGetApp().m_settings->m_rootPath);
			//std::wcout << L"  relative file path is : " << fn.GetFullPath().wc_str() << std::endl;

			m_songBookWnd->addSongFile(data->m_path);

			//wxXmlNode *rootNode = wxGetApp().m_songBook.getRootNode();
			//wxXmlNode *songNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("song"));
			//wxXmlProperty *songPathProp = new wxXmlProperty(wxT("path"), fileName.GetFullPath());
			//songNode->AddProperty(songPathProp);
			/*if (wxXmlNode *child = rootNode->GetChildren())
			{
				// look for last child
				while (child->GetNext())
					child = child->GetNext();
				child->SetNext(songNode);
			}
			else
				rootNode->AddChild(songNode);
			*/
			m_songBookWnd->Update();
		}
	}
}

void MainWnd::OnFSBrowserItemMenu(wxTreeEvent& event)
{
    //wxTreeItemId itemId = event.GetItem();
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

void MainWnd::OpenFile(const wxString filePath)
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

	m_file.m_path = fileName.GetFullPath();

	bool success = false;
    wxFile file(m_file.m_path, wxFile::read);
    if (file.IsOpened())
    {
        wxString contents;
        // get the file size (assume it is not huge file...)
        ssize_t len = (ssize_t)file.Length();
        if (len > 0)
        {
            wxMemoryBuffer buffer(len+1);
            success = (file.Read(buffer.GetData(), len) == len);
            if (success) {
                ((char*)buffer.GetData())[len] = 0;
                contents = wxString(buffer, wxConvUTF8, len);
            }
        }
        else
        {
            if (len == 0)
                success = true;  // empty file is ok
            else
                success = false; // len == wxInvalidOffset
        }

        if (success)
        {
            m_songContent->SetText(contents);
            m_songContent->EmptyUndoBuffer();
            m_songContent->SetSavePoint();
        }
    }
}

void MainWnd::CloseFile()
{
	if (m_songContent->GetModify())
	{
		if (wxMessageBox(_("Do you want to save changes?"), wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_ASTERISK) == wxYES)
		{
			SaveFile();
		}
	}
	m_file.clear();
	m_songContent->ClearAll();
	m_songContent->EmptyUndoBuffer();
	m_songContent->SetSavePoint();
	UpdateTitle();
}

void MainWnd::SaveFile()
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

	wxFile file(fileName.GetFullPath(), wxFile::write);

    if (file.IsOpened())
	{
		bool success = file.Write(m_songContent->GetText(), wxConvUTF8);

		if (success)
		{
			m_songContent->SetSavePoint();
			UpdateTitle();
		}
	}
}

void MainWnd::SaveSongBook()
{
	std::cout << "SaveSongBook called" << std::endl;

	if (m_songBookPath.Length() == 0)
	{
		wxFileName fileName(m_dirCtrl->GetPath());
		wxString dir = fileName.GetPath();
		wxString name = _("untitled.xml");
		wxFileDialog* saveDlg = new wxFileDialog(this, _("Save file as"), dir, name, _("*.xml"), wxSAVE, wxDefaultPosition);

		if (saveDlg->ShowModal() == wxID_OK )
			m_songBookPath = saveDlg->GetPath();
		else
			return;
	}

	wxFileName fileName(m_songBookPath);

	if (fileName.IsDir())
	{
		wxMessageBox(_("Cannot save song to directory"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	std::wcout << L"saving song book to file " << fileName.GetFullPath().wc_str() << std::endl;
	std::wcout << L"Number of items: " << fileName.GetFullPath().wc_str() << std::endl;

	wxGetApp().m_songBook.saveToXmlFile(fileName.GetFullPath(), wxGetApp().m_settings->m_rootPath);
}

void MainWnd::OpenSongBook(const wxString filePath)
{
	// TODO Check if old songbook needs to be saved

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

	std::wcout << L"loading songbook file " << fileName.GetFullPath().wc_str() << std::endl;

	wxGetApp().m_songBook.loadFromXmlFile(fileName.GetFullPath());

	m_songBookWnd->Update();
}

void MainWnd::UpdateTitle()
{
	wxString title;

	if (m_file.m_path.Length() > 0)
	{
		wxFileName tmp(m_file.m_path);

		if (m_songContent->GetModify())
			title += _("*");
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

void MainWnd::OnPaneClose(wxAuiManagerEvent& evt)
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

void MainWnd::updateEditorStyles()
{
	if (!m_songContent)
		return;

    m_songContent->StyleSetForeground(wxSTC_CHORDPRO_TEXT,	wxGetApp().m_settings->m_editorColorText);
	m_songContent->StyleSetForeground(wxSTC_CHORDPRO_CHORD,	wxGetApp().m_settings->m_editorColorChords);
    m_songContent->StyleSetForeground(wxSTC_CHORDPRO_CMD,	wxGetApp().m_settings->m_editorColorCommands);
    m_songContent->StyleSetFont(wxSTC_CHORDPRO_TEXT, wxGetApp().m_settings->m_editorFont);
    m_songContent->StyleSetFont(wxSTC_CHORDPRO_CHORD, wxGetApp().m_settings->m_editorFont);
    m_songContent->StyleSetFont(wxSTC_CHORDPRO_CMD, wxGetApp().m_settings->m_editorFont);
}

// ----------------------------------------------------------------------------
// BSChordsPrintout
// ----------------------------------------------------------------------------

void BSChordsPrintout::OnPreparePrinting()
{
	std::cout << "OnPreparePrinting" << std::endl;

	// set user scale to fit a4
	//GetPageSizeMM(&pageSizeXMM, &pageSizeYMM);
	//GetPageSizePixels(&pageSizeXPx, &pageSizeYPx);

    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);

	wxSize paperPixels;
	paperPixels.SetWidth(ppiScreenX * 210 / MM_PER_IN);
	paperPixels.SetHeight(ppiScreenY * 290 / MM_PER_IN);
	FitThisSizeToPaper(paperPixels);
	double newScaleX, newScaleY;
	GetDC()->GetUserScale(&newScaleX, &newScaleY);
	//cout << "new user scale: " << newScaleX << "x" << newScaleY << endl;

	float scale = ppiScreenX / MM_PER_IN;

	TSetDCPainter pn(*GetDC(), scale);
	bschordpro::Parser p(&pn);
	p.parse(std::wstring(mContents.wc_str()));
	mPages = pn.getPages();
}

void BSChordsPrintout::OnEndPrinting()
{
	std::cout << "OnEndPrinting" << std::endl;
	//delete mPainter;
	//mPainter = NULL;
};

bool BSChordsPrintout::OnPrintPage(int page)
{
	std::cout << "OnPrintPage " << page << std::endl;
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page >= 1 && page <= (int)mPages)
			mPainter->drawPage(page - 1);

        return true;
    }
    else
        return false;
}

/* it is necessary to redraw document since DC context is new - e.g. when
  print preview window zoom is chnaged. This cannot be done only once in
  OnPreparePrinting.  */
bool BSChordsPrintout::OnBeginDocument(int startPage, int endPage)
{
	std::cout << "OnBeginDocument" << std::endl;
	if (!mPainter)
		std::cout << "mPainter is NULL" << std::endl;
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);

	wxSize paperPixels;
	paperPixels.SetWidth(ppiScreenX * 210 / MM_PER_IN);
	paperPixels.SetHeight(ppiScreenY * 290 / MM_PER_IN);
	FitThisSizeToPaper(paperPixels);
	double newScaleX, newScaleY;
	GetDC()->GetUserScale(&newScaleX, &newScaleY);
	float scale = ppiScreenX / MM_PER_IN;

	mPainter = new TSetDCPainter(*GetDC(), scale);
	bschordpro::Parser p(mPainter);
	p.parse(std::wstring(mContents.wc_str()));

    return true;
}

void BSChordsPrintout::OnEndDocument()
{
	wxPrintout::OnEndDocument();
	delete(mPainter);
	mPainter = NULL;
}

void BSChordsPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
	std::cout << "OnGetPageInfo" << std::endl;
    *minPage = 1;
    *maxPage = mPages;
    *selPageFrom = 1;
    *selPageTo = *maxPage;
}

bool BSChordsPrintout::HasPage(int pageNum)
{
	std::cout << "HasPage" << std::endl;
    return (pageNum >= 1 && pageNum <= (int)mPages);
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
    dc->DrawLine((long)leftMarginLogical, (long)(topMarginLogical+yExtent),
                  (long)rightMarginLogical, (long)topMarginLogical+yExtent);

    return true;
}
