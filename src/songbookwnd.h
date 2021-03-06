/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Songbook Window Declaration
 */

#ifndef SONGBOOKWND_H
#define SONGBOOKWND_H

#include <wx/xml/xml.h>
#include <wx/listctrl.h>
#include "songbook.h"

namespace bschords
{

/** \brief Class for List control showing Songbook items */
class SongBookListCtrl: public wxListCtrl
{
public:
    SongBookListCtrl(
        wxWindow *parent,
        const wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style): wxListCtrl(parent, id, pos, size, style),   mAttr(*wxBLUE, *wxLIGHT_GREY, wxNullFont), mActiveItem(-1) { };
    void OnBeginDrag(wxListEvent& event);
    void OnBeginLabelEdit(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnSongProperties(wxCommandEvent& event);

private:
    wxListItemAttr mAttr;
    long mActiveItem;

    DECLARE_NO_COPY_CLASS(SongBookListCtrl)
    DECLARE_EVENT_TABLE()
};

class SongBookWnd : public wxWindow
{
public:
    SongBookWnd(wxWindow *parent, wxWindowID id);
    virtual ~SongBookWnd();
    void OnNewSection(wxCommandEvent &event);
    void addSongFile(wxString filePath);
    void Update();

private:
    //SongBookTreeCtrl *m_treeCtrl;
    SongBookListCtrl *m_listCtrl;
    void copySelectionToSongbook();
    void OnSize(wxSizeEvent& event);
    void OnSongBookItemActivated(wxListEvent& event);
    void OnSongBookItemRightClick(wxListEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void ShowContextMenu(const wxPoint& pos);
    void OnListKeyDown(wxListEvent& event);
    void OnMoveUp(wxCommandEvent &event);
    void OnMoveDown(wxCommandEvent &event);
    void OnSort(wxCommandEvent& event);
    void OnDeleteSelected(wxCommandEvent& event);
    void OnPrintOn(wxCommandEvent& event);
    void OnPrintOff(wxCommandEvent& event);
    void OnSongbookProperties(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

} // namespace

#endif // SONGBOOKWND_H
