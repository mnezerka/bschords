#ifndef SONGBOOK_H
#define SONGBOOK_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include <wx/xml/xml.h>

namespace bschords
{
	// songbook representation
	class SongBook
	{
		public:
			SongBook();
			virtual ~SongBook();
			void empty();
			void loadFromFile(wxString rootPath);
			void saveToFile(wxString path, wxString rootPath);
			void addSong(wxString path, wxXmlNode *parent = NULL);
			void addSection(wxXmlNode *parent = NULL);
			wxXmlNode *getRootNode() { return m_songs.GetRoot(); };
			static void setNodeProperty(wxXmlNode *node, wxString name, wxString value);
			static bool moveNode(wxXmlNode *node, wxXmlNode *newParent);

		private:
			wxXmlDocument m_songs;

			void addNode(wxXmlNode *newNode, wxXmlNode *parent = NULL);
	};
}

#endif // SONGBOOK_H
