#include <iostream>

#include <vector>
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
	wxXmlNode *rootNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("songbook"));
	m_songs.SetRoot(rootNode);
}

void SongBook::addNode(wxXmlNode *newNode, wxXmlNode *parent)
{
	// use root node of tree if no specific parent provided
	wxXmlNode *realParent = parent ? parent : m_songs.GetRoot();

	// look for last child
	if (wxXmlNode *child = realParent->GetChildren())
	{
		std::cout << "adding new node to song book (as child)" << std::endl;
		while (child->GetNext())
			child = child->GetNext();
		child->SetNext(newNode);
	}
	else
	{
		std::cout << "adding new node to song book (as root)" << std::endl;
		realParent->AddChild(newNode);
	}
}

void SongBook::addSong(wxString path, wxXmlNode *parent)
{
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

	wxXmlNode *songNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("song"));
	wxXmlProperty *prop = new wxXmlProperty(wxT("path"), filePath.GetFullPath());
	songNode->AddProperty(prop);
	addNode(songNode, parent);
}

// add new section item
void SongBook::addSection(wxXmlNode *parent)
{
	wxXmlNode *sectionNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("section"));
	wxXmlProperty *prop = new wxXmlProperty(wxT("name"), wxT("Section"));
	sectionNode->AddProperty(prop);
	addNode(sectionNode, parent);
}

void SongBook::saveToFile(wxString path, wxString rootPath)
{
	// TODO: convert path separators to normalized form
	if (!m_songs.Save(path))
		wxMessageBox(_("Error saving songbook"));
}

void SongBook::loadFromFile(wxString path)
{
	if (!m_songs.Load(path))
		wxMessageBox(_("Error loading songbook"));
}

void SongBook::setNodeProperty(wxXmlNode *node, wxString name, wxString value)
{
	if (!node)
		return;

	if (node->HasProp(name))
	{
		wxXmlProperty *p = node->GetProperties();
		while (p)
		{
			if (p->GetName() == name)
			{
				p->SetValue(value);
				break;
			}
			p = p->GetNext();
		}
	}
	else
		node->AddProperty(name, value);
}

bool SongBook::moveNode(wxXmlNode *node, wxXmlNode *newParent)
{
	// check if nodes are valid pointers
	if (!node || !newParent)
		return false;

	// look for newParent in children of node
	std::vector<wxXmlNode *> toVisit;
	toVisit.push_back(node);
	bool foundInSubtree = false;
	while (!toVisit.empty() && !foundInSubtree)
	{
		wxXmlNode *curNode = toVisit.back();
		toVisit.pop_back();

		wxXmlNode *c =curNode->GetChildren();
		while (c)
		{
			if (c == newParent)
			{
				foundInSubtree = true;
				break;
			}
			if (c->GetChildren())
				toVisit.push_back(c);
			c = c->GetNext();
		}
	}

	// if newParent isn't in children of node, we can move node to new parent
	if (!foundInSubtree)
	{
		std::cout << "moving item " << std::endl;

		// remove item from current position in xml tree
		if (node->GetParent())
			node->GetParent()->RemoveChild(node);

		// insert item into new position
		newParent->AddChild(node);
	}

	return true;
}

