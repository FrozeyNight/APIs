#pragma once
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/cmdline.h>

class App : public wxApp{
public:
    virtual bool OnInit() override;
    virtual int OnRun() override;

private:
    bool silent_mode;
    bool shouldExit = false;
    wxArrayString arguments;
    
    void AttachAppToConsole();
    void CreateNewConsole();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    virtual bool OnCmdLineHelp(wxCmdLineParser& parser);
};