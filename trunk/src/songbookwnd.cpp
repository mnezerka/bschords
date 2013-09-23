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

	//EVT_CONTEXT_MENU(SongBookWnd::OnContextMenu)

SongBookWnd::SongBookWnd(wxWindow *parent, wxWindowID id)
	: wxWindow(parent, id)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);

	// create control bar
	wxPanel *panel = new wxPanel(this);
	new wxStaticText(panel, wxID_ANY, _("This is list of songs"));
	sizer->Add(panel, 0, wxALL | wxEXPAND, 1);

	m_listCtrl = new wxListView(this, idSongBookListCtrlId, wxDefaultPosition, wxDefaultSize, wxLC_LIST);
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
	/*
	wxTreeItemId selItem = m_treeCtrl->GetSelection();

	if (selItem.IsOk())
	{
		SongTreeItemData* data = (SongTreeItemData*) m_treeCtrl->GetItemData(selItem);
		if (data)
		{
			wxGetApp().m_songBook.addSection(data->getXmlNode());
		}
	}
	else
	{
		wxGetApp().m_songBook.addSection();
	}

	m_treeCtrl->UpdateContent();
	*/
}

void SongBookWnd::addSongFile(wxString filePath)
{
	std::cout << "song book wnd - addSongFile" << std::endl;
	wxGetApp().m_songBook.addSong(filePath);

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
			item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            while (item != -1)
            {
            	std::cout << "delete item " << item << std::endl;
				//m_listCtrl->DeleteItem(item);
                //wxLogMessage(_T("Item %ld deleted"), item);

                // -1 because the indices were shifted by DeleteItem()
                //item = m_listCtrl->GetNextItem(item - 1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                item = m_listCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            }
            break;
		default:
			event.Skip();
	}
}
