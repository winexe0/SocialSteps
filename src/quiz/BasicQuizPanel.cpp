#include "BasicQuizPanel.h"
#include "../App.h"
#include <fstream>
#include "../rc/social_skills_unique_new_basic.h"
#include "../external/json.hpp"
#include <wx/graphics.h>
void BasicQuizPanel::ApplyTheme() {
    wxColour bgColor1(245, 250, 255); // Light pastel blue
    wxColour bgColor2(230, 240, 255); // Slightly deeper blue
    wxColour accentColor(60, 130, 250); // Friendly vibrant blue
    wxColour textColor(30, 30, 60);

    // Gradient background
    Bind(wxEVT_PAINT, [=](wxPaintEvent&) {
        wxPaintDC dc(this);
        wxSize size = GetClientSize();
        wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
        if (gc) {
            wxGraphicsGradientStops stops(bgColor1, bgColor2);
            wxGraphicsPath path = gc->CreatePath();
            path.AddRectangle(0, 0, size.x, size.y);
            gc->SetBrush(gc->CreateLinearGradientBrush(0, 0, 0, size.y, stops));
            gc->FillPath(path);
            delete gc;
        }
    });

    // Apply colors and font styles
    wxFont friendlyFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Segoe UI");
    SetBackgroundColour(bgColor1);
    SetForegroundColour(textColor);

    questionText->SetFont(friendlyFont.Bold());
    resultText->SetFont(friendlyFont);
    explanationText->SetFont(friendlyFont);

    // The buttons — rounded look with color
    auto styleButton = [&](wxButton* btn) {
        btn->SetBackgroundColour(accentColor);
        btn->SetForegroundColour(*wxWHITE);
        btn->SetFont(friendlyFont.Bold());
        btn->SetWindowStyleFlag(wxBORDER_NONE);
        btn->Bind(wxEVT_ENTER_WINDOW, [=](wxMouseEvent&) {
            btn->SetBackgroundColour(wxColour(90, 150, 255));
            btn->Refresh();
        });
        btn->Bind(wxEVT_LEAVE_WINDOW, [=](wxMouseEvent&) {
            btn->SetBackgroundColour(accentColor);
            btn->Refresh();
        });
    };

    styleButton(submitButton);
    styleButton(tryAgainButton);
    styleButton(explainButton);

    // Radio buttons: increase spacing, better legibility
    for (int i = 0; i < 4; ++i) {
        choiceButtons[i]->SetFont(friendlyFont);
        choiceButtons[i]->SetForegroundColour(textColor);
    }
}
BasicQuizPanel::BasicQuizPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY) {
    vbox = new wxBoxSizer(wxVERTICAL);

    questionText = new wxStaticText(this, wxID_ANY, "Loading...");
    questionText->Wrap(500); // Set a suitable width for wrapping
    wxFont baseFont = questionText->GetFont();
    baseFont.SetPointSize(12); // Consistent font size for all elements
    questionText->SetFont(baseFont);
    vbox->Add(questionText, 0, wxALL | wxEXPAND, 10);

    tryAgainButton = new wxButton(this, wxID_ANY, "More Questions");
    tryAgainButton->SetFont(baseFont);
    tryAgainButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        currentQuestionIndex = 0;
        score = 0;
        LoadQuestions();
        tryAgainButton->Hide();
        submitButton->Show();
        resultText->Show();
        explanationText->Show();
        resultText->SetLabel("");
        explanationText->SetLabel("");
        ShowNextQuestion(dummyEvent);
        Layout();
    });
    tryAgainButton->Hide();
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->AddStretchSpacer(1);
    hbox->Add(tryAgainButton, 0, wxALL | wxALIGN_CENTER, 10);
    hbox->AddStretchSpacer(1);
    vbox->Add(hbox, 0, wxEXPAND);

    for (int i = 0; i < 4; ++i) {
        choiceButtons[i] = new wxRadioButton(this, 1000 + i, "", wxDefaultPosition, wxDefaultSize, i == 0 ? wxRB_GROUP : 0);
        choiceButtons[i]->SetFont(baseFont);
        vbox->Add(choiceButtons[i], 0, wxLEFT | wxRIGHT | wxTOP, 15);
    }

    submitButton = new wxButton(this, wxID_ANY, "Submit");
    submitButton->SetFont(baseFont);
    vbox->Add(submitButton, 0, wxALL | wxALIGN_CENTER, 10);

    resultText = new wxStaticText(this, wxID_ANY, "");
    resultText->Wrap(500);
    resultText->SetFont(baseFont);
    vbox->Add(resultText, 0, wxALL | wxEXPAND, 10);

    explanationText = new wxStaticText(this, wxID_ANY, "");
    explanationText->Wrap(500);
    explanationText->SetFont(baseFont);
    vbox->Add(explanationText, 0, wxALL | wxEXPAND, 10);
    explainButton = new wxButton(this, wxID_ANY, "Explain More");
    explainButton->Hide();
    vbox->Add(explainButton, 0, wxALL, 10);
    explainButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        MyFrame* frame = dynamic_cast<MyFrame*>(wxGetTopLevelParent(this));
        if (frame && frame->chatbotPanel && frame->chatbotPanel->inputCtrl) {
            if (frame->chatbotPanel->newBtn->IsEnabled()) {
                frame->chatbotPanel->inputCtrl->SetValue("Using the Explain More button, from the basic quiz, explain the correct answer and why: " + questionText->GetLabel() + ". The choices are A. " + choiceButtons[0]->GetLabel() + ", B. " + choiceButtons[1]->GetLabel() + ", C. " + choiceButtons[2]->GetLabel() + ", and D. " + choiceButtons[3]->GetLabel() + " The answer selected was: " + choiceButtons[selectedIndex]->GetLabel());
                frame->chatbotPanel->OnSend(dummyEvent);
                frame->notebook->SetSelection(1); // Switch to Chatbot tab
            }
            else {
                frame->notebook->SetSelection(1); // Switch to Chatbot tab
                wxMessageBox("It looks like someone is already explaining a question to you. Please wait for the person to finish explaining your previous question and then click the Explain More button. Thank You!", "Please Wait", wxOK | wxICON_INFORMATION);
            }
        }
    });
    SetSizer(vbox);
    ApplyTheme();
    LoadQuestions();
    ShowNextQuestion(dummyEvent);

    submitButton->Bind(wxEVT_BUTTON, &BasicQuizPanel::OnSubmit, this);
}

void BasicQuizPanel::LoadQuestions() {
    // Parse JSON using nlohmann::json
    nlohmann::json root = nlohmann::json::parse(std::string(BasicjsonData));

    allQuestions.clear();
    for (const auto& item : root) {
        QuizQuestion q;
        q.context = item.value("context", "");
        q.question = item.value("question", "");
        for (const auto& c : item["choices"]) {
            q.choices.push_back(c.get<std::string>());
        }
        std::shuffle(q.choices.begin(), q.choices.end(), std::mt19937(std::random_device()()));
        q.answer = item.value("answer", "");
        q.explanation = item.value("explanation", "");
        allQuestions.push_back(q);
    }

    // Shuffle and pick 20
    std::shuffle(allQuestions.begin(), allQuestions.end(), std::mt19937(std::random_device()()));
    quizQuestions.assign(allQuestions.begin(), allQuestions.begin() + 20);
}

void BasicQuizPanel::ShowNextQuestion(wxCommandEvent& event) {
    if (currentQuestionIndex >= quizQuestions.size()) {
        // Compute percentage score
        double pct = (double)score / quizQuestions.size() * 100.0;

        // Tailored guidance based on score
        wxString guidance;
        if (pct >= 90) {
            guidance = "🌟 Excellent! You really understand these social skills! "
                       "Try the Advanced Quiz next to challenge yourself a bit more.";
        } else if (pct >= 70) {
            guidance = "👍 Good job! You're getting the hang of it. "
                       "You could review some Social Skills lessons to improve even further.";
        } else if (pct >= 50) {
            guidance = "🙂 Not bad! You got some answers right but could use more practice. "
                       "Try revisiting the lessons or asking the chatbot for help.";
        } else {
            guidance = "⚠️ That was tricky. Don't worry! "
                       "You can learn a lot by reviewing the Social Skills section or chatting with the bot.";
        }

        // Show final score and guidance
        questionText->SetLabel(wxString::Format(
            "Quiz complete!\nYour score: %d / %lu\n\n%s", 
            score, quizQuestions.size(), guidance
        ));
        questionText->Wrap(500); // Wrap text to fit panel

        // Hide other elements
        for (auto* btn : choiceButtons) btn->Hide();
        submitButton->Hide();
        explainButton->Hide();
        resultText->Hide();
        explanationText->Hide();
        tryAgainButton->Show();
        Layout();
        return;
    }

    // Existing code for showing the next question
    submitButton->SetLabel("Submit");
    submitButton->Unbind(wxEVT_BUTTON, &BasicQuizPanel::ShowNextQuestion, this);
    QuizQuestion& q = quizQuestions[currentQuestionIndex];
    questionText->SetLabel(wxString::Format("Q%d: %s", (int)(currentQuestionIndex + 1), q.context + "\n\n" + q.question));
    questionText->Wrap(500); 
    Layout();

    for (int i = 0; i < 4; ++i) {
        if (i < q.choices.size()) {
            choiceButtons[i]->SetLabel(q.choices[i]);
            choiceButtons[i]->SetValue(false);
            choiceButtons[i]->Show();
            wxSize best = choiceButtons[i]->GetBestSize();
            best.SetWidth(2000);
            choiceButtons[i]->SetMinSize(best);
        } else {
            choiceButtons[i]->Hide();
        }
    }
    resultText->SetLabel("");
    explanationText->SetLabel("");
    explainButton->Hide();
    if (currentQuestionIndex >= 0 && dynamic_cast<MyFrame*>(wxGetTopLevelParent(this))->basicQuizPanelCount >= 1) {
        SpeakText(questionText->GetLabel() + "Is it, A. " + choiceButtons[0]->GetLabel() + " or, B. " + choiceButtons[1]->GetLabel() + " or, C. " + choiceButtons[2]->GetLabel() + " or, D. " + choiceButtons[3]->GetLabel() + ". Please select the best answer and press the Submit Button.");
    }
    Layout();
}

void BasicQuizPanel::OnSubmit(wxCommandEvent& event) {
    if (currentQuestionIndex >= quizQuestions.size())
        return;

    QuizQuestion& q = quizQuestions[currentQuestionIndex];
    for (int i = 0; i < 4; ++i) {
        if (choiceButtons[i]->GetValue()) {
            selectedIndex = i;
            break;
        }
    }

    if (selectedIndex == -1) {
        wxMessageBox("Please select an answer.", "Notice", wxOK | wxICON_INFORMATION);
        return;
    }

    if (choiceButtons[selectedIndex]->GetLabel() == q.answer){
        resultText->SetLabel("Correct!");
        score++;
    }
    else {
        resultText->SetLabel("Incorrect! The correct answer was: " + q.answer);
    }
    resultText->Wrap(500); // Wrap the result text

    explanationText->SetLabel("Explanation: " + q.explanation);
    explanationText->Wrap(500); // Wrap the explanation text
    explainButton->Show();
    Layout();
    SpeakText(resultText->GetLabel() + ". " + explanationText->GetLabel());
    currentQuestionIndex++;
    submitButton->SetLabel("Next Question");
    submitButton->Fit();
    submitButton->Bind(wxEVT_BUTTON, &BasicQuizPanel::ShowNextQuestion, this);
}
