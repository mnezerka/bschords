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
    SongBookItem() : mSelected(false), mPrintFlag(true), mTransposeStep(0) { };

    /// Dtor
    virtual ~SongBookItem() = 0;

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
    virtual wxString getPath() = 0;

    /// Get item title
    virtual wxString getTitle() = 0;

    /// Set item title
    virtual void setTitle(wxString title) = 0;

    /// Get item contents in ChordPro format)
    virtual wxString getContents() = 0;

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
    virtual wxString getTocDirective() = 0;

    /// Create XML representation of item
    virtual wxXmlNode *createXmlNode(const wxString basePath) = 0;

    /// Read item data from XML representation
    virtual void readFromXmlNode(wxXmlNode *node);

    /// Write item data to XML representation
    virtual void writeToXmlNode(wxXmlNode *node);
private:
    bool mSelected;
    bool mPrintFlag;
    int mTransposeStep;
    wxString mComment;
};

class SongBookSection: public SongBookItem
{
public:
    SongBookSection(wxXmlNode *node) : SongBookItem()
    {
        readFromXmlNode(node);
    };
    SongBookSection(wxString title) : mTitle(title) { };
    virtual ~SongBookSection() { };
    virtual bool isPrintable();
    virtual wxString getTitle()
    {
        return (mTitle);
    };
    virtual void setTitle(wxString title)
    {
        mTitle = title;
    };
    virtual wxString getContents();
    virtual wxString getTocDirective();
    virtual wxString getPath()
    {
        return wxT("");
    };
    virtual wxXmlNode *createXmlNode(const wxString basePath);
    virtual void readFromXmlNode(wxXmlNode *node);
    virtual void writeToXmlNode(wxXmlNode *node);
private:
    wxString mTitle;
};

class SongBookSong: public SongBookItem
{
public:
    SongBookSong(wxString path);
    SongBookSong(wxXmlNode *node) : SongBookItem()
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
    virtual wxString getTitle();
    virtual void setTitle(wxString title) { };
    virtual wxString getPath();
    virtual wxString getContents();
    virtual wxString getTocDirective();
    virtual wxXmlNode *createXmlNode(const wxString basePath);
    virtual void readFromXmlNode(wxXmlNode *node);
    virtual void writeToXmlNode(wxXmlNode *node);
private:
    wxString mPath;
    wxString mTitle;
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
    std::list<SongBookItem *> m_items;
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
