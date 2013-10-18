/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Application Frame Declaration
 */

#ifndef BSCHORDSMAIN_H
#define BSCHORDSMAIN_H

#include <wx/textctrl.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stc/stc.h>
#include <wx/aui/aui.h>
#include <wx/dirctrl.h>
#include <wx/stc/stc.h>
#include <wx/log.h>

#include "app.h"
#include "previewwnd.h"
#include "songbookwnd.h"
#include "dcpainter.h"

namespace bschords
{

struct SongFile
{
    wxString m_path;
    bool m_changed;
    wxTextFileType m_type;

    SongFile()
    {
        clear();
    };
    void clear()
    {
        m_changed = false;
        m_path.Empty();
        m_type = wxTextFileType_None;
    };
};

class MainWnd: public wxFrame
{
public:
    void OpenFile(const wxString filePath);
    //wxRichTextCtrl *m_songContent;
    wxStyledTextCtrl *m_songContent;
    MainWnd(wxFrame *frame, const wxString& title);
    ~MainWnd();

private:
    PreviewWnd *m_preview;
    wxTextCtrl *mLogTextCtrl;
    wxLogTextCtrl *mLogWindow;
    //wxStyledTextCtrl *m_songContent;
    wxGenericDirCtrl* m_dirCtrl;
    SongBookWnd *m_songBookWnd;
    wxComboBox *m_zoomCtrl;
    wxToolBar *m_toolBar;
    wxPanel *m_chordsPanel;
    wxAuiToolBar* tb2;
    //wxButton *m_chordButtons[7];
    wxComboBox *m_cmdCtrl;
    wxAuiManager m_auiMgr;
    bool m_isInEditMode;

    SongFile m_file;
    wxString m_songBookPath;

    void SetEditMode(bool newEditMode);

    void SaveFile();
    void CloseFile();

    void OpenSongBook(const wxString filePath);
    void SaveSongBook();
    void CloseSongbook();
    void UpdateTitle();
    void updateEditorStyles();

    void OnFileNewSong(wxCommandEvent& event);
    void OnFileOpenSong(wxCommandEvent& event);
    void OnFileSaveSong(wxCommandEvent& event);
    void OnFileSaveAsSong(wxCommandEvent& event);
    void OnFileCloseSong(wxCommandEvent& event);
    void OnFileExportSong(wxCommandEvent& event);
    void OnFileNewSongBook(wxCommandEvent& event);
    void OnFileOpenSongBook(wxCommandEvent& event);
    void OnFileSaveSongBook(wxCommandEvent& event);
    void OnFileSaveSongBookAs(wxCommandEvent& event);
    void OnFileCloseSongBook(wxCommandEvent& event);
    void OnFilePrint(wxCommandEvent& event);
    void OnFilePrintPreview(wxCommandEvent& event);
    void OnFilePrintSongBook(wxCommandEvent& event);
    void OnFilePrintPreviewSongBook(wxCommandEvent& event);
    void OnFilePageSetup(wxCommandEvent& event);
    void OnFileExportSongbook(wxCommandEvent& event);

    void OnSongInsert(wxCommandEvent& event);
    void OnSongAddToSongbook(wxCommandEvent& event);
    void OnSongTranspose(wxCommandEvent& event);

    void OnSongbookProperties(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnPreferences(wxCommandEvent& event);
    void OnStyleSheet(wxCommandEvent& event);
    void OnViewPane(wxCommandEvent& event);
    void OnViewToolbar(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSongContentChange(wxCommandEvent& event);
    void OnSongEditorChange(wxStyledTextEvent& event);
    void OnSongEditorStyleNeeded(wxStyledTextEvent& event);
    void OnFSBrowserSelChanged(wxTreeEvent& event);
    void OnFSBrowserItemMenu(wxTreeEvent& event);
    void OnFSBrowserItemAddToSongbook(wxCommandEvent& event);
    void OnChordProToken(wxCommandEvent& event);

    wxAuiDockArt* GetDockArt()
    {
        return m_auiMgr.GetArtProvider();
    };
    void DoUpdate()
    {
        m_auiMgr.Update();
    };
    void OnEraseBackground(wxEraseEvent& event)
    {
        event.Skip();
    };
    void OnSize(wxSizeEvent& event)
    {
        event.Skip();
    };
    void OnPaneClose(wxAuiManagerEvent& evt);

    void OnChangeFontsSize(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

// Defines a new printout class to print our document
class BSChordsPrintout: public wxPrintout
{
public:
    BSChordsPrintout(MainWnd* frame, const wxString &contents, const wxString &title = wxT("My printout"))
        : wxPrintout(title), mContents(contents)
    {
        m_frame = frame;
        mPainter = NULL;
        mPages = 0;
    }

    virtual void OnPreparePrinting();
    virtual void OnEndPrinting();
    virtual bool OnPrintPage(int page);
    virtual bool HasPage(int page);
    virtual bool OnBeginDocument(int startPage, int endPage);
    virtual void OnEndDocument();
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    void DrawPageOne();
    void DrawPageTwo();

    // Writes a header on a page. Margin units are in millimetres.
    bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString& text, float mmToLogical);

private:
    MainWnd *m_frame;
    TSetDCPainter *mPainter;
    unsigned int mPages;
    wxString mContents;
};

} // namespace


#endif // BSCHORDSMAIN_H
