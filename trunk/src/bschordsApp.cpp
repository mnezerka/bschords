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
	//-----------------------------------------------------------------
	// load and initialize configuration data
	config = new wxConfig(_("BSChords"));

	//-----------------------------------------------------------------
	// initialize fonts
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

	//-----------------------------------------------------------------
	// initialize printing
	m_printData = new wxPrintData;
    // you could set an initial paper size here
	m_printData->SetPaperId(wxPAPER_A4); // for everyone else
    m_pageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*m_pageSetupData) = *m_printData;

    // set some initial page margins in mm.
    //m_pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
    //m_pageSetupData->SetMarginBottomRight(wxPoint(15, 15));

	//-----------------------------------------------------------------
	// open main window (frame)

    bschordsFrame* frame = new bschordsFrame(0L, _("BSChords"));

    frame->Show();

    return true;
}

int bschordsApp::OnExit()
{
	//-----------------------------------------------------------------
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

	//-----------------------------------------------------------------
    // save configuration data
	delete config;

	delete m_printData;
    delete m_pageSetupData;

    return wxApp::OnExit();
}
