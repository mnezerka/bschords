/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Songbook Window Implementation
 */

#include <iostream>
#include <wx/imaglist.h>
#include <wx/filename.h>
#include "app.h"
#include "songbook.h"
#include "songbookwnd.h"
#include "mainwnd.h"
#include "songbookdlg.h"
#include "songdlg.h"

//#include "res/song.xpm"
//#include "res/songbook.xpm"
//#include "res/songfolder.xpm"

/*
TODO: SongBook: add buttons for moving items
TODO: SongBook: allow editing of section names
*/

namespace bschords
{

// --------------- SongBookWnd -------------------------------------------------

enum
{
    idSongBookListCtrlId = 1000,
    ID_SONG_LIST,
    ID_BTN_NEW_SECTION,
    ID_BTN_REMOVE,
    ID_BTN_UP,
    ID_BTN_DOWN,
    idActionDeleteSelected,
    idActionPrintOn,
    idActionPrintOff,
    idActionSongbookProperties,
    idActionSongProperties
};

BEGIN_EVENT_TABLE(SongBookWnd, wxWindow)
    EVT_SIZE(SongBookWnd::OnSize)
    EVT_BUTTON(ID_BTN_NEW_SECTION, SongBookWnd::OnNewSection)
    EVT_BUTTON(ID_BTN_UP, SongBookWnd::OnMoveUp)
    EVT_BUTTON(ID_BTN_DOWN, SongBookWnd::OnMoveDown)
    EVT_LIST_ITEM_ACTIVATED(idSongBookListCtrlId, SongBookWnd::OnSongBookItemActivated)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_SONG_LIST, SongBookWnd::OnSongBookItemRightClick)
    EVT_LIST_KEY_DOWN(ID_SONG_LIST, SongBookWnd::OnListKeyDown)
    EVT_MENU(idActionDeleteSelected, SongBookWnd::OnDeleteSelected)
    EVT_MENU(idActionPrintOn, SongBookWnd::OnPrintOn)
    EVT_MENU(idActionPrintOff, SongBookWnd::OnPrintOff)
    EVT_MENU(idActionSongbookProperties, SongBookWnd::OnSongbookProperties)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SongBookListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(ID_SONG_LIST, SongBookListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(ID_SONG_LIST, SongBookListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(ID_SONG_LIST, SongBookListCtrl::OnEndLabelEdit)
    EVT_CONTEXT_MENU(SongBookListCtrl::OnContextMenu)
    EVT_RIGHT_DOWN(SongBookListCtrl::OnRightClick)
    EVT_MENU(idActionSongProperties, SongBookListCtrl::OnSongProperties)
END_EVENT_TABLE()

/* ---------------- SongBookListCtrl-------------------------------- */

void SongBookListCtrl::OnBeginDrag(wxListEvent& event)
{
    //const wxPoint& pt = event.m_pointDrag;

    //int flags;
    //std::cout << "OnBeginDrag at " << pt.x << ", " << pt.y << " hittest: " << HitTest(pt, flags) << std::endl;
}

void SongBookListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
    //std::cout << "start editing" << std::endl;
}

void SongBookListCtrl::OnEndLabelEdit(wxListEvent& event)
{
    //std::wcout << L"end editing " << event.m_item.m_text.c_str()  << std::endl;
    //std::cout << (event.IsEditCancelled() ? "cancelled" : event.m_item.m_text.c_str()) << std::endl;
    wxGetApp().m_songBook.setItemTitle(event.GetIndex(), event.GetItem().GetText());
}

void SongBookListCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint point = event.GetPosition();

    // If from keyboard
    if (point.x == -1 && point.y == -1)
    {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
    else
    {
        point = ScreenToClient(point);
    }

    //ShowContextMenu(point);
    wxMenu menu;

    menu.Append(idActionDeleteSelected, _T("Delete selected items"));
    menu.Append(idActionPrintOn, _T("Enable printing for selected items"));
    menu.Append(idActionPrintOff, _T("Disable printing for selected items"));
    menu.AppendSeparator();
    menu.Append(idActionSongProperties, _T("Item properties..."));
    menu.AppendSeparator();
    menu.Append(idActionSongbookProperties, _T("Songbook properties..."));

    PopupMenu(&menu, point.x, point.y);
}

void SongBookListCtrl::OnRightClick(wxMouseEvent& event)
{
    int flags;
    long subitem;
    mActiveItem = HitTest(event.GetPosition(), flags, &subitem);
    wxLogDebug(wxT("Setting Active Item to %ld"), mActiveItem);
    event.Skip();
}

void SongBookListCtrl::OnSongProperties(wxCommandEvent& event)
{
    if (mActiveItem == -1)
        return;

    SongBookItem* item = wxGetApp().m_songBook.getItem(mActiveItem);

    SongDlg* dlg = new SongDlg(0L, _("Song Properties"), *item);

    if (dlg->ShowModal() == wxID_OK)
    {
        ;
    }
    dlg->Destroy();
}
/* ---------------- SongBookWnd ----------------------------------- */

SongBookWnd::SongBookWnd(wxWindow *parent, wxWindowID id)
    : wxWindow(parent, id)
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(sizer);

    // create control bar
    wxPanel *panel = new wxPanel(this);
    new wxStaticText(panel, wxID_ANY, _("This is list of songs"));
    sizer->Add(panel, 0, wxALL | wxEXPAND, 1);

    m_listCtrl = new SongBookListCtrl(this, idSongBookListCtrlId, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
    m_listCtrl->InsertColumn(0, wxT("Name"));
    m_listCtrl->InsertColumn(1, wxT("P"));
    m_listCtrl->InsertColumn(2, wxT("T"));
    m_listCtrl->SetColumnWidth(0, 200);
    m_listCtrl->SetColumnWidth(1, 20);
    m_listCtrl->SetColumnWidth(2, 20);
    sizer->Add(m_listCtrl, 1, wxALL | wxEXPAND, 1);

    // create buttons bar
    wxPanel *panel2 = new wxPanel(this);
    wxBoxSizer *sizerBtn = new wxBoxSizer(wxHORIZONTAL);
    panel2->SetSizer(sizerBtn);
    sizerBtn->Add(new wxButton(panel2, ID_BTN_NEW_SECTION, _("Add section")), 0, wxALL | wxEXPAND, 1);
    sizerBtn->Add(new wxButton(panel2, ID_BTN_UP, _("Move Up")), 0, wxALL | wxEXPAND, 1);
    sizerBtn->Add(new wxButton(panel2, ID_BTN_DOWN, _("Move Down")), 0, wxALL | wxEXPAND, 1);
    sizer->Add(panel2, 0, wxALL | wxEXPAND, 1);
}

SongBookWnd::~SongBookWnd()
{
    //dtor
}

void SongBookWnd::copySelectionToSongbook()
{
    wxGetApp().m_songBook.selectAll(false);
    long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    while (item != -1)
    {
        wxGetApp().m_songBook.selectItem(item);
        item = m_listCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }
}

void SongBookWnd::OnSize(wxSizeEvent& event)
{
    if (GetAutoLayout())
        Layout();
}

void SongBookWnd::OnNewSection(wxCommandEvent &event)
{
    wxGetApp().m_songBook.addItem(new SongBookSection(wxT("Section")));
    Update();
}

void SongBookWnd::addSongFile(wxString filePath)
{
    //std::cout << "song book wnd - addSongFile" << std::endl;
    wxGetApp().m_songBook.addItem(new SongBookSong(filePath));
    Update();
}

void SongBookWnd::Update()
{
    //int scrollPos = m_listCtrl->GetScrollPos(wxVERTICAL);

    // update list ctrl according to song book contents
    m_listCtrl->DeleteAllItems();

    for (unsigned int i = 0; i < wxGetApp().m_songBook.getCount(); i++)
    {
        SongBookItem *item = wxGetApp().m_songBook.getItem(i);
        long itemIndex = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), item->getTitle());
        if (item->getPrintFlag())
            m_listCtrl->SetItem(itemIndex, 1, wxT("X"));
        if (item->isTransposeable())
        {
            wxString transpositionLabel(wxString::Format(wxT("%d"), item->getTransposeStep()));
            m_listCtrl->SetItem(itemIndex, 2, transpositionLabel);
        }

        //m_listCtrl->SetBackgroundColor();
        if (item->isSelected())
            m_listCtrl->SetItemState(itemIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

        // colorize section items
        if (item->getPath().Length() == 0)
        {
            wxListItem info;
            info.SetId(itemIndex);
            m_listCtrl->GetItem(info);
            wxListItemAttr *attr = info.GetAttributes();
            if ( !attr || !attr->HasTextColour() )
            {
                info.SetTextColour(*wxBLUE);
                m_listCtrl->SetItem(info);
                m_listCtrl->RefreshItem(info.GetId());
            }
        }
    }
    //m_listCtrl->SetScrollPos(wxVERTICAL, scrollPos);
}

void SongBookWnd::OnSongBookItemActivated(wxListEvent& event)
{
    SongBookItem *item = wxGetApp().m_songBook.getItem(event.GetIndex());
    if (item)
    {
        MainWnd *main = static_cast<MainWnd*>(GetParent());
        if (item->getPath().length() > 0)
            main->OpenFile(item->getPath());
    }
}

void SongBookWnd::OnSongBookItemRightClick(wxListEvent& event)
{
    event.Skip();

    //std::cout << "item right click" << event.GetIndex() << event.ShouldPropagate() << std::endl;
    /*(SongBookItem *item = wxGetApp().m_songBook.getItem(event.GetIndex());
    if (item)
    {
    	wxPoint point = event.GetPoint();

    	wxMenu menu;
    	menu.Append(wxID_ABOUT, _T("&Remove"));
    	menu.Append(wxID_ANY, _T("&Edit"));
    	menu.Append(wxID_ANY, _T("&Move"));
    	PopupMenu(&menu, point.x, point.y);
    }*/
}

void SongBookWnd::OnListKeyDown(wxListEvent& event)
{
    //long item;

    switch (event.GetKeyCode())
    {
    case WXK_DELETE:
        copySelectionToSongbook();
        wxGetApp().m_songBook.deleteSelected();
        Update();
        break;
    case WXK_UP:
        //std::cout << "move selected item(s) up" << std::endl;
        break;

    case WXK_DOWN:
        //std::cout << "move selected item(s) down" << std::endl;
        break;

    case WXK_F2:
    {
        long itemCur = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
        //std::cout << "item to edit: " << itemCur << std::endl;
        if (itemCur != -1)
        {
            m_listCtrl->EditLabel(itemCur);
        }
    }
    break;
    case WXK_ADD:
        wxLogDebug(wxT("Add key pressed"));
        break;
    default:
        event.Skip();
    }
}

void SongBookWnd::OnMoveUp(wxCommandEvent &event)
{
    copySelectionToSongbook();
    wxGetApp().m_songBook.moveSelectedUp();
    Update();
}

void SongBookWnd::OnMoveDown(wxCommandEvent &event)
{
    copySelectionToSongbook();
    wxGetApp().m_songBook.moveSelectedDown();
    Update();
}

void SongBookWnd::OnDeleteSelected(wxCommandEvent& event)
{
    wxMessageDialog* dialog = new wxMessageDialog(this, wxT("Delete selected items?"), wxT("Songbook"), wxYES_NO);

    int answer = dialog->ShowModal();
    dialog->Destroy();

    if (answer == wxID_YES)
    {
        wxLogDebug(wxT("Delete selected"));
        copySelectionToSongbook();
        wxGetApp().m_songBook.deleteSelected();
        Update();
    }
}

void SongBookWnd::OnPrintOn(wxCommandEvent& event)
{
    wxLogDebug(wxT("Print on for selected"));
    copySelectionToSongbook();
    wxGetApp().m_songBook.setPrintFlagForSelected(true);
    Update();
}

void SongBookWnd::OnPrintOff(wxCommandEvent& event)
{
    wxLogDebug(wxT("Print off for selected"));
    copySelectionToSongbook();
    wxGetApp().m_songBook.setPrintFlagForSelected(false);
    Update();
}

void SongBookWnd::OnSongbookProperties(wxCommandEvent& event)
{
    SongbookDlg* dlg = new SongbookDlg(0L, _("Songbook properties"), wxGetApp().m_songBook);

    if (dlg->ShowModal() == wxID_OK)
    {
        ;
    }
    dlg->Destroy();
}

} // namespace
