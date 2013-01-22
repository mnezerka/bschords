#ifndef SONGBOOK_H
#define SONGBOOK_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include <wx/xml/xml.h>

namespace bschords
{
	// represantion of one song of songbook
	struct Song
	{
		wxString m_name;
		wxString m_filePath;
		int m_transposition;
	};

	// songbook representation
	class SongBook
	{
		public:
			//wxString m_name;
			//std::vector<Song*> m_songs;


			SongBook();
			virtual ~SongBook();
			void empty();
			void loadFromFile(wxString rootPath);
			void saveToFile(wxString path, wxString rootPath);
			void add(Song *song);
			wxXmlNode *getRootNode() { return m_songs.GetRoot(); };
		protected:

		private:
			wxXmlDocument m_songs;
	};
}

#endif // SONGBOOK_H
