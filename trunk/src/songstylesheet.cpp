#include <iostream>
#include "songstylesheet.h"
#include <wx/arrstr.h>
#include <wx/xml/xml.h>

// page size
#define SIZE_A4_WIDTH 210
#define SIZE_A4_HEIGHT 297

SongStyleSheet::SongStyleSheet()
{
	// defaults for A4 page
	m_pageSize.SetWidth(SIZE_A4_WIDTH);
	m_pageSize.SetHeight(SIZE_A4_HEIGHT);

	m_marginLeft = m_marginRight = 15;
	m_marginTop = 15;
	m_marginBottom = 20;
	m_indentChorus = 0;
	m_lineSpacing = 0;
	m_chordLineSpacing = 0;
	m_cols = 1;
	m_songNewPage = true;
	m_songbookTitlePage = true;;
	m_songbookSectionPages = true;
	m_songbookToc = true;
}

SongStyleSheet::~SongStyleSheet()
{
	//dtor
}

void SongStyleSheet::LoadFromConfig(const wxConfigBase *config)
{
	// load stylesheets
	if (config->Exists(_("/stylesheet/")))
	{
		// load page size
		m_pageSize.SetWidth(config->Read(_("stylesheet/page/width"), m_pageSize.GetWidth()));
		m_pageSize.SetHeight(config->Read(_("stylesheet/page/height"), m_pageSize.GetHeight()));

		// load margins
		m_marginLeft = config->Read(_("stylesheet/page/margin-left"), m_marginLeft);
		m_marginTop = config->Read(_("stylesheet/page/margin-top"), m_marginTop);
		m_marginRight = config->Read(_("stylesheet/page/margin-right"), m_marginRight);
		m_marginBottom = config->Read(_("stylesheet/page/margin-bottom"), m_marginBottom);

		// load other page options
		m_cols = config->Read(_("stylesheet/page/cols"), m_cols);
		m_lineSpacing = config->Read(_("stylesheet/page/line-spacing"), m_lineSpacing);
		m_chordLineSpacing = config->Read(_("stylesheet/page/chord-line-spacing"), m_chordLineSpacing);
		m_indentChorus = config->Read(_("stylesheet/page/indent-chorus"), m_indentChorus);

		m_verseNumbering = config->Read(_("stylesheet/content/verse-numbering"), m_verseNumbering);
		m_showChords = config->Read(_("stylesheet/content/show-chords"), m_showChords);
		m_showSubtitles = config->Read(_("stylesheet/content/show-subtitles"), m_showSubtitles);
		m_showTabs = config->Read(_("stylesheet/content/show-tabs"), m_showTabs);
		m_showStructs = config->Read(_("stylesheet/content/show-structs"), m_showStructs);
		m_equalLineHeights = config->Read(_("stylesheet/content/equal-line-heights"), m_equalLineHeights);
		m_songNewPage = config->Read(_("stylesheet/content/song-new-page"), m_songNewPage);
		m_songbookTitlePage = config->Read(_("stylesheet/content/songbook-title-page"), m_songbookTitlePage);
		m_songbookSectionPages = config->Read(_("stylesheet/content/songbook-section-pages"), m_songbookSectionPages);
		m_songbookToc = config->Read(_("stylesheet/content/songbook-toc"), m_songbookToc);

		// read fonts
		for (int i = 0; i < BS_FONT_LAST; i++)
		{
			wxString key(_("stylesheet/fonts/"));
			key += SongStyleSheet::getFontName(i);
			wxString nativeFontInfo = config->Read(key, _(""));
			m_fonts[i] = *wxNORMAL_FONT;
			if (nativeFontInfo.size() > 0)
			{
				//std::cout << i << "native: " << nativeFontInfo.mb_str(wxConvUTF8) << std::endl;
				m_fonts[i].SetNativeFontInfo(nativeFontInfo);
			}
		}
	}
}

void SongStyleSheet::SaveToConfig(wxConfigBase *config)
{
	// save page size
	config->Write(_("stylesheet/page/width"), m_pageSize.GetWidth());
	config->Write(_("stylesheet/page/height"), m_pageSize.GetHeight());

	// save page margins
	config->Write(_("stylesheet/page/margin-left"), m_marginLeft);
	config->Write(_("stylesheet/page/margin-top"), m_marginTop);
	config->Write(_("stylesheet/page/margin-right"), m_marginRight);
	config->Write(_("stylesheet/page/margin-bottom"), m_marginBottom);

	// save other page options
	config->Write(_("stylesheet/page/cols"), m_cols);
	config->Write(_("stylesheet/page/line-spacing"), m_lineSpacing);
	config->Write(_("stylesheet/page/chord-line-spacing"), m_chordLineSpacing);
	config->Write(_("stylesheet/page/indent-chorus"), m_indentChorus);

	config->Write(_("stylesheet/content/verse-numbering"), m_verseNumbering);
	config->Write(_("stylesheet/content/show-chords"), m_showChords);
	config->Write(_("stylesheet/content/show-subtitles"), m_showSubtitles);
	config->Write(_("stylesheet/content/show-tabs"), m_showTabs);
	config->Write(_("stylesheet/content/show-structs"), m_showStructs);
	config->Write(_("stylesheet/content/equal-line-heights"), m_equalLineHeights);
	config->Write(_("stylesheet/content/song-new-page"), m_songNewPage);
	config->Write(_("stylesheet/content/songbook-title-page"), m_songbookTitlePage);
	config->Write(_("stylesheet/content/songbook-section-pages"), m_songbookSectionPages);
	config->Write(_("stylesheet/content/songbook-toc"), m_songbookToc);

    // save font information
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
        //std::cout << i << " user desc: " << m_fonts[i].GetNativeFontInfoDesc().mb_str(wxConvUTF8) << std::endl;
        wxString nativeFontInfo = m_fonts[i].GetNativeFontInfoDesc();
        wxString key(_("stylesheet/fonts/"));
        key += SongStyleSheet::getFontName(i);
        config->Write(key, nativeFontInfo);
        //std::cout << nativeFontInfo.c_str() << std::endl;
    }
}

wxString SongStyleSheet::getFontName(unsigned int index)
{
	static wxString fontNames[BS_FONT_LAST] = {
		wxT("Title"),
		wxT("Subtitle"),
		wxT("Text"),
		wxT("Chords"),
		wxT("Tab"),
		wxT("Structure"),
		wxT("Main Page Title"),
		wxT("Section Title")
	};

	wxString result(wxT(""));
	if (index < BS_FONT_LAST)
		result = fontNames[index];
	return result;
}
