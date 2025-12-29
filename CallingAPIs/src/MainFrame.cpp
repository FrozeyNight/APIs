#include "MainFrame.h"
#include <wx/wx.h>
#include <vector>
#include <string>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title){
    CreateControls();
    SetupSizers();
}

void MainFrame::CreateControls(){
    // boxSizer with 4 elements
    // the textCtrl and add button are in a nested sizer with the textCtrl having a proportion
    // that nested sizer can extend only horizontally
    // the checkListBox fills all available space
    // the clearButton is center aligned and the clear button is left aligned, they don't expand

    wxFont headlineFont(wxFontInfo(wxSize(0, 36)).Bold()); // 0 means "choose a suitable width"
    wxFont mainFont(wxFontInfo(wxSize(0, 24)));

    panel = new wxPanel(this); // here we're referring to the panel we created in the header file
    panel->SetFont(mainFont);

    headlineText = new wxStaticText(panel, wxID_ANY, "myWeather");
    headlineText->SetFont(headlineFont);

    autoCoords = new wxCheckBox(panel, wxID_ANY, "Get Coordinates Automatically", wxDefaultPosition, wxDefaultSize, wxCHK_CHECKED);
    latText = new wxStaticText(panel, wxID_ANY, "Latitude: ");
    latInput = new wxTextCtrl(panel, wxID_ANY);
    lonText = new wxStaticText(panel, wxID_ANY, "Longitude: ");
    lonInput = new wxTextCtrl(panel, wxID_ANY);
    allOptions = new wxCheckBox(panel, wxID_ANY, "Display All Data", wxDefaultPosition, wxDefaultSize, wxCHK_CHECKED);
    weatherOptions = new wxCheckListBox(panel, wxID_ANY);
    output = new wxListBox(panel, wxID_ANY);
}

void MainFrame::SetupSizers(){
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(headlineText, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(25);

    wxBoxSizer* optionsAndOutputSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);
    optionsSizer->Add(autoCoords);
    optionsSizer->AddSpacer(10);

    wxBoxSizer* latitudeSizer = new wxBoxSizer(wxHORIZONTAL);
    latitudeSizer->Add(latText);
    latitudeSizer->AddSpacer(10);
    latitudeSizer->Add(latInput);

    wxBoxSizer* longitudeSizer = new wxBoxSizer(wxHORIZONTAL);
    longitudeSizer->Add(lonText);
    longitudeSizer->AddSpacer(10);
    longitudeSizer->Add(lonInput);

    optionsSizer->Add(latitudeSizer);
    optionsSizer->AddSpacer(10);
    optionsSizer->Add(longitudeSizer);

    optionsSizer->Add(allOptions);
    optionsSizer->AddSpacer(10);
    optionsSizer->Add(weatherOptions, wxSizerFlags().Expand().Proportion(1));
    optionsAndOutputSizer->Add(optionsSizer, wxSizerFlags().Expand().Proportion(1));
    optionsAndOutputSizer->AddSpacer(50);
    optionsAndOutputSizer->Add(output, wxSizerFlags().Expand().Proportion(1));

    mainSizer->Add(optionsAndOutputSizer, wxSizerFlags().Expand().Proportion(1));

    wxGridSizer* outerSizer = new wxGridSizer(1);
    outerSizer->Add(mainSizer, wxSizerFlags().Expand().Border(wxALL, 25));

    panel->SetSizer(outerSizer);
    outerSizer->SetSizeHints(this);
}
