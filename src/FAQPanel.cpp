#include "FAQPanel.h"
#include "SpeechBubblePanel.h"
#include "SpeakText.h"
#include "App.h"
#include "../rc/social_skills_questions_answers.h"
#include <set>
#include "../external/json.hpp"
#include "../rc/character.h"
wxChoice* FAQPanel::categoryChoice = nullptr;
FAQPanel::FAQPanel(wxWindow* parent) : wxPanel(parent), bubbleTimer(this) {
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    SetBackgroundColourIncludingAllChildren(this, wxColour(245, 250, 255));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    // Category filter dropdown
    categoryChoice = new wxChoice(this, wxID_ANY);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Filter by category:"), 0, wxALL, 5);
    sizer->Add(categoryChoice, 0, wxEXPAND | wxALL, 5);

    // Search box
    searchCtrl = new wxTextCtrl(this, wxID_ANY);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Search questions:"), 0, wxALL, 5);
    sizer->Add(searchCtrl, 0, wxEXPAND | wxALL, 5);

    // Question list
    questionList = new wxListBox(this, wxID_ANY);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Select a question:"), 0, wxALL, 5);
    sizer->Add(questionList, 1, wxEXPAND | wxALL, 5);

    // Cartoon character Q&A display
    auto* cartoonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* speechBubbleSizer = new wxBoxSizer(wxHORIZONTAL);

    // Load images
    wxInitAllImageHandlers();
    wxMemoryInputStream image_stream(character_png, character_png_len);
    wxImage character(image_stream, wxBITMAP_TYPE_PNG);
    wxBitmap characterBitmap(character);
    // Left character and speech bubble
    characterLeftImage = new wxStaticBitmap(this, wxID_ANY, characterBitmap);
    leftSpeechBubble = new SpeechBubblePanel(this, " Ask me anything! ", BubbleDirection::Left);
    cartoonSizer->Add(characterLeftImage, 1, wxALIGN_CENTER, 20);
    speechBubbleSizer->Add(leftSpeechBubble, 0, wxALIGN_TOP | wxLEFT, 0);
    sizer->Add(speechBubbleSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT, 400);
    sizer->Add(cartoonSizer, 0, wxALIGN_CENTER, 5);
    (wxWindow*)characterLeftImage->SetBackgroundColour(wxColour(255, 255, 255));
    (wxWindow*)leftSpeechBubble->SetBackgroundColour(wxColour(255, 255, 255)); // Set default background colour
    SetSizer(sizer);

    LoadFAQsFromJSON();

    // Populate category dropdown
    std::set<wxString> categories;
    for (const auto& [category, question, answer] : faqs) {
        categories.insert(category);
    }

    categoryChoice->Append("All Categories");
    for (const auto& cat : categories) {
        categoryChoice->Append(cat);
    }
    categoryChoice->SetSelection(0);

    UpdateQuestionList();
    // Bind events
    questionList->Bind(wxEVT_LISTBOX, &FAQPanel::OnQuestionSelected, this);
    searchCtrl->Bind(wxEVT_TEXT, &FAQPanel::OnSearchText, this);
    categoryChoice->Bind(wxEVT_CHOICE, &FAQPanel::OnCategoryChanged, this);
    rng.seed(std::random_device{}());
    distX = std::uniform_real_distribution<double>(0, GetClientSize().GetWidth());
    distRadius = std::uniform_real_distribution<double>(5, 20);
    distSpeed = std::uniform_real_distribution<double>(0.5, 2.0);
    distColor = std::uniform_int_distribution<int>(150, 255);

    // Start timer
    bubbleTimer.Start(30);
    Bind(wxEVT_TIMER, &FAQPanel::OnBubbleTimer, this);
    Bind(wxEVT_PAINT, &FAQPanel::OnPaint, this);
    bubblesInitialized = false; // Add as a member variable
    Bind(wxEVT_SIZE, &FAQPanel::OnSize, this);
}

void FAQPanel::LoadFAQsFromJSON() {
    nlohmann::json root;
    try {
        root = nlohmann::json::parse(std::string(questionsjsonData));
    } catch (const std::exception& e) {
        wxLogError("Failed to parse JSON: %s", e.what());
        return;
    }

    if (!root.is_array()) {
        wxLogError("JSON root is not an array.");
        return;
    }

    faqs.clear();

    for (const auto& item : root) {
        wxString category = wxString::FromUTF8(item.value("category", ""));
        wxString question = wxString::FromUTF8(item.value("question", ""));
        wxString answer = wxString::FromUTF8(item.value("answer", ""));
        faqs.emplace_back(category, question, answer);
    }

    // 🔹 Shuffle the questions
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(faqs.begin(), faqs.end(), g);
}


void FAQPanel::UpdateQuestionList() {
    questionList->Clear();
    filteredIndexes.clear();

    wxString filterText = searchCtrl->GetValue().Lower();
    wxString selectedCategory = categoryChoice->GetStringSelection();
    bool filterCategory = (selectedCategory != "All Categories");

    for (size_t i = 0; i < faqs.size(); ++i) {
        const auto& [category, question, answer] = faqs[i];
        if ((!filterCategory || category == selectedCategory) &&
            (filterText.IsEmpty() || question.Lower().Contains(filterText))) {
            questionList->Append(question);
            filteredIndexes.push_back(i);
        }
    }
}

void FAQPanel::OnQuestionSelected(wxCommandEvent& event) {
    int sel = questionList->GetSelection();
    if (sel == wxNOT_FOUND || sel >= static_cast<int>(filteredIndexes.size())) {
        leftSpeechBubble->SetText("Select a question, and I'll tell you the answer!");
        return;
    }
    size_t originalIndex = filteredIndexes[sel];
    const auto& [category, question, answer] = faqs[originalIndex];

    // Set background color to the question's category
    auto it = categoryColors.find(category);
    if (it != categoryColors.end()) {
        this->SetBackgroundColour(it->second);
        SetBackgroundColourIncludingAllChildren(this, it->second);
        this->Refresh();
    }
    SpeakText(question + "" + answer);
    leftSpeechBubble->SetText(answer + " ");
}

void FAQPanel::OnSearchText(wxCommandEvent& event) {
    UpdateQuestionList();
}

void FAQPanel::OnCategoryChanged(wxCommandEvent& event) {
    wxString selectedCategory = categoryChoice->GetStringSelection();
    auto it = categoryColors.find(selectedCategory);
    if (it != categoryColors.end()) {
        this->SetBackgroundColour(it->second);
        SetBackgroundColourIncludingAllChildren(this, it->second);
        this->Refresh(); // Force repaint
    }
    SpeakText(selectedCategory + ". Select a question, and I'll tell you the answer!");
    leftSpeechBubble->SetText("Select a question, and I'll tell you the answer!");
    UpdateQuestionList();
}
void FAQPanel::OnBubbleTimer(wxTimerEvent&) {
    int height = GetClientSize().GetHeight();

for (auto& b : bubbles) {
    // Move bubble up by speed
    b.pos.m_y -= b.speed;

    // If bubble is above the top, reset to bottom
    if (b.pos.m_y + b.radius < 0) {
        b.pos = GetRandomBubblePosition();
        b.pos.m_y = height + b.radius; // start just below panel
        b.radius = distRadius(rng);
        b.speed = distSpeed(rng);
        b.color = wxColour(distColor(rng), distColor(rng), 255, 150);
    }
}

    Refresh(); // trigger OnPaint
}
void FAQPanel::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    // Draw bubbles behind all controls
    for (const auto& b : bubbles) {
        gc->SetBrush(gc->CreateBrush(wxBrush(b.color)));
        gc->DrawEllipse(b.pos.m_x - b.radius, b.pos.m_y - b.radius, 2*b.radius, 2*b.radius);
    }


    delete gc;
}
std::vector<wxRect> FAQPanel::GetOccupiedRects() const {
    std::vector<wxRect> rects;

    rects.push_back(characterLeftImage->GetRect());
    rects.push_back(leftSpeechBubble->GetRect());
    rects.push_back(questionList->GetRect());
    rects.push_back(searchCtrl->GetRect());
    rects.push_back(categoryChoice->GetRect());

    // Add some padding if needed
    for (auto& r : rects) {
        r.Inflate(5, 5);
    }

    return rects;
}
wxPoint2DDouble FAQPanel::GetRandomBubblePosition() {
    int width = GetClientSize().GetWidth();
    int height = GetClientSize().GetHeight();
    auto occupied = GetOccupiedRects();

    wxPoint2DDouble pos;
    int attempts = 0;

    // Temporary uniform distributions
    std::uniform_real_distribution<double> distXFull(0, width);
    std::uniform_real_distribution<double> distYFull(distRadius(rng), height - distRadius(rng));

    do {
        pos.m_x = distXFull(rng);
        pos.m_y = distYFull(rng);

        attempts++;
        if (attempts > 100) break; // prevent infinite loop
    } while (std::any_of(occupied.begin(), occupied.end(),
                         [&](const wxRect& r){ return r.Contains(pos.m_x, pos.m_y); }));

    return pos;
}
void FAQPanel::OnSize(wxSizeEvent& event) {
    if (!bubblesInitialized) {
        InitializeBubbles();
        bubblesInitialized = true;
    }
    event.Skip(); // allow normal resizing
}
void FAQPanel::InitializeBubbles() {
    int width = GetClientSize().GetWidth();
    int height = GetClientSize().GetHeight();

    for (int i = 0; i < 20; ++i) {
        Bubble b;
        b.pos = GetRandomBubblePosition();
        b.radius = distRadius(rng);
        b.speed = distSpeed(rng);
        b.color = wxColour(distColor(rng), distColor(rng), 255, 150);
        bubbles.push_back(b);
    }
}