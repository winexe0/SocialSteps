#include "OpenEndedQuizPanel.h"
#include "../rc/social_skills_open_ended_questions.h"
#include "../AudioRecorder.h"
#include "../SpeakText.h"
#include "../App.h"
#include <curl/curl.h>
#include <../external/json.hpp>
#include <fstream>
#include <random>
#include <string>
#include <wx/wx.h>
#include <thread>
// Constructor
extern wxString name, age, likes, strengths, improve;
OpenEndedQuizPanel::OpenEndedQuizPanel(wxWindow* parent)
    : wxPanel(parent), currentQuestionIndex(0)
{
    // ====== THEME ======
    SetBackgroundColour(wxColour(245, 250, 255)); // soft background blue

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Header banner
    wxPanel* headerPanel = new wxPanel(this);
    headerPanel->SetBackgroundColour(wxColour(120, 180, 255)); // blue gradient tone
    wxBoxSizer* headerSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* headerTitle = new wxStaticText(headerPanel, wxID_ANY, "🧠 Open-Ended Social Skills Quiz");
    headerTitle->SetFont(wxFont(18, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    headerTitle->SetForegroundColour(*wxWHITE);
    headerSizer->Add(headerTitle, 0, wxALL | wxALIGN_CENTER, 10);
    headerPanel->SetSizer(headerSizer);

    mainSizer->Add(headerPanel, 0, wxEXPAND | wxALL, 5);

    // ===== Rounded question box =====
    wxPanel* questionPanel = new wxPanel(this, wxID_ANY);
    questionPanel->SetBackgroundColour(wxColour(255, 255, 255)); // white card
    questionPanel->SetWindowStyle(wxBORDER_SIMPLE);

    wxBoxSizer* qSizer = new wxBoxSizer(wxVERTICAL);
    questionText = new wxStaticText(questionPanel, wxID_ANY, "");
    questionText->SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    questionText->Wrap(600);
    qSizer->Add(questionText, 0, wxALL | wxEXPAND, 10);
    questionPanel->SetSizer(qSizer);

    mainSizer->Add(questionPanel, 0, wxALL | wxEXPAND, 10);

    // ===== Answer input =====
    answerBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    answerBox->SetHint("💬 Type your response here...");
    answerBox->SetBackgroundColour(wxColour(255, 255, 245));
    mainSizer->Add(answerBox, 1, wxALL | wxEXPAND, 10);

    // ===== Feedback area =====
    evaluationText = new wxRichTextCtrl(this, wxID_ANY,
        "Feedback will be shown here after submission.",
        wxDefaultPosition, wxDefaultSize,
        wxTE_READONLY | wxTE_MULTILINE | wxBORDER_THEME);
    evaluationText->SetBackgroundColour(wxColour(250, 250, 255));
    evaluationText->SetFont(wxFont(11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    mainSizer->Add(evaluationText, 1, wxALL | wxEXPAND, 10);

    // ===== Buttons =====
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    speakButton = new wxButton(this, wxID_ANY, "🎙 Speak Answer");
    readAloudBtn = new wxButton(this, wxID_ANY, "🔊 Read Feedback Aloud");
    submitBtn = new wxButton(this, wxID_ANY, "Submit ✅");
    nextBtn = new wxButton(this, wxID_ANY, "Next ➡️");
    nextBtn->Hide();
    readAloudBtn->Fit();
    readAloudBtn->Disable();
    submitBtn->SetToolTip("If you are finished with your answer early, click here to submit it and get feedback.");
    submitBtn->SetBackgroundColour(wxColour(0, 150, 255));
    submitBtn->SetForegroundColour(*wxWHITE);
    nextBtn->SetBackgroundColour(wxColour(0, 200, 120));
    nextBtn->SetForegroundColour(*wxWHITE);
    btnSizer->Add(speakButton, 0, wxALL, 5);
    btnSizer->Add(readAloudBtn, 0, wxALL, 5);
    btnSizer->Add(submitBtn, 0, wxALL, 5);
    btnSizer->Add(nextBtn, 0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER);

    // ===== Timer =====
    timerText = new wxStaticText(this, wxID_ANY, "Time left: 30 seconds");
    timerText->SetForegroundColour(wxColour(80, 80, 80));
    mainSizer->Add(timerText, 0, wxALIGN_CENTER | wxBOTTOM, 10);
    micHint = new wxStaticText(this, wxID_ANY, "Tip: Click 🎙 Speak Answer to say your response out loud!");
    micHint->SetForegroundColour(wxColour(100, 100, 100));
    mainSizer->Add(micHint, 0, wxALIGN_CENTER | wxBOTTOM, 5);
    SetSizer(mainSizer);

    // Bind buttons
    speakButton->Bind(wxEVT_BUTTON, &OpenEndedQuizPanel::OnSpeak, this);
    readAloudBtn->Bind(wxEVT_BUTTON, [&] (wxCommandEvent& event) {
        if (readAloudBtn->GetLabel() == "🛑 Stop Reading Aloud") {
            readAloudBtn->SetLabel("🔊 Read Latest Response Aloud");
            SpeakText("");
            return;
        }
        readAloudBtn->SetLabel("🛑 Stop Reading Aloud");
        SpeakText(evaluationText->GetValue());
        readAloudBtn->SetLabel("🔊 Read Latest Response Aloud");
    });
    submitBtn->Bind(wxEVT_BUTTON, &OpenEndedQuizPanel::OnSubmit, this);
    nextBtn->Bind(wxEVT_BUTTON, &OpenEndedQuizPanel::OnNext, this);

    // Load questions and display first one
    LoadQuestions();
    ShowQuestion();
}


// Load questions from TXT
void OpenEndedQuizPanel::LoadQuestions()
{
    score = 0;
    std::shuffle(OpenEndedQuestions.begin(), OpenEndedQuestions.end(),
                 std::mt19937(std::random_device()()));
}

// Show current question
void OpenEndedQuizPanel::ShowQuestion() {
    if (currentQuestionIndex < 5) {
        questionText->SetLabel(wxString::Format(
            "Q%d: %s",
            (int)(currentQuestionIndex + 1),
            OpenEndedQuestions[currentQuestionIndex]
        ));
        answerBox->Clear();
        answerBox->Enable();
        evaluationText->Clear();
        evaluationText->WriteText("Feedback will be provided here after you submit your answer.");
        submitBtn->SetToolTip("If you are finished with your answer early, click here to submit it and get feedback.");
        readAloudBtn->SetToolTip("Please submit an answer to receive feedback.");
        readAloudBtn->Disable();
        speakButton->Show();
        speakButton->Enable();
        submitBtn->Enable();
        submitBtn->Show();
        nextBtn->Hide();

        // Timer UI
        if (!timerText) {
            timerText = new wxStaticText(this, wxID_ANY, "");
            GetSizer()->Insert(1, timerText, 0, wxALL | wxALIGN_CENTER, 5);
        }
        timeLeft = 30;
        timerText->SetForegroundColour(*wxBLACK);
        timerText->SetLabel(wxString::Format("Time left: %d seconds", timeLeft));
        timerText->Show();

        // Timer logic
        if (!questionTimer) {
            questionTimer = new wxTimer(this, wxID_ANY);
            Bind(wxEVT_TIMER, &OpenEndedQuizPanel::OnTimer, this, questionTimer->GetId());
        }

        Layout();
    } else {
        questionText->SetLabel(wxString::Format("Quiz complete! Your score: %d / 5", score));
        answerBox->Hide();
        evaluationText->Hide();
        micHint->Hide();
        submitBtn->Hide();
        speakButton->Hide();
        readAloudBtn->Hide();
        nextBtn->Show();
        if (timerText) timerText->Hide();
        if (questionTimer) questionTimer->Stop();
        Layout();
    }
}

void OpenEndedQuizPanel::OnSubmit(wxCommandEvent&) {
    if (questionTimer) questionTimer->Stop();
    answerBox->Disable();
    speakButton->Disable();
    submitBtn->Disable();
    submitBtn->SetToolTip("Please wait for all feedback to be received.");
    readAloudBtn->SetToolTip("Please wait for all feedback to be received");
    scoreChecked = false; // Reset score check for new question
    wxString userAnswer = answerBox->GetValue();
    wxString question = OpenEndedQuestions[currentQuestionIndex];
    using json = nlohmann::json;
    json messages = {
    {
        {"role", "system"},
        {"content", 
            "Evaluate the user's answer for correctness AND politeness/social appropriateness. "
            "You are currently giving feedback to " + name + ", who is " + age + " years old. They like to " + likes + ". Their strengths include " + strengths + ". They want to improve on " + improve + ".\n\n"
            "Please acknowledge and incorporate this information about them into your responses to make them more personalized and relevant.\n\n"
            "Your response MUST ALWAYS start with either the exact word 'Correct' or the exact word 'Incorrect'. "
            "If the user's answer is rude, uses profanity, demands something, or is impolite in any way, start your response with 'Incorrect'. "
            "Only start with 'Correct' if the answer is both factually correct AND polite. "
            "After stating Correct/Incorrect, explain why the answer is correct or incorrect. "
            "Be supportive, point out strengths, and gently suggest improvements. "
            "Do not ask follow-up questions, the user cannot interact with you. "
            "YOU MUST ALWAYS address the user as 'You' and use a friendly, encouraging tone. "
            "Also give a letter grade for the answer (A, B, C, D, or F). "
            "If the answer is correct and polite, give an A. If it is incorrect but polite, give a B or C depending on how close it was to being correct. "
            "If it is rude or impolite, give a D or F depending on how rude it was. "
            "If the answer was left blank, then mark the answer as 'Incorrect', but DO NOT suggest that the user try again or retry the question. "
            "There is a strict 30-second timer for each question to simulate real-world time pressure in conversations. "
            "Spelling or grammatical mistakes may occur because the timer can end before the user fully completes their response. "
            "Do not penalize the user for small spelling or grammatical errors caused by this timer constraint."
        }
    },
    {
        {"role", "user"},
        {"content", "Question: " + std::string(question.mb_str()) + "\nUser's Answer: " + std::string(userAnswer.mb_str())}
    }
    };
    // Prepare JSON payload
    json payload = {
    {"messages", messages},
    {"stream", true},
    {"stop", {"<|im_end|>", "<im_start>", "<im_end>", "<|im_start|>", "<|fim_end|>", "<fim_start|>", "|<|im_end|", "<|im_end", "|<|im_end|"}},
    {"model", "gpt-3.5-turbo"},
    {"temperature", 0.7},
    {"max_tokens", 500}
    };
    evaluationText->Clear();
    evaluationText->AppendText("Feedback:\n");

    // Start background thread to avoid blocking UI
    std::thread([this, payload]() {
        CURL* curl = curl_easy_init();
        if (!curl) return;

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8080/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        std::string payloadStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Streaming callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            wxTheApp->CallAfter([this]() {
                evaluationText->AppendText("\nError: could not contact model.");
            });
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        // After streaming ends, swap buttons
        wxTheApp->CallAfter([this]() {
            readAloudBtn->SetToolTip("Click to have your feedback read aloud.");
            readAloudBtn->Enable();
            submitBtn->Hide();
            nextBtn->Show();
            Layout();
        });
    }).detach();
}


// Next question handler
void OpenEndedQuizPanel::OnNext(wxCommandEvent&) {
    if (currentQuestionIndex >= 5) {
        std::shuffle(OpenEndedQuestions.begin(), OpenEndedQuestions.end(),
            std::mt19937(std::random_device()()));
        currentQuestionIndex = 0;
        score = 0;
        answerBox->Show();
        speakButton->Show();
        speakButton->Enable();
        submitBtn->Enable();
        submitBtn->Show();
        //tryAgainButton->Hide();
        evaluationText->Clear();
        evaluationText->Show();
        micHint->Show();
        ShowQuestion();
        return;
    }
    if (currentQuestionIndex < 5) {
        currentQuestionIndex++;
        ShowQuestion();
        questionTimer->Start(1000); // Restart timer for next question
    }
    if (currentQuestionIndex >= 0 && dynamic_cast<MyFrame*>(wxGetTopLevelParent(this))->openEndedQuizPanelCount >= 1) {
        SpeakText("Please answer the question: " + questionText->GetLabel() + ". Press the Submit Button when you are done and feedback will be provided below.");
    }
}
// Restart quiz
void OpenEndedQuizPanel::OnRestart(wxCommandEvent&)
{
    currentQuestionIndex = 0;
    LoadQuestions();
    answerBox->Show();
    speakButton->Show();
    submitBtn->Show();
    tryAgainButton->Hide();
    ShowQuestion();
}

// Speak button handler
void OpenEndedQuizPanel::OnSpeak(wxCommandEvent&) {
    if (timeLeft <= 0) {
        evaluationText->SetValue("⏱ Time is up — you can't record now.");
        return;
    }
    evaluationText->SetValue("🎤 Listening... you have " + std::to_string(timeLeft) + " seconds to answer.\n 👀 Look at the progress bar above to see the time remaining.");
    speakButton->Disable();
    submitBtn->Disable();
    submitBtn->SetToolTip("You can submit your answer once recording is complete.");

    // Create or show progress bar
    static wxGauge* progressBar = nullptr;
    if (!progressBar) {
        progressBar = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(250, 20));
        GetSizer()->Insert(2, progressBar, 0, wxALIGN_CENTER | wxALL, 5);
        Layout();
    }
    progressBar->SetValue(0);
    progressBar->Show();
    Layout();

    std::string audioFile = "/tmp/recorded.wav";
    int durationSec = timeLeft; // 🎯 use remaining quiz time

    // Thread for recording and progress updates
    std::thread([this, audioFile, durationSec]() {
        // Stop quiz timer during recording
        wxTheApp->CallAfter([this]() {
            if (questionTimer && questionTimer->IsRunning())
                questionTimer->Stop();
        });

        // Update progress bar every 100ms
        std::thread progressThread([durationSec]() {
            for (int i = 0; i <= durationSec * 10; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                int progress = (i * 100) / (durationSec * 10);
                wxTheApp->CallAfter([progress]() {
                    if (progressBar) progressBar->SetValue(progress);
                });
            }
        });
        AudioRecorder recorder;
        recorder.RecordToFile(audioFile, durationSec);
        progressThread.join();

        wxTheApp->CallAfter([this]() {
            if (progressBar) {
                progressBar->SetValue(100);
                progressBar->Hide();
            }
            evaluationText->SetValue("🎧 Processing your speech... please wait.");
            timerText->SetLabel("Time left: 0 seconds");
            timerText->SetForegroundColour(*wxRED);
            Layout();
        });

        // ===== Send audio to whisper.cpp server =====
        CURL* curl = curl_easy_init();
        if (!curl) return;

        std::string url = "http://127.0.0.1:8081/inference";
        struct curl_httppost* form = nullptr;
        struct curl_httppost* last = nullptr;

        curl_formadd(&form, &last,
                     CURLFORM_COPYNAME, "file",
                     CURLFORM_FILE, audioFile.c_str(),
                     CURLFORM_CONTENTTYPE, "audio/wav",
                     CURLFORM_END);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, form);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         +[](void* ptr, size_t size, size_t nmemb, void* userdata) {
                             std::string* resp = static_cast<std::string*>(userdata);
                             resp->append((char*)ptr, size * nmemb);
                             return size * nmemb;
                         });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_formfree(form);

        wxTheApp->CallAfter([this, res, response]() {
            speakButton->Enable();

            if (res == CURLE_OK) {
                try {
                    auto jsonResp = nlohmann::json::parse(response);
                    if (jsonResp.contains("text")) {
                        wxString transcript = wxString::FromUTF8(jsonResp["text"].get<std::string>());
                        answerBox->SetValue(transcript);
                        evaluationText->SetValue("✅ Speech recognized successfully! Submitting your answer...");
                        
                        // Auto-submit once transcription is ready
                        OnSubmit(dummyEvent);
                    } else {
                        evaluationText->SetValue("⚠️ Whisper.cpp response missing 'text' field.");
                    }
                } catch (...) {
                    evaluationText->SetValue("⚠️ Could not parse whisper.cpp response.");
                }
            } else {
                evaluationText->SetValue("❌ Error contacting whisper.cpp server.");
            }

            Layout();
        });
    }).detach();
}



static size_t StreamCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* panel = static_cast<OpenEndedQuizPanel*>(userp);
    std::string chunk((char*)contents, size * nmemb);

    static std::string buffer;
    buffer += chunk;

    size_t pos = 0;
    while ((pos = buffer.find('\n')) != std::string::npos) {
        std::string line = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);

        if (line.empty()) continue;

        // Remove "data: " prefix if present
        if (line.rfind("data: ", 0) == 0) line = line.substr(6);

        try {
            auto jsonChunk = nlohmann::json::parse(line);

            auto& choices = jsonChunk["choices"];
            if (!choices.is_array() || choices.empty()) continue;

            auto& delta = choices[0]["delta"];
            if (!delta.is_object() || !delta.contains("content")) continue;

            std::string token = delta["content"].is_null() ? "" : delta["content"].get<std::string>();
            if (token.empty()) continue;

            // Update UI on main thread
            wxTheApp->CallAfter([panel, token]() mutable {
                wxString wxToken = wxString::FromUTF8(token.c_str());
                panel->evaluationText->SetEditable(false);

                // Handle the first token that indicates correctness
                if (!panel->scoreChecked) {
                    if (wxToken.StartsWith("Correct")) {
                        wxRichTextAttr statusAttr;
                        statusAttr.SetTextColour(*wxGREEN);
                        panel->evaluationText->BeginStyle(statusAttr);
                        panel->evaluationText->WriteText("Correct");
                        panel->evaluationText->EndStyle();

                        panel->score++;
                        panel->scoreChecked = true;

                        wxString rest = wxToken.Mid(7); // skip "Correct"
                        if (!rest.IsEmpty()) {
                            wxRichTextAttr defAttr; defAttr.SetTextColour(*wxBLACK);
                            panel->evaluationText->BeginStyle(defAttr);
                            panel->evaluationText->WriteText(rest);
                            panel->evaluationText->EndStyle();
                        }
                        return;
                    }
                    else if (wxToken.StartsWith("Incorrect")) {
                        wxRichTextAttr statusAttr;
                        statusAttr.SetTextColour(*wxRED);
                        panel->evaluationText->BeginStyle(statusAttr);
                        panel->evaluationText->WriteText("Incorrect");
                        panel->evaluationText->EndStyle();

                        panel->scoreChecked = true;

                        wxString rest = wxToken.Mid(9); // skip "Incorrect"
                        if (!rest.IsEmpty()) {
                            wxRichTextAttr defAttr; defAttr.SetTextColour(*wxBLACK);
                            panel->evaluationText->BeginStyle(defAttr);
                            panel->evaluationText->WriteText(rest);
                            panel->evaluationText->EndStyle();
                        }
                        return;
                    }
                }

                // All other tokens in black
                if (!wxToken.IsEmpty()) {
                    wxRichTextAttr defAttr; defAttr.SetTextColour(*wxBLACK);
                    panel->evaluationText->BeginStyle(defAttr);
                    panel->evaluationText->WriteText(wxToken);
                    panel->evaluationText->EndStyle();
                }

                panel->evaluationText->ShowPosition(panel->evaluationText->GetLastPosition());
            });

        } catch (const std::exception& e) {
            continue; // skip partial JSON lines
        }
    }

    return size * nmemb;
}

void OpenEndedQuizPanel::OnTimer(wxTimerEvent&) {
    timeLeft--;
    if (timerText){
        if (timeLeft == 5) {
            timerText->SetForegroundColour(*wxRED);
        }
        timerText->SetLabel(wxString::Format("Time left: %d seconds", timeLeft));
    }
    if (timeLeft <= 0) {
        if (questionTimer) questionTimer->Stop();
        submitBtn->Disable();
        evaluationText->AppendText("\nTime's up!");
        OnSubmit(dummyEvent);
        Layout();
    }
}





