#include <iostream>

#include <wx/filename.h>
#include <wx/xml/xml.h>

#include "songbook.h"

using namespace bschords;

SongBook::SongBook()
{
	empty();
}

SongBook::~SongBook()
{
	empty();
}

void SongBook::empty()
{
	m_songs.SetRoot(new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("songbook")));

	//wxXmlNode *x = m_songs.GetRoot()->GetChildren();

	/*
	wxXmlNode *rn = m_songs->GetRoot();
	if (rn)
	{
		rn.DetachRoot()
	}
	if (m_songs->GetRoot().)
	while (!m_songs.empty() > 0)
	{
		delete(m_songs.back());
		m_songs.pop_back();
	}
	*/
}

void SongBook::add(Song *song)
{
	// TODO: check path prefix (must be rootpath)
	/*
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
	*/

	std::cout << "songbook - new item added" << std::endl;
	//m_songs.push_back(song);

	//std::wcout << L"adding file " << filePath.GetFullPath().wc_str() << std::endl;
}

void SongBook::saveToFile(wxString path, wxString rootPath)
{
	// TODO: strip root path (only relative path is stored)
	// TODO: convert path separators to normalized form

	/*
	wxXmlDocument doc;

	wxXmlNode *rootNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("songbook"));

	for (size_t i = 0; i < m_songs.size(); i++)
	{
		std::cout << "adding item " << i << std::endl;
		Song *s = m_songs[i];

		wxXmlNode *songNode = new wxXmlNode(rootNode, wxXML_ELEMENT_NODE, wxT("song"));
		wxXmlProperty *songPathProp = new wxXmlProperty(wxT("path"), s->m_filePath);
		songNode->AddProperty(songPathProp);
		rootNode->AddChild(songNode);
	}

	doc.SetRoot(rootNode);
	*/

	// loop through all nodes and modify path

	if (!m_songs.Save(path))
		wxMessageBox(_("Error saving songbook"));
}

void SongBook::loadFromFile(wxString path)
{
	if (!m_songs.Load(path))
	{
		wxMessageBox(_("Error loading songbook"));
		return;
	}

	/*

	// clear current content
	empty();

	wxXmlNode *rootNode = doc.GetRoot();

	// if root node is songbook
	if (rootNode->GetName() == wxT("songbook"))
	{
		wxXmlNode *ch = rootNode->GetChildren();
		while (ch)
		{
			if (ch->GetName() == wxt("song"))
			{
				//ch->GetPropVal(wxT("path"))
			}
			ch = ch->GetNext();
		}
	}
	*/
}
