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

#include "bschordsMain.h"
#include <wx/treectrl.h>
#include <wx/dir.h>
#include <wx/stc/stc.h>


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
    EVT_MENU(idMenuAbout, bschordsFrame::OnAbout)
    EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, bschordsFrame::OnSongContentChange)
END_EVENT_TABLE()

bschordsFrame::bschordsFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* editMenu = new wxMenu(_T(""));
    editMenu->Append(idMenuPreferences, _("&Preferences"), _("Application preferences"));
    mbar->Append(editMenu, _("&Edit"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("This is BSChords application!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

	wxSplitterWindow *splitterMain = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(400, 400), wxSP_BORDER);

	wxSplitterWindow *splitterSong = new wxSplitterWindow(splitterMain, -1, wxPoint(0, 0), wxSize(400, 400), wxSP_BORDER);

	m_songContent = new wxRichTextCtrl(splitterSong, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);

    //wxString *defaultText = new wxString(_("Default text"));
    //m_songContent->AppendText(L"Ahoj");

	m_preview = new bschordsPreview(splitterSong, m_songContent);
	m_preview->SetScrollbars(20, 20, 50, 50);

	//wxTextCtrl *editWindow = new wxTextCtrl(splitter, -1, _T("initial text"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	wxTreeCtrl *tree = new wxTreeCtrl(splitterMain);

	splitterMain->SplitVertically( tree, splitterSong);
	splitterSong->SplitVertically( m_songContent, m_preview);

	//ctor
	wxDir dir(wxGetCwd());

	wxString file;

	bool cont = dir.GetFirst(&file, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

	while (cont)
	{
		wxPuts(file);
		wxLogInfo(file);
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
    Destroy();
}

void bschordsFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("BSChords application, Author: michal.nezerka@gmail.com"));
}

void bschordsFrame::OnSongContentChange(wxCommandEvent& event)
{
	//wxMessageBox(_("This is content change event"), _("BSChords application"));
	m_preview->Refresh();
	m_preview->Update();

}



