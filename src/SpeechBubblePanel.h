#pragma once

#include <wx/wx.h>

enum class BubbleDirection {
    Left,
    Right
};

class SpeechBubblePanel : public wxPanel {
public:
    SpeechBubblePanel(wxWindow* parent, const wxString& text = "", BubbleDirection direction = BubbleDirection::Left);

    void SetText(const wxString& text);
    void SetDirection(BubbleDirection direction);

private:
    void OnPaint(wxPaintEvent& event);
    void UpdateSize();  // Resizes panel based on text dimensions

    wxString m_text;
    BubbleDirection m_direction;
};
