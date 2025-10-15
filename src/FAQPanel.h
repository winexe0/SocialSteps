#pragma once
#include <vector>
#include <tuple>
#include <set>
#include <map>
#include <random>
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/stdpaths.h>
#include <wx/statline.h>
#include <wx/mstream.h>
#include <wx/process.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include "SpeechBubblePanel.h"
struct Bubble {
    wxPoint2DDouble pos;
    double radius;
    double speed; // vertical speed
    wxColour color;
};
class FAQPanel : public wxPanel {
public:
    FAQPanel(wxWindow* parent);
    static wxChoice* categoryChoice;
private:
    wxTextCtrl* searchCtrl = nullptr;
    wxListBox* questionList = nullptr;
    wxTextCtrl* answerBox = nullptr;
    SpeechBubblePanel* leftSpeechBubble;
    SpeechBubblePanel* rightSpeechBubble;

    // Store FAQs as vector of (category, question, answer)
    std::vector<std::tuple<wxString, wxString, wxString>> faqs;

    // Current filtered FAQs shown in list
    std::vector<size_t> filteredIndexes;

    void LoadFAQsFromJSON();
    void UpdateQuestionList();
    
    void OnQuestionSelected(wxCommandEvent& event);
    void OnSearchText(wxCommandEvent& event);
    void OnCategoryChanged(wxCommandEvent& event);
    void LoadExpressions(const wxString& path);
    void SpeakIntroText(wxProcessEvent& event);
    wxString DetectEmotionCategory(const wxString& question);
    wxStaticBitmap* characterLeftImage;
    wxStaticBitmap* characterRightImage;
    SpeechBubblePanel* characterLeftSpeech;
    SpeechBubblePanel* characterRightSpeech;
    wxBitmap expressions[6];
    std::map<wxString, wxColour> categoryColors = {
    {"All Categories",              wxColour(255, 255, 255)},        // White (default)
    {"Assertiveness Skills",       wxColour(255, 235, 238)},        // Soft rose
    {"Communication Skills",       wxColour(227, 242, 253)},        // Pale blue
    {"Cultural Awareness",         wxColour(240, 244, 195)},        // Light lime
    {"Digital Communication",      wxColour(224, 242, 241)},        // Teal mint
    {"Emotional Expression",       wxColour(255, 236, 179)},        // Soft orange-yellow
    {"Emotional Regulation",       wxColour(200, 230, 201)},        // Mint green
    {"Friendship Skills",          wxColour(248, 187, 208)},        // Soft pink
    {"Group Skills",               wxColour(197, 202, 233)},        // Lavender blue
    {"IEP Goal Bank",              wxColour(232, 245, 233)},        // Soft green
    {"Listening Skills",           wxColour(225, 245, 254)},        // Light sky blue
    {"Nonverbal Communication",    wxColour(255, 249, 196)},        // Pale yellow
    {"Relationship Skills",        wxColour(255, 224, 178)},        // Soft peach
    {"Self Advocacy",              wxColour(255, 205, 210)},        // Light coral
    {"Self Confidence",            wxColour(255, 213, 79)},         // Amber yellow
    {"Social Awareness",           wxColour(200, 230, 201)},        // Mint green
    {"Social Etiquette",           wxColour(255, 236, 179)},        // Soft orange
    {"Social Play and Emotional Development", wxColour(244, 143, 177)}, // Rosy pink
    {"Social Problem Solving",     wxColour(207, 216, 220)}         // Cool gray-blue
    };
    std::vector<Bubble> bubbles;
    wxTimer bubbleTimer;
    std::mt19937 rng;
    std::uniform_real_distribution<double> distX;
    std::uniform_real_distribution<double> distRadius;
    std::uniform_real_distribution<double> distSpeed;
    std::uniform_int_distribution<int> distColor;
    void OnBubbleTimer(wxTimerEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void InitializeBubbles();
    std::vector<wxRect> GetOccupiedRects() const;
    wxPoint2DDouble GetRandomBubblePosition();
    bool bubblesInitialized = false;
};
