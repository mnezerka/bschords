/**
 * \file
 * \author  michal.nezerka@gmail.com
 * \brief   Implementation of Songbook class and all related classes (e.g. SongBookSong)
 */

#include <iostream>
#include <vector>
#include <stdexcept>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>

#include "app.h"
#include "bschordpro.h"
#include "songbook.h"

namespace bschords
{

/*bool SongBookItemGreater(const SongBookItem* a, const SongBookItem& b)
{
    return true;

	if(a.age == b.age)
		return a.name < b.name;

	return a.age > b.age;
} */

template <typename T> bool PComp(T * & a, T * & b)
{
   return *a < *b;
};

/* ------------------- SongBookWriterHtml --------------------------------------- */

class SongBookWriterHtml : public bschordpro::EventHandler
{
public:
    SongBookWriterHtml(SongBook *songbook) : mSongbook(songbook) { };
    void write(const wxString &path);
private:
    SongBook *mSongbook;
    wxString mOutput;
    std::vector<std::wstring> mLineChordBuffer;
    std::vector<std::wstring> mLineTextBuffer;
    std::vector<std::wstring> mTocBuffer;

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
    mOutput.Clear();
}

void SongBookWriterHtml::onEnd()
{
    wxString mOutputHeader;
    mOutputHeader += wxT("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-strict.dtd\">");
    mOutputHeader += wxT("<html>\n");
    mOutputHeader += wxT("  <head>\n");
    mOutputHeader += wxT("    <meta charset=\"UTF-8\" />\n");
    mOutputHeader += wxString::Format(wxT("    <title>Songbook: %s</title>\n"), mSongbook->getName().wc_str());
    mOutputHeader += wxT("    <style>\n");
    mOutputHeader += wxT("      table { border-collapse: collapse; }\n");
    mOutputHeader += wxT("      th { text-align: left; }\n");
    mOutputHeader += wxT("      td { text-align: left; }\n");
    mOutputHeader += wxT("      div.hspace { height: 20px; }\n");
    mOutputHeader += wxT("    </style>\n");
    mOutputHeader += wxT("  </head>\n");
    mOutputHeader += wxT("<body>\n");

    if (wxGetApp().m_styleSheet.m_songbookTitlePage)
        mOutputHeader += wxString::Format(wxT("<h1>%s</h1>\n"), mSongbook->getName().wc_str());

    mOutputHeader += wxT("<ul class=\"toc\">\n");
    for (std::vector<std::wstring>::iterator it = mTocBuffer.begin(); it != mTocBuffer.end(); it++)
        mOutputHeader += wxString::Format(wxT("  <li><a class=\"toc-item\" href=\"#%s\">%s</a></li>\n"), (*it).c_str(), (*it).c_str());
    mOutputHeader += wxT("</ul>\n");

    mOutput = mOutputHeader + mOutput;

    mOutput += wxT("</body>\n");
    mOutput += wxT("</html>\n");
}

void SongBookWriterHtml::onLineBegin()
{
    mLineChordBuffer.clear();
    mLineTextBuffer.clear();
}

void SongBookWriterHtml::onLineEnd()
{
    if (mLineTextBuffer.size() == 0)
    {
        mOutput += wxT("<div class=\"hspace\"></div>\n");
    }
    else
    {
        mOutput += wxT("<table>\n");
        if (mLineChordBuffer.size() > 0)
        {
            mOutput += wxT("  <tr>\n");
            for (std::vector<std::wstring>::iterator it = mLineChordBuffer.begin(); it != mLineChordBuffer.end(); it++)
                mOutput += wxString::Format(wxT("<th>%s</th>"), (*it).c_str());
            mOutput += wxT("  </tr>\n");
        }
        mOutput += wxT("  <tr>\n");
        for (std::vector<std::wstring>::iterator it = mLineTextBuffer.begin(); it != mLineTextBuffer.end(); it++)
            mOutput += wxString::Format(wxT("<td>%s</td>"), (*it).c_str());
        mOutput += wxT("  </tr>\n");
        mOutput += wxT("</table>\n");
    }
}

void SongBookWriterHtml::onChord(const std::wstring& chord, const bschordpro::RawPos &pos)
{
    mLineChordBuffer.push_back(chord);
}

void SongBookWriterHtml::onText(const std::wstring& text, const bschordpro::RawPos &pos)
{
    mLineTextBuffer.push_back(text);
    while (mLineChordBuffer.size() < mLineTextBuffer.size())
        mLineChordBuffer.push_back(L"");
}

void SongBookWriterHtml::onCommand(const bschordpro::CommandType command, const std::wstring& value, const bschordpro::RawPos &pos)
{
    if (command == bschordpro::CMD_TITLE)
    {
        mOutput += wxString::Format(wxT("<a name=\"%s\"></a>\n"), value.c_str());
        mOutput += wxString::Format(wxT("<h2>%s</h2>\n"), value.c_str());
        mTocBuffer.push_back(value.c_str());
    }
    else if (command == bschordpro::CMD_SUBTITLE)
    {
        mOutput += wxString::Format(wxT("<h3>%s</h3>\n"), value.c_str());
    }
}

void SongBookWriterHtml::write(const wxString &path)
{
    wxLogDebug(wxT("export songbook to html"));

    //wxString contents = mSongbook.getContents();
    bschordpro::Parser p(this);
    p.parse(mSongbook->getContents().wc_str());

    wxFile mFile;
    mFile.Open(path, wxFile::write);
    mFile.Write(mOutput, wxConvUTF8);
    mFile.Close();
}

/* ------------------- SongBookWriterTxt --------------------------------------- */

class SongbookWriterTxt : public bschordpro::EventHandler
{
public:
    SongbookWriterTxt(SongBook *songbook) : mSongbook(songbook) { };
    void write(const wxString &path);
private:
    SongBook *mSongbook;
    wxString mChordBuffer;
    wxString mTextBuffer;
    wxString mOutput;

    virtual void onBegin();
    virtual void onEnd();
    virtual void onLineBegin();
    virtual void onLineEnd();
    virtual void onLine(const std::wstring& line, const bschordpro::RawPos &pos);
    virtual void onCommand(const bschordpro::CommandType command, const std::wstring& value, const bschordpro::RawPos &pos);
    //virtual void onCommandUnknown(const std::wstring &cmd, const std::wstring &value, const RawPos &pos) {};
    virtual void onChord(const std::wstring& chord, const bschordpro::RawPos &pos);
    virtual void onText(const std::wstring& text, const bschordpro::RawPos &pos);
    //virtual void onLine(const std::wstring& line, const RawPos &pos) {};
};

void SongbookWriterTxt::onBegin()
{
    mOutput.Clear();
}

void SongbookWriterTxt::onEnd()
{

}

void SongbookWriterTxt::onLineBegin()
{
    mChordBuffer.Clear();
    mTextBuffer.Clear();
}

void SongbookWriterTxt::onLineEnd()
{
    int pos = -1;
    for (unsigned int i = 0; i < mChordBuffer.Length(); i++)
        if (mChordBuffer[i] != wxChar(' '))
        {
            pos = i;
            break;
        }

    if (pos != -1)
        mOutput += mChordBuffer + wxT("\n");

    mOutput += mTextBuffer + wxT("\n");
}

void SongbookWriterTxt::onLine(const std::wstring& line, const bschordpro::RawPos &pos)
{
    mTextBuffer += line.c_str();
}

void SongbookWriterTxt::onChord(const std::wstring& chord, const bschordpro::RawPos &pos)
{
    mChordBuffer += chord.c_str();
    mChordBuffer += wxChar(' ');
}

void SongbookWriterTxt::onText(const std::wstring& text, const bschordpro::RawPos &pos)
{
    mTextBuffer += text.c_str();

    int diff = mTextBuffer.Length() - mChordBuffer.Length();

    if (diff > 0)
        mChordBuffer.Append(L' ', diff);
    else
    {
        mTextBuffer.Append(L' ', -1 * diff);
    }
}

void SongbookWriterTxt::onCommand(const bschordpro::CommandType command, const std::wstring& value, const bschordpro::RawPos &pos)
{
    switch (command)
    {
    case bschordpro::CMD_TITLE:
        mOutput += value + wxT("\n");
        mOutput += wxT("------------------------------------\n");
        break;
    default:
        ;
    }
}

void SongbookWriterTxt::write(const wxString &path)
{
    bschordpro::Parser parser(this);

    parser.parse(mSongbook->getContents().wc_str());

    wxFile mFile;
    mFile.Open(path, wxFile::write);
    mFile.Write(mOutput, wxConvUTF8);
    mFile.Close();
}

/* ------------------- SongBookItem --------------------------------------- */

void SongBookItem::readFromXmlNode(wxXmlNode *node)
{
    if (!node)
        return;

    wxXmlNode *c = node->GetChildren();
    while (c)
    {
        if (c->GetName() == wxT("comment"))
        {
            wxXmlNode *t = c->GetChildren();
            if (t && t->GetType() == wxXML_TEXT_NODE)
                mComment = t->GetContent();

            break;
        }
        c = c->GetNext();
    }
}

void SongBookItem::writeToXmlNode(wxXmlNode *node)
{
    if (!node)
        return;

    // create comment node
    if (mComment.Length() > 0)
    {
        wxXmlNode *commentNode = new wxXmlNode(node, wxXML_ELEMENT_NODE, wxT("comment"));
        wxXmlNode *commentNodeText = new wxXmlNode(commentNode, wxXML_TEXT_NODE, wxEmptyString);
        commentNodeText->SetContent(getComment());
        commentNode->AddChild(commentNodeText);
    }
}

/* ------------------- SongBookSection ------------------------------------ */

bool SongBookSection::isPrintable()
{
    return wxGetApp().m_styleSheet.m_songbookSectionPages;
}

void SongBookSection::readFromXmlNode(wxXmlNode *node)
{
    if (node->GetName() != wxT("section"))
        return;

    SongBookItem::readFromXmlNode(node);

    setTitle(node->GetPropVal(wxT("name"), wxT("section")));
}

wxXmlNode* SongBookSection::createXmlNode(wxString basePath)
{
    return new wxXmlNode(wxXML_ELEMENT_NODE, wxT("section"));
}

void SongBookSection::writeToXmlNode(wxXmlNode *node)
{
    SongBookItem::writeToXmlNode(node);

    wxXmlProperty *prop = new wxXmlProperty(wxT("name"), getTitle());
    node->AddProperty(prop);
}

wxString SongBookSection::getContents()
{
    if (wxGetApp().m_styleSheet.m_songbookSectionPages)
        return (wxT("{bschords_section: ") + getTitle() + wxT("}"));
    else
        return (wxT(""));
}

wxString SongBookSection::getTocDirective()
{
    return (wxT("bschords_toc_section"));
}

/* ------------------- SongBookSong --------------------------------------- */
SongBookSong::SongBookSong(wxString path) : SongBookItem(wxT(""))
{
    wxFileName filePath(path);
    mPath = filePath.GetFullPath();

    setTitle(getTitleFromPath(mPath));
}

wxString SongBookSong::getTitleFromPath(wxString path)
{
    wxString contents;
    wxFile file(path, wxFile::read);
    if (file.IsOpened())
    {
        // get the file size (assume it is not huge file...)
        ssize_t len = (ssize_t)file.Length();

        if (len > 0)
        {
            wxMemoryBuffer buffer(len+1);
            bool success = (file.Read(buffer.GetData(), len) == len);
            if (success)
            {
                ((char*)buffer.GetData())[len] = 0;
                contents = wxString(buffer, wxConvUTF8, len);
            }
        }
    }

    std::wstring toParse(contents.wc_str());
    bschordpro::InfoReader infoReader(toParse);
    return infoReader.getTitle();
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
            if (success)
            {
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

wxXmlNode* SongBookSong::createXmlNode(wxString basePath)
{
    return new wxXmlNode(wxXML_ELEMENT_NODE, wxT("song"));
}

void SongBookSong::readFromXmlNode(wxXmlNode *node)
{
    if (node->GetName() != wxT("song"))
        return;

    SongBookItem::readFromXmlNode(node);

    wxString songPath = node->GetPropVal(wxT("path"), wxT(""));
    wxFileName fileName(songPath, wxPATH_UNIX);
    fileName.MakeAbsolute(wxGetApp().m_settings->m_rootPath);
    mPath = fileName.GetFullPath(wxPATH_NATIVE);

    setTitle(getTitleFromPath(mPath));
}

void SongBookSong::writeToXmlNode(wxXmlNode *node)
{
    SongBookItem::writeToXmlNode(node);

    wxFileName filePath(mPath);
    filePath.MakeRelativeTo(wxGetApp().m_settings->m_rootPath);
    wxXmlProperty *prop = new wxXmlProperty(wxT("path"), filePath.GetFullPath(wxPATH_UNIX));
    node->AddProperty(prop);
}

wxString SongBookSong::getTocDirective()
{
    return (wxT("bschords_toc_song"));
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
    m_items.clear();

    /*std::list<SongBookItem *>::iterator it;
    while (!m_items.empty())
    {
        it = m_items.begin();
        delete(*it);
        m_items.erase(it);
    }*/
}

SongBookItem *SongBook::getItem(unsigned int index)
{
    if (index > m_items.size())
        throw std::out_of_range("Item index out of range");

    SongBookItem *result = NULL;

    unsigned int i = 0;
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
    {
        if (i == index)
        {
            result = (*it);
            break;
        }
        else
            i++;
    }

    return result;
}

void SongBook::addItem(SongBookItem *item)
{

    std::list<SongBookItem*>::iterator lastSelected = m_items.end();
    // loop through the list in reverse direction to find last selected item
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
        if ((*it)->isSelected())
            lastSelected = it;

    // if item was not added after last selected item (in previous step), push it to the back of the list
    if (lastSelected == m_items.end())
        m_items.push_back(item);
    else
    {
        //lastSelected++;
        m_items.insert(lastSelected, item);
    }

    mModified = true;
}

void SongBook::deleteSelected()
{
    std::list<SongBookItem*>::iterator it = m_items.begin();

    while (it != m_items.end())
    {
        if ((*it)->isSelected())
        {
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
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
    {
        wxXmlNode *itemNode = (*it)->createXmlNode(m_basePath);
        (*it)->writeToXmlNode(itemNode);

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
            m_items.push_back(new SongBookSong(c));
        }
        else if (c->GetName() == wxT("section"))
        {
            m_items.push_back(new SongBookSection(c));
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

    if (wxGetApp().m_styleSheet.m_songbookToc)
    {
        result.Append(wxT("{bschords_toc_begin}\n"));
        for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
        {
            if (!(*it)->isPrintable() || !(*it)->getPrintFlag())
                continue;

            result += wxString::Format(wxT("{%s: %s}\n"), (*it)->getTocDirective().c_str(), (*it)->getTitle().c_str());
        }
        result.Append(wxT("{bschords_toc_end}\n"));
    }

    unsigned int i = 0;
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
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
    getItem(index)->setTitle(title);
    mModified = true;
}

void SongBook::selectAll(bool select)
{
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
        (*it)->select(select);
}

void SongBook::selectItem(unsigned int index, bool select)
{
    getItem(index)->select(select);
}

int SongBook::getLastSelected()
{
    int result = -1;
    unsigned int i = 0;
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
    {
        if ((*it)->isSelected())
            result = i;
        i++;
    }
    return result;
}

void SongBook::moveSelectedUp()
{
    std::list<SongBookItem*>::iterator it = m_items.begin();

    while (it != m_items.end())
    {
        if ((*it)->isSelected())
        {
            // cannot move up if selection contains first item
            if (it == m_items.begin())
                break;

            // get previous item
            std::list<SongBookItem*>::iterator prev = it;
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
    std::list<SongBookItem*>::iterator it = m_items.end();

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
            std::list<SongBookItem*>::iterator next = it;
            next++;
            if (next != m_items.end())
            {
                std::list<SongBookItem*>::iterator next2 = next;
                next2++;
                SongBookItem *movedItem = *it;
                std::list<SongBookItem*>::iterator toMove = it;
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
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
    {
        if ((*it)->isSelected())
            (*it)->setPrintFlag(printFlag);
    }
}

void SongBook::sort()
{
    unsigned int groupId = 0;

    // mark all items with group id reflecting current position in songbook
    for (std::list<SongBookItem*>::iterator it = m_items.begin(); it != m_items.end(); it++)
    {
        // check if current item is group separator
        if ((*it)->isGroupSeparator())
        {
            // set unique id to group and move to next item
            groupId++;
            (*it)->setGroupId(groupId);
            groupId++;
        }
        else
            (*it)->setGroupId(groupId);

    }

    m_items.sort(PComp<SongBookItem>);
}

void SongBook::exportHtml(const wxString path)
{
    SongBookWriterHtml writer(this);
    writer.write(path);
}

void SongBook::exportTxt(const wxString path)
{
    SongbookWriterTxt writer(this);
    writer.write(path);
}

} // namespace
