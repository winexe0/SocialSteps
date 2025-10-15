#include "LandingPanel.h"
#include "App.h"
extern wxString name;
enum
{
    ID_CHATBOT = wxID_HIGHEST + 1,
    ID_FAQ,
    ID_SOCIALSKILLS,
    ID_BASICQUIZ,
    ID_ADVANCEDQUIZ,
    ID_OPENENDEDQUIZ
};

LandingPanel::LandingPanel(wxWindow* parent)
    : wxPanel(parent)
{
    SetBackgroundColour(wxColour(245, 250, 255));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title
    wxStaticText* title = new wxStaticText(this, wxID_ANY, "🌟 Welcome to SocialSteps, " + name + "!");
    title->SetFont(wxFontInfo(28).Bold());
    title->SetForegroundColour(wxColour(40, 80, 180));
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxTOP, 40);

    // Subtitle
    wxStaticText* subtitle = new wxStaticText(this, wxID_ANY,
        "Helping kids learn real-world social skills through guided lessons, practice, and friendly conversation.");
    subtitle->SetFont(wxFontInfo(14));
    subtitle->SetForegroundColour(wxColour(70, 70, 70));
    subtitle->Wrap(700);
    mainSizer->Add(subtitle, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);

    // Explanation text
    wxStaticText* message = new wxStaticText(this, wxID_ANY,
        "SocialSteps is a safe and encouraging space to practice how we connect with others. "
        "Each section in this app helps you grow — whether you're learning, practicing, or just talking things through.\n\n"
        "Choose where you'd like to begin:");
    message->SetFont(wxFontInfo(12));
    message->Wrap(700);
    mainSizer->Add(message, 0, wxALIGN_CENTER | wxALL, 10);

    // Grid of navigation buttons
    wxGridSizer* buttonSizer = new wxGridSizer(2, 3, 15, 15);

    auto makeButton = [&](const wxString& label, const wxString& desc, int id) {
        wxPanel* card = new wxPanel(this, wxID_ANY);
        card->SetBackgroundColour(wxColour(255, 255, 255));
        card->SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* cardSizer = new wxBoxSizer(wxVERTICAL);

        wxButton* btn = new wxButton(card, id, label);
        btn->SetFont(wxFontInfo(12).Bold());
        btn->SetBackgroundColour(wxColour(220, 235, 255));
        btn->SetForegroundColour(wxColour(0, 60, 120));

        wxStaticText* descText = new wxStaticText(card, wxID_ANY, desc);
        descText->SetFont(wxFontInfo(10));
        descText->Wrap(200);
        descText->SetForegroundColour(wxColour(60, 60, 60));

        cardSizer->Add(btn, 0, wxEXPAND | wxALL, 5);
        cardSizer->Add(descText, 1, wxEXPAND | wxALL, 5);
        card->SetSizer(cardSizer);

        btn->Bind(wxEVT_BUTTON, &LandingPanel::OnNavigate, this);
        return card;
    };

    buttonSizer->Add(makeButton("💬 Someone to Talk To",
        "Talk to someone who listens, explains social concepts, and offers emotional support whenever you need it.",
        ID_CHATBOT));
    buttonSizer->Add(makeButton("❓ Commonly Asked Questions",
        "Browse easy-to-understand answers to everyday social questions, like how to join groups or calm down when upset.",
        ID_FAQ));
    buttonSizer->Add(makeButton("🌈 Social Skills",
        "Learn core skills like empathy, active listening, and teamwork — with examples and tips for real-life practice.",
        ID_SOCIALSKILLS));
    buttonSizer->Add(makeButton("🧩 Basic Quiz",
        "Simple, friendly questions that help you remember key ideas. Perfect for gentle practice.",
        ID_BASICQUIZ));
    buttonSizer->Add(makeButton("🧠 Advanced Quiz",
        "More challenging scenarios that help you think deeply about social reasoning and emotions.",
        ID_ADVANCEDQUIZ));
    buttonSizer->Add(makeButton("🗣️ Open-Ended Quiz",
        "Write your own answers to real-life situations, then get thoughtful feedback on how you did.",
        ID_OPENENDEDQUIZ));

    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 20);

    SetSizer(mainSizer);
    SpeakText("Hello! I'm your SocialSteps coach. SocialSteps is a safe and encouraging space to practice how we connect with others. Each section in this app helps you grow — whether you're learning, practicing, or just talking things through. Take a look at the sections and let's begin!");
}

void LandingPanel::OnNavigate(wxCommandEvent& event)
{
    int pageIndex = 0;

    switch (event.GetId()) {
        case ID_CHATBOT: pageIndex = 1; break;
        case ID_FAQ: pageIndex = 2; break;
        case ID_SOCIALSKILLS: pageIndex = 3; break;
        case ID_BASICQUIZ: pageIndex = 4; break;
        case ID_ADVANCEDQUIZ: pageIndex = 5; break;
        case ID_OPENENDEDQUIZ: pageIndex = 6; break;
        default: return;
    }

    MyFrame* frame = dynamic_cast<MyFrame*>(wxGetTopLevelParent(this));
    if (frame && frame->notebook) {
        frame->notebook->SetSelection(pageIndex); 
    };
}
