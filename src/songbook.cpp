
#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include <iostream>

#include <vector>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>

#include "songbook.h"

using namespace bschords;


/* ------------------- SongBookItem --------------------------------------- */

/* ------------------- SongBookSection ------------------------------------ */

void SongBookSection::readFromXmlNode(wxXmlNode *node, wxString basePath)
{
	if (node->GetName() != wxT("section"))
		return;

	mTitle = node->GetPropVal(wxT("name"), wxT("section"));
}

wxXmlNode* SongBookSection::createXmlNode(wxString basePath)
{
	wxXmlNode *sectionNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("section"));
	wxXmlProperty *prop = new wxXmlProperty(wxT("name"), mTitle);
	sectionNode->AddProperty(prop);
	return sectionNode;
}

/* ------------------- SongBookSong --------------------------------------- */
SongBookSong::SongBookSong(wxString path)
{
	wxFileName filePath(path);
	mPath = filePath.GetFullPath();
}

wxString SongBookSong::getTitle()
{
	wxFileName filePath(mPath);
	return filePath.GetName();
}

wxString SongBookSong::getPath()
{
	return mPath;
}

wxString SongBookSong::getContents()
{
	wxString contents;
	bool success = false;
    wxFile file(mPath, wxFile::read);

    std::wcout << L"Reading file contents from: " << mPath.mb_str(wxConvUTF8) << std::endl;

    if (file.IsOpened())
    {
        // get the file size (assume it is not huge file...)
        ssize_t len = (ssize_t)file.Length();
        if (len > 0)
        {
            wxMemoryBuffer buffer(len+1);
            success = (file.Read(buffer.GetData(), len) == len);
            if (success) {
                ((char*)buffer.GetData())[len] = 0;
                contents = wxString(buffer, wxConvUTF8, len);
            }
        }
        else
        {
            if (len < 0)
				std::cout << "Invalid offset while reading file " << mPath.mb_str(wxConvUTF8) << std::endl;
        }
    }
    else
    {
    	std::cout << "Cannot open file " << mPath.mb_str(wxConvUTF8) << std::endl;
    }

    std::wcout << L"File contents: " << contents.mb_str(wxConvUTF8) << std::endl;

	return contents;
}

void SongBookSong::readFromXmlNode(wxXmlNode *node, wxString basePath)
{
	if (node->GetName() != wxT("song"))
		return;

	wxString songPath = node->GetPropVal(wxT("path"), wxT(""));
	wxFileName fileName(songPath, wxPATH_UNIX);
	fileName.MakeAbsolute(basePath);
	mPath = fileName.GetFullPath(wxPATH_NATIVE);
}

wxXmlNode* SongBookSong::createXmlNode(wxString basePath)
{
	wxFileName filePath(mPath);
	std::wcout << L"File to be added: " << filePath.GetFullPath().mb_str(wxConvUTF8) << std::endl;
	filePath.MakeRelativeTo(basePath);
	std::wcout << L"File to be added after relative step: " << filePath.GetFullPath().mb_str(wxConvUTF8) << std::endl;

	wxXmlNode *songNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("song"));
	wxXmlProperty *prop = new wxXmlProperty(wxT("path"), filePath.GetFullPath(wxPATH_UNIX));
	songNode->AddProperty(prop);

	return songNode;
}

/* ------------------- SongBook -------------------------------------------- */

SongBook::SongBook()
{
	empty();
}

SongBook::~SongBook()
{
	empty();
}

void SongBook::setBasePath(wxString path)
{
	// check if path is valid text file
	wxFileName filePath(path, wxEmptyString);

	if (!filePath.IsDir())
	{
		wxMessageBox(_("Songbook Base Path must be valid directory - " + path), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_ERROR);
		return;
	}

	m_basePath = filePath.GetPath();
}

void SongBook::empty()
{
	std::list<SongBookItem *>::iterator it;
	while (!m_items.empty())
	{
		it = m_items.begin();
		delete(*it);
		m_items.erase(it);
	}
}

SongBookItem *SongBook::getItem(unsigned int index)
{
	SongBookItem *result = NULL;
	unsigned int i = 0;
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		if (i == index)
		{
			result = *it;
			break;
		}
		else
			i++;
	}

	return result;
}

void SongBook::addItem(SongBookItem *item)
{
	if (!item)
		return;

	m_items.push_back(item);

	/*
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

	SongBookSong *item = new SongBookSong(filePath.GetFullPath());
	m_items.push_back(item);
	*/
}

void SongBook::deleteSelected()
{
	std::list<SongBookItem *>::iterator it = m_items.begin();

	while (it != m_items.end())
	{
		if ((*it)->isSelected())
		{
			delete(*it);
			m_items.erase(it);
			it = m_items.begin();
		}
		else
			it++;
	}
}

void SongBook::saveToXmlFile(wxString path, wxString rootPath)
{
	wxXmlDocument doc;
	wxXmlNode *rootNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("songbook"));

	std::cout << "Saving to XML file (begin)" << std::endl;
	std::cout << "Number of items to be saved: " << m_items.size() << std::endl;
	std::wcout << L"Base path: " << m_basePath.mb_str(wxConvUTF8) << std::endl;

	wxXmlNode *lastChild = NULL;
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		wxXmlNode *itemNode = (*it)->createXmlNode(m_basePath);

		std::cout << "Adding node to xml file" << std::endl;
		if (lastChild != NULL)
			lastChild->SetNext(itemNode);
		else
			rootNode->AddChild(itemNode);
		lastChild = itemNode;
	}

	doc.SetRoot(rootNode);

	if (!doc.Save(path))
		wxMessageBox(_("Error saving songbook"));

	std::cout << "Saving to XML file (end)" << std::endl;
}

void SongBook::loadFromXmlFile(wxString path)
{
	wxXmlDocument doc;
	if (!doc.Load(path))
		wxMessageBox(_("Error loading songbook"));

	// clear contents of current song book
	empty();

	wxXmlNode *rootNode = doc.GetRoot();

	if (rootNode->GetName() != wxT("songbook"))
		return;

	wxXmlNode *c = rootNode->GetChildren();
	while (c)
	{
		if (c->GetName() == wxT("song"))
		{
			SongBookSong *s = new SongBookSong(c, m_basePath);
			m_items.push_back(s);
		}
		else if (c->GetName() == wxT("section"))
		{
			SongBookSection *s = new SongBookSection(c, m_basePath);
			m_items.push_back(s);
		}
		c = c->GetNext();
	}
}

unsigned int SongBook::getCount()
{
	return m_items.size();
}

wxString SongBook::getContents()
{
	wxString result;
	unsigned int i = 0;
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		if (i > 0)
			result.Append(wxT("\n\n"));
		result.Append((*it)->getContents());
		i++;
	}

	return result;
}

void SongBook::setItemTitle(unsigned int index, wxString title)
{
	SongBookItem *item = getItem(index);
	if (item)
		item->setTitle(title);
}

void SongBook::selectAll(bool select)
{
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
		(*it)->select(select);
}

void SongBook::selectItem(unsigned int index, bool select)
{
	SongBookItem *item = getItem(index);
	if (item)
		item->select(select);
}

void SongBook::moveSelectedUp()
{
	std::list<SongBookItem *>::iterator it = m_items.begin();

	while (it != m_items.end())
	{
		if ((*it)->isSelected())
		{
			// get previous item
			std::list<SongBookItem *>::iterator prev = it;
			std::advance(prev, -1);
			// if previous item exists
			if (it != prev && it != m_items.begin())
			{
				SongBookItem *movedItem = *it;
				m_items.erase(it);
				m_items.insert(prev, movedItem);
				it = prev;
				continue;
			}
		}
		it++;
	}
}

void SongBook::moveSelectedDown()
{
	std::list<SongBookItem *>::iterator it = m_items.end();

	if (m_items.empty())
		return;

	// go to last element (since end() retruns iterator pointing after last element)
	it--;

	// checking is against end, because this is only way how to detect crossing beginning
	// of the list (list is cyclic)
	while (it != m_items.end())
	{
		if ((*it)->isSelected())
		{
			// get 2 next items
			std::list<SongBookItem *>::iterator next = it;
			next++;
			if (next != m_items.end())
			{
				std::wcout << "moving item by 1" << std::endl;
				std::list<SongBookItem *>::iterator next2 = next;
				next2++;
				SongBookItem *movedItem = *it;
				std::list<SongBookItem *>::iterator toMove = it;
				it--;
				m_items.erase(toMove);
				m_items.insert(next2, movedItem);
				continue;
			}
		}
		it--;
	}
}
