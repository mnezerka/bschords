
#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include <iostream>

#include <vector>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>

#include "app.h"
#include "bschordpro.h"
#include "songbook.h"

using namespace bschords;

/* ------------------- SongBookWriterHtml --------------------------------------- */

class SongBookWriterHtml : public bschordpro::EventHandler
{
	public:
		SongBookWriterHtml(SongBook *songbook) : mSongbook(songbook) { };
		void write(const wxString &path);
	private:
		SongBook *mSongbook;
		wxFile mFile;
		std::vector<std::wstring> mLineChordBuffer;
		std::vector<std::wstring> mLineTextBuffer;

		virtual void onBegin();
		virtual void onEnd();
		virtual void onLineBegin();
		virtual void onLineEnd();
		virtual void onCommand(const bschordpro::CommandType command, const std::wstring& value, const bschordpro::RawPos &pos);
		//virtual void onCommandUnknown(const std::wstring &cmd, const std::wstring &value, const RawPos &pos) {};
		virtual void onChord(const std::wstring& chord, const bschordpro::RawPos &pos);
		virtual void onText(const std::wstring& text, const bschordpro::RawPos &pos);
		//virtual void onLine(const std::wstring& line, const RawPos &pos) {};
};

void SongBookWriterHtml::onBegin()
{
    if (!mFile.IsOpened())
		return;
	mFile.Write(wxT("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-strict.dtd\">"));
	mFile.Write(wxT("<html>\n"));
	mFile.Write(wxT("  <head>\n"));
	mFile.Write(wxT("    <meta charset=\"UTF-8\" />\n"));
	mFile.Write(wxString::Format(wxT("    <title>Songbook: %s</title>\n"), mSongbook->getName().wc_str()), wxConvUTF8);
	mFile.Write(wxT("  </head>\n"));
	mFile.Write(wxT("<body>\n"));

	if (wxGetApp().m_styleSheet.m_songbookTitlePage)
		mFile.Write(wxString::Format(wxT("<h1>%s</h1>\n"), mSongbook->getName().wc_str()), wxConvUTF8);
}

void SongBookWriterHtml::onEnd()
{
    if (!mFile.IsOpened())
		return;

	mFile.Write(wxT("</body>\n"));
	mFile.Write(wxT("</html>\n"));
}

void SongBookWriterHtml::onLineBegin()
{
	mLineChordBuffer.clear();
	mLineTextBuffer.clear();
}

void SongBookWriterHtml::onLineEnd()
{
	mFile.Write(wxT("<table>\n"));
	if (mLineChordBuffer.size() > 0)
	{
		mFile.Write(wxT("  <tr>\n"));
		for (std::vector<std::wstring>::iterator it = mLineChordBuffer.begin(); it != mLineChordBuffer.end(); it++)
			mFile.Write(wxString::Format(wxT("<th>%s</th>"), (*it).c_str()), wxConvUTF8);
		mFile.Write(wxT("  </tr>\n"));
	}
	mFile.Write(wxT("  <tr>\n"));
	for (std::vector<std::wstring>::iterator it = mLineTextBuffer.begin(); it != mLineTextBuffer.end(); it++)
		mFile.Write(wxString::Format(wxT("<td>%s</td>"), (*it).c_str()), wxConvUTF8);
	mFile.Write(wxT("  </tr>\n"));
	mFile.Write(wxT("</table>\n"));
}

void SongBookWriterHtml::onChord(const std::wstring& chord, const bschordpro::RawPos &pos)
{
	//if (mLineChordBuffer.Empty())
	mLineChordBuffer.push_back(chord);

	//mFile.Write(wxString::Format(wxT("<td>%s</td>\n"), chord.c_str()), wxConvUTF8);
}

void SongBookWriterHtml::onText(const std::wstring& text, const bschordpro::RawPos &pos)
{
	mLineTextBuffer.push_back(text);
	while (mLineChordBuffer.size() < mLineTextBuffer.size())
		mLineChordBuffer.push_back(L"");
	//mFile.Write(wxString::Format(wxT("<td>%s</td>\n"), text.c_str()), wxConvUTF8);
}

void SongBookWriterHtml::onCommand(const bschordpro::CommandType command, const std::wstring& value, const bschordpro::RawPos &pos)
{
	if (command == bschordpro::CMD_TITLE)
    {
    	mFile.Write(wxString::Format(wxT("<h2>%s</h2>\n"), value.c_str()), wxConvUTF8);
	}
	else if (command == bschordpro::CMD_SUBTITLE)
    {
    	mFile.Write(wxString::Format(wxT("<h3>%s</h3>\n"), value.c_str()), wxConvUTF8);
	}
}



void SongBookWriterHtml::write(const wxString &path)
{
	wxLogDebug(wxT("export songbook to html"));

	mFile.Open(path, wxFile::write);

	//wxString contents = mSongbook.getContents();
	bschordpro::Parser p(this);
	p.parse(mSongbook->getContents().wc_str());

	mFile.Close();
}



/* ------------------- SongBookItem --------------------------------------- */
SongBookItem::~SongBookItem()
{

}
/* ------------------- SongBookSection ------------------------------------ */

bool SongBookSection::isPrintable()
{
	return wxGetApp().m_styleSheet.m_songbookSectionPages;
}

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

wxString SongBookSection::getContents()
{
	if (wxGetApp().m_styleSheet.m_songbookSectionPages)
		return (wxT("{bschords_section: ") + mTitle + wxT("}"));
	else
		return (wxT(""));
}

/* ------------------- SongBookSong --------------------------------------- */
SongBookSong::SongBookSong(wxString path)
{
	wxFileName filePath(path);
	mPath = filePath.GetFullPath();
}

wxString SongBookSong::getTitle()
{
	if (mTitle.Length() > 0)
		return mTitle;

	wxString contents;

    wxFile file(mPath, wxFile::read);
    if (file.IsOpened())
    {
        // get the file size (assume it is not huge file...)
        ssize_t len = (ssize_t)file.Length();

        if (len > 0)
        {
            wxMemoryBuffer buffer(len+1);
            bool success = (file.Read(buffer.GetData(), len) == len);
            if (success) {
                ((char*)buffer.GetData())[len] = 0;
                contents = wxString(buffer, wxConvUTF8, len);
            }
        }
    }

	std::wstring toParse(contents.wc_str());
	bschordpro::InfoReader infoReader(toParse);
	mTitle = infoReader.getTitle();
	return mTitle;
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

	wxLogDebug(wxT("Reading file contents from: %s"), mPath.wc_str());

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
				wxLogError(wxT("Invalid offset while reading file: %s"), mPath.wc_str());
        }
    }
    else
    {
		wxLogError(wxT("Cannot open file: %s"), mPath.wc_str());
    }

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
	filePath.MakeRelativeTo(basePath);
	wxXmlNode *songNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("song"));
	wxXmlProperty *prop = new wxXmlProperty(wxT("path"), filePath.GetFullPath(wxPATH_UNIX));
	songNode->AddProperty(prop);

	return songNode;
}

/* ------------------- SongBook -------------------------------------------- */

SongBook::SongBook()
{
	mName = wxT("Songbook");
	mDescription = wxT("");
	mModified = false;
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
	mModified = true;
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
	mModified = true;
}

void SongBook::saveToXmlFile(wxString path, wxString rootPath)
{
	wxXmlDocument doc;
	wxXmlNode *rootNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("songbook"));
	wxXmlProperty *prop = new wxXmlProperty(wxT("name"), mName);
	rootNode->AddProperty(prop);

	// create description node
	wxXmlNode *descriptionNode = new wxXmlNode(rootNode, wxXML_ELEMENT_NODE, wxT("description"));
	wxXmlNode *descriptionNodeText = new wxXmlNode(descriptionNode, wxXML_TEXT_NODE, wxEmptyString);
	descriptionNodeText->SetContent(mDescription);
	descriptionNode->AddChild(descriptionNodeText);
	rootNode->AddChild(descriptionNode);

	wxXmlNode *lastChild = descriptionNode;
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		wxXmlNode *itemNode = (*it)->createXmlNode(m_basePath);

		if (lastChild != NULL)
			lastChild->SetNext(itemNode);
		else
			rootNode->AddChild(itemNode);
		lastChild = itemNode;
	}

	doc.SetRoot(rootNode);

	if (!doc.Save(path))
		wxMessageBox(_("Error saving songbook"));
	else
		mModified = false;
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

	// get songbook name
	mName = rootNode->GetPropVal(wxT("name"), wxT("Songbook"));

	wxXmlNode *c = rootNode->GetChildren();
	while (c)
	{
		if (c->GetName() == wxT("description"))
		{
			wxXmlNode *t = c->GetChildren();
			if (t && t->GetType() == wxXML_TEXT_NODE)
				mDescription = t->GetContent();

		}
		else if (c->GetName() == wxT("song"))
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
	mModified = false;
}

unsigned int SongBook::getCount()
{
	return m_items.size();
}

wxString SongBook::getContents()
{
	wxString result;

	if (wxGetApp().m_styleSheet.m_songbookTitlePage)
	{
		result.Append(wxT("{bschords_title_page: "));
		result.Append(mName);
		//result.Append(wxT(":"));
		//result.Append(mDescription);
		result.Append(wxT("}\n"));
	}

	unsigned int i = 0;
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		if (!(*it)->isPrintable())
			continue;

		if (!(*it)->getPrintFlag())
			continue;

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
	if (!item)
		return;

	item->setTitle(title);
	mModified = true;
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
			// cannot move up if selection contains first item
			if (it == m_items.begin())
				break;

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
	mModified = true;
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
				std::list<SongBookItem *>::iterator next2 = next;
				next2++;
				SongBookItem *movedItem = *it;
				std::list<SongBookItem *>::iterator toMove = it;
				it--;
				m_items.erase(toMove);
				m_items.insert(next2, movedItem);
				continue;
			}
			else
				// cannot move down if selection contains last item
				break;
		}
		it--;
	}

	mModified = true;
}

void SongBook::setPrintFlagForSelected(bool printFlag)
{
	for (std::list<SongBookItem *>::iterator it = m_items.begin(); it != m_items.end(); it++)
	{
		if ((*it)->isSelected())
			(*it)->setPrintFlag(printFlag);
	}
}

void SongBook::exportHtml(const wxString path)
{
	SongBookWriterHtml writer(this);
	writer.write(wxT("d:\\export.html"));

}
