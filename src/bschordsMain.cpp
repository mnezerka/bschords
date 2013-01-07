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

/*
#if !wxUSE_PRINTING_ARCHITECTURE
    #error "You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h, and recompile the library."
#endif
*/
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
//#include <wx/pdfdoc.h>
//#include <wx/pdffontmanager.h>

#include "bschordsMain.h"
#include "bschordsPreferences.h"
#include "songstylesheetdlg.h"
#include "bschordsicon.xpm"
#include "bschordsdcpainter.h"

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

#include "res/new.xpm"
#include "res/open.xpm"
#include "res/save.xpm"
#include "res/copy.xpm"
#include "res/cut.xpm"
#include "res/preview.xpm"  // paste XPM
#include "res/print.xpm"
#include "res/help.xpm"
#include "res/chord.xpm"

enum
{
	idMenuQuit = 1000,
	idMenuAbout,
	idMenuPreferences,
	ID_MENU_STYLESHEET,
	idMenuViewEditor,
	idMenuViewPreview,
	ID_MENU_FILE_EXPORT,
	ID_COMBO,
	ID_FSBROWSER,
	ID_SPIN,
	ID_TOOLBAR_CHORD,
	ID_TOOLBAR ,
	IDM_TOOLBAR_TOGGLETOOLBARSIZE,
	IDM_TOOLBAR_TOGGLETOOLBARROWS,
	IDM_TOOLBAR_TOGGLETOOLTIPS,
	IDM_TOOLBAR_TOGGLECUSTOMDISABLED,
	IDM_TOOLBAR_ENABLEPRINT,
	IDM_TOOLBAR_DELETEPRINT,
	IDM_TOOLBAR_INSERTPRINT,
	IDM_TOOLBAR_TOGGLEHELP,
	IDM_TOOLBAR_TOGGLERADIOBTN1,
	IDM_TOOLBAR_TOGGLERADIOBTN2,
	IDM_TOOLBAR_TOGGLERADIOBTN3,
	IDM_TOOLBAR_TOGGLE_TOOLBAR,
	IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
	IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
	IDM_TOOLBAR_CHANGE_TOOLTIP,
	IDM_TOOLBAR_SHOW_TEXT,
	IDM_TOOLBAR_SHOW_ICONS,
	IDM_TOOLBAR_SHOW_BOTH,
	IDM_TOOLBAR_CUSTOM_PATH,
	IDM_TOOLBAR_TOP_ORIENTATION,
	IDM_TOOLBAR_LEFT_ORIENTATION,
	IDM_TOOLBAR_BOTTOM_ORIENTATION,
	IDM_TOOLBAR_RIGHT_ORIENTATION,
	IDM_TOOLBAR_OTHER_1,
	IDM_TOOLBAR_OTHER_2,
	IDM_TOOLBAR_OTHER_3,
};

static const double zoomFactors[] = {.1, .5, 1, 1.5, 2, 3 };

BEGIN_EVENT_TABLE(bschordsFrame, wxFrame)
	EVT_CLOSE(bschordsFrame::OnClose)
	EVT_MENU(wxID_NEW, bschordsFrame::OnFileNewSong)
	EVT_MENU(wxID_OPEN, bschordsFrame::OnFileOpenSong)
	EVT_MENU(wxID_SAVE, bschordsFrame::OnFileSaveSong)
	EVT_MENU(wxID_SAVEAS, bschordsFrame::OnFileSaveAsSong)
	EVT_MENU(wxID_CLOSE, bschordsFrame::OnFileCloseSong)
	EVT_MENU(ID_MENU_FILE_EXPORT, bschordsFrame::OnFileExportSong)
	EVT_MENU(wxID_PRINT, bschordsFrame::OnFilePrint)
	EVT_MENU(wxID_PREVIEW, bschordsFrame::OnFilePrintPreview)
	EVT_MENU(wxID_PRINT_SETUP, bschordsFrame::OnFilePageSetup)
	EVT_MENU(idMenuQuit, bschordsFrame::OnQuit)
	EVT_MENU(idMenuPreferences, bschordsFrame::OnPreferences)
	EVT_MENU(ID_MENU_STYLESHEET, bschordsFrame::OnStyleSheet)
	EVT_MENU(idMenuAbout, bschordsFrame::OnAbout)
	EVT_MENU(idMenuViewEditor, bschordsFrame::OnViewEditor)
	EVT_MENU(idMenuViewPreview, bschordsFrame::OnViewPreview)
	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, bschordsFrame::OnSongContentChange)
	EVT_TOOL(ID_TOOLBAR_CHORD, bschordsFrame::OnToolChord)
	EVT_TREE_SEL_CHANGED(wxID_TREECTRL, bschordsFrame::OnFSBrowserSelChanged )
	EVT_COMBOBOX(ID_COMBO, bschordsFrame::OnZoomChanged)
END_EVENT_TABLE()

bschordsFrame::bschordsFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title), m_fileChanged(false)
{
	SetIcon(wxICON(bschordsicon));

	// create a menu bar
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_T(""));
	fileMenu->Append(wxID_NEW, _("&New song"), _("Create a new song file"));
	fileMenu->Append(wxID_OPEN, _("&Open song..."), _("Open a song file"));
	fileMenu->Append(wxID_SAVE, _("&Save song"), _("Save the active song file"));
	fileMenu->Append(wxID_SAVEAS, _("Save song &as..."), _("Save the active song under different a name"));
	fileMenu->Append(wxID_CLOSE, _("&Close song"), _("Close song file"));
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
	viewMenu->Append(idMenuViewEditor, _("&Editor"), _("Song editor"), true);
	viewMenu->Append(idMenuViewPreview, _("&Preview"), _("Song preview"), true);
	mbar->Append(viewMenu, _("&View"));

	wxMenu* helpMenu = new wxMenu(_T(""));
	helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
	mbar->Append(helpMenu, _("&Help"));

	// these items should be initially checked
	mbar->Check(idMenuViewEditor, true);
	mbar->Check(idMenuViewPreview, true);

	SetMenuBar(mbar);

	// Create the toolbar
	m_toolBar = CreateToolBar(wxTB_TOP, ID_TOOLBAR);
	PopulateToolbar();

	// create a status bar with some information about the used wxWidgets version
	CreateStatusBar(2);
	SetStatusText(_("This is BSChords application!"),0);
	SetStatusText(wxbuildinfo(short_f), 1);

	int top = wxGetApp().config->Read(_("/global/top"), 100);
	int left = wxGetApp().config->Read(_("/global/left"), 100);
	int width = wxGetApp().config->Read(_("/global/width"), 500);
	int height = wxGetApp().config->Read(_("/global/height"), 500);
	SetSize(left, top, width, height, 0);

	m_splitterMain = new wxSplitterWindow(this, -1, wxDefaultPosition,  wxDefaultSize, wxSP_BORDER);
	m_splitterSong = new wxSplitterWindow(m_splitterMain, -1, wxDefaultPosition, wxDefaultSize, wxSP_BORDER);

	m_songContent = new wxRichTextCtrl(m_splitterSong, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
	m_songContent->DiscardEdits();

	m_preview = new bschordsPreview(m_splitterSong, m_songContent);
	//m_preview->SetScrollbars(20, 20, 50, 50);
	//m_preview->SetBackgroundColour(wxColour(255, 255, 255));
	//m_preview->SetBackgroundStyle(wxBG_STYLE_COLOUR);

	m_dirCtrl = new wxGenericDirCtrl(m_splitterMain, ID_FSBROWSER, _T(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("Chordpro songs (*.txt)|*.txt"), 0 );
	wxString path = wxGetApp().config->Read(_("/global/path"));
	m_dirCtrl->SetPath(path);

	m_splitterMain->SplitVertically(m_dirCtrl, m_splitterSong, 100);
	m_splitterSong->SplitVertically(m_songContent, m_preview, 0);
}

bschordsFrame::~bschordsFrame()
{

}


void bschordsFrame::PopulateToolbar()
{
	// Set up toolbar
	enum
	{
		Tool_new,
		Tool_open,
		Tool_save,
		Tool_copy,
		Tool_cut,
		Tool_paste,
		Tool_print,
		Tool_help,
		Tool_chord,
		Tool_Max
	};

	wxBitmap toolBarBitmaps[Tool_Max];

#define INIT_TOOL_BMP(bmp) \
	toolBarBitmaps[Tool_##bmp] = wxBitmap(bmp##_xpm)

	INIT_TOOL_BMP(new);
	INIT_TOOL_BMP(open);
	INIT_TOOL_BMP(save);
	INIT_TOOL_BMP(copy);
	INIT_TOOL_BMP(cut);
	INIT_TOOL_BMP(paste);
	INIT_TOOL_BMP(print);
	INIT_TOOL_BMP(help);
	INIT_TOOL_BMP(chord);

	int w = toolBarBitmaps[Tool_new].GetWidth(),
			h = toolBarBitmaps[Tool_new].GetHeight();

	for ( size_t n = Tool_new; n < WXSIZEOF(toolBarBitmaps); n++ )
	{
		toolBarBitmaps[n] = wxBitmap(toolBarBitmaps[n].ConvertToImage().Scale(w, h));
	}

	m_toolBar->SetToolBitmapSize(wxSize(w, h));

	m_toolBar->AddTool(wxID_NEW, _T("New"), toolBarBitmaps[Tool_new], wxNullBitmap, wxITEM_NORMAL, _T("New file"), _T("This is help for new file tool"));
	m_toolBar->AddTool(wxID_OPEN, _T("Open"), toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL, _T("Open file"), _T("This is help for open file tool"));
	m_toolBar->AddTool(wxID_SAVE, _T("Save"), toolBarBitmaps[Tool_save], _T("Save file"), wxITEM_NORMAL);
	//m_toolBar->AddTool(wxID_COPY, _T("Copy"), toolBarBitmaps[Tool_copy], _T("Toggle button 2"), wxITEM_CHECK);
	//m_toolBar->AddTool(wxID_CUT, _T("Cut"), toolBarBitmaps[Tool_cut], _T("Toggle/Untoggle help button"));
	//m_toolBar->AddTool(wxID_PASTE, _T("Paste"), toolBarBitmaps[Tool_paste], _T("Paste"));

	m_toolBar->AddSeparator();
	m_toolBar->AddTool(ID_TOOLBAR_CHORD, _T("Chord"), toolBarBitmaps[Tool_chord], _T("Chord"));

	m_toolBar->AddSeparator();
	// adding a combo to a vertical toolbar is not very smart
	m_zoomCtrl = new wxComboBox(m_toolBar, ID_COMBO, wxEmptyString, wxDefaultPosition, wxSize(100,-1), 0, NULL, wxCB_READONLY);
	m_zoomCtrl->Append(_("10%"), (void*)&zoomFactors[0]);
	m_zoomCtrl->Append(_("50%"), (void*)&zoomFactors[1]);
	m_zoomCtrl->Append(_("100%"), (void*)&zoomFactors[2]);
	m_zoomCtrl->Append(_("150%"), (void*)&zoomFactors[3]);
	m_zoomCtrl->Append(_("200%"), (void*)&zoomFactors[4]);
	m_zoomCtrl->Select(2);
	m_toolBar->AddControl(m_zoomCtrl);

	/*wxSpinCtrl *spin = new wxSpinCtrl( toolBar, ID_SPIN, wxT("0"), wxDefaultPosition, wxSize(80,wxDefaultCoord), 0, 0, 100 );
	toolBar->AddControl( spin );*/

	// after adding the buttons to the toolbar, must call Realize() to reflect
	// the changes
	m_toolBar->Realize();

	m_toolBar->SetRows(1);
}

void bschordsFrame::OnClose(wxCloseEvent &event)
{
	// close open file (if any)
	CloseFile();

	Destroy();
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

	if (m_filePath.Length() > 0)
	{
		wxFileName fileName(m_filePath);
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
		m_filePath = saveDlg->GetPath();
		SaveFile();
	}
}

void bschordsFrame::OnFileCloseSong(wxCommandEvent& event)
{
	CloseFile();
}

void bschordsFrame::OnFileExportSong(wxCommandEvent& event)
{
	wxString dir;
	wxString name;

	if (m_filePath.Length() == 0)
		return;


	wxFileName fileName(m_filePath);
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
    	std::cout << "deleting preview" << std::endl;
        delete preview;
        wxLogError(wxT("There was a problem previewing.\nPerhaps your current printer is not set correctly?"));
        return;
    }

	std::cout << "Creating preview frame" << std::endl;

    wxPreviewFrame *frame = new wxPreviewFrame(
		preview, this, wxT("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
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

	if (dlg->ShowModal() == wxID_OK)
    {
        // copy font information from preferences to application
        for (int i = 0; i < BS_FONT_LAST; i++)
        {
            std::cout << i << " native info: " << dlg->m_fonts[i].GetNativeFontInfoDesc().mb_str(wxConvUTF8) << std::endl;
            wxGetApp().m_styleSheet.m_fonts[i] = dlg->m_fonts[i];
        }
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

void bschordsFrame::OnViewEditor(wxCommandEvent& event)
{
	wxMenuBar *mBar = GetMenuBar();
	std::cout << "OnViewEditor" << mBar->IsChecked(idMenuViewEditor) << std::endl;
	if (mBar->IsChecked(idMenuViewEditor))
	{
		// hide editor window
		m_songContent->Show(true);
		m_splitterSong->SplitVertically(m_songContent, m_preview);
		m_toolBar->EnableTool(ID_TOOLBAR_CHORD, true);
	}
	else
	{
		// show editor window
		m_songContent->Show(false);
		m_splitterSong->Unsplit(m_songContent);
		m_toolBar->EnableTool(ID_TOOLBAR_CHORD, false);
	}
}

void bschordsFrame::OnViewPreview(wxCommandEvent& event)
{

}

void bschordsFrame::OnAbout(wxCommandEvent &event)
{
	wxString msg = wxbuildinfo(long_f);
	msg.append(_("\n\nmichal.nezerka@gmail.com\n\nhttp://blue.pavoucek.cz"));
	wxMessageBox(msg, _("BSChords application"));
}

void bschordsFrame::OnSongContentChange(wxCommandEvent& event)
{
	//wxMessageBox(_("This is content change event"), _("BSChords application"));
	m_fileChanged = true;
	UpdateTitle();
	m_preview->Refresh();
	m_preview->Update();
}

void bschordsFrame::OnToolChord(wxCommandEvent& WXUNUSED(event))
{
	/*wxMessageBox(_("chord pressed"));
	m_ textWindow->AppendText(
	        wxString::Format(_T("Tool %d right clicked.\n"),
	                         (int) event.GetInt()));*/

	m_songContent->WriteText(_("[]"));
}

void bschordsFrame::OnFSBrowserSelChanged(wxTreeEvent& event)
{
	wxTreeItemId id = event.GetItem();
	if (!id)
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
		lines = fileIn.GetFirstLine();
		// Read all the lines (one by one)
		while(!fileIn.Eof())
		{
			if (lines.size() > 0)
				lines += wxT('\n');
			lines += fileIn.GetNextLine();
		}
		fileIn.Close(); // Close the opened file
		//wxLogMessage(lines);
		m_songContent->Clear();
		m_songContent->AppendText(lines);
		m_songContent->DiscardEdits();

		m_filePath = fileName.GetFullPath();
		m_fileChanged = false;

		UpdateTitle();
	}
}

void bschordsFrame::CloseFile()
{
	if (m_songContent->IsModified())
	{
		if (wxMessageBox(_("Do you want to save changes?"), wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_ASTERISK) == wxYES)
		{
			SaveFile();
		}
	}

	m_filePath.Empty();
	m_songContent->Clear();
}

void bschordsFrame::SaveFile()
{
	std::cout << "SaveFile called" << std::endl;

	if (m_filePath.Length() == 0)
	{
		wxFileName fileName(m_dirCtrl->GetPath());
		wxString dir = fileName.GetPath();
		wxString name = _("untitled.txt");
		wxFileDialog* saveDlg = new wxFileDialog(this, _("Save file as"), dir, name, _("*.txt"), wxSAVE, wxDefaultPosition);

		if (saveDlg->ShowModal() == wxID_OK )
			m_filePath = saveDlg->GetPath();
		else
			return;
	}

	wxFileName fileName(m_filePath);

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
		int lines = m_songContent->GetNumberOfLines();
		for (int i = 0; i < lines; i++)
			fileOut.AddLine(m_songContent->GetLineText(i));
		fileOut.Write();
		fileOut.Close(); // Close the opened file
		m_songContent->DiscardEdits();
		UpdateTitle();
	}

}

void bschordsFrame::UpdateTitle()
{
	wxString title;

	if (m_filePath.Length() > 0)
	{
		wxFileName tmp(m_filePath);

		if (m_songContent->IsModified())
		//if (m_fileChanged)
			title += _("*");
		title += tmp.GetFullName();
	}
	else
		title += _("untitled.txt");

	title += _(" - BSChords");
	SetTitle(title);
}

void bschordsFrame::OnZoomChanged(wxCommandEvent& event)
{
	int sel = m_zoomCtrl->GetSelection();
	if (sel!= wxNOT_FOUND)
	{
		double newZoom = *(double*)m_zoomCtrl->GetClientData(sel);
		std::cout << "New zoom value is: " << newZoom << std::endl;
		m_preview->setZoom(newZoom);
	}

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

        dc->DrawText(wxString::Format(wxT("PAGE %d"), page), 0, 0);

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
	cout << "page size mm: " << pageSizeXMM << "x" << pageSizeYMM << endl;

	int pageSizeXPx, pageSizeYPx;
	GetPageSizePixels(&pageSizeXPx, &pageSizeYPx);
	cout << "page size px: " << pageSizeXPx << "x" << pageSizeYPx << endl;

	wxSize dcSize = GetDC()->GetSize();
	cout << "dc size in px: " << dcSize.GetWidth() << "x" << dcSize.GetHeight() << endl;

	wxRect paperRect = GetPaperRectPixels();
	cout << "paper rect in px: " << paperRect.GetLeft() << "," << paperRect.GetTop() << " - " << paperRect.GetRight() << "," << paperRect.GetBottom() << " - " << paperRect.GetWidth() << "x" << paperRect.GetHeight() << endl;

    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    cout << "screen ppi: " << ppiScreenX << "x" << ppiScreenY << endl;
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    cout << "printer ppi: " << ppiPrinterX << "x" << ppiPrinterY << endl;

	wxSize paperPixels;
	if (IsPreview())
	{
		paperPixels.SetWidth(ppiScreenX * 210 / MM_PER_IN);
		paperPixels.SetWidth(ppiScreenY * 290 / MM_PER_IN);
	}
	else
	{
		paperPixels.SetWidth(ppiPrinterX * 210 / MM_PER_IN);
		paperPixels.SetWidth(ppiPrinterY * 290 / MM_PER_IN);
	}
	FitThisSizeToPaper(paperPixels);
	double newScaleX, newScaleY;
	GetDC()->GetUserScale(&newScaleX, &newScaleY);
	cout << "new user scale: " << newScaleX << "x" << newScaleY << endl;

    return true;
}

void BSChordsPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
	std::cout << "OnGetPageInfo" << std::endl;
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
    // You might use THIS code if you were scaling graphics of known size to fit
    // on the page. The commented-out code illustrates different ways of scaling
    // the graphics.

    // We know the graphic is 230x350. If we didn't know this, we'd need to
    // calculate it.
    //wxCoord maxX = 230;
    //wxCoord maxY = 350;

    // This sets the user scale and origin of the DC so that the image fits
    // within the paper rectangle (but the edges could be cut off by printers
    // that can't print to the edges of the paper -- which is most of them. Use
    // this if your image already has its own margins.
// FitThisSizeToPaper(wxSize(maxX, maxY));
// wxRect fitRect = GetLogicalPaperRect();

    // This sets the user scale and origin of the DC so that the image fits
    // within the page rectangle, which is the printable area on Mac and MSW
    // and is the entire page on other platforms.
// FitThisSizeToPage(wxSize(maxX, maxY));
// wxRect fitRect = GetLogicalPageRect();

    // This sets the user scale and origin of the DC so that the image fits
    // within the page margins as specified by g_PageSetupData, which you can
    // change (on some platforms, at least) in the Page Setup dialog. Note that
    // on Mac, the native Page Setup dialog doesn't let you change the margins
    // of a wxPageSetupDialogData object, so you'll have to write your own dialog or
    // use the Mac-only wxMacPageMarginsDialog, as we do in this program.
    //TODO: MN: FitThisSizeToPageMargins(wxSize(maxX, maxY), *wxGetApp().m_pageSetupData);
    //TODO: MN: wxRect fitRect = GetLogicalPageMarginsRect(*wxGetApp().m_pageSetupData);

    // This sets the user scale and origin of the DC so that the image appears
    // on the paper at the same size that it appears on screen (i.e., 10-point
    // type on screen is 10-point on the printed page) and is positioned in the
    // top left corner of the page rectangle (just as the screen image appears
    // in the top left corner of the window).
// MapScreenSizeToPage();
// wxRect fitRect = GetLogicalPageRect();

    // You could also map the screen image to the entire paper at the same size
    // as it appears on screen.
// MapScreenSizeToPaper();
// wxRect fitRect = GetLogicalPaperRect();

    // You might also wish to do you own scaling in order to draw objects at
    // full native device resolution. In this case, you should do the following.
    // Note that you can use the GetLogicalXXXRect() commands to obtain the
    // appropriate rect to scale to.
// MapScreenSizeToDevice();
// wxRect fitRect = GetLogicalPageRect();

    // Each of the preceding Fit or Map routines positions the origin so that
    // the drawn image is positioned at the top left corner of the reference
    // rectangle. You can easily center or right- or bottom-justify the image as
    // follows.

    // This offsets the image so that it is centered within the reference
    // rectangle defined above.
    //wxCoord xoff = (fitRect.width - maxX) / 2;
    //wxCoord yoff = (fitRect.height - maxY) / 2;
    //OffsetLogicalOrigin(xoff, yoff);

    // This offsets the image so that it is positioned at the bottom right of
    // the reference rectangle defined above.
// wxCoord xoff = (fitRect.width - maxX);
// wxCoord yoff = (fitRect.height - maxY);
// OffsetLogicalOrigin(xoff, yoff);

	GetDC()->SetBackground(*wxWHITE_BRUSH);
    // dc.Clear();
    //GetDC()->SetFont(m_testFont);

    // dc.SetBackgroundMode(wxTRANSPARENT);

    GetDC()->SetPen(*wxBLACK_PEN);
    GetDC()->SetBrush(*wxLIGHT_GREY_BRUSH);

    GetDC()->DrawRectangle(0, 0, 230, 350);
    GetDC()->DrawLine(0, 0, 229, 349);
    GetDC()->DrawLine(229, 0, 0, 349);
    GetDC()->SetBrush(*wxTRANSPARENT_BRUSH);

    GetDC()->SetBrush(*wxCYAN_BRUSH);
    GetDC()->SetPen(*wxRED_PEN);

    GetDC()->DrawRoundedRectangle(0, 20, 200, 80, 20);

    GetDC()->DrawText(_("Rectangle 200 by 80"), 40, 40);



	//int paperSizeXMM, paperSizeYMM;
	//GetPaperSizeMM(&paperSizeXMM, &paperSizeYMM);
	//cout << "paper size mm: " << paperSizeXMM << "x" << paperSizeYMM << endl;
	//GetDC()->SetUserScale(m_zoom, m_zoom);

    // get lines from song book control
	int lines = m_frame->m_songContent->GetNumberOfLines();
	wxString text;
	for (int i = 0; i < lines; i++)
    {
        if (text.size() > 0)
			text.Append(wxT("\n"));
		text.Append(m_frame->m_songContent->GetLineText(i));
	}

	BSChordsDCPainter y(*GetDC());
	BSChordProParser p(&y);

	//wcout << text.wc_str() << endl;
	p.parse(std::wstring(text.wc_str()));

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
