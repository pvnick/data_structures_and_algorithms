/***************************************************************
 * Name:      wxhwMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2014-09-25
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef WXHWMAIN_H
#define WXHWMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wxhwApp.h"


#include <wx/button.h>
#include <wx/statline.h>
class wxhwDialog: public wxDialog
{
    public:
        wxhwDialog(wxDialog *dlg, const wxString& title);
        ~wxhwDialog();

    protected:
        enum
        {
            idBtnQuit = 1000,
            idBtnAbout
        };
        wxStaticText* m_staticText1;
        wxButton* BtnAbout;
        wxStaticLine* m_staticline1;
        wxButton* BtnQuit;

    private:
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};

#endif // WXHWMAIN_H
