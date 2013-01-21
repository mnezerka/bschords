#include <iostream>

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
	idSongBookTreeCtrlId = 1000
};

// --------------- SongBookWnd -------------------------------------------------


BEGIN_EVENT_TABLE(SongBookTreeCtrl, wxTreeCtrl)
	EVT_TREE_BEGIN_DRAG(idSongBookTreeCtrlId, SongBookTreeCtrl::OnBeginDrag)
    EVT_TREE_END_DRAG(idSongBookTreeCtrlId, SongBookTreeCtrl::OnEndDrag)
    // EVT_TREE_ITEM_MENU is the preferred event for creating context menus
    // on a tree control, because it includes the point of the click or item,
    // meaning that no additional placement calculations are required.
    EVT_TREE_ITEM_MENU(idSongBookTreeCtrlId, SongBookTreeCtrl::OnItemMenu)

END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SongBookTreeCtrl, wxTreeCtrl)

SongBookTreeCtrl::SongBookTreeCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxTreeCtrl(parent, id, pos, size, style)
{
    //m_reverseSort = false;

    CreateImageList();

    // Add some items to the tree
    //AddTestItemsToTree(5, 2);
}

void SongBookTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
	std::cout << "OnBeginDrag" << std::endl;

    // need to explicitly allow drag
    if (event.GetItem() != GetRootItem())
    {
        m_draggedItem = event.GetItem();

        //wxPoint clientpt = event.GetPoint();
        //wxPoint screenpt = ClientToScreen(clientpt);

        //wxLogMessage(wxT("OnBeginDrag: started dragging %s at screen coords (%i,%i)"),
         //            GetItemText(m_draggedItem).c_str(),
         //            screenpt.x, screenpt.y);

        event.Allow();
    }
    else
    {
        //wxLogMessage(wxT("OnBeginDrag: this item can't be dragged."));
    }
}

void SongBookTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
	std::cout << "OnEndDrag" << std::endl;

    wxTreeItemId itemSrc = m_draggedItem;
    wxTreeItemId itemDst = event.GetItem();

    m_draggedItem = (wxTreeItemId)0l;

    // where to copy the item?
    if ( itemDst.IsOk() && !ItemHasChildren(itemDst) )
    {
        // copy to the parent then
        itemDst = GetItemParent(itemDst);
    }

    if ( !itemDst.IsOk() )
    {
        wxLogMessage(wxT("OnEndDrag: can't drop here."));

        return;
    }

    wxString text = GetItemText(itemSrc);

    /*wxLogMessage(wxT("OnEndDrag: '%s' copied to '%s'."),
                 text.c_str(), GetItemText(itemDst).c_str());
	*/
    // just do append here - we could also insert it just before/after the item
    // on which it was dropped, but this requires slightly more work... we also
    // completely ignore the client data and icon of the old item but could
    // copy them as well.
    //
    // Finally, we only copy one item here but we might copy the entire tree if
    // we were dragging a folder.
    //int image = wxGetApp().ShowImages() ? TreeCtrlIcon_File : -1;
    AppendItem(itemDst, text, 0);
}

void SongBookTreeCtrl::OnItemMenu(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    //MyTreeItemData *item = itemId.IsOk() ? (MyTreeItemData *)GetItemData(itemId)
    //                                     : NULL;
    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);

    //wxLogMessage(wxT("OnItemMenu for item \"%s\" at screen coords (%i, %i)"),
    //             item ? item->GetDesc() : _T(""), screenpt.x, screenpt.y);

    ShowMenu(itemId, clientpt);
    event.Skip();
}

void SongBookTreeCtrl::CreateImageList(int size)
{
    if (size == -1)
    {
        SetImageList(NULL);
        return;
    }

    if (size == 0)
        size = m_imageSize;
    else
        m_imageSize = size;

    // Make an image list containing small icons
    wxImageList *images = new wxImageList(size, size, true);

    // should correspond to TreeCtrlIcon_xxx enum
    wxBusyCursor wait;
    wxIcon icons[3];
    icons[0] = wxIcon(song_xpm);
    icons[1] = wxIcon(songbook_xpm);
    icons[2] = wxIcon(songfolder_xpm);

    int sizeOrig = icons[0].GetWidth();
    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        if (size == sizeOrig)
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    AssignImageList(images);
}


void SongBookTreeCtrl::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    wxString title;
    if ( id.IsOk() )
    {
        title << wxT("Menu for ") << GetItemText(id);
    }
    else
    {
        title = wxT("Menu for no particular item");
    }

#if wxUSE_MENUS
    wxMenu menu(title);
    menu.Append(wxID_ANY, wxT("&About..."));
    menu.AppendSeparator();
    menu.Append(wxID_ANY, wxT("&Highlight item"));
    menu.Append(wxID_ANY, wxT("&Dump"));

    PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}


// --------------- SongBookWnd -------------------------------------------------


enum
{
	ID_SONG_LIST = 1000
};

BEGIN_EVENT_TABLE(SongBookWnd, wxWindow)
	EVT_SIZE(SongBookWnd::OnSize)
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
	m_treeCtrl = new SongBookTreeCtrl(this, idSongBookTreeCtrlId, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxSUNKEN_BORDER | wxTR_HIDE_ROOT);
	wxTreeItemId rootId = m_treeCtrl->AddRoot(_("Root item"));
	m_treeCtrl->AppendItem(rootId, _("This is item 1"), 0);
	m_treeCtrl->AppendItem(rootId, _("This is item 2"), 0);
	m_treeCtrl->AppendItem(rootId, _("This is item 3"), 0);
	sizer->Add(m_treeCtrl, 1, wxALL | wxEXPAND, 1);
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

void SongBookWnd::UpdateContent()
{
	//m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), _("This is new item"));
	m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(), _("This is item"));
	/*
	m_listBox->Clear();



	SongBook &sb = wxGetApp().m_songBook;

	for (size_t i = 0; i < sb.m_songs.size(); i++)
	{
		Song *s = sb.m_songs[i];

		m_listBox->Append(s->m_name);
	}
	*/
}
