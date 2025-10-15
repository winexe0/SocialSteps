#include "SocialSkillsPanel.h"
#include <wx/splitter.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/filedlg.h>
#include <fstream>
#include <sstream>
#include "../external/json.hpp"
#include "../rc/social_skills.h"
#include "SpeakText.h"
#include "App.h"
using json = nlohmann::json;

SocialSkillsPanel::SocialSkillsPanel(wxWindow* parent) : wxPanel(parent) {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    auto* rightSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Choose a social skill to learn:"), 0, wxALL, 5);

    // Search box for filtering skills
    searchCtrl = new wxTextCtrl(this, wxID_ANY);
    mainSizer->Add(searchCtrl, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    searchCtrl->Bind(wxEVT_TEXT, &SocialSkillsPanel::OnSearchChanged, this);

    // Splitter for list and description
    splitter = new wxSplitterWindow(this, wxID_ANY);
    wxPanel* rightPanel = new wxPanel(splitter);
    skillList = new wxListBox(splitter, wxID_ANY, wxDefaultPosition, wxSize(250, -1));
    // Create an inner vertical splitter for description + video
    innerSplitter = new wxSplitterWindow(splitter, wxID_ANY);
    innerSplitter->SetSashGravity(0.6); // give more space to description

    skillDescription = new wxRichTextCtrl(innerSplitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                      wxRE_MULTILINE | wxRE_READONLY | wxVSCROLL);

    videoPlayer = wxWebView::New(innerSplitter, wxID_ANY, "about:blank", wxDefaultPosition, wxDefaultSize);
    videoPlayer->Hide(); // Hide initially

    innerSplitter->SplitHorizontally(skillDescription, videoPlayer);
    innerSplitter->Unsplit(videoPlayer); // start hidden

    splitter->SplitVertically(skillList, innerSplitter, 250);
    splitter->SetSashGravity(0.3);

    splitter->SetSashGravity(0.3);

    mainSizer->Add(splitter, 1, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);

    skillList->Bind(wxEVT_LISTBOX, &SocialSkillsPanel::OnSkillSelected, this);

    LoadSkillsFromFile();

    PopulateSkillList();
}

void SocialSkillsPanel::LoadSkillsFromFile() {
    nlohmann::json j = nlohmann::json::parse(std::string(skillsjsonData));
    skillsData.clear();
    for (const auto& item : j) {
        SkillEntry entry;
        entry.skill = wxString::FromUTF8(item.at("skill").get<std::string>());
        entry.definition = wxString::FromUTF8(item.at("definition").get<std::string>());
        entry.youtubeUrl = item.value("youtube_url", "");
        for (const auto& ex : item.at("examples"))
            entry.examples.push_back(wxString::FromUTF8(ex.get<std::string>()));
        for (const auto& why : item.at("why_it_matters"))
            entry.whyItMatters.push_back(wxString::FromUTF8(why.get<std::string>()));
            skillsData.push_back(entry);
    }
}
void SocialSkillsPanel::PopulateSkillList(const wxString& filter) {
    skillList->Clear();
    for (const auto& skill : skillsData) {
        if (filter.IsEmpty() || skill.skill.Lower().Find(filter.Lower()) != wxNOT_FOUND) {
            skillList->Append(skill.skill);
        }
    }

    // Clear description if no skill selected or list emptied
    skillDescription->Clear();
}

void SocialSkillsPanel::OnSearchChanged(wxCommandEvent& event) {
    wxString filter = searchCtrl->GetValue();
    PopulateSkillList(filter);
}

void SocialSkillsPanel::OnSkillSelected(wxCommandEvent& event) {
    wxString selected = skillList->GetStringSelection();
    if (selected.IsEmpty()) {
        skillDescription->Clear();
        return;
    }

    const SkillEntry* found = nullptr;
    for (const auto& skill : skillsData) {
        if (skill.skill == selected) {
            found = &skill;
            break;
        }
    }

    if (!found) {
        skillDescription->Clear();
        return;
    }
    auto it = skillThemeColors.find(found->skill);
    wxColour bg = (it != skillThemeColors.end()) ? it->second : wxColour(248, 248, 255); // fallback
    SetBackgroundColourIncludingAllChildren(this, bg);

    // Build formatted rich text
    skillDescription->Freeze();
    skillDescription->Clear();

    skillDescription->BeginBold();
    skillDescription->WriteText(found->skill + "\n\n");
    skillDescription->EndBold();

    skillDescription->BeginItalic();
    skillDescription->WriteText("Definition:\n");
    skillDescription->EndItalic();

    skillDescription->WriteText(found->definition + "\n\n");

    skillDescription->BeginItalic();
    skillDescription->WriteText("Examples:\n");
    skillDescription->EndItalic();

    for (const auto& ex : found->examples) {
        skillDescription->WriteText("• " + ex + "\n");
    }

    skillDescription->WriteText("\n");
    skillDescription->BeginItalic();
    skillDescription->WriteText("Why it matters:\n");
    skillDescription->EndItalic();

    for (const auto& why : found->whyItMatters) {
        skillDescription->WriteText("• " + why + "\n");
    }
    if (!found->youtubeUrl.IsEmpty()) {
        if (!innerSplitter->IsSplit())
            innerSplitter->SplitHorizontally(skillDescription, videoPlayer);
        videoPlayer->Show();
        videoPlayer->LoadURL(found->youtubeUrl);
    } else {
        if (innerSplitter->IsSplit())
            innerSplitter->Unsplit(videoPlayer);
        videoPlayer->Hide();
    }

    skillDescription->Thaw();
    skillDescription->ShowPosition(0);
    videoPlayer->GetParent()->Layout();
    SpeakText(found->skill + ". Definition:  " + found->definition + " Example: " + found->examples[0] + ". Why does this matter?" + found->whyItMatters[0]);
}