#ifndef SONGBOOKWND_H
#define SONGBOOKWND_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class SongBookWnd : public wxWindow
{
	public:
		SongBookWnd(wxWindow *parent);
		virtual ~SongBookWnd();
		void UpdateContent();

	private:
		wxListBox *m_listBox;
		void OnSize(wxSizeEvent& event);

	DECLARE_EVENT_TABLE()
};


#endif // SONGBOOKWND_H
