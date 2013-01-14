#ifndef SONGBOOK_H
#define SONGBOOK_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <list>

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

		SongBook();
		virtual ~SongBook();
		void LoadFromFile();
		void SaveToFile();

	protected:
	private:
		std::list<Song> m_songs;
};

#endif // SONGBOOK_H
