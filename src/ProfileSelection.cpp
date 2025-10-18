#include "ProfileSelection.h"
#include "App.h"
#include "../external/json.hpp"
#include "SpeakText.h"
#include <wx/statline.h>
#include <fstream>

using json = nlohmann::json;
wxString name, age, likes, strengths, improve;

ProfileSelection::ProfileSelection()
    : wxFrame(nullptr, wxID_ANY, "Welcome to SocialSteps", wxDefaultPosition, wxSize(900, 700))
{
    SetBackgroundColour(wxColour(245, 250, 255));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 🌟 Title
    wxStaticText* title = new wxStaticText(this, wxID_ANY, "🌟 Welcome to SocialSteps!");
    title->SetFont(wxFontInfo(28).Bold());
    title->SetForegroundColour(wxColour(40, 80, 180));
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxTOP, 25);

    // Subtitle
    wxStaticText* subtitle = new wxStaticText(this, wxID_ANY,
        "Before we start, let's set up your profile so we can personalize your experience.");
    subtitle->SetFont(wxFontInfo(14));
    subtitle->SetForegroundColour(wxColour(70, 70, 70));
    subtitle->Wrap(750);
    mainSizer->Add(subtitle, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    // 🧍 Profile Box styled like LandingPage
    wxPanel* card = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    card->SetBackgroundColour(wxColour(255, 255, 255));

    wxBoxSizer* cardSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* cardTitle = new wxStaticText(card, wxID_ANY, "👥 User Profiles");
    cardTitle->SetFont(wxFontInfo(16).Bold());
    cardTitle->SetForegroundColour(wxColour(0, 70, 150));
    cardSizer->Add(cardTitle, 0, wxLEFT | wxTOP, 10);

    wxStaticLine* line = new wxStaticLine(card);
    cardSizer->Add(line, 0, wxEXPAND | wxALL, 5);

    // Top (profile selector)
    wxBoxSizer* profileTopSizer = new wxBoxSizer(wxHORIZONTAL);
    profileChoice = new wxChoice(card, ID_SELECT_PROFILE);
    wxButton* newProfileBtn = new wxButton(card, ID_NEW_PROFILE, "+ New Profile");
    wxButton* deleteProfileBtn = new wxButton(card, wxID_ANY, "🗑️ Delete");

    newProfileBtn->SetBackgroundColour(wxColour(220, 235, 255));
    deleteProfileBtn->SetBackgroundColour(wxColour(255, 230, 230));

    profileTopSizer->Add(new wxStaticText(card, wxID_ANY, "Select Profile:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    profileTopSizer->Add(profileChoice, 0, wxRIGHT, 10);
    profileTopSizer->Add(newProfileBtn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    profileTopSizer->Add(deleteProfileBtn, 0, wxALIGN_CENTER_VERTICAL);
    cardSizer->Add(profileTopSizer, 0, wxALL | wxEXPAND, 5);

    // Profile fields
    nameCtrl = new wxTextCtrl(card, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    ageCtrl = new wxTextCtrl(card, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    likesCtrl = new wxTextCtrl(card, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));
    strengthsCtrl = new wxTextCtrl(card, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));
    improveCtrl = new wxTextCtrl(card, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));

    wxFlexGridSizer* grid = new wxFlexGridSizer(5, 2, 10, 15);
    grid->Add(new wxStaticText(card, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(nameCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(card, wxID_ANY, "Age:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(ageCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(card, wxID_ANY, "What do you like to do?"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(likesCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(card, wxID_ANY, "Your strengths:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(strengthsCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(card, wxID_ANY, "Things to improve on:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(improveCtrl, 1, wxEXPAND);

    grid->AddGrowableCol(1, 1);
    cardSizer->Add(grid, 1, wxALL | wxEXPAND, 15);

    // Buttons row
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* saveBtn = new wxButton(card, ID_SAVE_PROFILE, "💾 Save");
    continueBtn = new wxButton(card, wxID_ANY, "Continue ➡️");
    continueBtn->Hide();

    saveBtn->SetBackgroundColour(wxColour(220, 245, 220));
    continueBtn->SetBackgroundColour(wxColour(200, 230, 255));

    buttonSizer->Add(saveBtn, 0, wxALL, 5);
    buttonSizer->Add(continueBtn, 0, wxALL, 5);
    cardSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

    card->SetSizer(cardSizer);
    mainSizer->Add(card, 0, wxALL | wxEXPAND, 20);

    SetSizer(mainSizer);
    Centre();

    // 🔗 Event bindings
    saveBtn->Bind(wxEVT_BUTTON, &ProfileSelection::OnSaveProfile, this);
    continueBtn->Bind(wxEVT_BUTTON, &ProfileSelection::OnContinue, this);
    newProfileBtn->Bind(wxEVT_BUTTON, &ProfileSelection::OnNewProfile, this);
    deleteProfileBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        int index = profileChoice->GetSelection();
        if (index == wxNOT_FOUND) return;

        wxMessageDialog confirmDlg(this,
            "Are you sure you want to delete the profile \"" + profileChoice->GetString(index) + "\"?",
            "Confirm Delete", wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
        if (confirmDlg.ShowModal() == wxID_YES) {
            profilesData.erase(profilesData.begin() + index);
            SaveAllProfiles();
            LoadAllProfiles();
        }
    });
    profileChoice->Bind(wxEVT_CHOICE, &ProfileSelection::OnSelectProfile, this);

    LoadAllProfiles();

    SpeakText("Welcome to SocialSteps. Please select your profile or create a new one to get started.");
}

// ✅ The rest of your logic remains the same below

void ProfileSelection::LoadAllProfiles() {
    std::ifstream f("profiles.json");
    if (f.is_open()) {
        f >> profilesData;
    }

    profileChoice->Clear();
    for (auto& prof : profilesData) {
        profileChoice->Append(prof["name"].get<std::string>());
    }

    if (!profilesData.empty()) {
        currentProfileIndex = 0;
        profileChoice->SetSelection(0);
        LoadProfile(currentProfileIndex);
    }
}

void ProfileSelection::LoadProfile(int index) {
    if (index < 0 || index >= (int)profilesData.size()) return;

    auto& p = profilesData[index];
    nameCtrl->SetValue(p.value("name", ""));
    ageCtrl->SetValue(p.value("age", ""));
    likesCtrl->SetValue(p.value("likes", ""));
    strengthsCtrl->SetValue(p.value("strengths", ""));
    improveCtrl->SetValue(p.value("improve", ""));

    continueBtn->SetLabel("Continue as " + nameCtrl->GetValue() + " ➡️");
    continueBtn->SetToolTip("Continue to SocialSteps as " + nameCtrl->GetValue());
    continueBtn->Show();
    Layout();

    SpeakText("Welcome back, " + nameCtrl->GetValue() + "!");
}

void ProfileSelection::OnSelectProfile(wxCommandEvent& event) {
    int index = profileChoice->GetSelection();
    if (index != wxNOT_FOUND) {
        currentProfileIndex = index;
        LoadProfile(index);
    }
}

void ProfileSelection::OnNewProfile(wxCommandEvent& event) {
    wxString name = wxGetTextFromUser("Enter a name for the new profile:", "New Profile");
    if (name.IsEmpty()) return;

    json newProfile = {
        {"name", name.ToStdString()},
        {"age", ""},
        {"likes", ""},
        {"strengths", ""},
        {"improve", ""}
    };

    profilesData.push_back(newProfile);
    profileChoice->Append(name);
    profileChoice->SetSelection(profilesData.size() - 1);
    currentProfileIndex = profilesData.size() - 1;

    SaveAllProfiles();
    LoadProfile(currentProfileIndex);
}

void ProfileSelection::OnSaveProfile(wxCommandEvent& event) {
    if (currentProfileIndex < 0 || currentProfileIndex >= (int)profilesData.size()) return;

    profilesData[currentProfileIndex]["name"] = nameCtrl->GetValue().ToStdString();
    profilesData[currentProfileIndex]["age"] = ageCtrl->GetValue().ToStdString();
    profilesData[currentProfileIndex]["likes"] = likesCtrl->GetValue().ToStdString();
    profilesData[currentProfileIndex]["strengths"] = strengthsCtrl->GetValue().ToStdString();
    profilesData[currentProfileIndex]["improve"] = improveCtrl->GetValue().ToStdString();

    SaveAllProfiles();
    SpeakText("Profile saved successfully!");
}

void ProfileSelection::SaveAllProfiles() {
    std::ofstream f("profiles.json");
    f << profilesData.dump(4);
}

void ProfileSelection::OnContinue(wxCommandEvent& event) {
    if (nameCtrl->GetValue().IsEmpty() || ageCtrl->GetValue().IsEmpty() ||
        likesCtrl->GetValue().IsEmpty() || strengthsCtrl->GetValue().IsEmpty() ||
        improveCtrl->GetValue().IsEmpty()) {
        wxMessageBox("Please complete all fields before continuing.", "Missing Information", wxOK | wxICON_WARNING);
        return;
    }

    if (nameCtrl->GetValue() != profilesData[currentProfileIndex].value("name", "") ||
        ageCtrl->GetValue() != profilesData[currentProfileIndex].value("age", "") ||
        likesCtrl->GetValue() != profilesData[currentProfileIndex].value("likes", "") ||
        strengthsCtrl->GetValue() != profilesData[currentProfileIndex].value("strengths", "") ||
        improveCtrl->GetValue() != profilesData[currentProfileIndex].value("improve", "")) {
        OnSaveProfile(event);
    }

    name = nameCtrl->GetValue();
    age = ageCtrl->GetValue();
    likes = likesCtrl->GetValue();
    strengths = strengthsCtrl->GetValue();
    improve = improveCtrl->GetValue();

    Close();
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    frame->SetMinSize(wxSize(1200, 768));
}
