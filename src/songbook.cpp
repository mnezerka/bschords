
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

SongBookItem::SongBookItem(wxString path)
{
	// check if path is valid text file
	wxFileName filePath(path);

	m_path = filePath.GetFullPath();
	m_title = filePath.GetName();
}

wxString SongBookItem::getTitle()
{
	return m_title;
}

wxString SongBookItem::getPath()
{
	return m_path;
}

wxString SongBookItem::getContents()
{

	wxString contents;
	bool success = false;
    wxFile file(m_path, wxFile::read);

    std::wcout << L"Reading file contents from: " << m_path.mb_str(wxConvUTF8) << std::endl;

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
				std::cout << "Invalid offset while reading file " << m_path.mb_str(wxConvUTF8) << std::endl;
        }
    }
    else
    {
    	std::cout << "Cannot open file " << m_path.mb_str(wxConvUTF8) << std::endl;
    }

    std::wcout << L"File contents: " << contents.mb_str(wxConvUTF8) << std::endl;

	return contents;
}

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
	for (unsigned i = 0; i < m_items.size(); i++)
		delete(m_items[i]);
	m_items.clear();
}

SongBookItem *SongBook::getItem(unsigned int index)
{
	SongBookItem *result = NULL;
	if (index < m_items.size())
		result = m_items[index];
	return result;
}

void SongBook::addSong(wxString path)
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

	SongBookItem *item = new SongBookItem(filePath.GetFullPath());
	m_items.push_back(item);
}

void SongBook::saveToXmlFile(wxString path, wxString rootPath)
{
	wxXmlDocument doc;
	wxXmlNode *rootNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("songbook"));

	std::cout << "Saving to XML file (begin)" << std::endl;
	std::cout << "Number of items to be saved: " << m_items.size() << std::endl;
	std::wcout << L"Base path: " << m_basePath.mb_str(wxConvUTF8) << std::endl;

	wxXmlNode *lastChild = NULL;
	for (unsigned i = 0; i < m_items.size(); i++)
	{
		wxFileName filePath(m_items[i]->getPath());
		std::wcout << L"File to be added: " << filePath.GetFullPath().mb_str(wxConvUTF8) << std::endl;
		filePath.MakeRelativeTo(m_basePath);
		std::wcout << L"File to be added after relative step: " << filePath.GetFullPath().mb_str(wxConvUTF8) << std::endl;

		wxXmlNode *songNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("song"));
		wxXmlProperty *prop = new wxXmlProperty(wxT("path"), filePath.GetFullPath(wxPATH_UNIX));
		songNode->AddProperty(prop);
		std::cout << "Adding node to xml file" << std::endl;
		if (lastChild != NULL)
			lastChild->SetNext(songNode);
		else
			rootNode->AddChild(songNode);
		lastChild = songNode;
	}

	doc.SetRoot(rootNode);

	// TODO: convert path separators to normalized form
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
			wxString songPath = c->GetPropVal(wxT("path"), wxT(""));
			wxFileName fileName(songPath, wxPATH_UNIX);
			fileName.MakeAbsolute(m_basePath);
			std::wcout << "New dir is: " << fileName.GetFullPath().mb_str(wxConvUTF8) << std::endl;
			addSong(fileName.GetFullPath(wxPATH_NATIVE));
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
	for (unsigned i = 0; i < m_items.size(); i++)
	{
		if (i > 0)
			result.Append(wxT("\n\n"));
		result.Append(m_items[i]->getContents());
	}

	return result;
}
