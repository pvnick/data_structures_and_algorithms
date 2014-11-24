/***************************************************************
 * Name:      wxhwApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2014-09-25
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "wxhwApp.h"
#include "wxhwMain.h"

IMPLEMENT_APP(wxhwApp);

bool wxhwApp::OnInit()
{
    
    wxhwDialog* dlg = new wxhwDialog(0L, _("wxWidgets Application Template"));
    
    dlg->Show();
    return true;
}
