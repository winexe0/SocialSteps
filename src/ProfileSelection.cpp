#include "ProfileSelection.h"
#include "App.h"
#include "../external/json.hpp"
#include "SpeakText.h"
#include <fstream>
using json = nlohmann::json;
wxString name, age, likes, strengths, improve;
ProfileSelection::ProfileSelection() : wxFrame(nullptr, wxID_ANY, "Select Your Profile or Create a New One", wxDefaultPosition, wxSize(800, 600)) {
    wxBoxSizer* profileBox = new wxStaticBoxSizer(wxVERTICAL, this, "👥 User Profiles");
    wxBoxSizer* profileTopSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    profileChoice = new wxChoice(this, ID_SELECT_PROFILE);
    wxButton* newProfileBtn = new wxButton(this, ID_NEW_PROFILE, "+ New Profile");

    profileTopSizer->Add(new wxStaticText(this, wxID_ANY, "Select Profile:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    profileTopSizer->Add(profileChoice, 0, wxRIGHT, 10);
    profileTopSizer->Add(newProfileBtn, 0, wxALIGN_CENTER_VERTICAL);
    profileBox->Add(profileTopSizer, 0, wxALL, 5);

    nameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    ageCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    likesCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));
    strengthsCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));
    improveCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));

    wxFlexGridSizer* grid = new wxFlexGridSizer(5, 2, 5, 10);
    grid->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(nameCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "Age:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(ageCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "What do you like to do?"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(likesCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "Your strengths:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(strengthsCtrl, 1, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "Things to improve on:"), 0, wxALIGN_CENTER_VERTICAL);
    grid->Add(improveCtrl, 1, wxEXPAND);

    grid->AddGrowableCol(1, 1);
    profileBox->Add(grid, 1, wxALL | wxEXPAND, 10);
    continueBtn = new wxButton(this, wxID_ANY);
    continueBtn->Hide();
    wxButton* saveBtn = new wxButton(this, ID_SAVE_PROFILE, "💾 Save Profile");
    buttonSizer->Add(saveBtn, 0, wxALL, 5);
    buttonSizer->Add(continueBtn, 0, wxALL, 5);
    profileBox->Add(buttonSizer, 0, wxALL | wxALIGN_RIGHT, 10);
    saveBtn->Bind(wxEVT_BUTTON, &ProfileSelection::OnSaveProfile, this);
    continueBtn->Bind(wxEVT_BUTTON, &ProfileSelection::OnContinue, this);
    newProfileBtn->Bind(wxEVT_BUTTON, &ProfileSelection::OnNewProfile, this);
    profileChoice->Bind(wxEVT_CHOICE, &ProfileSelection::OnSelectProfile, this);
    SetSizerAndFit(profileBox);
    LoadAllProfiles();
    SpeakText("Welcome to SocialSteps. Please select your profile or create a new one if you are using SocialSteps for the first time.");
}
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
    profileChoice->Fit();
    //systemPrompt.Append(" You are speaking to " + nameCtrl->GetValue() + ", who is " + ageCtrl->GetValue() + " years old. They like to " + likesCtrl->GetValue() + ". Their strengths include " + strengthsCtrl->GetValue() + ". They want to improve on " + improveCtrl->GetValue() + ".");
    continueBtn->SetLabel("Continue as " + nameCtrl->GetValue() + " ➡️");
    continueBtn->SetToolTip("Continue to SocialSteps as " + nameCtrl->GetValue());
    continueBtn->Fit();
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
void ProfileSelection::OnContinue(wxCommandEvent& event){
    if (nameCtrl->GetValue().IsEmpty() || ageCtrl->GetValue().IsEmpty() || likesCtrl->GetValue().IsEmpty() || strengthsCtrl->GetValue().IsEmpty() || improveCtrl->GetValue().IsEmpty()) {
        wxMessageBox("Please answer all the questions in order for us to help you in the best way possible. Thank You!", "One or More fields Not Complete", wxOK | wxICON_ERROR);
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
    ProfileSelection* profileselection = dynamic_cast<ProfileSelection*>(wxGetTopLevelParent(this));
    profileselection->Close();
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    frame->SetMinSize(wxSize(1200, 768));
}