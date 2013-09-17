#ifndef SONGBOOK_H
#define SONGBOOK_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include <wx/xml/xml.h>
#include <wx/filename.h>

namespace bschords
{
	// single song representation
	class SongBookItem
	{
		public:
			SongBookItem(wxString path);
			wxString getPath();
			wxString getTitle();
		private:
			wxString m_path;
			wxString m_title;
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
			void addSong(wxString path);
		private:
			std::vector<SongBookItem *> m_items;
			wxString m_basePath;
	};
}

#endif // SONGBOOK_H
