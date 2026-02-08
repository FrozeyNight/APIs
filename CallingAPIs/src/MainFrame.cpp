#include "MainFrame.h"
#include <wx/wx.h>
#include <vector>
#include <string>
#include "CallAPI.h"
#include <wx/dialup.h>

MainFrame::MainFrame(const wxString& title, const wxArrayString& args) : wxFrame(nullptr, wxID_ANY, title){
    CreateControls();
    SetupSizers();
    BindEventHandlers();
    HandleCmdArguments(args);
}

void MainFrame::CreateControls(){

    wxFont headlineFont(wxFontInfo(wxSize(0, 36)).Bold()); // 0 means "choose a suitable width"
    wxFont mainFont(wxFontInfo(wxSize(0, 24)));

    panel = new wxPanel(this); // here we're referring to the panel we created in the header file
    panel->SetFont(mainFont);

    headlineText = new wxStaticText(panel, wxID_ANY, "myWeather");
    headlineText->SetFont(headlineFont);

    autoCoords = new wxCheckBox(panel, wxID_ANY, "Get Coordinates Automatically");
    autoCoords->SetValue(1);

    latText = new wxStaticText(panel, wxID_ANY, "Latitude: ");
    latText->Disable();
    wxTextValidator onlyAllowNumbers = wxTextValidator(wxFILTER_NUMERIC);
    latInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, onlyAllowNumbers);
    latInput->Disable();

    lonText = new wxStaticText(panel, wxID_ANY, "Longitude: ");
    lonText->Disable();
    lonInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, onlyAllowNumbers);
    lonInput->Disable();

    allOptions = new wxCheckBox(panel, wxID_ANY, "Display All Data");
    allOptions->SetValue(1);
    weatherOptions = new wxCheckListBox(panel, wxID_ANY);
    weatherOptions->InsertItems(CallAPI::weatherOptions, 0);
    weatherOptions->Disable();
    showDataButton = new wxButton(panel, wxID_ANY, "Show Data");
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

    wxBoxSizer* outputSizer = new wxBoxSizer(wxVERTICAL);
    outputSizer->Add(showDataButton, wxSizerFlags().Expand());
    outputSizer->AddSpacer(10);
    outputSizer->Add(output, wxSizerFlags().Expand().Proportion(1));

    optionsAndOutputSizer->Add(outputSizer, wxSizerFlags().Expand().Proportion(1));

    mainSizer->Add(optionsAndOutputSizer, wxSizerFlags().Expand().Proportion(1));

    wxGridSizer* outerSizer = new wxGridSizer(1);
    outerSizer->Add(mainSizer, wxSizerFlags().Expand().Border(wxALL, 25));

    panel->SetSizer(outerSizer);
    outerSizer->SetSizeHints(this);
}

void MainFrame::BindEventHandlers(){
    showDataButton->Bind(wxEVT_BUTTON, &MainFrame::OnShowDataButtonClicked, this);
    autoCoords->Bind(wxEVT_CHECKBOX, &MainFrame::OnAutoCoordsCheckBoxClicked, this);
    allOptions->Bind(wxEVT_CHECKBOX, &MainFrame::OnAllOptionsCheckBoxClicked, this);
}

void MainFrame::HandleCmdArguments(wxArrayString cmdParsedArguments){
    for(wxString argument : cmdParsedArguments){
        if(argument[0] == '-' && argument[1] == 'c'){
            autoCoords->SetValue(0);
            latInput->SetValue(argument.substr(2, argument.find(',') - 2));
            lonInput->SetValue(argument.substr(argument.find(',') + 1, argument.length() - argument.find(',') - 1));
            
            latText->Enable();
            latInput->Enable();
            lonText->Enable();
            lonInput->Enable();
        }
        else if(argument[0] == '-' && argument[1] == 'o'){
            std::string argumentHolder = argument.ToStdString();
            std::vector<int> options = CallAPI::ParseOptions(argumentHolder);

            for(int option : options){
                weatherOptions->Check(option - 1);
            }

            allOptions->SetValue(0);
            weatherOptions->Enable();
        }
    }
}

void MainFrame::OnShowDataButtonClicked(wxCommandEvent& evt){
    showDataButton->Disable();
    output->Clear();
    std::string argument1Holder = "-gc";
    std::string argument2Holder = "-ao";

    if(!autoCoords->IsChecked()){
        if(latInput->IsEmpty() || lonInput->IsEmpty()){
            wxLogWarning("You must enter coordinates manually in the \"Latitude\" and \"Longitude\" input boxes if the \"Get Coordinates Automatically\" option is disabled.");
            return;
        }

        wxString holder = latInput->GetValue();
        holder.Replace(",", ".", true);
        latInput->SetValue(holder);
        
        holder = lonInput->GetValue();
        holder.Replace(",", ".", true);
        lonInput->SetValue(holder);

        argument1Holder = ("-c" + latInput->GetValue() + "," + lonInput->GetValue()).ToStdString();
    }

    if(!allOptions->IsChecked()){

        argument2Holder = "-o";
        
        bool atLeastOneOptionChecked = false;
        for (size_t i = 0; i < weatherOptions->GetCount(); i++)
        {
            if(weatherOptions->IsChecked(i)){
                argument2Holder += std::to_string(i + 1) + ",";
                atLeastOneOptionChecked = true;
            }
        }

        if(!atLeastOneOptionChecked){
            wxLogWarning("You must select at least 1 weather option to show data for if the \"Display All Data\" option is disabled.");
            return;
        }

    }

    char* argv[] = {(char*)"", argument1Holder.data(), argument2Holder.data()};
    int argc = 1 + !argument1Holder.empty() + !argument2Holder.empty();

    output->Insert("Loading...", 0);
    wxTheApp->Yield();
    std::vector<std::string> weatherData = CallAPI::RunMyWeather(argc, argv);

    showDataButton->Enable();

    if(CallAPI::isCurlOK == false){
        wxLogError(weatherData[0]);
        return;
    }

    if(!weatherData.empty()){
        output->Clear();
        output->InsertItems(weatherData, 0);
    }
}

void MainFrame::OnAutoCoordsCheckBoxClicked(wxCommandEvent& evt){
    if(autoCoords->GetValue() == 0){
        latText->Enable();
        latInput->Enable();
        lonText->Enable();
        lonInput->Enable();
    }
    else{
        latText->Disable();
        latInput->Disable();
        lonText->Disable();
        lonInput->Disable();
    }
}

void MainFrame::OnAllOptionsCheckBoxClicked(wxCommandEvent& evt){
    if(allOptions->GetValue() == 0){
        weatherOptions->Enable();
    }
    else{
        weatherOptions->Disable();
    }
}
