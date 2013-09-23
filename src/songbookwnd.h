#ifndef SONGBOOKWND_H
#define SONGBOOKWND_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/xml/xml.h>
//#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include "songbook.h"

namespace bschords
{
	class SongBookWnd : public wxWindow
	{
		public:
			SongBookWnd(wxWindow *parent, wxWindowID id);
			virtual ~SongBookWnd();
			void OnNewSection(wxCommandEvent &event);
			void addSongFile(wxString filePath);
			void Update();

		private:
			//SongBookTreeCtrl *m_treeCtrl;
			wxListView *m_listCtrl;
			void OnSize(wxSizeEvent& event);
			void OnSongBookItemActivated(wxListEvent& event);
			void OnSongBookItemRightClick(wxListEvent& event);
			void OnContextMenu(wxContextMenuEvent& event);
			void ShowContextMenu(const wxPoint& pos);
			void OnListKeyDown(wxListEvent& event);

		DECLARE_EVENT_TABLE()
	};


}

#endif // SONGBOOKWND_H
