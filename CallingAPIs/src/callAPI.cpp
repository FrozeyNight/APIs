#include <iostream>
#include <string>
#include <curl/curl.h>
#include <set>
#include <sstream>
#include <nlohmann/json.hpp>
#include <vector>
#include "CallAPI.h"
#include <exception>

size_t CallAPI::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::vector<std::string> CallAPI::userOptions;
std::vector<std::string> CallAPI::userOptionsLiteral;
std::vector<std::string> CallAPI::coordinates;
std::vector<std::string> CallAPI::weatherOptions = {"Temperature (Celsius)", "Apparent Temperature (Celsius)", "Relative Humidity (%)", "Wind Speed (km/h)", "Cloud Cover (%)"};
std::vector<std::string> CallAPI::weatherOptionsLiteral = {"temperature_2m", "apparent_temperature", "relative_humidity_2m", "wind_speed_10m", "cloud_cover"};
bool CallAPI::isCurlOK = false;
bool CallAPI::isArgumentsOK = false;
std::string curlTimeoutErrorCode = "2";
std::string curlGeneralErrorCode = "1";
int APICallRetryLimit = 5;

std::vector<std::string> CallAPI::RunMyWeather(int argc, char* argv[], bool doShowConsoleErrorMessages){

    std::string apiAddress = "";

    apiAddress = "https://api.open-meteo.com/v1/forecast?timezone=Europe%2FBerlin&latitude=";

    double latitude = 0;
    double longitude = 0;
    std::string arguments[argc];
    bool areCoordsSet = false;
    bool areOptionsSet = false;
    bool displayOutputInConsole = false;

    for(int i = 1; i < argc; i++){
        arguments[i - 1] = argv[i];
    }

    for(std::string argument : arguments){
        if(argument == "-gc"){
            std::string geolocationData;
            
            if(displayOutputInConsole){
                std::cout << "Fetching Coordinates..." << std::endl;
            }

            if(callAPI("http://ip-api.com/json/", &geolocationData)){
                nlohmann::json data = nlohmann::json::parse(geolocationData);
                latitude = data["lat"];
                coordinates.push_back(std::to_string(latitude));

                longitude = data["lon"];
                coordinates.push_back(std::to_string(longitude));
                areCoordsSet = true;
            }
            else{
                CallAPI::isCurlOK = false;
                
                std::vector<std::string> errorMessage;
                if(geolocationData == curlTimeoutErrorCode){
                    errorMessage.push_back("The Coordinate fetching API responds, but does not return any data. Please try again later.");
                }
                else{
                    errorMessage.push_back("Failed to fetch coordinates from the internet. Please ensure you have a stable internet connection");
                }
                
                if(doShowConsoleErrorMessages){
                    std::cout << errorMessage[0] << std::endl;
                }
                
                ClearData();
                return errorMessage; 
            }
        }
        else if(argument[0] == '-' && argument[1] == 'c'){
            try{
                latitude = std::stod(argument.substr(2, argument.find(',') - 2));
                longitude = std::stod(argument.substr(argument.find(',') + 1, argument.length() - argument.find(',') - 1));
                
                bool incorrectLatitude = latitude > 90 || latitude < -90;
                bool incorrectLongitude = longitude > 180 || longitude < -180;

                if(doShowConsoleErrorMessages){
                    if(incorrectLatitude){
                        std::cout << "Latitude must be a number between -90 and 90" << std::endl;
                    }
                    if(incorrectLongitude){
                        std::cout << "Longitude must be a number between -180 and 180" << std::endl;
                    }
                }
                else{
                    std::vector<std::string> errorMessage = {""};
                    bool error = incorrectLatitude || incorrectLongitude;
                    if(incorrectLatitude){
                        CallAPI::isArgumentsOK = false;
                        errorMessage[0] = "Latitude must be a number between -90 and 90\n";
                    }
                    if(incorrectLongitude){
                        CallAPI::isArgumentsOK = false;
                        errorMessage[0] += "Longitude must be a number between -180 and 180";
                    }

                    if(error){
                        ClearData();
                        return errorMessage;
                    }
                }

                if(incorrectLatitude == false && incorrectLongitude == false){
                    coordinates.push_back(std::to_string(latitude));
                    coordinates.push_back(std::to_string(longitude));
                    areCoordsSet = true;
                }
            }
            catch(std::exception& e){
                CallAPI::isArgumentsOK = false;
                std::vector<std::string> errorMessage = {"Failed to parse coordinates. Please ensure you follow the correct format \"-c10.2,40.42\" or \"--coords10.2,40.42\""};
                
                if(doShowConsoleErrorMessages){
                    std::cout << "Failed to parse coordinates. Please ensure you follow the correct format \"-c10.2,40.42\" or \"--coords10.2,40.42\"\n";
                }

                ClearData();
                return errorMessage; 
            }
        }
        else if(argument == "-ao"){
            for (size_t i = 1; i < 6; i++)
            {
                saveUserInput(i, &apiAddress);
            }
            areOptionsSet = true;
        }
        else if(argument[0] == '-' && argument[1] == 'o'){
            std::vector<int> reversedOptions = ParseOptions(argument);

            for (int i = reversedOptions.size() - 1; i > -1; i--)
            {
                saveUserInput(reversedOptions[i], &apiAddress);
            }
            

            areOptionsSet = true;
        }
        else if(argument == "-s"){
            displayOutputInConsole = true;
        }
    }

    if(!areCoordsSet){
        getUserCoordinates(&latitude, &longitude);
    }

    apiAddress = apiAddress + std::to_string(latitude) + "&longitude=" + std::to_string(longitude) + "&current=";

    if(!areOptionsSet){
        getUserOptions(&apiAddress);
    }

    for(std::string userOptionLiteral : userOptionsLiteral){
        apiAddress += "," + userOptionLiteral;
    }


    std::string weatherData = "";
    std::vector<std::string> formattedWeatherData;

    if(displayOutputInConsole){
        std::cout << "Fetching WeatherData..." << std::endl;
    }

    if(callAPI(apiAddress, &weatherData)){
        if(displayOutputInConsole){
            std::cout << std::endl;
            std::cout << "The chosen data for " << coordinates[0].substr(0, coordinates[0].find('.') + 3) << ", " << coordinates[1].substr(0, coordinates[1].find('.') + 3) << ":\n";
        }
        nlohmann::json data = nlohmann::json::parse(weatherData);
        const auto& current = data["current"];
        for (size_t i = 0; i < userOptionsLiteral.size() && !userOptionsLiteral[i].empty(); ++i) {
            if (current.contains(userOptionsLiteral[i])) {
                if(displayOutputInConsole){
                    std::cout << userOptions[i] << current[userOptionsLiteral[i]] << '\n';
                }
                else{
                    formattedWeatherData.push_back(userOptions[i] + current[userOptionsLiteral[i]].dump());
                }
            } else {
                if(displayOutputInConsole){
                    std::cout << userOptions[i] << "Not available\n";
                }
                else{
                    formattedWeatherData.push_back(userOptions[i] + "Not available");
                }
            }
        }
    }
    else{
        CallAPI::isCurlOK = false;
        std::vector<std::string> errorMessage;
        if(weatherData == curlTimeoutErrorCode){
            errorMessage.push_back("The WeatherData DataBase responds, but does not return any data. Please try again later.");
        }
        else{
            errorMessage.push_back("Failed to fetch weather data from the internet. Please ensure you have a stable internet connection");
        }
        if(doShowConsoleErrorMessages){
            std::cout << errorMessage[0] << std::endl;
        }
        
        ClearData();
        return errorMessage;
    }
    
    ClearData();

    CallAPI::isCurlOK = true;
    CallAPI::isArgumentsOK = true;
    return formattedWeatherData;
}

double CallAPI::getValidatedCoordinate(const std::string& name, double min, double max) {
    double value;
    std::string line;
    while (true) {
        std::cout << name << ": ";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (ss >> value && ss.eof()){
            if (value >= min && value <= max) return value;
            std::cout << name << " must be between " << min << " and " << max << "\n";
        }
        else
            std::cout << "Please enter a valid number\n";
    }
}

void CallAPI::saveUserInput(int input, std::string *apiAddress){
    switch (input)
    {
        case 1:
            userOptionsLiteral.push_back(weatherOptionsLiteral[0]);
            userOptions.push_back(weatherOptions[0] + ": ");
            break;
        case 2:
            userOptionsLiteral.push_back(weatherOptionsLiteral[1]);
            userOptions.push_back(weatherOptions[1] + ": ");
            break;
        case 3:
            userOptionsLiteral.push_back(weatherOptionsLiteral[2]);
            userOptions.push_back(weatherOptions[2] + ": ");
            break;
        case 4:
            userOptionsLiteral.push_back(weatherOptionsLiteral[3]);
            userOptions.push_back(weatherOptions[3] + ": ");
            break;
        case 5:
            userOptionsLiteral.push_back(weatherOptionsLiteral[4]);
            userOptions.push_back(weatherOptions[4] + ": ");
            break;
        default:
            std::cout << "\"" << input << "\" is not a valid option. Please only enter numbers from 0 to " << weatherOptions.size() << std::endl;
    }
}

void CallAPI::getUserOptions(std::string *apiAddress){
    int input = 0;
    std::set<int> chosenOptions = {};
    while(true){
        std::cout << "Which data would you like to display? (0 to quit):" << "\n"
        "1. temperature" << "\n"
        "2. aparent temperature" << "\n"
        "3. humidity" << "\n"
        "4. wind speed" << "\n"
        "5. cloud cover" << "\n";

        std::string line;
        std::getline(std::cin, line);
        std::stringstream ss(line);
        
        if(ss >> input && ss.eof()){
            std::cout << "You have chosen option " << input << "\n";
        }
        else{
            std::cout << "Please enter a valid number\n";
            continue;
        }

        if(input == 0)
            break;

        if(!chosenOptions.insert(input).second){
            std::cout << "You have already chosen option " << input << "\n";
            continue;
        }

        saveUserInput(input, apiAddress);
    }

}

void CallAPI::getUserCoordinates(double *latitude, double *longitude){
    *latitude = getValidatedCoordinate("Latitude", -90, 90);
    coordinates.push_back(std::to_string(*latitude));

    *longitude = getValidatedCoordinate("Longitude", -180, 180);
    coordinates.push_back(std::to_string(*longitude));
}

bool CallAPI::callAPI(std::string apiAddress, std::string *output, int retryCount){
    CURL* curl;
    CURLcode response;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl){
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, apiAddress.c_str()); // curl is more for C, so it doesn't know what a std::string is. You have to convert it into a char*
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        response = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if(response == CURLE_OK){
            *output = readBuffer;
            return true;
        }
        else{
            if(response == CURLE_OPERATION_TIMEDOUT){
                *output = curlTimeoutErrorCode;

                if(retryCount < APICallRetryLimit){
                    return callAPI(apiAddress, output, retryCount + 1);
                }
                // for testing
                //std::cout << "Request timed out: " << curl_easy_strerror(response) << "\n";
            }
            else{
                //std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(response) << "\n";
                *output = curlGeneralErrorCode;
            }
            return false;
        }

    }

    return false;
}

std::vector<int> CallAPI::ParseOptions(std::string argument){
    std::vector<int> options;

    int optionIndex = 0;
    int position = 1;
    int argumentLength = argument.length();
    if(argument[argumentLength - 1] == ','){
        argumentLength -= 1;
    }
    for (size_t i = argumentLength - 1; i > 1; i--)
    {
        if(argument[i] == ','){
            options.push_back(optionIndex);
            optionIndex = 0;
            position = 1;
            continue;
        }

        optionIndex += (argument[i] - '0') * position;
        position *= 10;
    }
    options.push_back(optionIndex);

    return options;
}

void CallAPI::ClearData(){
    coordinates.clear();
    userOptionsLiteral.clear();
    userOptions.clear();
}