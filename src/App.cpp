#include "App.h"
#include "ProfileSelection.h"
#include "FAQPanel.h"
#include "SpeakText.h"
#include <fstream>
#include <thread>
#ifdef __WXMSW__
#elif __WXOSX__
#else
#include <piper.h>
piper_synthesizer *speech = nullptr;
#endif
bool MyApp::OnInit() {
    #ifdef __WXMSW__
    #include "../rc/wsay.h"
    std::ofstream f("wsay.exe", std::ios::binary);
    f.write((char*)wsay_exe, wsay_exe_len);
    f.close();
    #elif __WXOSX__
    #else
    speech = piper_create("/home/aryan/Downloads/en_US-lessac-medium.onnx", "/home/aryan/Downloads/en_US-lessac-medium.onnx.json", "/usr/lib/x86_64-linux-gnu/espeak-ng-data");
    #endif
    ProfileSelection* profileselection = new ProfileSelection();
    profileselection->Show();
    return true;
}
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "SocialSteps", wxDefaultPosition, wxSize(800, 600)) {
        notebook = new wxNotebook(this, wxID_ANY);
        landingPanel = new LandingPanel(notebook);
        faqPanel = new FAQPanel(notebook);
        socialSkillsPanel = new SocialSkillsPanel(notebook);
        //videosPanel = new VideosPanel(notebook);
        basicquizPanel = new BasicQuizPanel(notebook);
        advancedquizPanel = new AdvancedQuizPanel(notebook);
        openEndedQuizPanel = new OpenEndedQuizPanel(notebook);
        chatbotPanel = new ChatbotPanel(notebook);
        notebook->AddPage(landingPanel, "Welcome");
        notebook->AddPage(chatbotPanel, "💬 Someone To Talk To");
        notebook->AddPage(faqPanel, "❓ Commonly Asked Social Skills Questions");
        notebook->AddPage(socialSkillsPanel, "🌈 Social Skills");
        //notebook->AddPage(videosPanel, "Videos");
        notebook->AddPage(basicquizPanel, "🧩 Basic Quiz");
        notebook->AddPage(advancedquizPanel, "🧠 Advanced Quiz");
        notebook->AddPage(openEndedQuizPanel, "🗣️ Open-Ended Quiz");
        notebook->Bind(wxEVT_BOOKCTRL_PAGE_CHANGED, &MyFrame::OnPageChanged, this);
}
void SetBackgroundColourIncludingAllChildren(wxWindow* w, const wxColour& clr)
{
    // Skip categoryChoice itself
    if (w == FAQPanel::categoryChoice)
        return;
    w->SetBackgroundColour(clr);
    const wxWindowList& children = w->GetChildren();
    for (wxWindowList::compatibility_iterator iterator = children.GetFirst(); iterator; iterator = iterator->GetNext())
        SetBackgroundColourIncludingAllChildren(dynamic_cast<wxWindow*>(iterator->GetData()), clr);
}
void MyFrame::OnPageChanged(wxBookCtrlEvent& event)
    {
        int newSelection = event.GetSelection();
        wxWindow* selectedPage = notebook->GetPage(newSelection);
        if (selectedPage == faqPanel) {
            faqPanelCount++;
        }
        if (selectedPage == faqPanel && faqPanelCount == 1) {
            SpeakText("Welcome to the commonly asked social skills section! You can browse through some common questions on social skills. "
              "Select a question from the list or search for a specific topic. If you have a question that isn't listed, feel free to ask me in the 'Someone To Talk To' section.");
        }
        if (selectedPage == socialSkillsPanel)
        {
            socialSkillsPanelCount++;
        }
        if (selectedPage == socialSkillsPanel && socialSkillsPanelCount == 1) {
            SpeakText("Welcome to the Social Skills Panel! Here you can explore various social skills and their descriptions.");
        }
        if (selectedPage == basicquizPanel) {
            basicQuizPanelCount++;
        }
        if (selectedPage == basicquizPanel && basicQuizPanelCount == 1) {
            SpeakText(basicquizPanel->questionText->GetLabel() + " Is it, A. " + basicquizPanel->choiceButtons[0]->GetLabel() + " or, B. " + basicquizPanel->choiceButtons[1]->GetLabel() + " or, C. " + basicquizPanel->choiceButtons[2]->GetLabel() + " or, D. " + basicquizPanel->choiceButtons[3]->GetLabel() + ". Please select the best answer and press the Submit Button.");
        }
        if (selectedPage == advancedquizPanel) {
            advancedQuizPanelCount++;
        }
        if (selectedPage == advancedquizPanel && advancedQuizPanelCount == 1) {
            SpeakText(advancedquizPanel->questionText->GetLabel() + " Is it, A. " + advancedquizPanel->choiceButtons[0]->GetLabel() + " or, B. " + advancedquizPanel->choiceButtons[1]->GetLabel() + " or, C. " + advancedquizPanel->choiceButtons[2]->GetLabel() + " or, D. " + advancedquizPanel->choiceButtons[3]->GetLabel() + ". Please select the best answer and press the Submit Button.");
        }
        if (selectedPage == openEndedQuizPanel){
            openEndedQuizPanelCount++;
        }
        if (selectedPage == openEndedQuizPanel && openEndedQuizPanelCount == 1) {
            SpeakText("Please answer the question: " + openEndedQuizPanel->questionText->GetLabel() + ". Press the Submit Button when you are done and feedback will be provided below.");
            openEndedQuizPanel->questionTimer->Start(1000); // 1 second interval
        }
        if (chatbotPanel == selectedPage) {
            chatbotPanelCount++;
        }
        if (chatbotPanel == selectedPage && chatbotPanelCount == 1) {
            SpeakText("Hello, ask me anything about social skills or just talk to me about anything you would like.\nBy the way, after every ten questions, I need to rest for 10 minutes.");
        }
        // Always call event.Skip() if you don’t fully handle it
        event.Skip();
    }
