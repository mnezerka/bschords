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


#include "bschordsApp.h"
#include "bschordsMain.h"

IMPLEMENT_APP(bschordsApp);

bool bschordsApp::OnInit()
{
	config = new wxConfig(_("BSChords"));
	config->Read(_("xxx"));

    bschordsFrame* frame = new bschordsFrame(0L, _("BSChords"));

    frame->Show();

    return true;
}

int bschordsApp::OnExit()
{
	delete config;

	return 0;
}
