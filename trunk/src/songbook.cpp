#include <iostream>

#include <wx/filename.h>
#include "songbook.h"

using namespace bschords;

SongBook::SongBook()
{
	m_name = wxT("");
}

SongBook::~SongBook()
{
	empty();
}

void SongBook::empty()
{
	while (!m_songs.empty() > 0)
	{
		delete(m_songs.back());
		m_songs.pop_back();
	}
}

void SongBook::add(wxString path)
{
	std::cout << "adding file to songbook" << std::endl;

	// check if path is valid text file
	wxFileName filePath(path);

	if (filePath.IsDir())
	{
		wxMessageBox(_("Cannot add directory"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	if (!filePath.FileExists())
	{
		wxMessageBox(_("File doesn't exist"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	Song *song = new Song();
	song->m_name = filePath.GetName();
	song->m_filePath = path;

	m_songs.push_back(song);

	std::wcout << L"adding file " << filePath.GetFullPath().wc_str() << std::endl;
}
