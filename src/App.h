#ifndef APP_H
#define APP_H
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/process.h>
#include "ChatbotPanel.h"
#include "FAQPanel.h"
#include "SocialSkillsPanel.h"
#include "LandingPanel.h"
//#include "VideosPanel.h"
#include "./quiz/BasicQuizPanel.h"
#include "./quiz/AdvancedQuizPanel.h"
#include "./quiz/OpenEndedQuizPanel.h"
class MyFrame : public wxFrame {
public:
    MyFrame();
    int faqPanelCount = 0;
    int socialSkillsPanelCount = 0;
    int chatbotPanelCount = 0;
    int basicQuizPanelCount = 0;
    int advancedQuizPanelCount = 0;
    int openEndedQuizPanelCount = 0;
    wxNotebook* notebook;
    ChatbotPanel* chatbotPanel;
private:
    LandingPanel* landingPanel;
    FAQPanel* faqPanel;
    SocialSkillsPanel* socialSkillsPanel;
    BasicQuizPanel* basicquizPanel;
    AdvancedQuizPanel* advancedquizPanel;
    OpenEndedQuizPanel* openEndedQuizPanel;
    void OnPageChanged(wxBookCtrlEvent& event);
};
class MyApp : public wxApp {
public:
    virtual bool OnInit() override;
};
void SetBackgroundColourIncludingAllChildren(wxWindow* w, const wxColour& clr);
#endif
// APP_H