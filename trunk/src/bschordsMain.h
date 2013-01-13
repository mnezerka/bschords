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

#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>
#include <wx/aui/aui.h>
#include <wx/dirctrl.h>

#include "bschordsApp.h"
#include "bschordsPreview.h"
#include "songbookwnd.h"

struct SongFile
{
	wxString m_path;
	bool m_changed;
	wxTextFileType m_type;

	SongFile() { clear(); };
	void clear() { m_changed = false; m_path.Empty(); m_type = wxTextFileType_None; };
};

class bschordsFrame: public wxFrame
{
    public:
        wxRichTextCtrl *m_songContent;
        bschordsFrame(wxFrame *frame, const wxString& title);
		~bschordsFrame();

    private:
		BSChordsPreview *m_preview;
		//wxStyledTextCtrl *m_songContent;
		wxGenericDirCtrl* m_dirCtrl;
		SongBookWnd *m_songBook;
		wxComboBox *m_zoomCtrl;
		wxToolBar *m_toolBar;
		wxPanel *m_chordsPanel;
		wxAuiToolBar* tb2;
		//wxButton *m_chordButtons[7];
		wxComboBox *m_chordCtrl;
		wxComboBox *m_cmdCtrl;
		wxAuiManager m_auiMgr;

		SongFile m_file;

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

		void OnFileNewSongBook(wxCommandEvent& event);
		void OnFileOpenSongBook(wxCommandEvent& event);
		void OnFileSaveSongBook(wxCommandEvent& event);
		void OnFileSaveAsSongBook(wxCommandEvent& event);
		void OnFileCloseSongBook(wxCommandEvent& event);

		void OnFilePrint(wxCommandEvent& event);
		void OnFilePrintPreview(wxCommandEvent& event);
		void OnFilePageSetup(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnPreferences(wxCommandEvent& event);
        void OnStyleSheet(wxCommandEvent& event);
        void OnViewPane(wxCommandEvent& event);
        void OnViewToolbar(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnSongContentChange(wxCommandEvent& event);
        void OnToolChord(wxCommandEvent& WXUNUSED(event));
        void OnFSBrowserSelChanged(wxTreeEvent& event);
		void OnChordProToken(wxCommandEvent& event);

		wxAuiDockArt* GetDockArt() { return m_auiMgr.GetArtProvider(); };
		void DoUpdate() { m_auiMgr.Update(); };
		void OnEraseBackground(wxEraseEvent& event) { event.Skip(); };
		void OnSize(wxSizeEvent& event) { event.Skip(); };
		void OnPaneClose(wxAuiManagerEvent& evt);

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
