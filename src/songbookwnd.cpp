#include <iostream>

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
	EVT_TREE_BEGIN_LABEL_EDIT(idSongBookTreeCtrlId, SongBookTreeCtrl::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(idSongBookTreeCtrlId, SongBookTreeCtrl::OnEndLabelEdit)
    EVT_TREE_DELETE_ITEM(idSongBookTreeCtrlId, SongBookTreeCtrl::OnDeleteItem)
	EVT_TREE_ITEM_ACTIVATED(idSongBookTreeCtrlId, SongBookTreeCtrl::OnItemActivated)
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
    if (!itemDst.IsOk() || !itemSrc.IsOk())
		return;

	// items must be different (you cannot move item to itself)
	if (itemDst == itemSrc)
		return;

	// get nodes data
	SongTreeItemData* dataSrc = (SongTreeItemData*)GetItemData(itemSrc);
	SongTreeItemData* dataDst = (SongTreeItemData*)GetItemData(itemDst);
	if (!dataSrc || !dataDst)
		return;

	// appropriate node of dest item and check if it could act as drag destination
	if (!(dataDst->getXmlNode()->GetName() == wxT("songbook") || dataDst->getXmlNode()->GetName() == wxT("section")))
		return;

	// cannot move songbook (it must stay root)
	if (dataSrc->getXmlNode()->GetName() == wxT("songbook"))
		return;

	if (SongBook::moveNode(dataSrc->getXmlNode(), dataDst->getXmlNode()))
		UpdateContent();
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

void SongBookTreeCtrl::OnBeginLabelEdit(wxTreeEvent& event)
{
	/*
    // for testing, prevent this item's label editing
    wxTreeItemId itemId = event.GetItem();

    if ( IsTestItem(itemId) )
    {
        wxMessageBox(wxT("You can't edit this item."));

        event.Veto();
    }
    else if ( itemId == GetRootItem() )
    {
        // test that it is possible to change the text of the item being edited
        SetItemText(itemId, _T("Editing root item"));
    }
    */
}

void SongBookTreeCtrl::OnEndLabelEdit(wxTreeEvent& event)
{
	std::cout << "end of item label edit" << std::endl;

    // don't allow anything except letters in the labels
    if (event.GetLabel().Length() == 0)
    {
        wxMessageBox(wxT("The new label should be a least one character long."));
        event.Veto();
    }

	wxTreeItemId itemId = event.GetItem();
	if (!itemId.IsOk())
	{
		std::cout << "item is not ok" << std::endl;
		event.Veto();
		return;
	}

	SongTreeItemData* data = (SongTreeItemData*)GetItemData(itemId);
	if (data)
	{
		std::cout << "data are present" << std::endl;
		wxXmlNode *node = data->getXmlNode();
		if (node->GetName() == wxT("section") || node->GetName() == wxT("songbook"))
		{
			std::cout << "setting node property name: " << event.GetLabel() << std::endl;
			SongBook::setNodeProperty(node, wxT("name"), event.GetLabel());
		}
		else
		{
			event.Veto();
		}
	}
}

void SongBookTreeCtrl::OnDeleteItem(wxTreeEvent& event)
{
	std::cout << "item deleted" << std::endl;
	return;
}

void SongBookTreeCtrl::OnItemActivated(wxTreeEvent& event)
{
	std::cout << "item activated" << std::endl;

	// show some info about this item
    wxTreeItemId itemId = event.GetItem();
    SongTreeItemData *data = (SongTreeItemData *)GetItemData(itemId);
    if (data)
    {
    	wxXmlNode *node = data->getXmlNode();
		if (node->GetName() == wxT("song") && node->HasProp(wxT("path")))
		{
			wxFileName fn = node->GetPropVal(wxT("path"), wxEmptyString);
		}

    }

	return;
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

void SongBookTreeCtrl::UpdateContent()
{
	DeleteAllItems();

	wxXmlNode *nodeRoot = wxGetApp().m_songBook.getRootNode();
	if (!nodeRoot)
		return;

	// check that root node is songbook
	if (nodeRoot->GetName() != wxT("songbook"))
		return;

	wxTreeItemId rootId = AddRoot(nodeRoot->GetPropVal(wxT("name"), wxT("Songbook")), 1, 1, new SongTreeItemData(nodeRoot));

	CreateTreeLevelContent(GetRootItem(), nodeRoot);
}

void SongBookTreeCtrl::CreateTreeLevelContent(wxTreeItemId treeParentId, wxXmlNode *nodeParent)
{
	if (!nodeParent)
		return;

	wxXmlNode *childNode = nodeParent->GetChildren();
	while (childNode)
	{
		if (childNode->GetName() == wxT("song"))
		{
			wxFileName fileName = childNode->GetPropVal(wxT("path"), wxEmptyString);
			AppendItem(treeParentId, fileName.GetName(), 0, 0, new SongTreeItemData(childNode));
		}
		else if (childNode->GetName() == wxT("section"))
		{
			wxTreeItemId itemId = AppendItem(treeParentId, childNode->GetPropVal(wxT("name"), wxT("Section")), 2, 2, new SongTreeItemData(childNode));
			CreateTreeLevelContent(itemId, childNode);
		}
		childNode = childNode->GetNext();
	}
}

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
	m_treeCtrl = new SongBookTreeCtrl(this, idSongBookTreeCtrlId, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
	//wxTreeItemId rootId = m_treeCtrl->AddRoot(_("Empty songbook"), 1, 1);
	sizer->Add(m_treeCtrl, 1, wxALL | wxEXPAND, 1);

	// create buttons bar
	wxPanel *panel2 = new wxPanel(this);
	new wxButton(panel2, ID_BTN_NEW_SECTION, _("Add section"));
	sizer->Add(panel2, 0, wxALL | wxEXPAND, 1);

	m_treeCtrl->UpdateContent();
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
}

void SongBookWnd::addSongFile(wxString filePath)
{
	std::cout << "song book wnd - addSongFile" << std::endl;
	//Song *song = new Song();
	//song->m_filePath = filePath;
	//wxGetApp().m_songBook.add(song);
	//m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(), filePath, 0, 0, new wxTreeItemData(song));


}

void SongBookWnd::Update()
{
	m_treeCtrl->UpdateContent();
}
