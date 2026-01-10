#include <iostream>
#include <string>
#include <curl/curl.h>
#include <set>
#include <sstream>
#include <nlohmann/json.hpp>
#include <vector>
#include "CallAPI.h"

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

std::vector<std::string> CallAPI::RunMyWeather(int argc, char* argv[]){

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
                std::vector<std::string> errorMessage = {"Failed to fetch coordinates from the internet. Please ensure you have a stable internet connection"};
                return errorMessage; 
            }
        }
        else if(argument[0] == '-' && argument[1] == 'c'){
            latitude = std::stod(argument.substr(2, argument.find(',') - 2));
            coordinates.push_back(std::to_string(latitude));

            longitude = std::stod(argument.substr(argument.find(',') + 1, argument.length() - argument.find(',') - 1));
            coordinates.push_back(std::to_string(longitude));
            areCoordsSet = true;
        }
        else if(argument == "-ao"){
            for (size_t i = 1; i < 6; i++)
            {
                saveUserInput(i, &apiAddress);
            }
            areOptionsSet = true;
        }
        else if(argument[0] == '-' && argument[1] == 'o'){
            std::vector<int> options = ParseOptions(argument);
            
            for(int option : options){
                saveUserInput(option, &apiAddress);
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
    if(callAPI(apiAddress, &weatherData)){
        if(displayOutputInConsole){
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
        std::vector<std::string> errorMessage = {"Failed to fetch weather data from the internet. Please ensure you have a stable internet connection"};
        return errorMessage;
    }
    
    coordinates.clear();
    userOptionsLiteral.clear();
    userOptions.clear();

    CallAPI::isCurlOK = true;
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
            std::cout << "Please enter a number from 0-5" << "\n";
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

bool CallAPI::callAPI(std::string apiAddress, std::string *output){
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

        response = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if(response == CURLE_OK){
            *output = readBuffer;
            return true;
        }
        else{
            std::cout << "curl_easy_perform() failed: " << curl_easy_strerror(response) << "\n";
            return false;
        }

    }

    return false;
}

std::vector<int> CallAPI::ParseOptions(std::string argument){
    std::vector<int> options;

    int optionIndex = 0;
    int position = 1;
    if(argument[argument.length() - 1] != ','){
        argument = argument + ',';
    }
    for (size_t i = 2; i < argument.length(); i++)
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

    return options;
}