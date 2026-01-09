#pragma once
#include <wx/wx.h>
#include <string>
#include <vector>

class MainFrame : public wxFrame{
public:
    MainFrame(const wxString& title, const wxArrayString& args);
private:
    void CreateControls();
    void SetupSizers();
    void BindEventHandlers();
    void HandleCmdArguments(wxArrayString cmdParsedArguments);

    void OnShowDataButtonClicked(wxCommandEvent& evt);
    void OnAutoCoordsCheckBoxClicked(wxCommandEvent& evt);
    void OnAllOptionsCheckBoxClicked(wxCommandEvent& evt);

    wxPanel* panel;
    wxStaticText* headlineText;
    wxCheckBox* autoCoords;
    wxCheckBox* allOptions;
    wxStaticText* latText;
    wxTextCtrl* latInput;
    wxStaticText* lonText;
    wxTextCtrl* lonInput;
    wxCheckListBox* weatherOptions;
    wxButton* showDataButton;
    wxListBox* output;
};