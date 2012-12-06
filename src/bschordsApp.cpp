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
    bschordsFrame* frame = new bschordsFrame(0L, _("wxWidgets Application Template"));
    
    frame->Show();
    
    return true;
}
