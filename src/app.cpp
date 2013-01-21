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

#include "app.h"
#include "mainwnd.h"

using namespace bschords;

IMPLEMENT_APP(App);

bool App::OnInit()
{
	// load and initialize configuration data
	config = new wxConfig(_("BSChords"));

	m_editorFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);

	// load default stylesheet
	m_styleSheet.LoadFromConfig(config);

	// get editor font
	wxString editorFontNativeInfo;
	if (config->Read(_("global/editor-font"), &editorFontNativeInfo))
		m_editorFont.SetNativeFontInfo(editorFontNativeInfo);

	// get editor colors
	wxString c;
	if (config->Read(_("global/editor-color-text"), &c))
		m_editorColorText.Set(c);
	if (config->Read(_("global/editor-color-chords"), &c))
		m_editorColorChords.Set(c);
	if (config->Read(_("global/editor-color-commands"), &c))
		m_editorColorCommands.Set(c);

	//-----------------------------------------------------------------
	// initialize printing
	m_printData = new wxPrintData;
    // you could set an initial paper size here
	m_printData->SetPaperId(wxPAPER_A4); // for everyone else
    m_pageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*m_pageSetupData) = *m_printData;

	//-----------------------------------------------------------------
	// open main window (frame)
    MainWnd* frame = new MainWnd(0L, _("BSChords"));

    frame->Show();

    return true;
}

int App::OnExit()
{
	m_styleSheet.SaveToConfig(config);

	config->Write(_("global/editor-font"), m_editorFont.GetNativeFontInfoDesc());
	config->Write(_("global/editor-color-text"), m_editorColorText.GetAsString(wxC2S_HTML_SYNTAX));
	config->Write(_("global/editor-color-chords"), m_editorColorChords.GetAsString(wxC2S_HTML_SYNTAX));
	config->Write(_("global/editor-color-commands"), m_editorColorCommands.GetAsString(wxC2S_HTML_SYNTAX));

	//-----------------------------------------------------------------
    // save configuration data
	delete config;

	delete m_printData;
    delete m_pageSetupData;

    return wxApp::OnExit();
}
