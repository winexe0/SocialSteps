#pragma once

#include <wx/wx.h>
#include "../external/json.hpp"
#include "quiz.h"
#include <wx/file.h>
#include <wx/filename.h>
#include <vector>
#include <string>
#include <random>
class AdvancedQuizPanel : public wxPanel {
public:
    AdvancedQuizPanel(wxWindow* parent);
    wxStaticText* questionText;
    wxRadioButton* choiceButtons[4];
    wxStaticText* explanationText;
    wxStaticText* resultText;
private:
    void LoadQuestions();
    void ShowNextQuestion(wxCommandEvent& event);
    void OnSubmit(wxCommandEvent& event);
    void ApplyTheme();
    std::vector<QuizQuestion> allQuestions;
    std::vector<QuizQuestion> quizQuestions;

    size_t currentQuestionIndex = 0;
    int score = 0;
    int selectedIndex = -1;
    wxButton* submitButton;
    wxCommandEvent dummyEvent;
    wxButton* tryAgainButton;
    wxButton* explainButton;
    wxBoxSizer* vbox;
};