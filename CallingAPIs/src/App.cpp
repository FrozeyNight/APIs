#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include "CallAPI.h"
#include <vector>
#include "sample.xpm"
#ifdef _WIN32
    #include <iostream>
    #include <windows.h>
    #include <wx/msgout.h>
#endif


wxIMPLEMENT_APP(App);

bool App::OnInit(){

    #if defined(_WIN32) && defined(RUNNING_AS_CONSOLE)
        // Tell wxWidgets to print "Message" outputs (like -h) to the console (stderr) instead of popping up a MessageBox.
        delete wxMessageOutput::Set(new wxMessageOutputStderr);
        delete wxLog::SetActiveTarget(new wxLogStderr);
    #endif

    silent_mode = false;
    // This allows the app to read command line arguments
    wxApp::OnInit();

    if(shouldExit){
        return true;
    }

    if(silent_mode){
        #if defined(_WIN32) && !defined(RUNNING_AS_CONSOLE)
            CreateNewConsole();
        #endif

        size_t argc = arguments.size() + 1;
        char** argv = new char*[argc];

        std::vector<std::string> storage;
        storage.reserve(argc - 1);

        argv[0] = const_cast<char*>("");
        for (size_t i = 0; i < argc - 1; i++)
        {
            storage.push_back(arguments[i].ToStdString());
            argv[i + 1] = storage.back().data();     
        }

        CallAPI::RunMyWeather(argc, argv, true);

        delete [] argv;
        shouldExit = true;
        return true;
    }
    else{
        MainFrame* mainFrame = new MainFrame("myWeather App", arguments);
        mainFrame->SetClientSize(800, 600);
        mainFrame->Center();
        wxInitAllImageHandlers();
        wxIcon icon(sample_xpm);
        mainFrame->SetIcon(icon);
        mainFrame->Show();
    }
    return true;
}

void App::AttachAppToConsole(){
    #ifdef _WIN32
        if (AttachConsole(ATTACH_PARENT_PROCESS))
        {
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            freopen_s(&fp, "CONIN$", "r", stdin);
            
            std::cout.clear();
            std::cerr.clear();
            std::cin.clear();
        }
    #endif
}

void App::CreateNewConsole(){
    #ifdef _WIN32
        if (!AllocConsole()) {
            return;
        }

        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);

        std::cout.clear();
        std::clog.clear();
        std::cerr.clear();
        std::cin.clear();
        
        SetConsoleTitle(L"myWeather");
    #endif
}

void App::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddSwitch("v", "version", "Shows the application version", 0);
    parser.AddSwitch("s", "silent", "Makes the application work in the console only, no window/GUI will be shown", 0);
    parser.AddSwitch("gc", "getcoords", "Automatically fetch coordinates using the internet");
    parser.AddSwitch("ao", "alloptions", "Choose all weather options for the program to display (the user no longer has to input them during the program runtime)");
    #ifdef __linux__
        parser.AddOption("c", "coords", "Manually specify coordinates the program will use to get weather data. Example: \"-c 42,16\", \"--coords 66.21,53.112\"");
        parser.AddOption("o", "options", "Manually specify weather options the program will display data for. Example: \"-o 2,4,1\", \"--options 1\"");
    #else
        parser.AddOption("c", "coords", "Manually specify coordinates the program will use to get weather data. Example: \" -c \"42,16\" \", \" --coords \"66.21,53.112\" \"");
        parser.AddOption("o", "options", "Manually specify weather options the program will display data for. Example: \" -o \"2,4,1\" \", \"--options \"1\" \"");
    #endif
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (!wxApp::OnCmdLineParsed(parser))
        return false;

    wxString optionValueHolder = "";
    
    if (parser.Found("version")){
        #ifdef _WIN32
            AttachAppToConsole();
        #endif
        std::cout << "MyWeather 0.9.5" << std::endl;
        shouldExit = true;
    }
    if(parser.Found("silent")){
        silent_mode = true;
        arguments.Add("-s");
    }
    if(parser.Found("getcoords")){
        arguments.Add("-gc");
    }
    if(parser.Found("alloptions")){
        arguments.Add("-ao");
    }
    if(parser.Found("coords", &optionValueHolder)){
        arguments.Add("-c" + optionValueHolder);
    }
    if(parser.Found("options", &optionValueHolder)){
        arguments.Add("-o" + optionValueHolder);
    }
    

    return true;
}

bool App::OnCmdLineHelp(wxCmdLineParser& parser)
{
    parser.Usage();
    shouldExit = true;
    return false;
}

int App::OnRun(){
    if(shouldExit){
        return 0;
    }

    return wxApp::OnRun();
}