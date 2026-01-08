#pragma once
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/cmdline.h>

class App : public wxApp{
public:
    bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

private:
    bool silent_mode;
    wxArrayString arguments;
};