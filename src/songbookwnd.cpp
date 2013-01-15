
#include "bschordsApp.h"
#include "songbook.h"
#include "songbookwnd.h"

using namespace bschords;

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

	m_listBox = new wxListBox(this, wxID_ANY);
	//m_listBox->Append(_("first item"));
	//m_listBox->Append(_("second item"));
	sizer->Add(m_listBox, 1, wxALL | wxEXPAND, 1);
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
	m_listBox->Clear();

	SongBook &sb = wxGetApp().m_songBook;

	for (size_t i = 0; i < sb.m_songs.size(); i++)
	{
		Song *s = sb.m_songs[i];

		m_listBox->Append(s->m_name);
	}

}