#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame{
public:
    MainFrame(const wxString& title);
private:
    void CreateControls();
    void SetupSizers();
    wxPanel* panel;
    wxStaticText* headlineText;
    wxCheckBox* autoCoords;
    wxCheckBox* allOptions;
    wxStaticText* latText;
    wxTextCtrl* latInput;
    wxStaticText* lonText;
    wxTextCtrl* lonInput;
    wxCheckListBox* weatherOptions;
    wxListBox* output;
};