#include "ChatbotPanel.h"
#include "SpeakText.h"
#include <curl/curl.h>
#include "../external/json.hpp"
#include "AudioRecorder.h"
#include <wx/dcbuffer.h> 
#include <wx/graphics.h>
#include <sstream>
#include <iostream>
using json = nlohmann::json;
extern wxString name, age, likes, strengths, improve;
ChatbotPanel::ChatbotPanel(wxWindow* parent) : wxPanel(parent) {
    wxInitAllImageHandlers();
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &ChatbotPanel::OnPaint, this);
chatHistory.push_back({
    {"role", "system"},
    {"content", "You are a warm, thoughtful social coach who gives long, clear, and deeply explanatory answers using simple language with words that are not advanced about social skills. When appropriate, you also respond with creative or emotional support, like writing poems, lullabies, or gentle stories, especially when the user seems to be seeking comfort. For every question, explain the concept, provide a real-world example, describe its importance in social functioning, and offer a short takeaway tip — unless the user is asking for emotional help, in which case, prioritize empathy and comfort. Whenever possible, gently encourage the user to reflect on their own thoughts, feelings, and experiences, and help them build insight and confidence in navigating social situations.\n\n"
"You are currently speaking to " + name + ", who is " + age + " years old. They like to " + likes + ". Their strengths include " + strengths + ". They want to improve on " + improve + ".\n\n"
"Please acknowledge and incorporate this information about them into your responses to make them more personalized and relevant.\n\n"
"Additionally, you know the full contents of an app called SocialSteps, a safe and encouraging space to practice how we connect with others, and can guide users like a librarian. You know all the FAQ questions and the Social Skills section so that when someone asks about a social skill or scenario, you can answer and also suggest they look in the relevant FAQ or Social Skills topic for more details.\n\n"
"These are the commonly asked Social Skills questions with their category in the Commonly Asked Social Skills Questions section:\n"
"Social Play and Emotional Development:\n"
"How can you stay close to others in social situations without feeling overwhelmed?\n"
"What are some ways to observe and understand what people around you are doing during group activities?\n"
"How can playing alongside others with similar interests help you in social situations?\n"
"What are some benefits of imitating others' social behaviors or conversation styles?\n"
"How do you manage taking turns in conversations or group discussions?\n"
"What does playing or interacting associatively with others look like in social settings?\n"
"How can you respond when someone offers you something or asks a question in social situations?\n"
"What are some natural ways to greet people to start a conversation or interaction?\n"
"How can you join a social group or activity without feeling awkward or intrusive?\n"
"What are effective ways to invite others to join you in an activity or conversation?\n"
"How do you take turns during more structured social activities or games?\n"
"What are some polite ways to ask for toys, food, or materials during group activities?\n"
"How can you play cooperatively with others in pretend or role play situations?\n"
"What are some ways to talk about what you are doing during group activities to include others?\n"
"How do you organize play or social plans with friends or peers?\n"
"How can you follow another person's ideas during social activities while still contributing your own?\n"
"What are some ways to share or give up toys, food, or materials with peers during social situations?\n"
"Emotional Regulation:\n"
"How do you identify what you like and dislike in social situations or activities?\n"
"What are some ways to recognize and label your own emotions when you are feeling upset or happy?\n"
"How can you notice and interpret the emotions others are showing?\n"
"What does it mean to show empathy toward someone, and how can you do it?\n"
"What are some ways you can calm yourself when you feel upset or overwhelmed?\n"
"How do you accept comfort or help from others when you are feeling stressed?\n"
"How do you cope when things don't go as planned or when you make mistakes?\n"
"What steps can you take to solve a problem that happens during a social interaction?\n"
"What do you do when you're feeling overwhelmed?\n"
"How do you calm yourself when angry?\n"
"How do you express frustration in a healthy way?\n"
"What do you do if your feelings get hurt?\n"
"Group Skills:\n"
"How do you ask for help from adults or peers when you need it in social or group settings?\n"
"What are ways to participate when only one other person is around versus when several people are present?\n"
"How can you stay with a group and follow its routines or activities comfortably?\n"
"How do you ask to join a group activity?\n"
"Communication Skills:\n"
"How do you start a conversation naturally with someone new or in a casual setting?\n"
"What are ways to show you are listening and interested during a conversation without using words?\n"
"How do you ask questions to keep a conversation going and show interest?\n"
"What is a good way to give compliments to peers or friends sincerely?\n"
"How can you end a conversation politely when you need to leave?\n"
"How do you appropriately ask for help?\n"
"What should you do if someone is using something you want?\n"
"What is the right way to enter a group conversation?\n"
"How do you respond to a compliment?\n"
"How can you start a friendly conversation?\n"
"What's a respectful way to disagree?\n"
"Social Awareness:\n"
"What are some social rules to keep in mind when meeting someone for the first time?\n"
"How do you know when you are standing too close or too far away from someone in a conversation?\n"
"What does it mean when someone crosses their arms or avoids eye contact?\n"
"How can you tell when a conversation is ending or when it's time to change the topic?\n"
"What are some ways to stay calm and respectful when you disagree with someone?\n"
"How can you show respect for someone who has different interests or opinions than you?\n"
"How can you tell if someone is joking or being sarcastic?\n"
"What are signs that someone might be bullying you or someone else?\n"
"How do you ask for help if you or someone else is being bullied?\n"
"How can you tell if someone is uncomfortable?\n"
"How do you recognize a joke from an insult?\n"
"How do you know if someone wants to talk?\n"
"Friendship Skills:\n"
"What are some ways to start making new friends in school or social groups?\n"
"How do you keep a friendship strong over time?\n"
"What can you do if you have a disagreement with a friend?\n"
"How can you include others who might feel left out in your social group?\n"
"What does a healthy friendship look like?\n"
"What is an example of good sportsmanship?\n"
"How do you ask someone to be your friend?\n"
"Assertiveness Skills:\n"
"How do you ask for what you need or want in a respectful way?\n"
"What are some polite ways to say no when you don't want to do something?\n"
"How can you stand up to peer pressure in social situations?\n"
"How do you share your opinions in a group without interrupting others?\n"
"Self Advocacy:\n"
"What are your rights in social or educational settings?\n"
"How do you ask for accommodations or support that help you succeed socially or academically?\n"
"How can recognizing your strengths help you in social situations?\n"
"What are strategies for managing challenges you face in social settings?\n"
"What is self-advocacy?\n"
"Social Problem Solving:\n"
"How do you recognize when there is a problem in a social situation?\n"
"What are ways to think of solutions when a social problem arises?\n"
"How do you try out a solution to a social problem carefully?\n"
"How can you tell if a social problem solution worked well?\n"
"When should you ask for help if you can't solve a social problem on your own?\n"
"Emotional Expression:\n"
"How can you share your feelings with others in a way that they can understand?\n"
"What are ways to show that you understand how someone else is feeling?\n"
"What should you do if you feel very angry or sad and want to express it safely?\n"
"Relationship Skills:\n"
"How do you build trust in new or existing relationships?\n"
"Why is it important to respect others' privacy and personal information?\n"
"How can you offer help to a friend or peer in need without being intrusive?\n"
"What is a good way to apologize when you have hurt someone's feelings?\n"
"How do you accept an apology from someone else graciously?\n"
"Listening Skills:\n"
"What does it mean to listen actively during a conversation?\n"
"How can you ask for clarification if you don't understand something someone said?\n"
"How do you show someone that you understand what they are saying?\n"
"Why is it important to avoid distractions during conversations?\n"
"How do you show you're listening?\n"
"How do you practice active listening?\n"
"Nonverbal Communication:\n"
"How do you maintain appropriate eye contact without staring?\n"
"What are some facial expressions that show you are friendly and approachable?\n"
"How can gestures like waving or thumbs up help in communication?\n"
"What does an open body posture look like, and why is it helpful?\n"
"Social Etiquette:\n"
"What are some polite phrases to use in social situations?\n"
"What are basic table manners to follow when eating with others?\n"
"How do you use phones or devices politely when you are with others?\n"
"Why is it important to ask before using someone else's belongings?\n"
"What should you do when you arrive late?\n"
"What do you say when you're late?\n"
"Cultural Awareness:\n"
"How can learning about other cultures improve your social skills?\n"
"What does it mean to be open-minded about other people's beliefs and traditions?\n"
"Why is it important not to assume things about people based on stereotypes?\n"
"How do you ask questions about someone's culture without being rude?\n"
"Digital Communication:\n"
"What are some rules for staying safe when communicating online?\n"
"How do you use polite language when chatting or emailing online?\n"
"Why is it sometimes hard to understand tone in online messages?\n"
"What should you do if you receive a mean or upsetting message online?\n"
"Self Confidence:\n"
"What are ways to build confidence when talking with others?\n"
"How do you react positively when you make a social mistake?\n"
"What is positive self-talk, and how can it help you socially?\n"
"How do you ask for feedback to improve your social skills?\n"
"Why is it important to celebrate your social successes, big or small?\n"
"Respect and Boundaries:\n"
"What should you do if someone says 'stop'?\n"
"What do you do when someone says no?\n"
"What do you do if someone is being too loud near you?\n"
"What is personal space and why is it important?\n"
"Conversation Skills:\n"
"How do you end a conversation politely?\n"
"How do you know when it's your turn to talk?\n"
"What's a polite way to interrupt?\n"
"Empathy:\n"
"How do you show empathy?\n"
"What do you do if someone is crying?\n"
"Self-Improvement:\n"
"What should you do if you make a mistake?\n"
"Inclusion:\n"
"What can you do if someone is being left out?\n"
"Feedback Skills:\n"
"How do you accept feedback?\n"
"How do you deal with being corrected?\n"
"Gratitude:\n"
"What do you do if someone gives you a gift?\n"
"How can you show gratitude?\n"
"Bullying Prevention:\n"
"What should you do if you see someone being bullied?\n"
"Ethics:\n"
"What do you do if a friend breaks a rule?\n"
"Encouragement:\n"
"What can you say when someone shares good news?\n"
"Resilience:\n"
"What should you do if someone ignores you?\n"
"What can you do when you feel left out?\n"
"Forgiveness:\n"
"How do you respond when someone apologizes?\n"
"Teamwork:\n"
"What is one way to be a good team member?\n"
"How can you be more flexible in group work?\n"
"Conflict Resolution:\n"
"What is a good way to resolve a conflict?\n"
"What does it mean to compromise?\n"
"Kindness:\n"
"What is one way to show kindness?\n\n"
"Topics in the Social Skills Section include:\n"
"Active Listening, Emotional Regulation, Empathy, Non-Verbal Communication, Self-Awareness, Conflict Resolution, Adaptability, Perspective-Taking, Cooperation & Teamwork, Emotional Intelligence, Assertiveness, Building Rapport, Giving & Receiving Feedback, Patience, Humor, Trustworthiness, Clarity in Communication, Respectfulness, Questioning, Listening for Understanding, Encouragement, Building Consensus, Expressing Gratitude, Managing Social Anxiety, Negotiation, Time Management in Social Settings, Storytelling, Respecting Personal Space, Using Appropriate Tone and Volume, Networking, Mindfulness in Interaction, Showing Appreciation, Being Approachable, Social Perceptiveness, Offering Help, Accepting Differences, Managing Silence, Summarizing, Apologizing Effectively, Sharing Personal Stories, Providing Supportive Feedback, Expressing Opinions Respectfully, Giving Compliments, Following Social Norms, Using Humor Appropriately, Supporting Others' Success, Using Appropriate Eye Contact, Handling Criticism, Reading Group Dynamics, Sharing Credit, Expressing Needs Clearly, Celebrating Diversity, Giving Clear Instructions, Expressing Enthusiasm, Listening Without Judgment, Admitting Mistakes, Balancing Talking and Listening, Using Names, Maintaining Boundaries, Giving Encouragement, Clarifying Misunderstandings, Showing Appreciation for Differences, Being Present\n\n"
"You should continue to answer questions as before, but also suggest where in SocialSteps the user can explore further (Commonly Asked Social Skills Questions or their categories in the Commonly Asked Social Skills section or Social Skills topics in the Social Skills section) when relevant.\n\n"
"You also know that SocialSteps contains quizzes for practicing social skills:\n"
"In the Basic Quiz and the Advanced Quiz, there are multiple-choice questions with one correct answer. In the Basic Quiz and the Advanced Quiz, there is an Explain More! button that the user can click on to get a more detailed explanation from you about the correct answer and why the other options are incorrect. If the person selected the wrong answer, you should gently explain why the correct answer is right and why the user's choice was wrong. You should always be positive and encouraging, even if the user answers incorrectly. You should never make the user feel bad for getting a question wrong. You should also praise the user for getting questions right. You should always use simple language that is easy to understand.\n\n"
"Categories do not exist for each level of quiz, and the quiz have many problems with questions testing a multitude of social skills, but they can be grouped by difficulty:\n"
"- A section named Basic Quiz for users on the severe end of the autistic spectrum. Example: Q1: Aiden needs help tying his shoes. What should he do? A. Throw his shoes B. Run without shoes C. Cry loudly D. Ask a grownup\n"
"- A section named Advanced Quiz for more nuanced social reasoning. Example: Q7: Fiona looked at her friend and smiled after he told a funny story. Why did Fiona smile? A. To show she enjoyed the story B. To confuse her friend C. Because she wanted to change the topic? D. To hide that she was upset\n"
"- A section named Open-Ended Quiz that evaluate the user's answer and give feedback. Example: Someone is talking about a topic you know a lot about. How do you join in without taking over?\n\n"
"You should continue to answer questions as before (concept explanation, real-world example, importance, takeaway tip) but also suggest where in SocialSteps the user can explore further—Commonly Asked Social Skills Questions or one of its categories, but make sure to explicitly mention that it came from the Commonly Asked Social Skills Questions Section, Social Skills topic, or a relevant quiz—when appropriate. Always explain why the suggested section or quiz would be helpful."}});
    wxColour panelBg(250, 248, 255);        // very soft lilac background
    wxColour inputBg(255, 255, 255);
    wxColour textBg(255, 255, 255);
    wxColour textBorder(220, 220, 235);
    wxColour accent(130, 110, 250);         // calm violet accent
    wxColour btnBg(240, 238, 255);
    wxColour btnText(70, 60, 150);
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    outputCtrl = new wxRichTextCtrl(this, wxID_ANY, "👋 Hello! I'm your SocialSteps coach. Ask me about social skills, or just talk about how you feel.\n💬 After 10 questions, I'll rest for 10 minutes to recharge. During this time, you can utilize your newly learned social skills and interact with other people.\n",
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE | wxTE_READONLY | wxBORDER_SIMPLE);
    outputCtrl->SetBackgroundColour(textBg);
    outputCtrl->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    outputCtrl->SetMargins(wxPoint(10, 10));
    inputCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                               wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxBORDER_SIMPLE);
    inputCtrl->SetHint("Type your message here and press Enter to send");
    inputCtrl->SetBackgroundColour(inputBg);
    inputCtrl->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    auto makeButton = [&](const wxString& label, int id) {
        auto* b = new wxButton(this, id, label, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
        b->SetBackgroundColour(btnBg);
        b->SetForegroundColour(btnText);
        b->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_SEMIBOLD));
        b->SetWindowStyleFlag(wxBORDER_NONE);
        b->SetMinSize(wxSize(100, 30));
        return b;
    };
    saveBtn = makeButton("💾 Save", ID_SAVE_BUTTON);
    loadBtn = makeButton("📂 Load", ID_LOAD_BUTTON);
    newBtn  = makeButton("🆕 New", ID_NEW_BUTTON);
    sendBtn = makeButton("➡️ Send", ID_SEND_BUTTON);
    voiceBtn = makeButton("🎤 Voice", wxID_HIGHEST + 100);
    saveBtn->SetToolTip("Save the current conversation to a file.");
    loadBtn->SetToolTip("Contine an existing conversation from a file.");
    newBtn->SetToolTip("Start a new conversation, clearing the current conversation.");
    sendBtn->SetToolTip("Send your message to the person.");
    voiceBtn->SetToolTip("Press to say your message aloud.");
    readAloudBtn = makeButton("🔊 Read Latest Response Aloud", wxID_HIGHEST + 101);
    readAloudBtn->SetToolTip("Click to have the latest response read aloud.");
    readAloudBtn->SetMinSize(wxSize(220, 30));
    readAloudBtn->Fit();
    readAloudBtn->Disable();
    sendBtn->Disable();
    buttonSizer->Add(saveBtn, 0, wxALL, 5);
    buttonSizer->Add(loadBtn, 0, wxALL, 5);
    buttonSizer->Add(newBtn, 0, wxALL, 5);
    buttonSizer->Add(readAloudBtn, 0, wxALL, 5);
    buttonSizer->AddStretchSpacer(1); // Optional: adds space between buttons
    buttonSizer->Add(voiceBtn, 0, wxALL, 5);
    buttonSizer->Add(sendBtn, 0, wxALL, 5);
    sizer->Add(outputCtrl, 1, wxEXPAND | wxALL, 10);
    sizer->Add(inputCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    sizer->Add(buttonSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    saveBtn->Bind(wxEVT_BUTTON, &ChatbotPanel::OnSave, this, ID_SAVE_BUTTON);
    sendBtn->Bind(wxEVT_BUTTON, &ChatbotPanel::OnSend, this, ID_SEND_BUTTON);
    loadBtn->Bind(wxEVT_BUTTON, &ChatbotPanel::OnLoad, this, ID_LOAD_BUTTON);
    newBtn->Bind(wxEVT_BUTTON, &ChatbotPanel::OnNewChat, this, ID_NEW_BUTTON);
    readAloudBtn->Bind(wxEVT_BUTTON, [&] (wxCommandEvent& event) {
        if (readAloudBtn->GetLabel() == "🛑 Stop Reading Aloud") {
            readAloudBtn->SetLabel("🔊 Read Latest Response Aloud");
            SpeakText("");
            return;
        }
        readAloudBtn->SetLabel("🛑 Stop Reading Aloud");
        SpeakText(wxString::FromUTF8(streamingAssistantReply.c_str()));
        readAloudBtn->SetLabel("🔊 Read Latest Response Aloud");
    });
    voiceBtn->Bind(wxEVT_BUTTON, &ChatbotPanel::OnVoiceInput, this);
    Bind(wxEVT_TEXT_ENTER, &ChatbotPanel::OnSend, this);
    Bind(wxEVT_TEXT, &ChatbotPanel::CheckIfTextIsEmpty, this);
    restTimer.SetOwner(this);
    Bind(wxEVT_TIMER, &ChatbotPanel::OnRestTimer, this);
    SetSizer(sizer);
}
void ChatbotPanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetClientSize();

    wxColour top(245, 242, 255);
    wxColour bottom(220, 215, 255);

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (gc) {
        wxGraphicsGradientStops stops(top, bottom);
        wxGraphicsPath rect = gc->CreatePath();
        rect.AddRectangle(0, 0, size.GetWidth(), size.GetHeight());
        gc->SetBrush(gc->CreateLinearGradientBrush(0, 0, 0, size.GetHeight(), stops));
        gc->DrawPath(rect);
        delete gc;
    }
    event.Skip();
}
void ChatbotPanel::OnSend(wxCommandEvent& event) {
    if (chatCount >= 10) {
        StartRestPeriod();
        voiceBtn->Disable();
        sendBtn->Disable();
        newBtn->Disable();
        return;
        // Start wxTimer and say a message having the remaining time in another thread and once time complete remove timer and block
        // Once timer is done in the thread reset chatCount to zero and remove the time remaining
    }
    if (inputCtrl->GetValue().IsEmpty()) {
        sendBtn->Disable();
        return;
    }
    Unbind(wxEVT_TEXT_ENTER, &ChatbotPanel::OnSend, this);
    Unbind(wxEVT_TEXT, &ChatbotPanel::CheckIfTextIsEmpty, this);
    sendBtn->Disable();
    voiceBtn->Disable();
    newBtn->Disable();
    readAloudBtn->Disable();
    wxString input = inputCtrl->GetValue();
    if (input.IsEmpty()) return;

    chatHistory.push_back({
        {"role", "user"},
        {"content", input.ToStdString()}
    });

    outputCtrl->AppendText("\n> " + input + "\n");
    inputCtrl->Clear();

    // If a previous thread is running, join it first (optional)
    if (curlThread.joinable()) {
        curlThread.join();
    }

    // Run QueryChatbot in a separate thread
    curlThread = std::thread([this, input]() {
        QueryChatbot(input);
    });

    chatCount++;
}
void ChatbotPanel::QueryChatbot(const wxString& input) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8080/v1/chat/completions");

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Expect:"); // disables 100-continue
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        json requestJson = {
            {"model", "your-model-name"},
            {"messages", chatHistory},
            {"stream", true},
            {"stop", {"<|im_end|>", "<im_start>", "<im_end>", "<|im_start|>", "<|fim_end|>", "<fim_start|>"}}
        };

        std::string data = requestJson.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        // Reset buffers before new stream
        streamingAssistantReply.clear();
        wxTheApp->CallAfter([this]() {
            typingDots.clear();
            typingPos = outputCtrl->GetLastPosition();
            isTyping = true;
            typingTimer.SetOwner(this);
            Bind(wxEVT_TIMER, &ChatbotPanel::OnTypingTimer, this, typingTimer.GetId());
            typingTimer.Start(500); // every half second
        });
        streamingBuffer.clear();

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &ChatbotPanel::WriteCallback);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            wxTheApp->CallAfter([this]() {
                                    isTyping = false;
                                    typingTimer.Stop();
                                    outputCtrl->Freeze();
                                    outputCtrl->Remove(typingPos, outputCtrl->GetLastPosition());
                                    outputCtrl->Thaw();
            outputCtrl->AppendText("\nError contacting chatbot server.\n");
            Bind(wxEVT_TEXT_ENTER, &ChatbotPanel::OnSend, this);
            Bind(wxEVT_TEXT, &ChatbotPanel::CheckIfTextIsEmpty, this);
            if (!isResting) {
                sendBtn->Enable();
                newBtn->Enable();
            }
        });
        } else {
            if (streamingAssistantReply.size() == 0) {
                streamingAssistantReply = "I'm sorry, something went wrong. Can you please try again?";
            }
            wxString streamingAssistantReplyTemp(streamingAssistantReply);
            streamingAssistantReplyTemp.Replace("<im_start>", "");
            streamingAssistantReplyTemp.Replace("<im_end>", "");
            streamingAssistantReplyTemp.Replace("<|im_start|>", "");
            streamingAssistantReplyTemp.Replace("<|im_end|>", "");
            streamingAssistantReplyTemp.Replace("|<|im_end|", "");
            streamingAssistantReplyTemp.Replace("<|im_end|", "");
            streamingAssistantReplyTemp.Replace("|endoftext", "");
            streamingAssistantReplyTemp.Replace("|endoftext|", "");
            if (streamingAssistantReplyTemp.size() != 0) {
                streamingAssistantReply = streamingAssistantReplyTemp.ToStdString();
            }
    // Add to history for next round
        chatHistory.push_back({
        {"role", "assistant"},
        {"content", streamingAssistantReply}
        });

        // Replace streamed tokens with final markdown
        wxTheApp->CallAfter([this]() {
            outputCtrl->Freeze();

            if (assistantStartPos != -1) {
                outputCtrl->Remove(assistantStartPos, outputCtrl->GetLastPosition());
            }

            AppendMarkdown(outputCtrl, wxString::FromUTF8(streamingAssistantReply.c_str()));
            outputCtrl->WriteText("\n");

            outputCtrl->Thaw();

            assistantStartPos = -1;
        });
    }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    Bind(wxEVT_TEXT_ENTER, &ChatbotPanel::OnSend, this);
    Bind(wxEVT_TEXT, &ChatbotPanel::CheckIfTextIsEmpty, this);
    readAloudBtn->Enable();
    if (!isResting) {
        sendBtn->Enable();
        newBtn->Enable();
    }
}


size_t ChatbotPanel::WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    ChatbotPanel* panel = static_cast<ChatbotPanel*>(userdata);
    size_t totalSize = size * nmemb;

    panel->streamingBuffer.append(ptr, totalSize);

    size_t pos;
    while ((pos = panel->streamingBuffer.find("\n")) != std::string::npos) {
        std::string line = panel->streamingBuffer.substr(0, pos);
        panel->streamingBuffer.erase(0, pos + 1);

        if (line.rfind("data: ", 0) == 0) {
            std::string jsonData = line.substr(6);

            if (jsonData == "[DONE]") {
                break;
            }

            try {
                json j = json::parse(jsonData);
                if (j.contains("choices") && j["choices"].is_array()) {
                    auto delta = j["choices"][0]["delta"];
                    if (delta.contains("content") && delta["content"].is_string()) {
                        std::string token = delta["content"].get<std::string>();
                        panel->streamingAssistantReply += token;
                        panel->speechMutex.lock();
                        panel->speechBuffer += token;
                        panel->speechMutex.unlock();
                        wxString wxToken(token);
                        wxTheApp->CallAfter([panel, wxToken]() {
                            if (panel->assistantStartPos == -1) {
                                if (panel->isTyping) {
                                    panel->isTyping = false;
                                    panel->typingTimer.Stop();
                                    panel->outputCtrl->Freeze();
                                    panel->outputCtrl->Remove(panel->typingPos, panel->outputCtrl->GetLastPosition());
                                    panel->outputCtrl->Thaw();
                                }
                                panel->assistantStartPos = panel->outputCtrl->GetLastPosition();
                            }
                            panel->outputCtrl->AppendText(wxToken);
                            //SpeakText(wxToken); // <-- Wait for previous token to finish
                            panel->outputCtrl->ScrollIntoView(panel->outputCtrl->GetLastPosition(), WXK_PAGEDOWN);
                            std::string bufferCopy; {
                                std::lock_guard<std::mutex> lock(panel->speechMutex);
                                bufferCopy = panel->speechBuffer;
                            }
                            size_t pos = bufferCopy.find_first_of(".!?,");
                            if (pos != std::string::npos) {
                            // Look ahead to confirm end of sentence (space or end)
                                if (pos + 1 == bufferCopy.size() || isspace(bufferCopy[pos + 1])) {
                                    // Extract sentence including punctuation
                                    std::string sentenceToSpeak = bufferCopy.substr(0, pos + 1);

                                    // Remove it from buffer
                                    {
                                    std::lock_guard<std::mutex> lock(panel->speechMutex);
                                    panel->speechBuffer.erase(0, pos + 1);
                                    }

                                    // Speak sentence (make sure calls don't overlap!)
                                    if (!panel->isSpeaking) {
                                        panel->isSpeaking = true;
                                        // Run SpeakText in a separate thread to avoid UI freeze
                                        std::thread([panel, sentenceToSpeak]() {
                                            //SpeakText(wxString::FromUTF8(sentenceToSpeak.c_str()));
                                            panel->isSpeaking = false;
                                        }).detach();
                                    }
                                }
                            }
                        });
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Stream parse error: " << e.what() << std::endl;
            }
        }
    }

    return totalSize;
}


void ChatbotPanel::AppendMarkdown(wxRichTextCtrl* ctrl, const wxString& markdown) {
    wxTextAttr normalAttr;
    wxTextAttr boldAttr;
    wxTextAttr italicAttr;
    wxTextAttr codeAttr;

    boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    italicAttr.SetFontStyle(wxFONTSTYLE_ITALIC);
    codeAttr.SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    codeAttr.SetBackgroundColour(wxColour(240, 240, 240));  // optional background for block code
    int pos = 0;
    while (pos < markdown.Length()) {
        // Code block (``` ... ```)
        if (markdown.Mid(pos, 3) == "```") {
            pos += 3;
            int endCodeBlock = markdown.find("```", pos);
            if (endCodeBlock != wxNOT_FOUND) {
                ctrl->BeginStyle(codeAttr);
                ctrl->WriteText(markdown.Mid(pos, endCodeBlock - pos) + "\n");
                ctrl->EndStyle();
                pos = endCodeBlock + 3;
                continue;
            }
        }

        // Inline code (`code`)
        if (markdown[pos] == '`') {
            pos += 1;
            int endCode = markdown.find('`', pos);
            if (endCode != wxNOT_FOUND) {
                ctrl->BeginStyle(codeAttr);
                ctrl->WriteText(markdown.Mid(pos, endCode - pos));
                ctrl->EndStyle();
                pos = endCode + 1;
                continue;
            }
        }

        // Bold (**bold**)
        if (markdown.Mid(pos, 2) == "**") {
            pos += 2;
            int endBold = markdown.find("**", pos);
            if (endBold != wxNOT_FOUND) {
                ctrl->BeginStyle(boldAttr);
                ctrl->WriteText(markdown.Mid(pos, endBold - pos));
                ctrl->EndStyle();
                pos = endBold + 2;
                continue;
            }
        }

        // Italic (*italic*)
        if (markdown[pos] == '*') {
            pos += 1;
            int endItalic = markdown.find('*', pos);
            if (endItalic != wxNOT_FOUND) {
                ctrl->BeginStyle(italicAttr);
                ctrl->WriteText(markdown.Mid(pos, endItalic - pos));
                ctrl->EndStyle();
                pos = endItalic + 1;
                continue;
            }
        }

        // Regular text
        ctrl->BeginStyle(normalAttr);
        ctrl->WriteText(markdown.Mid(pos, 1));
        ctrl->EndStyle();
        pos++;
    }
}
ChatbotPanel::~ChatbotPanel() {
    if (curlThread.joinable()) {
        curlThread.join();
    }
}
void ChatbotPanel::CheckIfTextIsEmpty(wxCommandEvent& event) {
    if (inputCtrl->GetValue().IsEmpty()) {
        sendBtn->Disable();
    } else {
        if (!isResting) {
            sendBtn->Enable();
        }
    }
}
void ChatbotPanel::StartRestPeriod() {
    timeRemaining = 600;  // 10 minutes = 600 seconds
    restTimer.Start(1000); // 1 second interval
    isResting = true;
    sendBtn->Disable();
    voiceBtn->Disable();
    newBtn->Disable();
    inputCtrl->Disable();
    outputCtrl->AppendText("I'm resting now for 10 minutes. I'll let you know when I'm ready!\n");
    lastPosition = outputCtrl->GetLastPosition();
    UpdateRestMessage();
}
void ChatbotPanel::OnRestTimer(wxTimerEvent& event) {
    timeRemaining--;
    if (timeRemaining > 0) {
        UpdateRestMessage();
    } else {
        restTimer.Stop();
        chatCount = 0;
        lastPosition = -1; // Reset last position
        isResting = false;
        outputCtrl->AppendText("\nI'm back! You can ask me more questions now.\n");
        sendBtn->Enable();
        newBtn->Enable();
        inputCtrl->Enable();
    }
}
void ChatbotPanel::UpdateRestMessage() {
    wxMemoryInputStream hourglass_stream(hourglass_not_done_png, hourglass_not_done_png_len);
    wxImage hourglass_image(hourglass_stream, wxBITMAP_TYPE_PNG);
    wxBitmap hourglass_bitmap(hourglass_image);
    int minutes = timeRemaining / 60;
    int seconds = timeRemaining % 60;
    wxString message;
    message.Printf("Time remaining: %d:%02d\n", minutes, seconds);
    outputCtrl->Freeze();
    if (lastPosition != -1) {
        outputCtrl->Remove(lastPosition, outputCtrl->GetLastPosition());
    }
    lastPosition = outputCtrl->GetLastPosition();
    if (hourglass_bitmap.IsOk()) {
        outputCtrl->WriteImage(hourglass_bitmap);
    }
    outputCtrl->AppendText(message);
    outputCtrl->Thaw();
}
void ChatbotPanel::OnSave(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, "Save Chat", "", "chat.txt", "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled
    }
    wxString filePath = saveFileDialog.GetPath();
    
    if (!outputCtrl->SaveFile(filePath)) {
        wxLogError("Could not write to file '%s'.", filePath);
    } else {
        wxLogMessage("Chat saved successfully to '%s'.", filePath);
    }
}
void ChatbotPanel::OnTypingTimer(wxTimerEvent& event) {
    if (!isTyping) return;

    typingDots += ".";
    if (typingDots.length() > 3) typingDots.clear();

    outputCtrl->Freeze();
    outputCtrl->Remove(typingPos, outputCtrl->GetLastPosition());
    outputCtrl->AppendText(typingDots);
    outputCtrl->Thaw();
}
void ChatbotPanel::OnLoad(wxCommandEvent& event) {
    wxFileDialog openFileDialog(
        this,
        "Open Chat",
        "",
        "chat.txt",
        "Text files (*.txt)|*.txt",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled
    }

    wxString filePath = openFileDialog.GetPath();
    wxTextFile file;

    if (!file.Open(filePath)) {
        wxLogError("Could not open file '%s'.", filePath);
        return;
    }

    outputCtrl->Clear();
    chatHistory.clear();

    wxString currentRole;
    wxString currentContent;

    auto flushCurrent = [&](bool force = false) {
        if (!currentRole.IsEmpty() && (!currentContent.IsEmpty() || force)) {
            chatHistory.push_back({
                {"role", currentRole.ToStdString()},
                {"content", currentContent.ToStdString()}
            });
        }
        currentRole.clear();
        currentContent.clear();
    };

    for (size_t i = 0; i < file.GetLineCount(); ++i) {
        wxString line = file.GetLine(i);
        outputCtrl->AppendText(line + "\n");

        // Detect user messages
        if (line.StartsWith("> ")) {
            flushCurrent();
            currentRole = "user";
            currentContent = line.Mid(2); // Remove "> "
        }
        // Skip initial greeting lines from history
        else if (line.StartsWith("Hello, ask me anything") ||
                 line.StartsWith("By the way")) {
            continue;
        }
        // Assistant message (may be multi-line)
        else {
            if (currentRole != "assistant") {
                flushCurrent();
                currentRole = "assistant";
            }
            if (!currentContent.IsEmpty())
                currentContent += "\n";
            currentContent += line;
        }
    }

    // Flush last pending message
    flushCurrent();

    file.Close();
    wxLogMessage("Chat loaded successfully from '%s'.", filePath);
}
void ChatbotPanel::OnNewChat(wxCommandEvent& event) {
    if (curlThread.joinable()) {
        curlThread.join();
    }
    chatHistory.clear();
    chatHistory.push_back({
    {"role", "system"},
    {"content", "You are a warm, thoughtful social coach who gives long, clear, and deeply explanatory answers using simple language with words that are not advanced about social skills. When appropriate, you also respond with creative or emotional support, like writing poems, lullabies, or gentle stories, especially when the user seems to be seeking comfort. For every question, explain the concept, provide a real-world example, describe its importance in social functioning, and offer a short takeaway tip — unless the user is asking for emotional help, in which case, prioritize empathy and comfort. Whenever possible, gently encourage the user to reflect on their own thoughts, feelings, and experiences, and help them build insight and confidence in navigating social situations.\n\n"
"You are currently speaking to " + name + ", who is " + age + " years old. They like to " + likes + ". Their strengths include " + strengths + ". They want to improve on " + improve + ".\n\n"
"Please acknowledge and incorporate this information about them into your responses to make them more personalized and relevant.\n\n"
"Additionally, you know the full contents of an app called SocialSteps and can guide users like a librarian. You know all the FAQ questions and the Social Skills section so that when someone asks about a social skill or scenario, you can answer and also suggest they look in the relevant FAQ or Social Skills topic for more details.\n\n"
"These are the commonly asked Social Skills questions with their category in the Commonly Asked Social Skills Questions section:\n"
"Social Play and Emotional Development:\n"
"How can you stay close to others in social situations without feeling overwhelmed?\n"
"What are some ways to observe and understand what people around you are doing during group activities?\n"
"How can playing alongside others with similar interests help you in social situations?\n"
"What are some benefits of imitating others' social behaviors or conversation styles?\n"
"How do you manage taking turns in conversations or group discussions?\n"
"What does playing or interacting associatively with others look like in social settings?\n"
"How can you respond when someone offers you something or asks a question in social situations?\n"
"What are some natural ways to greet people to start a conversation or interaction?\n"
"How can you join a social group or activity without feeling awkward or intrusive?\n"
"What are effective ways to invite others to join you in an activity or conversation?\n"
"How do you take turns during more structured social activities or games?\n"
"What are some polite ways to ask for toys, food, or materials during group activities?\n"
"How can you play cooperatively with others in pretend or role play situations?\n"
"What are some ways to talk about what you are doing during group activities to include others?\n"
"How do you organize play or social plans with friends or peers?\n"
"How can you follow another person's ideas during social activities while still contributing your own?\n"
"What are some ways to share or give up toys, food, or materials with peers during social situations?\n"
"Emotional Regulation:\n"
"How do you identify what you like and dislike in social situations or activities?\n"
"What are some ways to recognize and label your own emotions when you are feeling upset or happy?\n"
"How can you notice and interpret the emotions others are showing?\n"
"What does it mean to show empathy toward someone, and how can you do it?\n"
"What are some ways you can calm yourself when you feel upset or overwhelmed?\n"
"How do you accept comfort or help from others when you are feeling stressed?\n"
"How do you cope when things don't go as planned or when you make mistakes?\n"
"What steps can you take to solve a problem that happens during a social interaction?\n"
"What do you do when you're feeling overwhelmed?\n"
"How do you calm yourself when angry?\n"
"How do you express frustration in a healthy way?\n"
"What do you do if your feelings get hurt?\n"
"Group Skills:\n"
"How do you ask for help from adults or peers when you need it in social or group settings?\n"
"What are ways to participate when only one other person is around versus when several people are present?\n"
"How can you stay with a group and follow its routines or activities comfortably?\n"
"How do you ask to join a group activity?\n"
"Communication Skills:\n"
"How do you start a conversation naturally with someone new or in a casual setting?\n"
"What are ways to show you are listening and interested during a conversation without using words?\n"
"How do you ask questions to keep a conversation going and show interest?\n"
"What is a good way to give compliments to peers or friends sincerely?\n"
"How can you end a conversation politely when you need to leave?\n"
"How do you appropriately ask for help?\n"
"What should you do if someone is using something you want?\n"
"What is the right way to enter a group conversation?\n"
"How do you respond to a compliment?\n"
"How can you start a friendly conversation?\n"
"What's a respectful way to disagree?\n"
"Social Awareness:\n"
"What are some social rules to keep in mind when meeting someone for the first time?\n"
"How do you know when you are standing too close or too far away from someone in a conversation?\n"
"What does it mean when someone crosses their arms or avoids eye contact?\n"
"How can you tell when a conversation is ending or when it's time to change the topic?\n"
"What are some ways to stay calm and respectful when you disagree with someone?\n"
"How can you show respect for someone who has different interests or opinions than you?\n"
"How can you tell if someone is joking or being sarcastic?\n"
"What are signs that someone might be bullying you or someone else?\n"
"How do you ask for help if you or someone else is being bullied?\n"
"How can you tell if someone is uncomfortable?\n"
"How do you recognize a joke from an insult?\n"
"How do you know if someone wants to talk?\n"
"Friendship Skills:\n"
"What are some ways to start making new friends in school or social groups?\n"
"How do you keep a friendship strong over time?\n"
"What can you do if you have a disagreement with a friend?\n"
"How can you include others who might feel left out in your social group?\n"
"What does a healthy friendship look like?\n"
"What is an example of good sportsmanship?\n"
"How do you ask someone to be your friend?\n"
"Assertiveness Skills:\n"
"How do you ask for what you need or want in a respectful way?\n"
"What are some polite ways to say no when you don't want to do something?\n"
"How can you stand up to peer pressure in social situations?\n"
"How do you share your opinions in a group without interrupting others?\n"
"Self Advocacy:\n"
"What are your rights in social or educational settings?\n"
"How do you ask for accommodations or support that help you succeed socially or academically?\n"
"How can recognizing your strengths help you in social situations?\n"
"What are strategies for managing challenges you face in social settings?\n"
"What is self-advocacy?\n"
"Social Problem Solving:\n"
"How do you recognize when there is a problem in a social situation?\n"
"What are ways to think of solutions when a social problem arises?\n"
"How do you try out a solution to a social problem carefully?\n"
"How can you tell if a social problem solution worked well?\n"
"When should you ask for help if you can't solve a social problem on your own?\n"
"Emotional Expression:\n"
"How can you share your feelings with others in a way that they can understand?\n"
"What are ways to show that you understand how someone else is feeling?\n"
"What should you do if you feel very angry or sad and want to express it safely?\n"
"Relationship Skills:\n"
"How do you build trust in new or existing relationships?\n"
"Why is it important to respect others' privacy and personal information?\n"
"How can you offer help to a friend or peer in need without being intrusive?\n"
"What is a good way to apologize when you have hurt someone's feelings?\n"
"How do you accept an apology from someone else graciously?\n"
"Listening Skills:\n"
"What does it mean to listen actively during a conversation?\n"
"How can you ask for clarification if you don't understand something someone said?\n"
"How do you show someone that you understand what they are saying?\n"
"Why is it important to avoid distractions during conversations?\n"
"How do you show you're listening?\n"
"How do you practice active listening?\n"
"Nonverbal Communication:\n"
"How do you maintain appropriate eye contact without staring?\n"
"What are some facial expressions that show you are friendly and approachable?\n"
"How can gestures like waving or thumbs up help in communication?\n"
"What does an open body posture look like, and why is it helpful?\n"
"Social Etiquette:\n"
"What are some polite phrases to use in social situations?\n"
"What are basic table manners to follow when eating with others?\n"
"How do you use phones or devices politely when you are with others?\n"
"Why is it important to ask before using someone else's belongings?\n"
"What should you do when you arrive late?\n"
"What do you say when you're late?\n"
"Cultural Awareness:\n"
"How can learning about other cultures improve your social skills?\n"
"What does it mean to be open-minded about other people's beliefs and traditions?\n"
"Why is it important not to assume things about people based on stereotypes?\n"
"How do you ask questions about someone's culture without being rude?\n"
"Digital Communication:\n"
"What are some rules for staying safe when communicating online?\n"
"How do you use polite language when chatting or emailing online?\n"
"Why is it sometimes hard to understand tone in online messages?\n"
"What should you do if you receive a mean or upsetting message online?\n"
"Self Confidence:\n"
"What are ways to build confidence when talking with others?\n"
"How do you react positively when you make a social mistake?\n"
"What is positive self-talk, and how can it help you socially?\n"
"How do you ask for feedback to improve your social skills?\n"
"Why is it important to celebrate your social successes, big or small?\n"
"Respect and Boundaries:\n"
"What should you do if someone says 'stop'?\n"
"What do you do when someone says no?\n"
"What do you do if someone is being too loud near you?\n"
"What is personal space and why is it important?\n"
"Conversation Skills:\n"
"How do you end a conversation politely?\n"
"How do you know when it's your turn to talk?\n"
"What's a polite way to interrupt?\n"
"Empathy:\n"
"How do you show empathy?\n"
"What do you do if someone is crying?\n"
"Self-Improvement:\n"
"What should you do if you make a mistake?\n"
"Inclusion:\n"
"What can you do if someone is being left out?\n"
"Feedback Skills:\n"
"How do you accept feedback?\n"
"How do you deal with being corrected?\n"
"Gratitude:\n"
"What do you do if someone gives you a gift?\n"
"How can you show gratitude?\n"
"Bullying Prevention:\n"
"What should you do if you see someone being bullied?\n"
"Ethics:\n"
"What do you do if a friend breaks a rule?\n"
"Encouragement:\n"
"What can you say when someone shares good news?\n"
"Resilience:\n"
"What should you do if someone ignores you?\n"
"What can you do when you feel left out?\n"
"Forgiveness:\n"
"How do you respond when someone apologizes?\n"
"Teamwork:\n"
"What is one way to be a good team member?\n"
"How can you be more flexible in group work?\n"
"Conflict Resolution:\n"
"What is a good way to resolve a conflict?\n"
"What does it mean to compromise?\n"
"Kindness:\n"
"What is one way to show kindness?\n\n"
"Topics in the Social Skills Section include:\n"
"Active Listening, Emotional Regulation, Empathy, Non-Verbal Communication, Self-Awareness, Conflict Resolution, Adaptability, Perspective-Taking, Cooperation & Teamwork, Emotional Intelligence, Assertiveness, Building Rapport, Giving & Receiving Feedback, Patience, Humor, Trustworthiness, Clarity in Communication, Respectfulness, Questioning, Listening for Understanding, Encouragement, Building Consensus, Expressing Gratitude, Managing Social Anxiety, Negotiation, Time Management in Social Settings, Storytelling, Respecting Personal Space, Using Appropriate Tone and Volume, Networking, Mindfulness in Interaction, Showing Appreciation, Being Approachable, Social Perceptiveness, Offering Help, Accepting Differences, Managing Silence, Summarizing, Apologizing Effectively, Sharing Personal Stories, Providing Supportive Feedback, Expressing Opinions Respectfully, Giving Compliments, Following Social Norms, Using Humor Appropriately, Supporting Others' Success, Using Appropriate Eye Contact, Handling Criticism, Reading Group Dynamics, Sharing Credit, Expressing Needs Clearly, Celebrating Diversity, Giving Clear Instructions, Expressing Enthusiasm, Listening Without Judgment, Admitting Mistakes, Balancing Talking and Listening, Using Names, Maintaining Boundaries, Giving Encouragement, Clarifying Misunderstandings, Showing Appreciation for Differences, Being Present\n\n"
"You should continue to answer questions as before, but also suggest where in SocialSteps the user can explore further (Commonly Asked Social Skills Questions or their categories in the Commonly Asked Social Skills section or Social Skills topics in the Social Skills section) when relevant.\n\n"
"You also know that SocialSteps contains quizzes for practicing social skills:\n"
"In the Basic Quiz and the Advanced Quiz, there are multiple-choice questions with one correct answer. In the Basic Quiz and the Advanced Quiz, there is an Explain More! button that the user can click on to get a more detailed explanation from you about the correct answer and why the other options are incorrect. If the person selected the wrong answer, you should gently explain why the correct answer is right and why the user's choice was wrong. You should always be positive and encouraging, even if the user answers incorrectly. You should never make the user feel bad for getting a question wrong. You should also praise the user for getting questions right. You should always use simple language that is easy to understand.\n\n"
"Categories do not exist for each level of quiz, and the quiz have many problems with questions testing a multitude of social skills, but they can be grouped by difficulty:\n"
"- A section named Basic Quiz for users on the severe end of the autistic spectrum. Example: Q1: Aiden needs help tying his shoes. What should he do? A. Throw his shoes B. Run without shoes C. Cry loudly D. Ask a grownup\n"
"- A section named Advanced Quiz for more nuanced social reasoning. Example: Q7: Fiona looked at her friend and smiled after he told a funny story. Why did Fiona smile? A. To show she enjoyed the story B. To confuse her friend C. Because she wanted to change the topic? D. To hide that she was upset\n"
"- A section named Open-Ended Quiz that evaluate the user's answer and give feedback. Example: Someone is talking about a topic you know a lot about. How do you join in without taking over?\n\n"
"You should continue to answer questions as before (concept explanation, real-world example, importance, takeaway tip) but also suggest where in SocialSteps the user can explore further—Commonly Asked Social Skills Questions or one of its categories, but make sure to explicitly mention that it came from the Commonly Asked Social Skills Questions Section, Social Skills topic, or a relevant quiz—when appropriate. Always explain why the suggested section or quiz would be helpful."}});
    outputCtrl->Clear();
    inputCtrl->Clear();
    streamingBuffer.clear();
    streamingAssistantReply.clear();
    assistantStartPos = -1;
    lastPosition = -1;
    timeRemaining = 0;
    outputCtrl->AppendText("👋 Hello! I'm your SocialSteps coach. Ask me about social skills, or just talk about how you feel.\n💬 After 10 questions, I'll rest for 10 minutes to recharge. During this time, you can utilize your newly learned social skills and interact with other people.\n");
}
void ChatbotPanel::OnVoiceInput(wxCommandEvent& event) {
    if (listening) {
        listening = false;
        voiceBtn->SetLabel("🎤 Voice");
        recorder.Stop();
        SpeakText("Stopped listening.");
        return;
    }

    listening = true;
    voiceBtn->SetLabel("🛑 Stop");
    SpeakText("Listening started.");
    std::string audioFile = "/tmp/recorded.wav";
    outputCtrl->AppendText("\n🎤 Listening for 1 minute...\nPress the 🛑 Stop button whenever you are done speaking");
    whisperThread = std::thread([this, audioFile]() {
        CURL* curl = curl_easy_init();
        if (!curl) return;

        const char* whisper_url = "http://127.0.0.1:8081/inference";
        // Record 5 seconds of audio from mic to temp file (replace with arecord on Linux)
        recorder.RecordToFile(audioFile, 60);
        outputCtrl->AppendText("\n🎧 Processing your speech... please wait.");
        // send to whisper.cpp
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");

        curl_mime* mime;
        curl_mimepart* part;
        mime = curl_mime_init(curl);

        part = curl_mime_addpart(mime);
        curl_mime_name(part, "file");
        curl_mime_filedata(part, audioFile.c_str());

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_URL, whisper_url);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
            +[](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
                std::string* r = static_cast<std::string*>(userdata);
                r->append(ptr, size * nmemb);
                return size * nmemb;
            });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK && !response.empty()) {
            wxTheApp->CallAfter([this, response]() {
                try {
                    auto j = json::parse(response);
                    if (j.contains("text")) {
                        wxString transcript = wxString::FromUTF8(j["text"].get<std::string>());
                        transcript.Replace("Listening started.", "");
                        inputCtrl->ChangeValue(transcript);
                        SpeakText("Got it. You said: " + transcript);
                        sendBtn->Enable();
                        // Optionally auto-send:
                        // wxCommandEvent dummy;
                        // OnSend(dummy);
                    }
                } catch (...) {
                    wxTheApp->CallAfter([this]() {
                        outputCtrl->AppendText("\n⚠️ Could not parse transcription.\n");
                    });
                }
            });
        } else {
            wxTheApp->CallAfter([this]() {
                outputCtrl->AppendText("\n❌ Whisper server error.\n");
            });
        }

        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        listening = false;
        wxTheApp->CallAfter([this]() { voiceBtn->SetLabel("🎤 Voice"); });
    });
    whisperThread.detach();
}
