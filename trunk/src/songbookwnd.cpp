#include <iostream>

#include <wx/imaglist.h>

#include <wx/filename.h>

#include "app.h"
#include "songbook.h"
#include "songbookwnd.h"

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
	ID_BTN_NEW_SECTION
};

BEGIN_EVENT_TABLE(SongBookWnd, wxWindow)
	EVT_SIZE(SongBookWnd::OnSize)
	EVT_BUTTON(ID_BTN_NEW_SECTION, SongBookWnd::OnNewSection)
END_EVENT_TABLE()

SongBookWnd::SongBookWnd(wxWindow *parent)
	: wxWindow(parent, wxID_ANY)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(sizer);

	// create control bar
	wxPanel *panel = new wxPanel(this);
	new wxStaticText(panel, wxID_ANY, _("This is list of songs"));
	sizer->Add(panel, 0, wxALL | wxEXPAND, 1);

	//m_listBox = new wxListBox(this, wxID_ANY);
	//m_treeCtrl = new SongBookTreeCtrl(this, idSongBookTreeCtrlId, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
	m_listCtrl = new wxListView(this, idSongBookListCtrlId, wxDefaultPosition, wxDefaultSize, wxLC_LIST);
	//wxTreeItemId rootId = m_treeCtrl->AddRoot(_("Empty songbook"), 1, 1);
	sizer->Add(m_listCtrl, 1, wxALL | wxEXPAND, 1);

	// create buttons bar
	wxPanel *panel2 = new wxPanel(this);
	new wxButton(panel2, ID_BTN_NEW_SECTION, _("Add section"));
	sizer->Add(panel2, 0, wxALL | wxEXPAND, 1);

	//m_treeCtrl->UpdateContent();
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

	//Song *song = new Song();
	//song->m_filePath = filePath;
	//wxGetApp().m_songBook.add(song);
	//m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(), filePath, 0, 0, new wxTreeItemData(song));
	//wxListItem *item = new wxListItem();
	//item->SetText(filePath);

	wxGetApp().m_songBook.addSong(filePath);

	//m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), filePath);
	Update();
}

void SongBookWnd::Update()
{
	//m_listCtrl->UpdateContent();
	// update list ctrl according to song book contents
	m_listCtrl->DeleteAllItems();

	for (unsigned int i = 0; i < wxGetApp().m_songBook.getCount(); i++)
	{
		SongBookItem *item = wxGetApp().m_songBook.getItem(i);
		m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), item->getTitle());
	}


}
