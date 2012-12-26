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
#include <wx/pdfdoc.h>
#include <wx/pdffontmanager.h>

#include "bschordsMain.h"
#include "bschordsPreferences.h"
#include "bschordsicon.xpm"

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
	EVT_MENU(idMenuQuit, bschordsFrame::OnQuit)
	EVT_MENU(idMenuPreferences, bschordsFrame::OnPreferences)
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
	fileMenu->Append(wxID_ANY, _("Song &information..."), _("Close song file"));
	fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
	mbar->Append(fileMenu, _("&File"));

	wxMenu* editMenu = new wxMenu(_T(""));
	editMenu->Append(idMenuPreferences, _("&Preferences..."), _("Application preferences"));
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
		wxPdfDocument pdf;
		pdf.Open();
		pdf.SetFont(_("Helvetica"), _(""), 10);

		// Page 1
		pdf.AddPage();
		pdf.Write(20, _("This is sample text"));
		pdf.SaveAsFile(saveDlg->GetPath());
	}
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
            wxGetApp().m_fonts[i] = dlg->m_fonts[i];
        }
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

