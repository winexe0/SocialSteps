#pragma once

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/webview.h>
#include <vector>
#include <map>

struct SkillEntry {
    wxString skill;
    wxString definition;
    std::vector<wxString> examples;
    std::vector<wxString> whyItMatters;
    wxString youtubeUrl;
};

class SocialSkillsPanel : public wxPanel {
public:
    SocialSkillsPanel(wxWindow* parent);

private:
    wxSplitterWindow* splitter;
    wxTextCtrl* searchCtrl;
    wxListBox* skillList;
    wxRichTextCtrl* skillDescription;
    wxWebView* videoPlayer;
    wxSplitterWindow* innerSplitter;
    std::vector<SkillEntry> skillsData;
    void LoadSkillsFromFile();
    void PopulateSkillList(const wxString& filter = "");
    void OnSearchChanged(wxCommandEvent& event);
    void OnSkillSelected(wxCommandEvent& event);
    std::map<wxString, wxColour> skillThemeColors = {
    // Empathy & Emotional Skills (Soft Rose: 255, 239, 239)
    { "Empathy", wxColour(255, 239, 239) },
    { "Being Present", wxColour(255, 239, 239) },
    { "Emotional Regulation", wxColour(255, 239, 239) },
    { "Self-Awareness", wxColour(255, 239, 239) },
    { "Patience", wxColour(255, 239, 239) },
    { "Managing Social Anxiety", wxColour(255, 239, 239) },
    { "Listening Without Judgment", wxColour(255, 239, 239) },
    { "Expressing Gratitude", wxColour(255, 239, 239) },
    { "Apologizing Effectively", wxColour(255, 239, 239) },
    { "Admitting Mistakes", wxColour(255, 239, 239) },

    // Communication & Listening (Light Sky Blue: 220, 240, 255)
    { "Active Listening", wxColour(220, 240, 255) },
    { "Listening for Understanding", wxColour(220, 240, 255) },
    { "Assertiveness", wxColour(220, 240, 255) },
    { "Clarity in Communication", wxColour(220, 240, 255) },
    { "Expressing Needs Clearly", wxColour(220, 240, 255) },
    { "Expressing Enthusiasm", wxColour(220, 240, 255) },
    { "Expressing Opinions Respectfully", wxColour(220, 240, 255) },
    { "Summarizing", wxColour(220, 240, 255) },
    { "Giving & Receiving Feedback", wxColour(220, 240, 255) },
    { "Providing Supportive Feedback", wxColour(220, 240, 255) },
    { "Questioning", wxColour(220, 240, 255) },
    { "Storytelling", wxColour(220, 240, 255) },
    { "Sharing Personal Stories", wxColour(220, 240, 255) },
    { "Using Names", wxColour(220, 240, 255) },
    { "Giving Compliments", wxColour(220, 240, 255) },
    { "Giving Encouragement", wxColour(220, 240, 255) },
    { "Clarifying Misunderstandings", wxColour(220, 240, 255) },

    // Social Integration & Relationships (Cornsilk Yellow: 255, 248, 220)
    { "Cooperation & Teamwork", wxColour(255, 248, 220) },
    { "Building Rapport", wxColour(255, 248, 220) },
    { "Encouragement", wxColour(255, 248, 220) },
    { "Building Consensus", wxColour(255, 248, 220) },
    { "Trustworthiness", wxColour(255, 248, 220) },
    { "Respectfulness", wxColour(255, 248, 220) },
    { "Networking", wxColour(255, 248, 220) },
    { "Showing Appreciation", wxColour(255, 248, 220) },
    { "Accepting Differences", wxColour(255, 248, 220) },
    { "Sharing Credit", wxColour(255, 248, 220) },
    { "Celebrating Diversity", wxColour(255, 248, 220) },
    { "Supporting Others' Success", wxColour(255, 248, 220) },
    { "Being Approachable", wxColour(255, 248, 220) },
    { "Offering Help", wxColour(255, 248, 220) },
    { "Maintaining Boundaries", wxColour(255, 248, 220) },
    { "Showing Appreciation for Differences", wxColour(255, 248, 220) },

    // Conflict & Problem Solving (Misty Rose: 255, 228, 225)
    { "Conflict Resolution", wxColour(255, 228, 225) },
    { "Negotiation", wxColour(255, 228, 225) },
    { "Handling Criticism", wxColour(255, 228, 225) },
    { "Managing Silence", wxColour(255, 228, 225) },

    // Self-Management & Planning (Honeydew: 240, 255, 240)
    { "Adaptability", wxColour(240, 255, 240) },
    { "Perspective-Taking", wxColour(240, 255, 240) },
    { "Mindfulness in Interaction", wxColour(240, 255, 240) },
    { "Time Management in Social Settings", wxColour(240, 255, 240) },
    { "Balancing Talking and Listening", wxColour(240, 255, 240) },

    // Nonverbal / Social Cues (Beige: 245, 245, 220)
    { "Non-Verbal Communication", wxColour(245, 245, 220) },
    { "Respecting Personal Space", wxColour(245, 245, 220) },
    { "Using Appropriate Tone and Volume", wxColour(245, 245, 220) },
    { "Using Appropriate Eye Contact", wxColour(245, 245, 220) },
    { "Social Perceptiveness", wxColour(245, 245, 220) },
    { "Reading Group Dynamics", wxColour(245, 245, 220) },

    // Miscellaneous fallback (Lavender: 248, 248, 255)
    { "Humor", wxColour(248, 248, 255) },
    { "Using Humor Appropriately", wxColour(248, 248, 255) },
    { "Giving Clear Instructions", wxColour(248, 248, 255) }
};
};
