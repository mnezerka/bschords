/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Songbook Declaration
 *
 * Definition of Songbook class and all related classes (e.g. SongBookSong)
 */

#ifndef SONGBOOK_H
#define SONGBOOK_H

#include <list>
#include <wx/xml/xml.h>
#include <wx/filename.h>

namespace bschords
{

/** \brief Song representation */
class SongBookItem
{
public:
    /// Ctor
    SongBookItem(wxString title) : mTitle(title), mSelected(false), mPrintFlag(true), mTransposeStep(0), mGroupId(0) { };

    /// Dtor
    virtual ~SongBookItem() {};

    /// Get flag which item behaves as group separator - e.g. section name
    virtual bool isGroupSeparator()
    {
        return false;
    }

    /// Mark item as selected
    void select(bool select = true)
    {
        mSelected = select;
    };

    /// Get selection for this item
    bool isSelected()
    {
        return(mSelected);
    };

    /// Enable printing for this item
    void setPrintFlag(bool printFlag = true)
    {
        mPrintFlag = printFlag;
    };

    /// Get status of printing for this item
    bool getPrintFlag()
    {
        return(mPrintFlag);
    };

    /// Check if item could be printed
    virtual bool isPrintable()
    {
        return false;
    };

    /// Check if item is transposeable (musicaly)
    virtual bool isTransposeable()
    {
        return false;
    };

    /// Get transpose step (item must be transposeable)
    virtual int getTransposeStep()
    {
        return mTransposeStep;
    }

    /// Set transpose step (item must be transposeable)
    virtual void setTransposeStep(int newStep)
    {
        mTransposeStep = newStep;
    }

    /// Get path for items that represent file in filesystem
    virtual wxString getPath() { return wxT(""); };

    /// Get item title
    virtual wxString getTitle() { return (mTitle); };

    /// Set item title
    virtual void setTitle(wxString title) { mTitle = title; };

    /// Get item contents in ChordPro format)
    virtual wxString getContents() { return wxT(""); };

    /// Get item comment
    wxString getComment()
    {
        return (mComment);
    };

    /// Set item comment
    void setComment(wxString comment)
    {
        mComment = comment;
    };

    /// Get BSChords toc directive
    virtual wxString getTocDirective() { return wxT(""); };

    /// Create XML representation of item
    virtual wxXmlNode *createXmlNode(const wxString basePath) { return NULL; };

    /// Read item data from XML representation
    virtual void readFromXmlNode(wxXmlNode *node);

    /// Write item data to XML representation
    virtual void writeToXmlNode(wxXmlNode *node);

    void setGroupId(unsigned int groupId)
    {
        mGroupId = groupId;
    };

    bool operator < (SongBookItem& rhs)
	{
	    //return mGroupId < rhs.mGroupId;

	    if (mGroupId != rhs.mGroupId)
            return mGroupId < rhs.mGroupId;
        else
            return getTitle() < rhs.getTitle();
	}

private:
    wxString mTitle;
    bool mSelected;
    bool mPrintFlag;
    int mTransposeStep;
    wxString mComment;
    unsigned int mGroupId;  // this attributes is used when sorting, it sotres index of grouop
};

class SongBookSection: public SongBookItem
{
public:
    SongBookSection(wxXmlNode *node) : SongBookItem(wxT(""))
    {
        readFromXmlNode(node);
    };
    SongBookSection(wxString title) : SongBookItem(title) { };
    virtual ~SongBookSection() { };
    virtual bool isGroupSeparator()
    {
        return true;
    }
    virtual bool isPrintable();
    virtual wxString getContents();
    virtual wxString getTocDirective();
    virtual wxXmlNode *createXmlNode(const wxString basePath);
    virtual void readFromXmlNode(wxXmlNode *node);
    virtual void writeToXmlNode(wxXmlNode *node);
};

class SongBookSong: public SongBookItem
{
public:
    SongBookSong(wxString path);
    SongBookSong(wxXmlNode *node) : SongBookItem(wxT(""))
    {
        readFromXmlNode(node);
    };
    virtual ~SongBookSong() { };
    virtual bool isPrintable()
    {
        return true;
    };
    virtual bool isTransposeable()
    {
        return true;
    };
    //virtual wxString getTitle();
    virtual wxString getPath();
    virtual wxString getContents();
    virtual wxString getTocDirective();
    virtual wxXmlNode *createXmlNode(const wxString basePath);
    virtual void readFromXmlNode(wxXmlNode *node);
    virtual void writeToXmlNode(wxXmlNode *node);

private:
    wxString getTitleFromPath(wxString path);
    wxString mPath;
};

// songbook (group of songs with attributes) representation
class SongBook
{
public:
    SongBook();
    virtual ~SongBook();
    void setBasePath(wxString path);
    void empty();
    unsigned int getCount();
    SongBookItem *getItem(unsigned int index);
    void loadFromXmlFile(wxString rootPath);
    void saveToXmlFile(wxString path, wxString rootPath);
    void addItem(SongBookItem *item);
    wxString getContents();
    void setItemTitle(unsigned int index, wxString title);
    void selectAll(bool select = true);
    void selectItem(unsigned int index, bool select = true);
    void deleteSelected();
    int getLastSelected();
    void moveSelectedUp();
    void moveSelectedDown();
    void setPrintFlagForSelected(bool printFlag);
    void sort();

    void setName(const wxString &name)
    {
        mName = name;
        mModified = true;
    }
    wxString getName()
    {
        return mName;
    }
    void setDescription(const wxString &description)
    {
        mDescription = description;
        mModified = true;
    }
    wxString getDescription()
    {
        return mDescription;
    }
    bool isModified()
    {
        return mModified;
    }
    void exportHtml(const wxString path);
    void exportTxt(const wxString path);
private:
    std::list<SongBookItem*> m_items;
    wxString m_basePath;

    /// songbook name
    wxString mName;

    /// songbook description
    wxString mDescription;

    /// flag indicating unsaved changes
    bool mModified;
};

} // namespace

#endif // SONGBOOK_H
