/***************************************************************
 * Name:      bschordsMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2012-12-01
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef BSCHORDSMAIN_H
#define BSCHORDSMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>
//#include <wx/aui/aui.h>
#include <wx/dirctrl.h>

#include "bschordsApp.h"
#include "bschordsPreview.h"


class bschordsFrame: public wxFrame
{
    public:
        bschordsFrame(wxFrame *frame, const wxString& title);
        ~bschordsFrame();
    private:
		bschordsPreview *m_preview;
		wxRichTextCtrl *m_songContent;
		//wxStyledTextCtrl *m_songContent;
		wxGenericDirCtrl* m_dirCtrl;

        //wxAuiManager m_auiMgr;

        void PopulateToolbar(wxToolBarBase* toolBar);
        void RecreateToolbar();

        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnPreferences(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSongContentChange(wxCommandEvent& event);
        void OnToolChord(wxCommandEvent& WXUNUSED(event));
        void OnFSBrowserSelChanged(wxTreeEvent& event);

        DECLARE_EVENT_TABLE()
};


#endif // BSCHORDSMAIN_H
