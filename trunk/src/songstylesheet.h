#ifndef SONGSTYLESHEET_H
#define SONGSTYLESHEET_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class SongStylesheet
{
	public:
		SongStylesheet();
		virtual ~SongStylesheet();
	protected:
	private:
		wxString m_Name;
		wxSize m_pageSize;
		wxString m_pageSizeName;
		enum { SS_ORIENTATION_PORTRAIT, SS_ORIENTATION_LANDSCAPE} m_pageOrientation;
		wxCoord m_marginTop;
		wxCoord m_marginLeft;
		wxCoord m_marginBottom;
		wxCoord m_marginRight;
		int m_cols;
		int m_lineSpacing;
};

#endif // SONGSTYLESHEET_H
