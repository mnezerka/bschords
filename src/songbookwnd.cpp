#include <iostream>

#include <wx/imaglist.h>

#include <wx/filename.h>

#include "app.h"
#include "songbook.h"
#include "songbookwnd.h"
#include "mainwnd.h"

#include "res/song.xpm"
#include "res/songbook.xpm"
#include "res/songfolder.xpm"

/*
TODO: SongBook: add buttons for moving items
TODO: SongBook: allow editing of section names
*/

using namespace bschords;

// menu and control ids
enum
{
	idSongBookListCtrlId = 1000
};

// --------------- SongBookWnd -------------------------------------------------

/*BEGIN_EVENT_TABLE(SongBookTreeCtrl, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(idSongBookTreeCtrlId, SongBookTreeCtrl::OnBeginDrag)
    EVT_TREE_END_DRAG(idSongBookTreeCtrlId, SongBookTreeCtrl::OnEndDrag)
    // EVT_TREE_ITEM_MENU is the preferred event for creating context menus
    // on a tree control, because it includes the point of the click or item,
    // meaning that no additional placement calculations are required.
    EVT_TREE_ITEM_MENU(idSongBookTreeCtrlId, SongBookTreeCtrl::OnItemMenu)
	EVT_TREE_BEGIN_LABEL_EDIT(idSongBookTreeCtrlId, SongBookTreeCtrl::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(idSongBookTreeCtrlId, SongBookTreeCtrl::OnEndLabelEdit)
    EVT_TREE_DELETE_ITEM(idSongBookTreeCtrlId, SongBookTreeCtrl::OnDeleteItem)
	EVT_TREE_ITEM_ACTIVATED(idSongBookTreeCtrlId, SongBookTreeCtrl::OnItemActivated)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SongBookTreeCtrl, wxTreeCtrl)*/

// --------------- SongBookWnd -------------------------------------------------

enum
{
	ID_SONG_LIST = 1000,
	ID_BTN_NEW_SECTION,
	ID_BTN_REMOVE,
};

BEGIN_EVENT_TABLE(SongBookWnd, wxWindow)
	EVT_SIZE(SongBookWnd::OnSize)
	EVT_BUTTON(ID_BTN_NEW_SECTION, SongBookWnd::OnNewSection)
	EVT_LIST_ITEM_ACTIVATED(ID_SONG_LIST, SongBookWnd::OnSongBookItemActivated)
	EVT_LIST_ITEM_RIGHT_CLICK(ID_SONG_LIST, SongBookWnd::OnSongBookItemRightClick)
	EVT_LIST_KEY_DOWN(ID_SONG_LIST, SongBookWnd::OnListKeyDown)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(SongBookListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(ID_SONG_LIST, SongBookListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(ID_SONG_LIST, SongBookListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(ID_SONG_LIST, SongBookListCtrl::OnEndLabelEdit)
    //EVT_CONTEXT_MENU(SongBookListCtrl::OnContextMenu)
END_EVENT_TABLE()

/* ---------------- SongBookListCtrl-------------------------------- */

void SongBookListCtrl::OnBeginDrag(wxListEvent& event)
{
    const wxPoint& pt = event.m_pointDrag;

    int flags;
    std::cout << "OnBeginDrag at " << pt.x << ", " << pt.y << " hittest: " << HitTest(pt, flags) << std::endl;
}

void SongBookListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
	std::cout << "start editing" << std::endl;
	// TODO: update appropriate item in song book
}

void SongBookListCtrl::OnEndLabelEdit(wxListEvent& event)
{
	std::cout << "end editing" << std::endl;
	//std::cout << (event.IsEditCancelled() ? "cancelled" : event.m_item.m_text.c_str()) << std::endl;
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

	m_listCtrl = new SongBookListCtrl(this, idSongBookListCtrlId, wxDefaultPosition, wxDefaultSize, wxLC_LIST | wxSUNKEN_BORDER | wxLC_EDIT_LABELS);
	sizer->Add(m_listCtrl, 1, wxALL | wxEXPAND, 1);

	// create buttons bar
	wxPanel *panel2 = new wxPanel(this);
	new wxButton(panel2, ID_BTN_NEW_SECTION, _("Add section"));
	sizer->Add(panel2, 0, wxALL | wxEXPAND, 1);
}

SongBookWnd::~SongBookWnd()
{
	//dtor
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
	std::cout << "song book wnd - addSongFile" << std::endl;
	wxGetApp().m_songBook.addItem(new SongBookSong(filePath));
	Update();
}

void SongBookWnd::Update()
{
	// update list ctrl according to song book contents
	m_listCtrl->DeleteAllItems();

	for (unsigned int i = 0; i < wxGetApp().m_songBook.getCount(); i++)
	{
		SongBookItem *item = wxGetApp().m_songBook.getItem(i);
		m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), item->getTitle());
	}
}

void SongBookWnd::OnSongBookItemActivated(wxListEvent& event)
{
    std::cout << "item activated" << event.GetIndex() << event.ShouldPropagate() << std::endl;
    SongBookItem *item = wxGetApp().m_songBook.getItem(event.GetIndex());
    if (item)
    {
		MainWnd *main = static_cast<MainWnd*>(GetParent());
		main->OpenFile(item->getPath());
    }
}

void SongBookWnd::OnSongBookItemRightClick(wxListEvent& event)
{
    std::cout << "item right click" << event.GetIndex() << event.ShouldPropagate() << std::endl;
    SongBookItem *item = wxGetApp().m_songBook.getItem(event.GetIndex());
    if (item)
    {
    	wxPoint point = event.GetPoint();

		wxMenu menu;
		menu.Append(wxID_ABOUT, _T("&Remove"));
		menu.Append(wxID_ANY, _T("&Edit"));
		menu.Append(wxID_ANY, _T("&Move"));
		PopupMenu(&menu, point.x, point.y);
    }
}

void SongBookWnd::OnListKeyDown(wxListEvent& event)
{
	long item;

	switch (event.GetKeyCode())
	{
		case WXK_DELETE:
			wxGetApp().m_songBook.selectAll(false);
			item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            while (item != -1)
            {
            	std::cout << "delete item " << item << std::endl;
            	wxGetApp().m_songBook.selectItem(item);
                item = m_listCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            }
            wxGetApp().m_songBook.deleteSelected();
            Update();
            break;
		case WXK_UP:
			std::cout << "move selected item(s) up" << std::endl;
			break;

		case WXK_DOWN:
			std::cout << "move selected item(s) down" << std::endl;
			break;

		case WXK_F2:
			{
				long itemCur = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
				std::cout << "item to edit: " << itemCur << std::endl;
				if (itemCur != -1)
				{
					m_listCtrl->EditLabel(itemCur);
				}
			}
			break;

		default:
			event.Skip();
	}
}



