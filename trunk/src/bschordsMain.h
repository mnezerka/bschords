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
        wxRichTextCtrl *m_songContent;
        bschordsFrame(wxFrame *frame, const wxString& title);
        ~bschordsFrame();
    private:
		bschordsPreview *m_preview;

		//wxStyledTextCtrl *m_songContent;
		wxGenericDirCtrl* m_dirCtrl;
		wxComboBox *m_zoomCtrl;
		wxSplitterWindow *m_splitterMain;
		wxSplitterWindow *m_splitterSong;
		wxToolBar *m_toolBar;

		wxString m_filePath;
		bool m_fileChanged;

        //wxAuiManager m_auiMgr;

        void PopulateToolbar();
        void OpenFile(const wxString filePath);
		void SaveFile();
        void CloseFile();
        void UpdateTitle();

		void OnFileNewSong(wxCommandEvent& event);
		void OnFileOpenSong(wxCommandEvent& event);
		void OnFileSaveSong(wxCommandEvent& event);
		void OnFileSaveAsSong(wxCommandEvent& event);
		void OnFileCloseSong(wxCommandEvent& event);
		void OnFileExportSong(wxCommandEvent& event);
		void OnFilePrint(wxCommandEvent& event);
		void OnFilePrintPreview(wxCommandEvent& event);
		void OnFilePageSetup(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnPreferences(wxCommandEvent& event);
        void OnStyleSheet(wxCommandEvent& event);
        void OnViewEditor(wxCommandEvent& event);
        void OnViewPreview(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSongContentChange(wxCommandEvent& event);
        void OnToolChord(wxCommandEvent& WXUNUSED(event));
        void OnFSBrowserSelChanged(wxTreeEvent& event);
		void OnZoomChanged(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
};

// Defines a new printout class to print our document
class BSChordsPrintout: public wxPrintout
{
	public:
		BSChordsPrintout(bschordsFrame* frame, const wxString &title = _("My printout"))
        : wxPrintout(title) { m_frame = frame; }

    virtual bool OnPrintPage(int page);
    virtual bool HasPage(int page);
    virtual bool OnBeginDocument(int startPage, int endPage);
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    void DrawPageOne();
    void DrawPageTwo();

    // Writes a header on a page. Margin units are in millimetres.
    bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString& text, float mmToLogical);

private:
    bschordsFrame *m_frame;
};



#endif // BSCHORDSMAIN_H
