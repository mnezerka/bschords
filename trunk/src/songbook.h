#ifndef SONGBOOK_H
#define SONGBOOK_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>

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
			wxString m_name;
			std::vector<Song*> m_songs;

			SongBook();
			virtual ~SongBook();
			void empty();
			void loadFromFile();
			void saveToFile();
			void add(wxString path);
		protected:
	};
}

#endif // SONGBOOK_H
