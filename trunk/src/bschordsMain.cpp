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


#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/dir.h>

#include "bschordsMain.h"
#include "bschordsPreferences.h"
#include "bschordsicon.xpm"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

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

BEGIN_EVENT_TABLE(bschordsFrame, wxFrame)
    EVT_CLOSE(bschordsFrame::OnClose)
    EVT_MENU(idMenuQuit, bschordsFrame::OnQuit)
    EVT_MENU(idMenuPreferences, bschordsFrame::OnPreferences)
        EVT_MENU(idMenuAbout, bschordsFrame::OnAbout)
    EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, bschordsFrame::OnSongContentChange)
END_EVENT_TABLE()

bschordsFrame::bschordsFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
    //m_auiMgr.SetManagedWindow(this);

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

   //wxString *defaultText = new wxString(_("Default text"));
    //m_songContent->AppendText(L"Ahoj");

	m_preview = new bschordsPreview(splitterSong, m_songContent);
	//m_preview->SetScrollbars(20, 20, 50, 50);
	//m_preview->SetBackgroundColour(wxColour(255, 255, 255));
	//m_preview->SetBackgroundStyle(wxBG_STYLE_COLOUR);

	//wxTextCtrl *editWindow = new wxTextCtrl(splitter, -1, _T("initial text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	//wxTreeCtrl *tree = new wxTreeCtrl(splitterMain);

	wxListView *fsList = new wxListView(splitterMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST);

	splitterMain->SplitVertically(fsList, splitterSong, 100);
	splitterSong->SplitVertically(m_songContent, m_preview, 0);


	//wxImageList *m_pImageList = new wxImageList(16,16);
	//wxIcon icon;
	//icon.LoadFile(wxT("res/directory.ico"), wxBITMAP_TYPE_PNG);
	//m_pImageList->Add(icon);
	//fsList->SetImageList(m_pImageList, wxIMAGE_LIST_SMALL);

     // Add first column
	wxListItem col0;
    col0.SetId(0);
    col0.SetText(_("File"));
    //col0.SetWidth(50);
    fsList->InsertColumn(0, col0);

	//fsList->SetItem(item);

	//ctor
	wxDir dir(wxGetCwd());

	wxString file;

	bool cont = dir.GetFirst(&file, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

	while (cont)
	{
		wxListItem item;
		//item.SetId(34);
		item.SetText(file);
		fsList->InsertItem(item);

		//wxPuts(file);
		//wxLogInfo(file);
		cont = dir.GetNext(&file);

	}

    //wxStyledTextCtrl *x = new wxStyledTextCtrl(this);
}


bschordsFrame::~bschordsFrame()
{

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

    Destroy();
}

void bschordsFrame::OnPreferences(wxCommandEvent &event)
{
    bschordsPreferences* dlg = new bschordsPreferences(0L, _("wxWidgets Application Template"));

    dlg->Show();
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



