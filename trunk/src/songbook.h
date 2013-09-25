#ifndef SONGBOOK_H
#define SONGBOOK_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <list>
#include <wx/xml/xml.h>
#include <wx/filename.h>

namespace bschords
{
	// single song representation
	class SongBookItem
	{
		public:
			SongBookItem() : mSelected(false) { };
			virtual ~SongBookItem() = 0;
			void select(bool select = true) { mSelected = select; };
			bool isSelected() { return(mSelected); };
			virtual wxString getPath() = 0;
			virtual wxString getTitle() = 0;
			virtual void setTitle(wxString title) = 0;
			virtual wxString getContents() = 0;
			virtual wxXmlNode *createXmlNode(wxString basePath) = 0;
			virtual void readFromXmlNode(wxXmlNode *node, wxString basePath) = 0;
		private:
			bool mSelected;
	};

	class SongBookSection: public SongBookItem
	{
		public:
			SongBookSection(wxXmlNode *node, wxString basePath) : SongBookItem() { readFromXmlNode(node, basePath); };
			SongBookSection(wxString title) : mTitle(title) { };
			virtual ~SongBookSection() { };
			virtual wxString getTitle() { return (mTitle); };
			virtual void setTitle(wxString title) { mTitle = title; };
			virtual wxString getContents() { return wxT(""); };
			virtual wxString getPath() { return wxT(""); };
			virtual wxXmlNode *createXmlNode(wxString basePath);
			virtual void readFromXmlNode(wxXmlNode *node, wxString basePath);
		private:
			wxString mTitle;
	};

	class SongBookSong: public SongBookItem
	{
		public:
			SongBookSong(wxString path);
			SongBookSong(wxXmlNode *node, wxString basePath) : SongBookItem() { readFromXmlNode(node, basePath); };
			virtual ~SongBookSong() { };
			virtual wxString getTitle();
			virtual void setTitle(wxString title) { };
			wxString getPath();
			wxString getContents();
			virtual wxXmlNode *createXmlNode(wxString basePath);
			virtual void readFromXmlNode(wxXmlNode *node, wxString basePath);
		private:
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
			void moveSelectedUp();
			void moveSelectedDown();
		private:
			std::list<SongBookItem *> m_items;
			wxString m_basePath;
	};
}

#endif // SONGBOOK_H
