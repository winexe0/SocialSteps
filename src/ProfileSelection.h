#pragma once
#include <wx/wx.h>
#include "../external/json.hpp"
class ProfileSelection : public wxFrame {
    public:
        ProfileSelection();
    private:
        wxChoice* profileChoice;
        wxTextCtrl *nameCtrl, *ageCtrl, *likesCtrl, *strengthsCtrl, *improveCtrl;
        wxButton* continueBtn;
        nlohmann::json profilesData;
        int currentProfileIndex = -1;

        void LoadAllProfiles();
        void LoadProfile(int index);
        void SaveAllProfiles();

        void OnSelectProfile(wxCommandEvent& event);
        void OnNewProfile(wxCommandEvent& event);
        void OnSaveProfile(wxCommandEvent& event);
        void OnContinue(wxCommandEvent& event);
};
enum {
    ID_SAVE_PROFILE,
    ID_NEW_PROFILE,
    ID_SELECT_PROFILE
};