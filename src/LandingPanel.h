#ifndef LANDINGPANEL_H
#define LANDINGPANEL_H

#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/notebook.h>

class LandingPanel : public wxPanel
{
public:
    LandingPanel(wxWindow* parent);
private:
    void OnNavigate(wxCommandEvent& event);
};

#endif
