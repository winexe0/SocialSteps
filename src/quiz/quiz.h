#pragma once
#include <wx/wx.h>
#include "../SpeakText.h"
struct QuizQuestion {
    wxString context;
    wxString question;
    std::vector<wxString> choices;
    wxString answer;
    wxString explanation;
};