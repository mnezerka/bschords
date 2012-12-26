/***************************************************************
 * Name:      bschordsApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2012-12-01
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <iostream>

#include "bschordsApp.h"
#include "bschordsMain.h"

// TODO: Hiding of preview window

IMPLEMENT_APP(bschordsApp);

bool bschordsApp::OnInit()
{
	config = new wxConfig(_("BSChords"));

    for (int i = 0; i < BS_FONT_LAST; i++)
    {
        wxString key(_("/fonts/"));
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

    bschordsFrame* frame = new bschordsFrame(0L, _("BSChords"));

    frame->Show();

    return true;
}

int bschordsApp::OnExit()
{
    // save font information
    for (int i = 0; i < BS_FONT_LAST; i++)
    {
        std::cout << i << " user desc: " << m_fonts[i].GetNativeFontInfoDesc().mb_str(wxConvUTF8) << std::endl;
        wxString nativeFontInfo = m_fonts[i].GetNativeFontInfoDesc();
        wxString key(_("/fonts/"));
        key += fontNames[i];
        config->Write(key, nativeFontInfo);
        std::cout << nativeFontInfo.c_str() << std::endl;
    }

	delete config;

	return 0;
}
