#ifndef CHATBOTPANEL_H
#define CHATBOTPANEL_H

#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <wx/mstream.h>
#include <wx/textfile.h>
#include "../external/json.hpp"
#include "AudioRecorder.h"
#include "../rc/hourglass_not_done.h"
using json = nlohmann::json;
class ChatbotPanel : public wxPanel {
public:
    ChatbotPanel(wxWindow* parent);
    wxTextCtrl* inputCtrl;
    wxButton *newBtn;
    void OnSend(wxCommandEvent& event);
private:
    wxRichTextCtrl* outputCtrl;
    int chatCount = 0;
    void QueryChatbot(const wxString& input);
    static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
    std::vector<json> chatHistory;
    void AppendMarkdown(wxRichTextCtrl* ctrl, const wxString& markdown);
    void CheckIfTextIsEmpty(wxCommandEvent& event);
    std::string streamingBuffer;
    std::string streamingAssistantReply;
    std::thread curlThread;
    ~ChatbotPanel();
    long assistantStartPos = -1; // Marks where the streamed assistant reply starts
    wxButton *saveBtn;
    wxButton *loadBtn;
    wxButton *sendBtn;
    wxTimer restTimer;
    int timeRemaining = 0;
    long lastPosition = -1;
    bool isResting = false;
    void OnRestTimer(wxTimerEvent& event);
    void StartRestPeriod();
    void UpdateRestMessage();
    enum {
        ID_SEND_BUTTON = wxID_HIGHEST + 1,
        ID_LOAD_BUTTON = wxID_HIGHEST + 3,
        ID_SAVE_BUTTON = wxID_HIGHEST + 2,
        ID_NEW_BUTTON = wxID_HIGHEST + 4
    };
    void OnSave(wxCommandEvent& event);
    void OnLoad(wxCommandEvent& event);
    void OnNewChat(wxCommandEvent& event);
    wxTimer typingTimer;
    wxString typingDots;
    long typingPos = -1; // position where animation starts
    bool isTyping = false;
    void OnTypingTimer(wxTimerEvent& event);
    std::string speechBuffer;
    std::mutex speechMutex;
    bool isSpeaking = false;
    void OnPaint(wxPaintEvent& event);
    std::thread whisperThread;
    std::atomic<bool> listening{false};
    wxButton* voiceBtn = nullptr;
    void OnVoiceInput(wxCommandEvent& event);
    AudioRecorder recorder;
    wxButton* readAloudBtn = nullptr;
};
#endif
