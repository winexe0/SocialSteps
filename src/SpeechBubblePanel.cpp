#include "SpeechBubblePanel.h"
#include "SpeakText.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/renderer.h>
#include <wx/tokenzr.h>

SpeechBubblePanel::SpeechBubblePanel(wxWindow* parent, const wxString& text, BubbleDirection direction)
    : wxPanel(parent), m_text(text), m_direction(direction)
{
    Bind(wxEVT_PAINT, &SpeechBubblePanel::OnPaint, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    UpdateSize();
}

void SpeechBubblePanel::SetText(const wxString& text) {
    m_text = text;
    UpdateSize();
    Refresh();
}

void SpeechBubblePanel::SetDirection(BubbleDirection direction) {
    m_direction = direction;
    Refresh();
}

void SpeechBubblePanel::UpdateSize() {
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    int maxWidth = 300;
    wxArrayString lines = wxStringTokenize(m_text, "\n", wxTOKEN_RET_EMPTY);

    wxArrayString wrappedLines;
    for (auto& line : lines) {
        wxString currentLine;
        wxStringTokenizer tokenizer(line, " ", wxTOKEN_RET_EMPTY);
        while (tokenizer.HasMoreTokens()) {
            wxString word = tokenizer.GetNextToken();
            wxCoord width;
            dc.GetTextExtent(currentLine + word, &width, nullptr);
            if (width > maxWidth && !currentLine.IsEmpty()) {
                wrappedLines.Add(currentLine.Trim());
                currentLine = word + " ";
            } else {
                currentLine += word + " ";
            }
        }
        if (!currentLine.IsEmpty())
            wrappedLines.Add(currentLine.Trim());
    }

    wxCoord lineHeight;
    dc.GetTextExtent("Ay", nullptr, &lineHeight);

    int verticalPadding = 20;
    int tailHeight = 20;
    int extraBottomPadding = 10; // Extra space to avoid overlap

    int height = wrappedLines.GetCount() * lineHeight + verticalPadding * 2 + tailHeight + extraBottomPadding;
    int width = maxWidth + 20;  // horizontal padding

    SetMinSize(wxSize(width, height));
    SetSize(wxSize(width, height));
    Layout();
}


void SpeechBubblePanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    dc.SetFont(GetFont());
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxBLACK_PEN);

    wxSize sz = GetClientSize();
    int padding = 10;
    int tailSize = 15;

    wxRect bubbleRect(padding, padding, sz.GetWidth() - 2 * padding, sz.GetHeight() - 2 * padding - tailSize);
    dc.DrawRoundedRectangle(bubbleRect, 10);

    // Tail triangle
    wxPoint tail[3];
    if (m_direction == BubbleDirection::Left) {
        tail[0] = wxPoint(bubbleRect.GetLeft(), bubbleRect.GetBottom() - 1);
        tail[1] = wxPoint(bubbleRect.GetLeft(), bubbleRect.GetBottom() - 16);
        tail[2] = wxPoint(bubbleRect.GetLeft() - tailSize, bubbleRect.GetBottom() - 8);
    } else {
        tail[0] = wxPoint(bubbleRect.GetRight() - 20, bubbleRect.GetBottom());
        tail[1] = wxPoint(bubbleRect.GetRight() - 35, bubbleRect.GetBottom());
        tail[2] = wxPoint(bubbleRect.GetRight() - 27, bubbleRect.GetBottom() + tailSize);
    }
    dc.DrawPolygon(3, tail);

    // Draw text
    dc.SetTextForeground(*wxBLACK);
    wxCoord lineHeight;
    dc.GetTextExtent("Ay", nullptr, &lineHeight);

    int x = bubbleRect.x + 10;
    int y = bubbleRect.y + 10;
    int maxWidth = bubbleRect.width - 20;

    wxArrayString lines = wxStringTokenize(m_text, "\n", wxTOKEN_RET_EMPTY);
    for (auto& line : lines) {
        wxString currentLine;
        wxStringTokenizer tokenizer(line, " ", wxTOKEN_RET_EMPTY);
        while (tokenizer.HasMoreTokens()) {
            wxString word = tokenizer.GetNextToken();
            wxCoord width;
            dc.GetTextExtent(currentLine + word, &width, nullptr);
            if (width > maxWidth) {
                dc.DrawText(currentLine.Trim(), x, y);
                y += lineHeight;
                currentLine = word + " ";
            } else {
                currentLine += word + " ";
            }
        }
        if (!currentLine.IsEmpty()) {
            dc.DrawText(currentLine.Trim(), x, y);
            y += lineHeight;
        }
    }
}

