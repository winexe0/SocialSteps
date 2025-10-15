#pragma once

#include <wx/wx.h>
#include "../external/json.hpp"
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/richtext/richtextctrl.h>
#include <vector>
#include <string>
#include <random>
class OpenEndedQuizPanel : public wxPanel {
public:
    OpenEndedQuizPanel(wxWindow* parent);
    wxStaticText* questionText;
    wxStaticText* resultText;
    wxRichTextCtrl* evaluationText;
    int score;
    bool scoreChecked = false;
    wxTimer* questionTimer = nullptr;
private:
    void LoadQuestions();
    void OnSubmit(wxCommandEvent& event);
    void ShowQuestion();
    void OnSpeak(wxCommandEvent& event);
    void OnRestart(wxCommandEvent&);
    void OnNextQuestion(wxCommandEvent&);
    void OnNext(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    std::vector<nlohmann::json> messages;
    size_t currentQuestionIndex = 0;
    wxTextCtrl* answerBox;
    wxButton* speakButton;
    wxButton* readAloudBtn;
    wxButton* submitBtn;
    wxButton* nextBtn;
    wxCommandEvent dummyEvent;
    wxButton* tryAgainButton;
    wxBoxSizer* vbox;
    int timeLeft = 20;
    wxStaticText* timerText = nullptr;
    wxStaticText* micHint = nullptr;
};
static size_t StreamCallback(void* contents, size_t size, size_t nmemb, void* userp);
