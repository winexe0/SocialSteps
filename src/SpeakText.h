#pragma once
#include <atomic>
#include <thread>
#include <wx/wx.h>
void SpeakText(wxString text, bool waitForPrevious = false);