#include <iostream>
#include "songstylesheet.h"

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
}

SongStyleSheet::~SongStyleSheet()
{
	//dtor
}

void SongStyleSheet::LoadFromConfig(const wxConfig *config)
{
	// load stylesheets
	if (config->Exists(_("/stylesheet/")))
	{
		// read fonts
		for (int i = 0; i < BS_FONT_LAST; i++)
		{
			wxString key(_("stylesheet/fonts/"));
			key += fontNames[i];
			wxString nativeFontInfo = config->Read(key, _(""));
			m_fonts[i] = *wxNORMAL_FONT;
			if (nativeFontInfo.size() > 0)
			{
				std::cout << i << "native: " << nativeFontInfo.mb_str(wxConvUTF8) << std::endl;
				m_fonts[i].SetNativeFontInfo(nativeFontInfo);
			}
			//fonts[i].name.
			//cout << font;
		}
	}
}

void SongStyleSheet::SaveToConfig(wxConfig *config)
{
    // save font information
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
        std::cout << i << " user desc: " << m_fonts[i].GetNativeFontInfoDesc().mb_str(wxConvUTF8) << std::endl;
        wxString nativeFontInfo = m_fonts[i].GetNativeFontInfoDesc();
        wxString key(_("stylesheet/fonts/"));
        key += fontNames[i];
        config->Write(key, nativeFontInfo);
        std::cout << nativeFontInfo.c_str() << std::endl;
    }
}
