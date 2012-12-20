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

const int ID_TOOLBAR = 500;

enum
{
	IDM_TOOLBAR_TOGGLETOOLBARSIZE = 200,
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

enum
{
	idMenuQuit = 1000,
	idMenuAbout,
	idMenuPreferences,
	idMenuViewEditor,
	idMenuViewPreview,
	ID_COMBO,
	ID_FSBROWSER,
	ID_SPIN,
	ID_TOOLBAR_CHORD,
};

BEGIN_EVENT_TABLE(bschordsFrame, wxFrame)
	EVT_CLOSE(bschordsFrame::OnClose)
	EVT_MENU(idMenuQuit, bschordsFrame::OnQuit)
	EVT_MENU(idMenuPreferences, bschordsFrame::OnPreferences)
	EVT_MENU(idMenuAbout, bschordsFrame::OnAbout)
	EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, bschordsFrame::OnSongContentChange)
	EVT_TOOL(ID_TOOLBAR_CHORD, bschordsFrame::OnToolChord)
	EVT_TREE_SEL_CHANGED(wxID_TREECTRL, bschordsFrame::OnFSBrowserSelChanged )
END_EVENT_TABLE()

bschordsFrame::bschordsFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title)
{
	SetIcon(wxICON(bschordsicon));

	// create a menu bar
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_T(""));
	fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
	mbar->Append(fileMenu, _("&File"));

	wxMenu* editMenu = new wxMenu(_T(""));
	editMenu->Append(idMenuPreferences, _("&Preferences"), _("Application preferences"));
	mbar->Append(editMenu, _("&Edit"));

	wxMenu* viewMenu = new wxMenu(_T(""));
	viewMenu->Append(idMenuViewEditor, _("&Editor"), _("Song editor"));
	viewMenu->Append(idMenuViewEditor, _("&Preview"), _("Song preview"));
	mbar->Append(viewMenu, _("&View"));

	wxMenu* helpMenu = new wxMenu(_T(""));
	helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
	mbar->Append(helpMenu, _("&Help"));

	SetMenuBar(mbar);

	// Create the toolbar
	RecreateToolbar();

	// create a status bar with some information about the used wxWidgets version
	CreateStatusBar(2);
	SetStatusText(_("This is BSChords application!"),0);
	SetStatusText(wxbuildinfo(short_f), 1);

	int top = wxGetApp().config->Read(_("/global/top"), 100);
	int left = wxGetApp().config->Read(_("/global/left"), 100);
	int width = wxGetApp().config->Read(_("/global/width"), 500);
	int height = wxGetApp().config->Read(_("/global/height"), 500);
	SetSize(left, top, width, height, 0);

	wxSplitterWindow *splitterMain = new wxSplitterWindow(this, -1, wxDefaultPosition,  wxDefaultSize, wxSP_BORDER);

	wxSplitterWindow *splitterSong = new wxSplitterWindow(splitterMain, -1, wxDefaultPosition, wxDefaultSize, wxSP_BORDER);

	m_songContent = new wxRichTextCtrl(splitterSong, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);

	m_preview = new bschordsPreview(splitterSong, m_songContent);
	//m_preview->SetScrollbars(20, 20, 50, 50);
	//m_preview->SetBackgroundColour(wxColour(255, 255, 255));
	//m_preview->SetBackgroundStyle(wxBG_STYLE_COLOUR);

	m_dirCtrl = new wxGenericDirCtrl(splitterMain, ID_FSBROWSER, _T(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("Chordpro songs (*.txt)|*.txt"), 0 );
	wxString path = wxGetApp().config->Read(_("/global/path"));
	m_dirCtrl->SetPath(path);

	splitterMain->SplitVertically(m_dirCtrl, splitterSong, 100);
	splitterSong->SplitVertically(m_songContent, m_preview, 0);
}

bschordsFrame::~bschordsFrame()
{

}

void bschordsFrame::RecreateToolbar()
{
	wxToolBar *toolBar = CreateToolBar(wxTB_TOP, ID_TOOLBAR);

	PopulateToolbar(toolBar);
}

void bschordsFrame::PopulateToolbar(wxToolBarBase* toolBar)
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

	toolBar->SetToolBitmapSize(wxSize(w, h));

	toolBar->AddTool(wxID_NEW, _T("New"), toolBarBitmaps[Tool_new], wxNullBitmap, wxITEM_NORMAL,
					 _T("New file"), _T("This is help for new file tool"));
	toolBar->AddTool(wxID_OPEN, _T("Open"), toolBarBitmaps[Tool_open], wxNullBitmap, wxITEM_NORMAL,
					 _T("Open file"), _T("This is help for open file tool"));

	toolBar->AddTool(wxID_SAVE, _T("Save"), toolBarBitmaps[Tool_save], _T("Toggle button 1"), wxITEM_CHECK);
	toolBar->AddTool(wxID_COPY, _T("Copy"), toolBarBitmaps[Tool_copy], _T("Toggle button 2"), wxITEM_CHECK);
	toolBar->AddTool(wxID_CUT, _T("Cut"), toolBarBitmaps[Tool_cut], _T("Toggle/Untoggle help button"));
	toolBar->AddTool(wxID_PASTE, _T("Paste"), toolBarBitmaps[Tool_paste], _T("Paste"));

	toolBar->AddSeparator();
	toolBar->AddTool(ID_TOOLBAR_CHORD, _T("Chord"), toolBarBitmaps[Tool_chord], _T("Chord"));

	// adding a combo to a vertical toolbar is not very smart
	wxComboBox *combo = new wxComboBox(toolBar, ID_COMBO, wxEmptyString, wxDefaultPosition, wxSize(100,-1) );
	combo->Append(_T("100%"));
	combo->Append(_T("50%"));
	toolBar->AddControl(combo);

	/*wxSpinCtrl *spin = new wxSpinCtrl( toolBar, ID_SPIN, wxT("0"), wxDefaultPosition, wxSize(80,wxDefaultCoord), 0, 0, 100 );
	toolBar->AddControl( spin );*/

	// after adding the buttons to the toolbar, must call Realize() to reflect
	// the changes
	toolBar->Realize();

	toolBar->SetRows(1);
}

void bschordsFrame::OnClose(wxCloseEvent &event)
{
	Destroy();
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
			wxMessageBox(_("ahoj"));
		};
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
					//m_browserCtrl->ShowFolder(data->m_path);
					//std::cout << "loading file..." << data->m_path.c_str() << std::endl;
					std::cout << "loading file..." << std::endl;
					//wxMessageBox(data->m_path);
					wxTextFile fileIn;
					wxString lines;
					if (fileIn.Open(data->m_path))
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
							SetTitle(_("Ahoj"));
						}
				}
		}
}
